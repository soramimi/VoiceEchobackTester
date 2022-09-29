#ifndef MYAUDIO_H
#define MYAUDIO_H
#include <Qt>
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include "MyAudioInput5.h"
#include "MyAudioOutput5.h"
#else
#include <QMediaDevices>
#include "MyAudioInput6.h"
#include "MyAudioOutput6.h"
#endif

class MyAudio {
public:
	static QAudioFormat defaultAudioFormat();
};

#endif // MYAUDIO_H
