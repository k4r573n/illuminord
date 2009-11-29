/*  $Id: palette.h 156 2005-10-06 15:33:06Z lostrace $
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

#ifndef __PALETTE_H__
#define __PALETTE_H__
#define PALETTE_H_ID "$Id: palette.h 156 2005-10-06 15:33:06Z lostrace $"
extern char*palette_id;

#include <stdint.h>

typedef struct color_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color;

typedef enum palmode_t {
	PAL_STRENGTH,
	PAL_SPECTRUM,
	PAL_SIMPLE,
	PAL_CYCLIC,
	PAL_STATIC
} palmode;

// set color c from palmode, basecolor and value
void setcolor(color*c, palmode pm, color *bc, double bv, double val);

#endif

