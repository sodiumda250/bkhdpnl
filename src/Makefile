#!/usr/bin/make -f
# $Id: Makefile,v 1.14 2005/05/25 05:24:51 woods Exp $

MAJOR = 0
MINOR = 1
TEENY = 0

COPYRIGHT = '"Copyright (C) 2000-2005 woods"'

RELEASE_VERSION = $(MAJOR).$(MINOR).$(TEENY)

TARGET = bkhdpnl.dll
OBJS = bkhdpnl.$(OBJ) PluginSDK/BeckyAPI.$(OBJ) \
	panelwin.$(OBJ) panel.$(OBJ) \
	rectstr.$(OBJ) vec.$(OBJ) \
	dialog.$(OBJ) hookproc.$(OBJ)
RESOBJS = bkhdpnl_res.$(OBJ)
LIBS = 
SRCS = $(OBJS:.$(OBJ)=.cpp)
HDRS = bkhdpnl.h panelwin.h rectstr.h resource.h vec.h dialog.h hookproc.h

# Suffix
OBJ = o
#OBJ= obj # for bcc/cl(?)

# Commands
MAKE = make
CC = g++
LD = g++
RM = rm -f
CP = cp -f
STRIP = strip
MKDIR = mkdir -p
ZIP = zip
LHA = lha
TOUCH = touch
FIND = find
INSTALL = install

# Compile and Link option
CFLAGS = -g -O -Wall -pipe -mno-cygwin -I/usr/inlcude/w32api \
 -DRELEASE_VERSION=\"$(RELEASE_VERSION)\" \
 -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DTEENY=$(TEENY) \
 -DCOPYRIGHT=$(COPYRIGHT)
LDFLAGS = -pipe -Wl,--enable-stdcall-fixup -mno-cygwin -mwindows -mingw \
 -mdll -mshared \
 -L/usr/lib/w32api

INSTALLPATH = /cygdrive/c/Becky!/`whoami`/Plugins
INSTALLNAME = bkhdpnl.dll

DOCDIR = doc

RELEASE_DIR = bkhdpnl$(RELEASE_VERSION)

all: $(TARGET)

install: all
	$(MKDIR) $(INSTALLPATH)
	$(INSTALL) -s -p $(TARGET) $(INSTALLPATH)/$(INSTALLNAME)

deinstall:
	$(RM) -f $(INSTALLPATH)/$(INSTALLNAME)

clean:
	$(RM) $(OBJS) $(RESOBJS) exports.$(OBJ) bkhdpnl.lib bkhdpnl.res
	$(RM) -r $(RELEASE_DIR) release
	if [ -d test ]; then (cd test; $(MAKE) clean); fi
	if [ -d exe ]; then (cd exe; $(MAKE) clean); fi

test: all
	if [ -d test ]; then (cd test; $(MAKE) all); fi
	if [ -d exe ]; then (cd exe; $(MAKE) all); fi

testrun: test
	if [ -f exe/shhdr01.exe ]; then exe/shhdr01.exe; fi

tags: $(SRCS) $(HDRS)
	ctags $(SRCS) $(HDRS)

strip:
	$(STRIP) $(TARGET)

debug: install
	./b2.sh

cleanall: clean
	if [ -d test ]; then (cd test; $(MAKE) cleanall); fi
	if [ -d exe ]; then (cd exe; $(MAKE) cleanall); fi
	$(RM) $(TARGET) bkhdpnl$(RELEASE_VERSION).lzh bkhdpnl$(RELEASE_VERSION).zip tags release $(RELEASE_DIR)
	$(RM) -r $(DOCDIR)

release: $(TARGET)
	$(MKDIR) $(RELEASE_DIR)
	$(MKDIR) $(RELEASE_DIR)/src
	$(MKDIR) $(RELEASE_DIR)/src/PlugInSDK
	$(INSTALL) -p -s $(TARGET) $(RELEASE_DIR)
	$(CP) bkhdpnl.txt $(RELEASE_DIR)
	$(CP) *.c* *.h *.def *.rc Makefile bkhdpnl.doxygen $(RELEASE_DIR)/src
	$(CP) PlugInSDK/*.* $(RELEASE_DIR)/src/PlugInSDK
	$(FIND) $(RELEASE_DIR) -name '*.$(OBJ)' -exec rm -f {} \;
	$(ZIP) -9 -v -r bkhdpnl$(RELEASE_VERSION).zip $(RELEASE_DIR)
	$(LHA) av bkhdpnl$(RELEASE_VERSION).lzh $(RELEASE_DIR)
	$(RM) bkhdpnl$(RELEASE_VERSION).bak
	$(TOUCH) release

docs:
	doxygen bkhdpnl.doxygen

doc: docs

$(TARGET): $(OBJS) $(RESOBJS) bkhdpnl.def
	dlltool -d bkhdpnl.def -e exports.$(OBJ) -l bkhdpnl.lib $(OBJS)
	$(LD) $(LDFLAGS) -o bkhdpnl.dll exports.$(OBJ) $(OBJS) $(RESOBJS) $(LIBS)

PluginSDK/BeckyAPI.$(OBJ): PluginSDK/BeckyAPI.cpp PluginSDK/BeckyAPI.h
	$(CC) -c -o PluginSDK/BeckyAPI.$(OBJ) $(CFLAGS) PluginSDK/BeckyAPI.cpp

bkhdpnl_res.$(OBJ): bkhdpnl.res
	windres -v -i bkhdpnl.res -o bkhdpnl_res.$(OBJ) --language=00000411

bkhdpnl.res: bkhdpnl.rc Makefile
	rc /r /dMAJOR=$(MAJOR) /dMINOR=$(MINOR) /dTEENY=$(TEENY) /dRELEASE_VERSION='"$(RELEASE_VERSION)\0"' /dVERSION_STR='"bkhdpnl version $(RELEASE_VERSION)"' /dCOPYRIGHT=$(COPYRIGHT) bkhdpnl.rc
#windres -v -i shhdr01.rc -o shhdr01.res --language=00000411 -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DTEENY=$(TEENY)
#nkf -S --utf16 shhdr01.rc | windres -v --input-format=rc -o shhdr01.res --language=00000411

.cpp.$(OBJ):
	$(CC) -c $(CFLAGS) $<

bkhdpnl.h: PlugInSDK/BeckyAPI.h
	$(TOUCH) -c bkhdpnl.h

panelwin.h: vec.h rectstr.h
	$(TOUCH) -c panelwin.h

rectstr.h: vec.h
	$(TOUCH) -c rectstr.h

bkhdpnl.$(OBJ): bkhdpnl.cpp bkhdpnl.h panelwin.h resource.h dialog.h hookproc.h

panelwin.$(OBJ): panelwin.cpp resource.h bkhdpnl.h panelwin.h

rectstr.$(OBJ): rectstr.cpp vec.h rectstr.h

vec.$(OBJ): vec.cpp vec.h

dialog.$(OBJ): dialog.cpp dialog.h bkhdpnl.h panelwin.h

