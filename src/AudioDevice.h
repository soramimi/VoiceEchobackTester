#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include "MyAudio.h"

class AudioDevice {
public:
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	typedef QAudioDeviceInfo audio_device_t;
#else
	typedef QAudioDevice audio_device_t;
#endif
	AudioDevice() = default;
	AudioDevice(audio_device_t const &dev)
		: device_(dev)
	{
	}
	audio_device_t device_;
	QString text() const
	{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
		return device_.deviceName();
#else
		return device_.description();
#endif
	}
};
//#else
//class AudioDevice {
//public:
//	AudioDevice() = default;
//	AudioDevice(QAudioDevice const &dev)
//		: device_(dev)
//	{

//	}
//	QAudioDevice device_;
//	QString text() const
//	{
//		return device_.description();
//	}
//};
//#endif

class AudioDevices {
public:
	enum Mode {
		AudioInput,
		AudioOutput
	};
	void fetchDevices(Mode mode);
	std::vector<AudioDevice> devices;
	int size() const;
	AudioDevice device(int i);
	static AudioDevice defaultAudioInputDevice()
	{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
		return AudioDevice::audio_device_t(QAudioDeviceInfo::defaultInputDevice());
#else
		return AudioDevice::audio_device_t(QMediaDevices::defaultAudioInput());
#endif
	}
	static AudioDevice defaultAudioOutputDevice()
	{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
		return AudioDevice::audio_device_t(QAudioDeviceInfo::defaultOutputDevice());
#else
		return AudioDevice::audio_device_t(QMediaDevices::defaultAudioOutput());
#endif
	}
};

#endif // AUDIODEVICE_H
