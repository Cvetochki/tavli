######################################################################
# Automatically generated by qmake (2.01a) ??? 26. ???? 23:19:28 2007
######################################################################
CONFIG += embed_manifest_exe qt debug
TEMPLATE = app
QT += network
#QT += phonon
TARGET = tavli
DEPENDPATH += . debug
INCLUDEPATH += .

#win32:LIBS	+= freetype.lib
#unix:LIBS += -lopenal

# Input
HEADERS += board.h mainwindow.h player.h settings.h Network.h
FORMS += settings.ui
SOURCES += board.cpp main.cpp mainwindow.cpp player.cpp settings.cpp Network.cpp
RESOURCES += tavli.qrc
TRANSLATIONS += tavli_el.ts


UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs