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

SOURCES += \
	src/Audio.cpp \
	src/LevelMeterWidget.cpp \
	src/MainWindow.cpp \
	src/MySettings.cpp \
	src/joinpath.cpp \
	src/main.cpp

HEADERS += \
	src/Audio.h \
	src/AudioAbstractIO.h \
	src/LevelMeterWidget.h \
	src/MainWindow.h \
	src/MySettings.h \
	src/joinpath.h \
	src/main.h

FORMS += \
	src/MainWindow.ui

qt5 {
SOURCES += \
	src/AudioInputQt5.cpp \
	src/AudioOutputQt5.cpp
HEADERS += \
	src/AudioInputQt5.h \
	src/AudioOutputQt5.h
}

qt6 {
SOURCES += \
	src/AudioInputQt6.cpp \
	src/AudioOutputQt6.cpp
HEADERS += \
	src/AudioInputQt6.h \
	src/AudioOutputQt6.h
}
