
#include "AudioDevice.h"

QAudioFormat MyAudio::defaultAudioFormat()
{
	QAudioFormat format;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setChannelCount(2);
	format.setCodec("audio/pcm");
	format.setSampleRate(48000);
	format.setSampleSize(16);
	format.setSampleType(QAudioFormat::SignedInt);
#else
	format.setChannelCount(2);
	format.setSampleRate(48000);
	format.setSampleFormat(QAudioFormat::Int16);
#endif
	return format;
}

void AudioDevices::fetchDevices(Mode mode)
{
	devices.clear();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	QList<QAudioDeviceInfo> devs;
	switch (mode) {
	case AudioInput:
		devs = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
		break;
	case AudioOutput:
		devs = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
		break;
	}
	for (QAudioDeviceInfo const &d : devs) {
		devices.emplace_back(d);
	}
#else
	QList<QAudioDevice> devs;
	switch (mode) {
	case AudioInput:
		devs = QMediaDevices::audioInputs();
		break;
	case AudioOutput:
		devs = QMediaDevices::audioOutputs();
		break;
	}
	for (QAudioDevice const &d : devs) {
		devices.emplace_back(d);
	}
#endif
}

int AudioDevices::size() const
{
	return devices.size();
}

AudioDevice AudioDevices::device(int i)
{
	if (i >= 0 && i < devices.size()) {
		return devices[i];
	}
	return {};
}
