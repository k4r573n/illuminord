/*  $Id: palette.c 156 2005-10-06 15:33:06Z lostrace $
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

// for "NULL" :
#include <stdio.h>
#include <math.h>

#include "palette.h"
#include "config.h"

#define PALETTE_C_ID "$Id: palette.c 156 2005-10-06 15:33:06Z lostrace $"
#define PALETTE_ID PALETTE_H_ID "\n" PALETTE_C_ID
char *palette_id = PALETTE_ID;

static void normalize_color(color*c)
{
	double factor;

	factor = 255. / MAX( MAX(c->r, c->g), c->b);

	c->r *= factor;
	c->g *= factor;
	c->b *= factor;
}

// set color c from palmode, basecolor and value
void setcolor(color*c, palmode pm, color *bc, double bv, double val)
{
	double relval;

	// cap if not 0..val..1
	if(val > 1.)
		val = 1.;
	if(val < 0.)
		val = 0.;


	// check for handler
	switch( pm ) {
		case PAL_SPECTRUM:
			relval = val*3;

			if(val < 1./3.) {
				c->r = 255-(255*relval);
				c->g = 0;
				c->b = 255*relval;
			} else
				if(val < 2./3.) {
					c->r = 0;
					c->g = 255*(relval-1);
					c->b = 255-(255*(relval-1));
				} else {
					c->r = 255*(relval-2);
					c->g = 255-(255*(relval-2));
					c->b = 0;
				}
			normalize_color(c);
			break;

		case PAL_STRENGTH:
			relval = val*5;
			if(val < 0.2) { 						// black
				// 1
				c->r = 0;
				c->g = 0;
				c->b = (relval) * 255;
			} else								// blue
				if(val < 0.4) {
					// 2
					c->r = (relval - 1) * 255;
					c->g = 0;
					c->b = 255;
					normalize_color(c);
				} else							// purple
					if(val < 0.6) {
						// 3
						c->r = 255;
						c->g = 0;
						c->b = (relval - 2) * 255;
						normalize_color(c);
					} else						// red
						if(val < 0.8) {
							// 4
							c->r = 255;
							c->g = (relval - 3) * 255;
							c->b = 0;
							normalize_color(c);
						} else {				// yellow
							// 5
							c->r = 255;
							c->g = 255;
							c->b = (relval - 4) * 255;
							normalize_color(c);
						};					// white
			break;

		case PAL_CYCLIC:
			cyclic_value += cyclic_speed;
			while(cyclic_speed > 1)
				cyclic_speed -= 1;
			relval = bv + cyclic_value;
			while(relval > 1)
				relval -= 1;

			setcolor(bc, PAL_SPECTRUM, NULL, 0, relval);

			// fall through
		case PAL_SIMPLE:
			c->r = bc->r * val;
			c->g = bc->g * val;
			c->b = bc->b * val;
			break;

		case PAL_STATIC:
			c->r = bc->r;
			c->g = bc->g;
			c->b = bc->b;
			break;

		default:
			c->r = 0xAA;
			c->g = 0x88;
			c->b = 0x88;
			break;
	}
}

