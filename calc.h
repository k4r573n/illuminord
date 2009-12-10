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

#ifndef __CALC_H__
#define __CALC_H__
#define CALC_H_ID "$Id: calc.h 156 2005-10-06 15:33:06Z lostrace $"
extern char*calc_id;

// will scale 0.0 .. 1.0 logarithmic to 0..255
extern double scale(double d);

// will reverse scaling, thus do logarithmic scaling from 0..255 to 0.0 .. 1.0
extern double unscale(double d);

// modified version of binary search, will find the match in an array, that has the
// least delta to val
extern int quicksearch(double ar[], int mini, int maxi, double val);

#endif

