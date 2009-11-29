/*  $Id: slcd.h 156 2005-10-06 15:33:06Z lostrace $
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

#ifndef __SLCD_H__
#define __SLCD_H__
#define SLCD_H_ID "$Id: slcd.h 156 2005-10-06 15:33:06Z lostrace $"
extern char*slcd_id;

#include <gtk/gtk.h>
#include <stdint.h>

#include "palette.h"

// single light config dialog
typedef struct slcd_t {
	GtkWidget *button;	// button in main config-dialog

	GtkWidget *window;	// window for this configuration dialog (NULL if none exists)
	GtkWidget *colorpicker;	// colorpicker info
	GtkWidget *bcframe;	// frame with colorpicker
	GtkWidget *nframe;	// frame with band boundaries
	GtkWidget *bvframe;	// frame with basic value
} slcd;

// interpreter for a light
typedef enum ipr_t {
	BAND_ANALYZER,
	MOVING_PEAK,
	MOVING_AVERAGE
} ipr;

// status and configuration of a light
typedef struct light_t {
	// status-stuff:
	color c;		// color of this light, right now or to be set next time
	double v;		// value 0..1 to send through setfpal
	
	// config-stuff:
	uint8_t i2c_adr;	// i2c address of this light

	double hp;		// light-specific highpass filter
	double fs;		// light-specifig fade-speed
	ipr ipr;		// interpreter for this light
	palmode pm;		// palette mode to use for this light
	double bv;		// basic value for cyclic palettes
	color bc;		// base color for some palmodes
	int n_;			// if BAND_ANALYZER, band-start
	int n;			//		 and band-end
} light;

// create + show configure dialog for light l
void slcd_config(int l);
// fade specified light through a neat palette in some time
void strike_light(int n);
	
#endif // __SLCD_H__

