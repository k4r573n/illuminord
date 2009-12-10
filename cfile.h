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

#ifndef __CFILE_H__
#define __CFILE_H__
#define CFILE_H_ID "$Id: cfile.h 156 2005-10-06 15:33:06Z lostrace $"
extern char*cfile_id;

#define CONFIG_FILE ".illuminord.conf"

extern void configfile_callback(GtkObject*o, int d);

extern void load_from_configfile(char *config_file);
extern void save_to_configfile(char *config_file);

#endif // __CFILE_H__

