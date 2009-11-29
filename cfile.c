/*  $Id: cfile.c 156 2005-10-06 15:33:06Z lostrace $
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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>

#include "config.h"
#include "cfile.h"

#define CFILE_C_ID "$Id: cfile.c 156 2005-10-06 15:33:06Z lostrace $"
#define CFILE_ID CFILE_H_ID "\n" CFILE_C_ID
char *cfile_id = CFILE_ID;

char *config_file = NULL;
static GtkWidget*win = NULL;

void save_to_configfile(char *config_file)
{
	int fd;
	size_t s;

	// open file
	fd = creat(config_file, S_IRUSR | S_IWUSR);

	if(fd < 0)
		fprintf(stderr, "failed to save to config file \"%s\": %s\n",config_file, strerror(errno));
	else {
		// save sizeof(light), so we know if interface changed in between
		s = sizeof(light);
		write(fd, &s, sizeof(s));

		// save device count
		write(fd, &devcount, sizeof(devcount));
		
		// save cyclic speed
		write(fd, &cyclic_speed, sizeof(cyclic_speed));

		// then save light-dependent configuration
		write(fd, states, sizeof(light)*devcount);

		close(fd);
	}
}

static void save_to_configfile_callback(GtkWidget *w, GtkFileSelection *fs)
{
	if(config_file)
		free(config_file);
	config_file = strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
	gtk_widget_destroy(win);

	save_to_configfile(config_file);

	illuminord_config();
}

void load_from_configfile(char *config_file)
{
	int fd;
	size_t s;

	fd = open(config_file, O_RDWR);

	if(fd < 0)
		fprintf(stderr, "failed to read from config file \"%s\": %s\n", config_file, strerror(errno));
	else {
		read(fd, &s, sizeof(s));
		if(s != sizeof(light))
			fprintf(stderr, "interface changed (%d:%d). this config file is lost. please delete it.\n", (int)s, (int)sizeof(light));
		else {
			// read devcount
			read(fd, &devcount, sizeof(devcount));
			devcount_changed();
			// read cyclic speed
			read(fd, &cyclic_speed, sizeof(cyclic_speed));
			// and read device configurations
			read(fd, states, sizeof(light)*devcount);
			interpreter_changed();
		}
		close(fd);
	}
}

static void load_from_configfile_callback(GtkWidget *w, GtkFileSelection *fs)
{
	if(config_file)
		free(config_file);
	config_file = strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
	gtk_widget_destroy(win);

	load_from_configfile(config_file);

	illuminord_config();
}

void configfile_callback(GtkObject*o, int d)
{
	int i;
	
	if(!win) {
		// destroy config-dialog
		gtk_widget_destroy(config_window);
		// destroy all light-specific config dialogues
		for( i = 0; i < devcount; i++)
			if(slcds[i].window)
				gtk_widget_destroy(slcds[i].window);
		win = gtk_file_selection_new (d ? ("Save config to file") : ("Load config from file"));
		gtk_signal_connect(GTK_OBJECT(win), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), &win);
		gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION (win)->ok_button), "clicked",
					d ? ((GtkSignalFunc)save_to_configfile_callback) : ((GtkSignalFunc)load_from_configfile_callback), win);
		gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION (win)->cancel_button), "clicked", (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT (win));
		gtk_file_selection_set_filename (GTK_FILE_SELECTION(win), config_file ? config_file : CONFIG_FILE);
		gtk_widget_show(win);
	}
}

