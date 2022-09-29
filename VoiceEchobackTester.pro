QT       += core gui widgets multimedia

CONFIG += c++1z
CONFIG += nostrip debug_info

greaterThan(QT_MAJOR_VERSION, 5) {
	CONFIG += qt6
}
lessThan(QT_MAJOR_VERSION, 6) {
	CONFIG += qt5
}


DESTDIR = $$PWD/_bin

win32:LIBS += -L"C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.1A\\Lib"

SOURCES += \
	src/AudioDevice.cpp \
	src/LevelMeterWidget.cpp \
	src/MainWindow.cpp \
	src/MyAudio.cpp \
	src/MySettings.cpp \
	src/TestWidget.cpp \
	src/joinpath.cpp \
	src/main.cpp

HEADERS += \
	src/AudioDevice.h \
	src/LevelMeterWidget.h \
	src/MainWindow.h \
	src/MyAudio.h \
	src/MySettings.h \
	src/TestWidget.h \
	src/joinpath.h \
	src/main.h

FORMS += \
	src/MainWindow.ui

qt5 {
SOURCES += \
	src/MyAudioInput5.cpp \
	src/MyAudioOutput5.cpp
HEADERS += \
	src/MyAudioInput5.h \
	src/MyAudioOutput5.h
}

qt6 {
SOURCES += \
	src/MyAudioInput6.cpp \
	src/MyAudioOutput6.cpp
HEADERS += \
	src/MyAudioInput6.h \
	src/MyAudioOutput6.h
}
