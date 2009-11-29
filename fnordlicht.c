/*  $Id: fnordlicht.c 156 2005-10-06 15:33:06Z lostrace $
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

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "fnordlicht.h"

#define FNORDLICHT_C_ID "$Id: fnordlicht.c 156 2005-10-06 15:33:06Z lostrace $"
#define FNORDLICHT_ID FNORDLICHT_H_ID "\n" FNORDLICHT_C_ID
char *fnordlicht_id = FNORDLICHT_ID;

static int fnordlicht_fd = -1;

// open fnordlicht device
// if device == NULL, it defaults to /dev/fnordlicht
// returns 0 if failed.
int fnordlicht_open(char* device)
{
	static char *defaultdevice = FN_DEFAULTDEVICE;

	if(fnordlicht_fd < 0) {
		if(!device)
			device = defaultdevice;
			
#ifndef NOLIGHTS	
		fnordlicht_fd = open(device, O_CREAT | O_APPEND | O_RDWR /* | O_DIRECT */ , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#else
		fnordlicht_fd = 1;
#endif

		return (fnordlicht_fd < 0) ? 0 : 1;
	} else {
		// we already did this...
		fprintf(stderr, "multiple calls to fnordlicht_open... your app is broken.\n"); fflush(stdout);
		return 0;
	}
}

// set RGB values now.
void fnordlicht_setrgb(uint8_t address, uint8_t rval, uint8_t gval, uint8_t bval)
{
#ifndef NOLIGHTS	
	static char cbuf[6] = { 5, 0, 0x20, 0,0,0 };
	static char res;
	int i;
	
	cbuf[1] = address << 1;
	cbuf[3+FN_RED] = rval;
	cbuf[3+FN_GREEN] = gval;
	cbuf[3+FN_BLUE] = bval;

	if ( 6 != write(fnordlicht_fd, cbuf, 6) )
		fprintf(stderr, "illuminord-xmms :: failed write at %d!\n", (int)time(NULL));
	fsync(fnordlicht_fd);
	i = read(fnordlicht_fd, &res, 1);
	if(res!='F')
		fprintf(stderr, "did not get OK from fnordlicht. please check it.\n");
#endif
}

// release fnordlicht device
int fnordlicht_close()
{
	if(fnordlicht_fd >= 0) {
#ifndef NOLIGHTS	
		close(fnordlicht_fd);
#endif
		fnordlicht_fd = -1;
		return 1;
	} else
		return 0;
}

