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

#include <math.h>
#ifdef DEBUG
# include <stdio.h>
#endif

#include "config.h"
#include "calc.h"

#define CALC_C_ID "$Id: calc.c 156 2005-10-06 15:33:06Z lostrace $"
#define CALC_ID CALC_H_ID "\n" CALC_C_ID
char *calc_id = CALC_ID;

// will scale 0.0 .. 1.0 logarithmic to 0..255
double scale(double d)
{
	return pow(10,d/0.41524) - 1;
}

// will reverse scaling, thus do logarithmic scaling from 0..255 to 0.0 .. 1.0
double unscale(double d)
{
	return log(d+1)/log(10) * 0.41524;
}

// modified version of binary search, will find the match in a sorted array, that has the
// least delta to val
int quicksearch(double ar[], int mini, int maxi, double val)
{
	int half;
	
	half = (maxi + mini)/2;

	if(ar[half] == val)
		return half;
	else
		if(maxi-mini > 1)
			return (ar[half] > val) ? quicksearch(ar,mini,half-1,val) : quicksearch(ar,half+1,maxi,val);
		else
			if(mini == maxi)
				return mini;
			else
				// mini == maxi-1
				// this solves an infinite-recursion problem due to integer-rounding
				return (ar[mini] - val < ar[maxi] - val) ? mini : maxi;
}

