QT       += core gui widgets multimedia

CONFIG += c++1z
CONFIG += nostrip debug_info

DESTDIR = $$PWD/_bin

win32:LIBS += -L"C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.1A\\Lib"

SOURCES += \
	src/LevelMeterWidget.cpp \
	src/MainWindow.cpp \
	src/MySettings.cpp \
	src/TestWidget.cpp \
	src/joinpath.cpp \
	src/main.cpp

HEADERS += \
	src/LevelMeterWidget.h \
	src/MainWindow.h \
	src/MySettings.h \
	src/TestWidget.h \
	src/joinpath.h \
	src/main.h

FORMS += \
	src/MainWindow.ui

