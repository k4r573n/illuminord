/*  $Id: config.c 156 2005-10-06 15:33:06Z lostrace $
 *  illuminordxmms v0.3a
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "config.h"
#include "fnordlicht.h"
#include "illuminordxmms.h"
#include "palette.h"
#include "slcd.h"
#include "calc.h"
#include "cfile.h"
#include "language.h"

#define CONFIG_C_ID "$Id: config.c 156 2005-10-06 15:33:06Z lostrace $"
#define CONFIG_ID CONFIG_H_ID "\n" CONFIG_C_ID
char *config_id = CONFIG_ID;
char *language_id = LANGUAGE_H_ID;

light		*states = NULL;		// state table of all relevant fnordlichter
slcd		slcds[MAX_DEVICES];	// static table for all possible config dialogues

static int	odevcount = 0;		// old devcount
int		devcount=DEFAULT_DEVCOUNT;// number of addressable fnordlichter
int		num_bandanalyzer=0;	// number of "band-analyzer" lights
int		num_movingpeak=0;	// number of "moving-peak" lights
int		num_movingaverage=0;	// number of "moving-average" lights

double		cyclic_speed = 0.004;	// speed for the cyclic palette
double		cyclic_value = 0;	// counter for cycler

static void config_single_callback(GtkObject*o, int d);


static GtkWidget *about_window = NULL;



GtkWidget *config_window = NULL;
static GtkWidget *config_vbox;

static GtkWidget *config_sdev_hbox0, *config_sdev_hbox1;

static GtkWidget *config_done;

void interpreter_changed()
{
	int light;
	
	// recalculate num_bandanalyzer, num_movingpeak, num_movingaverage
	num_bandanalyzer = num_movingpeak = num_movingaverage = 0;
	for( light = 0; light < devcount; light++ )
		if( states[light].ipr == BAND_ANALYZER ) {
			num_bandanalyzer++;
		} else
			if( states[light].ipr == MOVING_AVERAGE )
				num_movingaverage++;
			else
				if( states[light].ipr == MOVING_PEAK )
					num_movingpeak++;

}

void devcount_changed()
{
	light*l;
	slcd*slcd;
	char b[5];

	if(odevcount != devcount) {
		if(odevcount > devcount) {
			while(odevcount > devcount) {
				l = &states[odevcount - 1];
				slcd = &slcds[odevcount - 1];
				
				// deallocate config-dialogues and buttons that may exist...
				if(slcd->button)
					gtk_widget_destroy( slcd->button );
				if(slcd->window)
					gtk_widget_destroy( slcd->window );

				odevcount--;
			}
		}
		
		states = realloc(states,sizeof(light)*devcount);
		
		if(odevcount < devcount) {
			// cycle through all new fnordlichter and set some sane defaults.
			while( odevcount < devcount ) {
				odevcount++;
				// this is the light that needs new setup:
				l = &states[odevcount - 1];
				slcd = &slcds[odevcount - 1];

				// we need to have a valid address. hopefully this is right...
				l->i2c_adr = odevcount;

				// color to black
				// of PAL_STRENGTH is black.
				l->c.r = 0;
				l->c.g = 0;
				l->c.b = 0;
				l->v = 0.;

				// set band boundaries
				l->n_ = 0;
				l->n = 0;
				// set default highpass for this light
				l->hp = 0.5;
				// fadespeed
				l->fs = 0.95;
				// BAND_ANALYZER MOVING_AVERAGE or MOVING_PEAK
				l->ipr = BAND_ANALYZER;
				// PAL_STRENGTH PAL_SPECTRUM PAL_SIMPLE or PAL_CYCLIC
				l->pm = PAL_SIMPLE;
				// basic value and basic color
				l->bv = 0;
				l->bc.r = 100;
				l->bc.g = 100;
				l->bc.b = 100;
				// this light does not have a config dialog right now.
				slcds[odevcount-1].window = NULL;
				// allocate new button if we are in the config-dialog
				if(config_window) {
					snprintf(b, sizeof(b), "%d", (odevcount-1)+1 );
					slcd->button = gtk_button_new_with_label(b);
					gtk_signal_connect(GTK_OBJECT(slcd->button), "clicked", GTK_SIGNAL_FUNC(config_single_callback), (void*)(odevcount-1));
					GTK_WIDGET_SET_FLAGS(slcd->button, GTK_CAN_DEFAULT);
					if((odevcount-1) >= MAX_DEVICES/2) 
					 	gtk_box_pack_start(GTK_BOX(config_sdev_hbox1),slcd->button, TRUE, TRUE, 0);
					else
						gtk_box_pack_start(GTK_BOX(config_sdev_hbox0),slcd->button, TRUE, TRUE, 0);
					gtk_widget_show(slcd->button);
				}
			}
		}
		odevcount = devcount;

		// recalculate interpreter-count
		interpreter_changed();
	}
}

void destroy_on_press(GtkObject*o, gpointer*d)
{
	int l;
	
	if( (GtkWidget*)d == config_window ) {
		// destroy all sub-config-dialogues
		for( l = 0; l < devcount ; l++)
			if(slcds[l].window)
				gtk_widget_destroy(slcds[l].window);
	}
	gtk_widget_destroy((GtkWidget*)d);
}

void illuminord_about()
{
	static char*buf = NULL;
	static int s;
	GtkWidget *frame, *label, *vbox, *versionvbox, *ok;

	if( !about_window ) {
		// create window
		about_window = gtk_window_new(GTK_WINDOW_DIALOG);
		gtk_container_set_border_width(GTK_CONTAINER(about_window), 10);
		gtk_window_set_title(GTK_WINDOW(about_window), ABOUT_TITLE);
		gtk_window_set_policy(GTK_WINDOW(about_window), FALSE, FALSE, FALSE);
		gtk_window_set_position(GTK_WINDOW(about_window), GTK_WIN_POS_MOUSE);
		gtk_signal_connect(GTK_OBJECT(about_window), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), &about_window);
			// create box
			vbox = gtk_vbox_new(FALSE,0);
				// create label
				label = gtk_label_new(ABOUT_LABEL);
				gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
				gtk_widget_show(label);
				// create frame for version-info
				frame = gtk_frame_new(ABOUT_VERSION_FRAMELABEL);
					versionvbox = gtk_vbox_new(FALSE, 0);
						// create label for fileversion
						if(!buf) {
							s = strlen(illuminordxmms_id)
								+strlen(fnordlicht_id)
								+strlen(config_id)
								+strlen(slcd_id)
								+strlen(language_id)
								+strlen(cfile_id)
								+strlen(calc_id)
								+strlen(palette_id)
								+10;  // needed for \n of each line...
							buf = malloc(s);
							snprintf(buf,s,"%s\n" "%s\n" "%s\n" "%s\n" "%s\n" "%s\n" "%s\n" "%s",
									illuminordxmms_id,
									fnordlicht_id,
									config_id,
									slcd_id,
									language_id,
									cfile_id,
									calc_id,
									palette_id);
						}
						label = gtk_label_new(buf);
						gtk_box_pack_start(GTK_BOX(versionvbox), label, TRUE, TRUE, 0);
						gtk_widget_show(label);
					gtk_container_add(GTK_CONTAINER(frame), versionvbox);
					gtk_widget_show(versionvbox);
				gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
				gtk_widget_show(frame);
				// create ok-button
				ok = gtk_button_new_with_label(ABOUT_OKTEXT);
				gtk_signal_connect(GTK_OBJECT(ok), "clicked", GTK_SIGNAL_FUNC(destroy_on_press), about_window);
				GTK_WIDGET_SET_FLAGS(ok, GTK_CAN_DEFAULT);
				gtk_box_pack_start(GTK_BOX(vbox), ok, TRUE, TRUE, 0);
				gtk_widget_show(ok);
			gtk_container_add(GTK_CONTAINER(about_window), vbox);
			gtk_widget_show(vbox);
		
		gtk_widget_show(about_window);
	} else
		return;
}

static void config_devcount_callback(GtkAdjustment*adj)
{
	devcount = adj->value;
	devcount_changed();
}

static void config_single_callback(GtkObject*o, int d)
{
	slcd_config(d);
}

static void apply_cyclic_color_callback(GtkObject*o, int t)
{
	int c, n, matches;

	switch(t) {
		case 0:
			// over all with pm=PAL_SIMPLE (base color)
			// count them
			for( c=0,n=0 ; n<devcount; n++)
				if(states[n].pm == PAL_SIMPLE)
					c++;	
			for( matches = 0, n = 0; matches < c; n++,matches++ ) {
				// find next n supposed to be a BAND_ANALYZER
				while( states[n].pm != PAL_SIMPLE)
					n++;
				// set color
				setcolor(&states[n].bc, PAL_SPECTRUM, NULL, 0, n/(double)(c));
			}
			break;
		case 1:
			// over all with pm=PAL_CYCLIC (base value)
			// count them
			for( c=0,n=0 ; n<devcount; n++)
				if(states[n].pm == PAL_CYCLIC)
					c++;	
			for( matches = 0, n = 0; matches < c; n++,matches++ ) {
				// find next n supposed to be a BAND_ANALYZER
				while( states[n].pm != PAL_CYCLIC )
					n++;
				// set basevalue
				states[n].bv = n/(double)(c);
			}
			break;
	}
	
	// refresh slcds:
	for( n = 0; n < devcount; n++)
		if(slcds[n].window) {
			gtk_widget_destroy(slcds[n].window);
			slcd_config(n);
		}
}

static void apply_logarithmic_band_callback(GtkObject*o, int t)
{
	int matches, n;
	int lastn = 0;

	for( matches = 0, n = 0; matches < num_bandanalyzer; n++,matches++ ) {
		// find next n supposed to be a BAND_ANALYZER
		while( states[n].ipr != BAND_ANALYZER )
			n++;
		// get boundaries of this band
		states[n].n_ = scale(  matches      / (double)(num_bandanalyzer) );
		
		if(states[n].n_ < lastn)
			states[n].n_ = lastn;

		states[n].n  = scale( (matches + 1) / (double)(num_bandanalyzer) );
		if(states[n].n <= states[n].n_)
			states[n].n = states[n].n_+1;

		lastn = states[n].n;
	}
	
	// refresh slcds:
	for( n = 0; n < devcount; n++)
		if(slcds[n].window) {
			gtk_widget_destroy(slcds[n].window);
			slcd_config(n);
		}

}

static void config_cyclicspeed_callback(GtkAdjustment*adj, gpointer*p)
{
	cyclic_speed = adj->value;
}

void illuminord_config()
{
	int i;
	char b[5];
	GtkWidget *hbox, *vbox, *button, *frame, *label, *spinner;
	GtkAdjustment *adj;

	// prevents segfault when configuring plugin without having it enables
	if( odevcount != devcount )
		devcount_changed();
	
	if( !config_window ) {
		// create window
		config_window = gtk_window_new(GTK_WINDOW_DIALOG);
		gtk_container_set_border_width(GTK_CONTAINER(config_window), 10);
		gtk_window_set_title(GTK_WINDOW(config_window), CONFIG_TITLE);
		gtk_window_set_policy(GTK_WINDOW(config_window), FALSE, FALSE, FALSE);
		gtk_window_set_position(GTK_WINDOW(config_window), GTK_WIN_POS_MOUSE);
		gtk_signal_connect(GTK_OBJECT(config_window), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), &config_window);
			// create a box for our stuff
			config_vbox = gtk_vbox_new(FALSE,0);
				// create devcount spinner hbox
				hbox = gtk_hbox_new(FALSE,0);
					// create label for devcount spinner
					label = gtk_label_new(CONFIG_DEVCOUNT_LABELTEXT);
					gtk_box_pack_start(GTK_BOX(hbox),label, TRUE, TRUE, 0);
					gtk_widget_show(label);
					// create a range widget to enter the number of fnordlichter
					adj = (GtkAdjustment*)gtk_adjustment_new(devcount, 1, MAX_DEVICES, 1, MIN(MAX_DEVICES, 5), 0);
					gtk_signal_connect (GTK_OBJECT (adj), "value_changed", GTK_SIGNAL_FUNC(config_devcount_callback), NULL);
					spinner = gtk_spin_button_new(adj, 1, 0);
					gtk_box_pack_start(GTK_BOX(hbox), spinner, TRUE, TRUE, 0);
					gtk_widget_show(spinner);
				gtk_box_pack_start(GTK_BOX(config_vbox), hbox, TRUE, TRUE, 0);
				gtk_widget_show(hbox);
				// create cyclic_speed spinner with surrounding hbox
				hbox = gtk_hbox_new(FALSE,0);
					// create label for devcount spinner
					label = gtk_label_new(CONFIG_CYCLIC_SPEED_LABELTEXT);
					gtk_box_pack_start(GTK_BOX(hbox),label, TRUE, TRUE, 0);
					gtk_widget_show(label);
					// create a range widget to enter the number of fnordlichter
					adj = (GtkAdjustment*)gtk_adjustment_new(cyclic_speed, 0, 1, 0.0005, 0.005, 0);
					gtk_signal_connect (GTK_OBJECT (adj), "value_changed", GTK_SIGNAL_FUNC(config_cyclicspeed_callback), NULL);
					spinner = gtk_spin_button_new(adj, 0.001, 5);
					gtk_box_pack_start(GTK_BOX(hbox), spinner, TRUE, TRUE, 0);
					gtk_widget_show(spinner);
				gtk_box_pack_start(GTK_BOX(config_vbox), hbox, TRUE, TRUE, 0);
				gtk_widget_show(hbox);
				// create config-sdev hbox{0,1}
				config_sdev_hbox0 = gtk_hbox_new(FALSE,0);
				config_sdev_hbox1 = gtk_hbox_new(FALSE,0);
					// create a button for EACH fnordlicht
					for(i = 0; i < devcount; i++) {
						snprintf(b, sizeof(b), "%d", i + 1);
						slcds[i].button = gtk_button_new_with_label(b);
						gtk_signal_connect(GTK_OBJECT(slcds[i].button), "clicked", GTK_SIGNAL_FUNC(config_single_callback), (void*)i);
						GTK_WIDGET_SET_FLAGS(slcds[i].button, GTK_CAN_DEFAULT);
						if(i >= MAX_DEVICES / 2)
							gtk_box_pack_start(GTK_BOX(config_sdev_hbox1), slcds[i].button, TRUE, TRUE, 0);
						else
							gtk_box_pack_start(GTK_BOX(config_sdev_hbox0), slcds[i].button, TRUE, TRUE, 0);
						gtk_widget_show(slcds[i].button);
					}
				gtk_box_pack_start(GTK_BOX(config_vbox), config_sdev_hbox0, TRUE, TRUE, 0);
				gtk_widget_show(config_sdev_hbox0);
				gtk_box_pack_start(GTK_BOX(config_vbox), config_sdev_hbox1, TRUE, TRUE, 0);
				gtk_widget_show(config_sdev_hbox1);
				// frame around it:
				frame = gtk_frame_new(CONFIG_GLOBAL_FRAMELABEL);
					// vbox for global stuff
					vbox = gtk_vbox_new(FALSE,0);
						// button for setting cyclic color over all simple-paletted lights
						button = gtk_button_new_with_label(CONFIG_GLOBAL_SIMPLE);
						gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(apply_cyclic_color_callback), (gpointer*)0);
						GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
						gtk_box_pack_start(GTK_BOX(vbox),button, TRUE, TRUE, 0);
						gtk_widget_show(button);
						// button for setting cyclic color over all cyclic-paletted lights
						button = gtk_button_new_with_label(CONFIG_GLOBAL_CYCLIC);
						gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(apply_cyclic_color_callback), (gpointer*)1);
						GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
						gtk_box_pack_start(GTK_BOX(vbox),button, TRUE, TRUE, 0);
						gtk_widget_show(button);
						// button for setting logarithmic band boundaries
						button = gtk_button_new_with_label(CONFIG_GLOBAL_LOGARITHMIC_BAND);
						gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(apply_logarithmic_band_callback), NULL);
						GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
						gtk_box_pack_start(GTK_BOX(vbox),button, TRUE, TRUE, 0);
						gtk_widget_show(button);
					gtk_container_add(GTK_CONTAINER(frame), vbox);
					gtk_widget_show(vbox);
				gtk_box_pack_start(GTK_BOX(config_vbox), frame, TRUE, TRUE, 0);
				gtk_widget_show(frame);
				// create config-file frame
				frame = gtk_frame_new(CONFIG_CONFIGFILE_FRAMELABEL);
					// hbox
					hbox = gtk_hbox_new(FALSE,0);
						// create button to save config
						button = gtk_button_new_with_label(CONFIG_SAVETOFILE);
						gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(configfile_callback), (gpointer*)1);
						GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
						gtk_box_pack_start(GTK_BOX(hbox),button, TRUE, TRUE, 0);
						gtk_widget_show(button);				
						// create button to load config
						button = gtk_button_new_with_label(CONFIG_LOADFROMFILE);
						gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(configfile_callback), (gpointer*)0);
						GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
						gtk_box_pack_start(GTK_BOX(hbox),button, TRUE, TRUE, 0);
						gtk_widget_show(button);				
					gtk_container_add(GTK_CONTAINER(frame), hbox);
					gtk_widget_show(hbox);
				gtk_box_pack_start(GTK_BOX(config_vbox), frame, TRUE, TRUE, 0);
				gtk_widget_show(frame);
				// create a button
				config_done = gtk_button_new_with_label(CONFIG_DONETEXT);
				gtk_signal_connect(GTK_OBJECT(config_done), "clicked", GTK_SIGNAL_FUNC(destroy_on_press), config_window);
				GTK_WIDGET_SET_FLAGS(config_done, GTK_CAN_DEFAULT);
				gtk_box_pack_start(GTK_BOX(config_vbox), config_done, TRUE, TRUE, 0);
				gtk_widget_show(config_done);				
			gtk_container_add(GTK_CONTAINER(config_window), config_vbox);
			gtk_widget_show(config_vbox);
		gtk_widget_show(config_window);
		gtk_widget_show(config_window);
	}
}

