/*  $Id: slcd.c 156 2005-10-06 15:33:06Z lostrace $
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

#include <gtk/gtk.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "slcd.h"
#include "fnordlicht.h"
#include "config.h"
#include "language.h"

#define SLCD_C_ID "$Id: slcd.c 156 2005-10-06 15:33:06Z lostrace $"
#define SLCD_ID SLCD_H_ID "\n" SLCD_C_ID
char *slcd_id = SLCD_ID;

static void slcd_show_only_needed(int n)
{
	light *l;
	slcd *slcd;

	l = &states[n];
	slcd = &slcds[n];

	if(slcd->window) {
		switch(l->pm) {
			case PAL_CYCLIC:
				gtk_widget_show(slcd->bvframe);
				gtk_widget_hide(slcd->bcframe);
				break;
			case PAL_SIMPLE:
			case PAL_STATIC:
				gtk_widget_hide(slcd->bvframe);
				gtk_widget_show(slcd->bcframe);
				break;
			default:
				gtk_widget_hide(slcd->bvframe);
				gtk_widget_hide(slcd->bcframe);
				break;
		}
		switch(l->ipr) {
			case BAND_ANALYZER:
				gtk_widget_show(slcd->nframe);
				break;
			default:
				gtk_widget_hide(slcd->nframe);
				break;
		}
	}
}

static void radio_ipr_callback(GtkObject*o, gpointer*d)
{
	int n;

	n = (int)d & 127;

	if((int)d & 256)
		states[n].ipr = BAND_ANALYZER;
	else
		if((int)d & 512)
			states[n].ipr = MOVING_AVERAGE;
		else
			if((int)d & 1024)
				states[n].ipr = MOVING_PEAK;
			else
				fprintf(stderr, "radio_ipr_callback(): no useful data.\n");
	// colorpicker is created after this
	if(slcds[n].colorpicker)
		slcd_show_only_needed(n);
	interpreter_changed();
}

static void radio_pm_callback(GtkObject*o, gpointer*d)
{
	int n;

	n = (int)d & 127;

	if((int)d & 256)
		states[n].pm = PAL_STRENGTH;
	else
		if((int)d & 512)
			states[n].pm = PAL_SPECTRUM;
		else
			if((int)d & 1024) 
				states[n].pm = PAL_CYCLIC;
			else
				if((int)d & 2048) 
					states[n].pm = PAL_SIMPLE;
				else
					if((int)d & 4096) 
						states[n].pm = PAL_STATIC;
					else
						fprintf(stderr, "radio_pm_callback(): no useful data.\n");
	// colorpicker is created after this
	if(slcds[n].colorpicker)
		slcd_show_only_needed(n);
}

// fade specified light through a neat palette in some time
void strike_light(int n)
{
	light*l;
	int i;

	l = &(states[n]);

	// fade all new fnordlichter to show that it's cool to have more of them
	// do we really want this?
	for(i = 0; i <= 255; i+=5) {
		setcolor(&l->c, PAL_STRENGTH, NULL, 0, i/(double)255);
		fnordlicht_setrgb(l->i2c_adr,
				l->c.r,
				l->c.g,
				l->c.b);
		usleep(1);
	}
	for(i = 255; i >=0; i-=5) {
		setcolor(&l->c, PAL_STRENGTH, NULL, 0, i/(double)255);
		fnordlicht_setrgb(l->i2c_adr,
				l->c.r,
				l->c.g,
				l->c.b);
		usleep(1);
	}
}

static void testit_callback(GtkObject*o, int n)
{
	strike_light(n);
}

static void slcd_i2c_changed(GtkAdjustment*adj, int n)
{
	states[n].i2c_adr = adj->value;
}

static void slcd_hp_changed(GtkAdjustment*adj, int n)
{
	states[n].hp = adj->value;
}

static void slcd_fs_changed(GtkAdjustment*adj, int n)
{
	states[n].fs = adj->value;
}

static void slcd_bv_changed(GtkAdjustment*adj, int n)
{
	states[n].bv = adj->value;
}

static void slcd_n_low_changed(GtkAdjustment*adj, int n)
{
	states[n].n_ = adj->value;
}

static void slcd_n_high_changed(GtkAdjustment*adj, int n)
{
	states[n].n = adj->value;
}

static void slcd_bc_changed(GtkColorSelection*c, int n)
{
	gdouble colorarray[4];
	
	gtk_color_selection_get_color(c, colorarray);
	states[n].bc.r = (colorarray[0] * 255.);
	states[n].bc.g = (colorarray[1] * 255.);
	states[n].bc.b = (colorarray[2] * 255.);
}

// create + show configure dialog for light n
void slcd_config(int n) 
{
	GtkWidget**win;
	light * l;
	char buf[30];
	GtkWidget *hbox, *vbox;
	GtkWidget *spinner;
	GtkWidget *label;
	GtkAdjustment*adj;
	GtkWidget *radiovbox, *radiohbox, *frame;
	GtkWidget *radio_band, *radio_peak, *radio_average;
	GtkWidget *radio_strength, *radio_spectrum, *radio_simple, *radio_cyclic, *radio_static;
	GtkWidget *done, *testit;

	GtkWidget **colorpicker;
	gdouble colorarray[4];

	win = &(slcds[n].window);
	
	l = &states[n];
	if(!*win) {
		*win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		snprintf(buf, sizeof(buf), SLCD_TITLE, n+1);
		gtk_container_set_border_width(GTK_CONTAINER(*win), 10);
		gtk_window_set_title(GTK_WINDOW(*win), buf);
		gtk_window_set_policy(GTK_WINDOW(*win), FALSE, FALSE, FALSE);
		gtk_window_set_position(GTK_WINDOW(*win), GTK_WIN_POS_MOUSE);
		gtk_signal_connect(GTK_OBJECT(*win), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), win);
			// create global framing vbox
			vbox = gtk_vbox_new(FALSE,0);
				// framing hbox for label, i2c-spinner and test-button
				hbox = gtk_hbox_new(FALSE,0);
					// i2c label:
					label = gtk_label_new(SLCD_I2C_LABELTEXT);
					gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
					gtk_widget_show(label);
					// spinbutton for i2c_adr: i2c-address
					adj = (GtkAdjustment*)gtk_adjustment_new(l->i2c_adr, 1, 127, 1, 1, 1);
					gtk_signal_connect (GTK_OBJECT (adj), "value_changed", GTK_SIGNAL_FUNC(slcd_i2c_changed), (gpointer*)n);
					spinner = gtk_spin_button_new(adj, 1, 0);
					gtk_box_pack_start(GTK_BOX(hbox), spinner, TRUE, TRUE, 0);
					gtk_widget_show(spinner);
					// test button
					testit = gtk_button_new_with_label(SLCD_TEST_LIGHT);
					gtk_signal_connect(GTK_OBJECT(testit), "clicked", GTK_SIGNAL_FUNC(testit_callback), (gpointer*)n);
					GTK_WIDGET_SET_FLAGS(testit, GTK_CAN_DEFAULT);
					gtk_box_pack_start(GTK_BOX(hbox), testit, TRUE, TRUE, 0);
					gtk_widget_show(testit);
				gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
				gtk_widget_show(hbox);
				// framing hbox for label and hp-spinner
				hbox = gtk_hbox_new(FALSE,0);
					// hp label:
					label = gtk_label_new(SLCD_HIGHPASS_LABELTEXT);
					gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
					gtk_widget_show(label);
					// spinbutton for hp:
					adj = (GtkAdjustment*)gtk_adjustment_new(l->hp, 0., 1., 0.05, 0.1, 0);
					gtk_signal_connect (GTK_OBJECT (adj), "value_changed", GTK_SIGNAL_FUNC(slcd_hp_changed), (gpointer*)n);
					spinner = gtk_spin_button_new(adj, 0.05, 5);
					gtk_box_pack_start(GTK_BOX(hbox), spinner, TRUE, TRUE, 0);
					gtk_widget_show(spinner);
				gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
				gtk_widget_show(hbox);
				// framing hbox for label and fs-spinner
				hbox = gtk_hbox_new(FALSE,0);
					// hp label:
					label = gtk_label_new(SLCD_FADESPEED_LABELTEXT);
					gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
					gtk_widget_show(label);
					// spinbutton for hp:
					adj = (GtkAdjustment*)gtk_adjustment_new(l->fs, 0., 1., 0.005, 0.05, 0);
					gtk_signal_connect (GTK_OBJECT (adj), "value_changed", GTK_SIGNAL_FUNC(slcd_fs_changed), (gpointer*)n);
					spinner = gtk_spin_button_new(adj, 0.005, 5);
					gtk_box_pack_start(GTK_BOX(hbox), spinner, TRUE, TRUE, 0);
					gtk_widget_show(spinner);
				gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
				gtk_widget_show(hbox);
				// create radio-framing hbox
				radiohbox = gtk_hbox_new(FALSE,0);
					// create frame for interpreter
					frame = gtk_frame_new( SLCD_IPR );
						// create framing vbox for interpreter
						radiovbox = gtk_vbox_new(FALSE,0);
							// create radio-buttons for interpreter: band analyzer
							radio_band    = gtk_radio_button_new_with_label(NULL, SLCD_IPR_BAND_ANALYZER);
							gtk_signal_connect(GTK_OBJECT(radio_band), "toggled", GTK_SIGNAL_FUNC(radio_ipr_callback), (gpointer*)(n | 256));
							gtk_box_pack_start(GTK_BOX(radiovbox), radio_band, TRUE, TRUE, 0);
							gtk_widget_show(radio_band);
							// moving average
							radio_average = gtk_radio_button_new_with_label(gtk_radio_button_group(GTK_RADIO_BUTTON(radio_band)), SLCD_IPR_MOVING_AVERAGE);
							gtk_signal_connect(GTK_OBJECT(radio_average), "toggled", GTK_SIGNAL_FUNC(radio_ipr_callback), (gpointer*)(n | 512));
							gtk_box_pack_start(GTK_BOX(radiovbox), radio_average, TRUE, TRUE, 0);
							gtk_widget_show(radio_average);
							// moving peak
							radio_peak    = gtk_radio_button_new_with_label(gtk_radio_button_group(GTK_RADIO_BUTTON(radio_band)), SLCD_IPR_MOVING_PEAK);
							gtk_signal_connect(GTK_OBJECT(radio_peak), "toggled", GTK_SIGNAL_FUNC(radio_ipr_callback), (gpointer*)(n | 1024));
							gtk_box_pack_start(GTK_BOX(radiovbox), radio_peak, TRUE, TRUE, 0);
							gtk_widget_show(radio_peak);
							
							switch(l->ipr) {
								case BAND_ANALYZER:
									gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_band), TRUE);
									break;
								case MOVING_PEAK:
									gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_peak), TRUE);
									break;
								case MOVING_AVERAGE:
									gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_average), TRUE);
									break;
							}
						gtk_container_add(GTK_CONTAINER(frame), radiovbox);
						gtk_widget_show(radiovbox);
					gtk_box_pack_start(GTK_BOX(radiohbox), frame, TRUE, TRUE, 0);
					gtk_widget_show(frame);
					// create frame for palmode
					frame = gtk_frame_new( SLCD_PM );
						// create framing vbox for interpreter
						radiovbox = gtk_vbox_new(FALSE,0);
							// create radio-buttons for palmodes: strength palette
							radio_strength = gtk_radio_button_new_with_label(NULL, SLCD_PM_STRENGTH);
							gtk_signal_connect(GTK_OBJECT(radio_strength), "toggled", GTK_SIGNAL_FUNC(radio_pm_callback), (gpointer*)(n | 256));
							gtk_box_pack_start(GTK_BOX(radiovbox), radio_strength, TRUE, TRUE, 0);
							gtk_widget_show(radio_strength);
							// spectral palette
							radio_spectrum = gtk_radio_button_new_with_label(gtk_radio_button_group(GTK_RADIO_BUTTON(radio_strength)), SLCD_PM_SPECTRUM);
							gtk_signal_connect(GTK_OBJECT(radio_spectrum), "toggled", GTK_SIGNAL_FUNC(radio_pm_callback), (gpointer*)(n | 512));
							gtk_box_pack_start(GTK_BOX(radiovbox), radio_spectrum, TRUE, TRUE, 0);
							gtk_widget_show(radio_spectrum);
							// cyclic simple palette
							radio_cyclic   = gtk_radio_button_new_with_label(gtk_radio_button_group(GTK_RADIO_BUTTON(radio_strength)), SLCD_PM_CYCLIC);
							gtk_signal_connect(GTK_OBJECT(radio_cyclic), "toggled", GTK_SIGNAL_FUNC(radio_pm_callback), (gpointer*)(n | 1024));
							gtk_box_pack_start(GTK_BOX(radiovbox), radio_cyclic, TRUE, TRUE, 0);
							gtk_widget_show(radio_cyclic);
							// simple palette
							radio_simple   = gtk_radio_button_new_with_label(gtk_radio_button_group(GTK_RADIO_BUTTON(radio_strength)), SLCD_PM_SIMPLE);
							gtk_signal_connect(GTK_OBJECT(radio_simple), "toggled", GTK_SIGNAL_FUNC(radio_pm_callback), (gpointer*)(n | 2048));
							gtk_box_pack_start(GTK_BOX(radiovbox), radio_simple, TRUE, TRUE, 0);
							gtk_widget_show(radio_simple);
							// static color
							radio_static   = gtk_radio_button_new_with_label(gtk_radio_button_group(GTK_RADIO_BUTTON(radio_strength)), SLCD_PM_STATIC);
							gtk_signal_connect(GTK_OBJECT(radio_static), "toggled", GTK_SIGNAL_FUNC(radio_pm_callback), (gpointer*)(n | 4096));
							gtk_box_pack_start(GTK_BOX(radiovbox), radio_static, TRUE, TRUE, 0);
							gtk_widget_show(radio_static);
							switch(l->pm) {
								case PAL_STRENGTH:
									gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_strength), TRUE);
									break;
								case PAL_SPECTRUM:
									gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_spectrum), TRUE);
									break;
								case PAL_CYCLIC:
									gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_cyclic), TRUE);
									break;
								case PAL_SIMPLE:
									gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_simple), TRUE);
									break;
								case PAL_STATIC:
									gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_static), TRUE);
									break;
							}
						gtk_container_add(GTK_CONTAINER(frame), radiovbox);
						gtk_widget_show(radiovbox);
					gtk_box_pack_start(GTK_BOX(radiohbox), frame, TRUE, TRUE, 0);
					gtk_widget_show(frame);
					// SLCD_PM
				gtk_box_pack_start(GTK_BOX(vbox), radiohbox, TRUE, TRUE, 0);
				gtk_widget_show(radiohbox);
				// frame and hbox for label and n_,n spinner
				slcds[n].nframe = frame = gtk_frame_new(SLCD_NFRAME_TEXT);
					hbox = gtk_hbox_new(FALSE,0);
						// n_ label:
						label = gtk_label_new(SLCD_N_LOW_LABELTEXT);
						gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
						gtk_widget_show(label);
						// spinbutton for n_:
						adj = (GtkAdjustment*)gtk_adjustment_new(l->n_, 0, 255, 1, 5, 0);
						gtk_signal_connect (GTK_OBJECT (adj), "value_changed", GTK_SIGNAL_FUNC(slcd_n_low_changed), (gpointer*)n);
						spinner = gtk_spin_button_new(adj, 1, 0);
						gtk_box_pack_start(GTK_BOX(hbox), spinner, TRUE, TRUE, 0);
						gtk_widget_show(spinner);
						// n label:
						label = gtk_label_new(SLCD_N_HIGH_LABELTEXT);
						gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
						gtk_widget_show(label);
						// spinbutton for n:
						adj = (GtkAdjustment*)gtk_adjustment_new(l->n, 0, 255, 1, 5, 0);
						gtk_signal_connect (GTK_OBJECT (adj), "value_changed", GTK_SIGNAL_FUNC(slcd_n_high_changed), (gpointer*)n);
						spinner = gtk_spin_button_new(adj, 1, 0);
						gtk_box_pack_start(GTK_BOX(hbox), spinner, TRUE, TRUE, 0);
						gtk_widget_show(spinner);
					gtk_container_add(GTK_CONTAINER(frame), hbox);
					gtk_widget_show(hbox);
				gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
				gtk_widget_show(frame);
				// frame and hbox for label and bv-spinner
				slcds[n].bvframe = frame = gtk_frame_new(SLCD_BVFRAME_TEXT);
					hbox = gtk_hbox_new(FALSE,0);
						// hp label:
						label = gtk_label_new(SLCD_BASICVAL_LABELTEXT);
						gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
						gtk_widget_show(label);
						// spinbutton for hp:
						adj = (GtkAdjustment*)gtk_adjustment_new(l->bv, 0., 1., 0.05, 0.5, 0);
						gtk_signal_connect (GTK_OBJECT (adj), "value_changed", GTK_SIGNAL_FUNC(slcd_bv_changed), (gpointer*)n);
						spinner = gtk_spin_button_new(adj, 0.005, 5);
						gtk_box_pack_start(GTK_BOX(hbox), spinner, TRUE, TRUE, 0);
						gtk_widget_show(spinner);
					gtk_container_add(GTK_CONTAINER(frame), hbox);
					gtk_widget_show(hbox);
				gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
				gtk_widget_show(frame);
				// framed colorpicker for bc : base color for simple and static palette
				colorpicker = &slcds[n].colorpicker;
				slcds[n].bcframe = frame = gtk_frame_new( SLCD_BCFRAME_TEXT );
					*colorpicker = gtk_color_selection_new();
					colorarray[0] = ((double)l->bc.r) / 255.;
					colorarray[1] = ((double)l->bc.g) / 255.;
					colorarray[2] = ((double)l->bc.b) / 255.;
					colorarray[3] = 1.;
					gtk_color_selection_set_color(GTK_COLOR_SELECTION(*colorpicker), colorarray);
					gtk_signal_connect (GTK_OBJECT (*colorpicker), "color_changed", GTK_SIGNAL_FUNC(slcd_bc_changed), (gpointer*)n);
					gtk_signal_connect(GTK_OBJECT(*colorpicker), "destroy", GTK_SIGNAL_FUNC(gtk_widget_destroyed), colorpicker);
					gtk_color_selection_set_update_policy(GTK_COLOR_SELECTION(*colorpicker), GTK_UPDATE_CONTINUOUS);
					gtk_container_add(GTK_CONTAINER(frame), *colorpicker);
					gtk_widget_show(*colorpicker);
				gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
				gtk_widget_show(frame);
				// create button to escape from the dialog
				done = gtk_button_new_with_label(CONFIG_DONETEXT);
				gtk_signal_connect(GTK_OBJECT(done), "clicked", GTK_SIGNAL_FUNC(destroy_on_press), *win);
				GTK_WIDGET_SET_FLAGS(done, GTK_CAN_DEFAULT);
				gtk_box_pack_start(GTK_BOX(vbox), done, TRUE, TRUE, 0);
				gtk_widget_show(done);
			gtk_container_add(GTK_CONTAINER(*win), vbox);
			gtk_widget_show(vbox);
		slcd_show_only_needed(n);
		gtk_widget_show(*win);
	} else
		return;
}

