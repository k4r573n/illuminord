 # $Id: Makefile 154 2005-10-06 01:08:44Z lostrace $
 #  illuminordxmms v0.3
 #  
 #  Copyleft (c) 2005 by
 #  losTrace aka "David R. Piegdon" and cebewee aka "Lars Noschinski"
 #  
 #  further edits by Karsten Hinz

.PHONY	: all debug nolights Linux Darwin clean install unintall

OBJECTS = $(patsubst %.c,%.o,$(shell echo *.c))
HEADERS = $(shell echo *.h)

CFLAGS += -O2 -fPIC -finline-functions  -ffast-math  -funroll-all-loops -Wall -Wpointer-arith -D_REENTRANT

all:	$(shell uname)
	-

english: all
	-

german: CFLAGS+= -DNO_DEFAULT_LANGUAGE -DLANGUAGE_GERMAN
german: all
	-

debug:	CFLAGS+= -DDEBUG -g
debug:	LDFLAGS+= -g
debug:	all
	-

nolights:CFLAGS+= -DNOLIGHTS
nolights:debug
	-

Linux:	CFLAGS+= -I/usr/include/libmowgli -I/usr/include/atk-1.0 -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/audacious -I/usr/lib/glib-2.0/include -I/usr/include/glib-2.0 -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include
Linux:	LDFLAGS+= -L/usr/lib
Linux:	LDFLAGS+= -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lgio-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lglib-2.0
Linux:	LDFLAGS+= -pthread -L/lib -laudclient -lgthread-2.0 -lgtk-x11-2.0 -lmcs -ldbus-glib-1 -lsamplerate -lgdk-x11-2.0 -latk-1.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lgio-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgmodule-2.0 -lmowgli -ldbus-1 -lpthread -lrt -lgobject-2.0 -lglib-2.0
Linux:	LDFLAGS+= -shared
Linux:	libilluminordxmms.so
	-

Darwin:	CFLAGS+= -I/usr/X11R6/include  -I/opt/local/include  -I/opt/local/include/xmms  -I/opt/local/include/gtk-1.2  -I/opt/local/include/glib-1.2  -I/opt/local/lib/glib/include
Darwin:	LDFLAGS+= -L/usr/lib  -L/opt/local/lib  -L/usr/X11R6/lib  -flat_namespace
#Darwin:	LDFLAGS+= -lxmms  -lm  -lglib  -lgtk  -lgdk
Darwin:	LDFLAGS+= -undefined suppress  -bundle
Darwin:	libilluminordxmms.so
	-

tags:	*.c *.h
	-ctags *.c *.h

clean:
	-$(RM) $(OBJECTS)
	-$(RM) libilluminordxmms.so
	-$(RM) tags
	
install: all
	mkdir -p ~/.xmms/Plugins
	cp libilluminordxmms.so ~/.xmms/Plugins/

uninstall:
	$(RM) ~/.xmms/Plugins/libilluminordxmms.so

edit:
	vim illuminordxmms.c

$(OBJECTS): $(HEADERS)

libilluminordxmms.so: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

