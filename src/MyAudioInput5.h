#ifndef MYAUDIOINPUT5_H
#define MYAUDIOINPUT5_H

#include <QAudioInput>
#include <QIODevice>
#include <memory>

class AudioDevice;

class MyAudioInput {
public:
	std::shared_ptr<QAudioInput> input_;
	QIODevice *reader_ = nullptr;
	~MyAudioInput()
	{
		stop();
	}
	void start(const AudioDevice &dev, const QAudioFormat &format);
	void stop();
	qint64 bytesAvailable() const
	{
		return input_->bytesReady();
	}
	int read(char *data, int maxlen)
	{
		return reader_->read(data, maxlen);
	}
	QByteArray readAll()
	{
		QByteArray ba;
		if (reader_) {
			ba = reader_->readAll();
		}
		return ba;
	}
};

#endif // MYAUDIOINPUT5_H
