/*  
 *  illuminord v0.4
 *  
 *  Copyright (C) 2005
 *  losTrace aka "David R. Piegdon" and cebewee aka "Lars Noschinski"
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include "audacious/plugin.h"

#include "config.h"
#include "cfile.h"
#include "fnordlicht.h"
#include "illuminord.h"
#include "calc.h"

#define ILLUMINORD_C_ID "$Id: illuminord.c 156 2005-10-06 15:33:06Z lostrace $"
#define ILLUMINORD_ID ILLUMINORD_H_ID "\n" ILLUMINORD_C_ID
char *illuminord_id = ILLUMINORD_ID;

static int  cmdok=0;		// cmd has been opened without error?

static void illuminord_init(void);
static void illuminord_cleanup(void);
static void illuminord_playback_start(void);
static void illuminord_playback_stop(void);
static void illuminord_render_freq(gint16 data[2][256]);

// main callback and data for plugin:
VisPlugin illuminord_vp = {
//	NULL,						/* reserved		*/
//	NULL,						/* reserved		*/
//	0,						/* _session		*/
	.description = "illuminord v0.4a",				/* description		*/
	.num_pcm_chs_wanted = 0,						/* num_pcm_chs_wanted	*/
	.num_freq_chs_wanted = 1,						/* num_freq_chs_wanted  */
	.init = illuminord_init,				/* init			*/
	.cleanup = illuminord_cleanup,				/* cleanup		*/
	.about = illuminord_about,				/* about		*/
	.configure = illuminord_config,				/* configure		*/
//	NULL,						/* disable_plugin	*/
	.playback_start = illuminord_playback_start,				/* playback_start	*/
	.playback_stop = illuminord_playback_stop,				/* playback_stop	*/
//	NULL,						/* render_pcm		*/
	.render_freq = illuminord_render_freq,				/* render_f req		*/
//	.get_widget = bscope_get_widget,
};

/*VisPlugin *get_vplugin_info(void)
{
	return &illuminord_vp;
}*/
VisPlugin *illuminord_vplist[] = { &illuminord_vp, NULL };

DECLARE_PLUGIN(illuminord, NULL, NULL, NULL, NULL, NULL, NULL, illuminord_vplist,NULL);

void commit()
{
	int light;
	
	for( light = 0; light < devcount; light++)
		fnordlicht_setrgb(states[light].i2c_adr,
					states[light].c.r,
					states[light].c.g,
 					states[light].c.b );
}

static void set_black()
{
 	int light;

	for( light = 0; light < devcount; light++) {
 		states[light].c.r = 0;
		states[light].c.g = 0;
		states[light].c.b = 0;
	}
	commit();
}

// set light color from value,
// where val = 0.0 .. 1.0
void setfpal(int n)
{
	double val;
	light*l;

	l = &states[n];
	val = l->v;

	// apply capping
	val = val - l->hp;
	val = val * 1/(1 - l->hp);
	
	setcolor( &l->c, l->pm, &l->bc, l->bv, val );

#ifdef DEBUG
	printf("[%2d] = %.5f = { %2x %2x %2x } ", n, val, l->c.r, l->c.g, l->c.b);
	{
		int i;
		for(i = 0; i < val*20; i++)
			putchar('#');
		putchar('\n');
	}
#endif
}

static void illuminord_init(void)
{
	int i;

	// init slcd-table
	for( i = 0; i < MAX_DEVICES; i++) {
		slcds[i].window = slcds[i].button = slcds[i].colorpicker = NULL;
	}

	// allocate state-table depending on number of fnordlichter
	devcount_changed();
	
	// load saved status from last time
	load_from_configfile(CONFIG_FILE);
	
	// open fnordlicht
	cmdok = fnordlicht_open(NULL);
	if(!cmdok)
		fprintf(stderr,"illuminord-: failed to open fnordlicht device \"%s\"!", FN_DEFAULTDEVICE),fflush(stdout);
	else
		commit();
}

static void illuminord_cleanup(void)
{
	if(cmdok)
		cmdok = !fnordlicht_close();

 	save_to_configfile(CONFIG_FILE);
}

static void illuminord_playback_start(void)
{
//	set_black();
}


static void illuminord_playback_stop(void)
{
	set_black();
}

static void illuminord_render_freq(gint16 data[2][256])
{
	int light;				// index running over all lights
	int intnum;				// counter for lights running specific interpreter
	int n;
	double val;				// value of specific light
	double intN[256];  			// area over 0..N of spectral graph
	int peak=0;				// peak of spectral graph

	if(cmdok) {
		// calculate intN[] and peak
		for(n = 0; n <= 255; n++) {
			if(n)
				intN[n] = intN[n-1] + data[0][n]*(n+1);
			else
				intN[n] = data[0][n]*(n+1);

			// while we are on it, find peak
			if(data[0][peak]*(peak+1) < data[0][n]*(n+1))
				peak = n;
		}

		// do all BAND_ANALYZER
		for( intnum = 0, light = 0; intnum < num_bandanalyzer; light++,intnum++ ) {
			// find next light supposed to be a BAND_ANALYZER
			while( states[light].ipr != BAND_ANALYZER )
				light++;

			// calculate normalized area of this band.
			if(states[light].n_ > 0)
				val = (intN[states[light].n] - intN[states[light].n_ - 1]) / (states[light].n - (states[light].n_ - 1));
			else
				val = intN[states[light].n] / (states[light].n + 1);

			val = fabs(val);
			
			if(0 != val) {
				// compensate normalized exponential amplitude
				val = log(val) / 11.;
			}

			if(states[light].v > val) {
				states[light].v*=states[light].fs;
				if( states[light].v < val )
					states[light].v = val;
			} else
				if(states[light].v < val)
					states[light].v = val;
			// calculate color from value and set it
			setfpal(light);
		}

		// do all MOVING_AVERAGE
		if(num_movingaverage) {
			// find n with intN=A/2
			val = unscale( quicksearch(intN, 0, 255, intN[255]/2) );

			// calc color for it
			// and apply for each MOVING_AVERAGE
			for( intnum = 0, light = 0; intnum < num_movingaverage; light++, intnum++ ) {
				while( states[light].ipr != MOVING_AVERAGE )
					light++;

				if(states[light].v == 0)
					states[light].v = 0.02;
				
				if(states[light].v > val) {
					states[light].v *= states[light].fs;
					if(states[light].v < val)
						states[light].v = val;
				} else
					if(states[light].v < val) {
						states[light].v /= states[light].fs;
						if(states[light].v > val)
							states[light].v = val;
					}
				// calculate color from value and set it
				setfpal(light);
			}
		}
		// do all moving_peak
		if(num_movingpeak) {
			val = unscale(peak);
			// and apply for each MOVING_PEAK
			for( intnum = 0, light = 0; intnum < num_movingpeak; light++, intnum++ ) {
				while( states[light].ipr != MOVING_PEAK )
					light++;

				if(states[light].v == 0)
					states[light].v = 0.02;
				
				if(states[light].v > val) {
					states[light].v *= states[light].fs;
					if(states[light].v < val)
						states[light].v = val;
				} else
					if(states[light].v < val) {
						states[light].v /= states[light].fs;
						if(states[light].v > val)
							states[light].v = val;
					}

				// calculate color from value and set it
				setfpal(light);
			}
		}
#ifdef DEBUG
		printf("--\n");
#endif
		// commit changes to lights
		commit();
 	}
}
