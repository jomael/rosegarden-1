
# These paths set up for Ubuntu 8.04 with kdelibs5-dev and qt4-dev.
# For some mad reason, KDE4 headers are in /usr/lib/kde4/include on
# this system.  Someone is really going to have to explain that to me.
# More sensibly, Qt4 headers are in /usr/include/Qt4.  Note also
# special names of moc and uic (to avoid messing with alternatives).

KDEDIR		= /usr
QTDIR		= /usr

KDEINC		= $(KDEDIR)/include/kde
KDELIB		= $(KDEDIR)/lib

QTINC		= $(QTDIR)/include/qt4
QTLIB		= $(QTDIR)/lib

CCDIR		= /usr

CCACHE		= 
CC		= $(CCACHE) $(CCDIR)/bin/cc
CXX		= $(CCACHE) $(CCDIR)/bin/c++
LD		= $(CXX)
MOC		= $(QTDIR)/bin/moc-qt4
UIC		= $(QTDIR)/bin/uic-qt4
RCC		= $(QTDIR)/bin/rcc

CXXFLAGS	= -DQT3_SUPPORT -Wall -O0 -g -DHAVE_ALSA -DHAVE_LIBJACK -DHAVE_DSSI -DHAVE_LADSPA -DHAVE_LIBLO -DHAVE_LIBLRDF -DHAVE_XFT -DNDEBUG -D'VERSION="2.0-yeah-whatever-innit"'
LDFLAGS		= 

INCLUDES	= \
		-I$(QTINC)/Qt3Support \
		-I$(QTINC)/QtCore \
		-I$(QTINC)/QtGui \
		-I$(QTINC)/QtXml \
		-I$(QTINC)/QtNetwork \
		-I$(QTINC) \
		-I$(KDEINC) \
		-I$(CCDIR)/include \
		-I/usr/local/include \
		-I/usr/include/freetype2 \
                -Isrc/base \
		-Isrc

LDLIBS		= \
		-L$(CCDIR)/lib \
		-L$(KDELIB) \
		-L$(QTLIB) \
		-lkdeprint -lkdeui -lkdecore -lQt3Support -lQtGui -lQtXml -lQtNetwork -lQtCore \
		-ljack -lasound -llo -llrdf -lraptor -llirc_client

RESOURCES	= $(wildcard data/*.qrc data/*/*.qrc src/*.qrc src/*/*.qrc src/*/*/*.qrc src/*/*/*/*.qrc)
QRCSOURCES	= $(patsubst %.qrc,%.cpp,$(RESOURCES))
HEADERS		= $(filter-out templates/template.h, $(wildcard src/*.h src/*/*.h src/*/*/*.h src/*/*/*/*.h src/*/*/*/*/*.h))
SOURCES		= $(QRCSOURCES) $(filter-out templates/template.cpp, $(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp src/*/*/*/*/*.cpp))
QHEADERS	= $(shell fgrep -l Q_OBJECT $(HEADERS))
OBJECTS		= $(patsubst %.cpp,%.o,$(SOURCES))
QSOURCES	= $(patsubst %.h,%.moc,$(QHEADERS))
UI		= $(wildcard src/gui/ui/*.ui)
RC		= $(wildcard src/gui/ui/*.rc)
UIHEADERS	= $(patsubst %.ui,%.h,$(UI))

all:	$(QSOURCES) $(UIHEADERS) $(UISOURCES) $(UIMOC) $(OBJECTS) $(LIBRARIES) $(EXECUTABLES) rosegarden

rosegarden:	$(OBJECTS)
		$(CXX) -o $@ $^ $(LDLIBS)

%.h: %.ui
	$(UIC) $< > $@

%.cpp: %.ui %.h
	$(UIC) -pch $(patsubst %.cpp,%.moc,$@) -impl $(patsubst %.cpp,%.h,$@) $(patsubst %.cpp,%.ui,$@) > $@

%.moc: %.h
	$(MOC) $< -o $@

%.cpp: %.qrc
	$(RCC) $< -root /$(firstword $(subst ., ,$(lastword $(subst /, ,$<)))) -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f $(QSOURCES) $(UIHEADERS) $(UISOURCES) $(UIMOC) $(OBJECTS) $(LIBRARIES) $(EXECUTABLES)

test:
	echo $(QSOURCES)
