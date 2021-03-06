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

//hinzugefügt
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "fnordlicht.h"
#include "libfnordlicht.h"

#define FNORDLICHT_C_ID "$Id: fnordlicht.c 156 2005-10-06 15:33:06Z lostrace $"
#define FNORDLICHT_ID FNORDLICHT_H_ID "\n" FNORDLICHT_C_ID
char *fnordlicht_id = FNORDLICHT_ID;

int fnordlicht_fd = -1;
static FILE *debug_file;
static struct tm *zeit;

// open fnordlicht device
// if device == NULL, it defaults to /dev/fnordlicht
// returns 0 if failed.
int fnordlicht_open(char* device)
{//stark angepasst
	static char *defaultdevice = FN_DEFAULTDEVICE;

	if(fnordlicht_fd < 0) {
		if(!device)
			device = defaultdevice;

#ifndef NOLIGHTS
		fnordlicht_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fnordlicht_fd > 0) {
		  struct termios options;

		  /* Get the current options for the port... */
		  tcgetattr(fnordlicht_fd, &options);

		  /* Set the baud rates to 19200... */
		  cfsetispeed(&options, B19200);
		  cfsetospeed(&options, B19200);

		  /* Enable the receiver and set local mode... */
		  options.c_cflag |= (CLOCAL | CREAD);

		  options.c_cflag &= ~CSIZE; /* Mask the character size bits */
		  options.c_cflag |= CS8;    /* Select 8 data bits */
		  options.c_cflag &= ~PARENB;
		  options.c_cflag &= ~CSTOPB;

		  /* Choosing Raw Input */
		  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

		  /* Set the new options for the port... */
		  tcsetattr(fnordlicht_fd, TCSANOW, &options);

      //sync all devices
      fnord_sync();
      //broadcast stop fading
      stop(255,255);
    }else{
      	perror("open_port: Unable to open the Fnordlicht ");
    }

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

  //send data
   fade_rgb(address, 255, 0, rval, gval, bval);

#endif
}

// release fnordlicht device
int fnordlicht_close()
{
  if(fnordlicht_fd >= 0) {
#ifndef NOLIGHTS
    //maybe send package to start fading

		close(fnordlicht_fd);
#endif
		fnordlicht_fd = -1;
 		return 1;
	} else
		return 0;
}

