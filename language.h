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

#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__
#define LANGUAGE_H_ID "$Id: language.h 156 2005-10-06 15:33:06Z lostrace $"
extern char*language_id;

# ifndef NO_DEFAULT_LANGUAGE
#  define LANGUAGE_GERMAN
//#  define LANGUAGE_ENGLISH
# endif

// ==================================== ENGLISH

# ifdef LANGUAGE_ENGLISH
#  define ABOUT_TITLE			"illuminord about"
#  define ABOUT_LABEL			"\nCopyright (c) 2005\n"							\
					"losTrace aka \"David R. Piegdon\" and cebewee aka \"Lars Noschinski\"\n"	\
					"further edits in 2009 by Karsten Hinz\n"					\
					"(see history http://github.com/k4r573n/illuminord)\n"				\
					"\n"										\
					"Sourcecode and program licensed under GPL v2.\n"				\
					"You should have received a copy of the GNU General Public License\n"		\
					"along with this program; if not, write to the Free Software\n"			\
					"Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA\n"	\
					"\n"										\
					"for more info see http://github.com/k4r573n/illuminord"			\
					"please send requests and bugreports to:\n"					\
					"mailto:k4r573n@gmail.com\n"
#  define ABOUT_VERSION_FRAMELABEL	"version:"
#  define ABOUT_OKTEXT			"Ok"

#  define CONFIG_TITLE			"configure illuminord"
#  define CONFIG_DONETEXT		"Done"
#  define CONFIG_DEVCOUNT_LABELTEXT	"number of fnordlichter:"

#  define CONFIG_CYCLIC_SPEED_LABELTEXT "cyclic palette speed"

#  define CONFIG_GLOBAL_FRAMELABEL	"global"
#  define CONFIG_GLOBAL_SIMPLE		"cyclic color for all lights with simple palette"
#  define CONFIG_GLOBAL_CYCLIC		"cyclic color for all lights with cyclic palette"
#  define CONFIG_GLOBAL_LOGARITHMIC_BAND "logarithmic boundaries for all static bands"

#  define CONFIG_CONFIGFILE_FRAMELABEL	"configfile"
#  define CONFIG_SAVETOFILE		"save"
#  define CONFIG_LOADFROMFILE		"load"

#  define SLCD_TITLE			"configure fordlicht #%d"

#  define SLCD_I2C_LABELTEXT		"light address:"
#  define SLCD_TEST_LIGHT		"test"
#  define SLCD_FADESPEED_LABELTEXT	"fadespeed (reversed):"
#  define SLCD_HIGHPASS_LABELTEXT	"highpass:"
#  define SLCD_BASICVAL_LABELTEXT	"offset:"
#  define SLCD_N_LOW_LABELTEXT		"start before:"
#  define SLCD_N_HIGH_LABELTEXT		"end after:"
	
#  define SLCD_NFRAME_TEXT		"band"
#  define SLCD_BVFRAME_TEXT		"cyclic palette"
#  define SLCD_BCFRAME_TEXT		"base color (for real black/white set hue=-1 and val=0/1)"
	
#  define SLCD_IPR			"interpreter"
#  define SLCD_IPR_BAND_ANALYZER	"static band"
#  define SLCD_IPR_MOVING_PEAK		"peak frq"
#  define SLCD_IPR_MOVING_AVERAGE	"average frq"

#  define SLCD_PM			"palette mode"
#  define SLCD_PM_STRENGTH		"strength"
#  define SLCD_PM_SPECTRUM		"spectral"
#  define SLCD_PM_SIMPLE		"simple"
#  define SLCD_PM_CYCLIC		"cyclic rotating compound"
#  define SLCD_PM_STATIC		"static"
# endif

// ==================================== GERMAN / DEUTSCH

# ifdef LANGUAGE_GERMAN
#  define ABOUT_TITLE			"illuminord about"
#  define ABOUT_LABEL			"\nCopyright (c) 2005\n"								\
					"losTrace aka \"David R. Piegdon\" und cebewee aka \"Lars Noschinski\"\n"	\
					"\n"										\
					"Quelltext und Programm lizensiert unter der GPL\n"				\
					"\n"										\
					"fuer mehr Info siehe http://0null.net/software/fnordlicht/\n"			\
					"Anfragen und Bugreports bitte senden an:\n"					\
					"mailto:k4r573n@gmail.com\n"
#  define ABOUT_VERSION_FRAMELABEL	"Version:"
#  define ABOUT_OKTEXT			"Ok"

#  define CONFIG_TITLE			"illuminord konfigurieren"
#  define CONFIG_DONETEXT		"Fertig"
#  define CONFIG_DEVCOUNT_LABELTEXT	"Anzahl der Fnordlichter:"

#  define CONFIG_CYCLIC_SPEED_LABELTEXT "Geschwindigkeit der zyklischen Palette"

#  define CONFIG_GLOBAL_FRAMELABEL	"Global"
#  define CONFIG_GLOBAL_SIMPLE		"zyklische Farbenverteilung ueber Lichter mit einfacher Palette"
#  define CONFIG_GLOBAL_CYCLIC		"zyklische Farbenverteilung ueber Lichter mit zyklischer Palette"
#  define CONFIG_GLOBAL_LOGARITHMIC_BAND "logarithmische Grenzen fuer alle statischen Baender"

#  define CONFIG_CONFIGFILE_FRAMELABEL	"Konfigurationsdatei"
#  define CONFIG_SAVETOFILE		"speichern"
#  define CONFIG_LOADFROMFILE		"laden"

#  define SLCD_TITLE			"Fordlicht #%d konfirurieren"

#  define SLCD_I2C_LABELTEXT		"Licht Adresse:"
#  define SLCD_TEST_LIGHT		"Test"
#  define SLCD_FADESPEED_LABELTEXT	"Fade-Geschwindigkeit (umgedreht):"
#  define SLCD_HIGHPASS_LABELTEXT	"Highpass:"
#  define SLCD_BASICVAL_LABELTEXT	"Offset:"
#  define SLCD_N_LOW_LABELTEXT		"Start vor:"
#  define SLCD_N_HIGH_LABELTEXT		"Ende nach:"
	
#  define SLCD_NFRAME_TEXT		"Band"
#  define SLCD_BVFRAME_TEXT		"zyklische Palette"
#  define SLCD_BCFRAME_TEXT		"Basis Farbe (fuer exaktes Schwarz/Weiss hue=-1 und val=0/1)"
	
#  define SLCD_IPR			"Interpreter"
#  define SLCD_IPR_BAND_ANALYZER	"statisches Band"
#  define SLCD_IPR_MOVING_PEAK		"maximale Frequenz"
#  define SLCD_IPR_MOVING_AVERAGE	"Durchschnittsfrequenz"

#  define SLCD_PM			"Palettenmodus"
#  define SLCD_PM_STRENGTH		"staerke"
#  define SLCD_PM_SPECTRUM		"spektral"
#  define SLCD_PM_SIMPLE		"einfach"
#  define SLCD_PM_CYCLIC		"zyklisch rotierender Verbund"
#  define SLCD_PM_STATIC		"statisch"
# endif

#endif // __LANGUAGE_H__
