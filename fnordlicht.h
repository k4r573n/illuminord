/*
 *  illuminord v0.4
 *  
 *  Copyright (C) 2005
 *  losTrace aka "David R. Piegdon" and cebewee aka "Lars Noschinski"
 *
 *  further edits in 2009 by Karsten Hinz (see history http://github.com/k4r573n/illuminord)
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

#ifndef __FNORDLICHT_H__
#define __FNORDLICHT_H__
#define FNORDLICHT_H_ID "$Id: fnordlicht.h 156 2005-10-06 15:33:06Z lostrace $"
extern char*fnordlicht_id;

#include <stdint.h>

#define FN_RED 0
#define FN_GREEN 1
#define FN_BLUE 2

#define FN_BROADCAST 0
//#define FN_DEFAULTDEVICE "/dev/fnordlicht"
#define FN_DEFAULTDEVICE "/dev/ttyUSB0"

// open fnordlicht device
// if device == NULL, it defaults to /dev/fnordlicht
// returns 0 if failed.
int fnordlicht_open(char* device);
// set RGB values now.
void fnordlicht_setrgb(uint8_t address, uint8_t rval, uint8_t gval, uint8_t bval);
// release fnordlicht device
int fnordlicht_close();

#endif

