/*  $Id: config.h 156 2005-10-06 15:33:06Z lostrace $
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

#ifndef __CONFIG_H__
#define __CONFIG_H__
#define CONFIG_H_ID "$Id: config.h 156 2005-10-06 15:33:06Z lostrace $"
extern char*config_id;

#include <gtk/gtk.h>
#include <stdint.h>

#include "palette.h"
#include "slcd.h"

// may be no larger than 127
#define MAX_DEVICES 14
// number of devices at the beginning
#define DEFAULT_DEVCOUNT 1
// standard config file

extern int 	devcount;		// number of addressable fnordlichter

extern light	*states;		// state table of all relevant fnordlichter
extern slcd	slcds[MAX_DEVICES];	// static table for all possible config dialogues

extern int	num_bandanalyzer;	// number of "band-analyzer" lights
extern int	num_movingpeak;		// number of "moving-peak" lights
extern int	num_movingaverage;	// number of "moving-average" lights

extern double	cyclic_speed;		// speed for the cyclic palette
extern double	cyclic_value;		// counter for cycler


extern GtkWidget *config_window;

extern void destroy_on_press(GtkObject*o, gpointer*d);

extern void interpreter_changed();

extern void devcount_changed();

extern void illuminord_about();

extern void illuminord_config();

#endif

