QT       += core gui widgets multimedia

CONFIG += c++1z
CONFIG += nostrip debug_info

DESTDIR = $$PWD/_bin

SOURCES += \
	src/MainWindow.cpp \
	src/MySettings.cpp \
	src/joinpath.cpp \
	src/main.cpp

HEADERS += \
	src/MainWindow.h \
	src/MySettings.h \
	src/joinpath.h \
	src/main.h

FORMS += \
	src/MainWindow.ui

