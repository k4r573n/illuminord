 # $Id: Makefile 154 2005-10-06 01:08:44Z lostrace $
 #  illuminordxmms v0.3
 #  
 #  Copyleft (c) 2005 by
 #  losTrace aka "David R. Piegdon" and cebewee aka "Lars Noschinski"
 #

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

Linux:	CFLAGS+= -I/usr/include/xmms -I/usr/include/glib-1.2 -I/usr/include/gtk-1.2 -I/usr/lib/glib/include
Linux:	LDFLAGS+= -L/usr/lib
Linux:	LDFLAGS+= -lxmms -lm -lgtk -shared
Linux:	libilluminordxmms.so
	-

Darwin:	CFLAGS+= -I/usr/X11R6/include  -I/opt/local/include  -I/opt/local/include/xmms  -I/opt/local/include/gtk-1.2  -I/opt/local/include/glib-1.2  -I/opt/local/lib/glib/include
Darwin:	LDFLAGS+= -L/usr/lib  -L/opt/local/lib  -L/usr/X11R6/lib  -flat_namespace
Darwin:	LDFLAGS+= -lxmms  -lm  -lglib  -lgtk  -lgdk
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

$(OBJECTS): $(HEADERS)

libilluminordxmms.so: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

