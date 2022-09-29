#ifndef MYAUDIOINPUT6_H
#define MYAUDIOINPUT6_H

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSource>
#include <QIODevice>
#include <deque>
#include <memory>

class QAudioDevice;
class QAudioInput;
class AudioDevice;

class QueueWriter : public QIODevice {
public:
	std::deque<uint8_t> *queue_;
	qint64 readData(char *data, qint64 len) override;
	qint64 writeData(const char *data, qint64 len) override;
	qint64 bytesAvailable() const override;
	qint64 size() const override;
};

class QueueReader : public QIODevice {
public:
	std::deque<uint8_t> *queue_;
	qint64 readData(char *data, qint64 len) override;
	qint64 writeData(const char *data, qint64 len) override;
	qint64 bytesAvailable() const override;
	qint64 size() const override;
};


class MyAudioInput {
public:
	QAudioDevice device_;
	std::shared_ptr<QAudioInput> input_;
	std::shared_ptr<QAudioSource> source_;
	std::deque<uint8_t> queue_;
	QueueWriter writer_;
	QueueReader reader_;
	void start(const AudioDevice &dev, QAudioFormat const &format);
	void stop();
	qint64 bytesAvailable() const
	{
		return reader_.bytesAvailable();
	}
	int read(char *data, int maxlen)
	{
		return reader_.read(data, maxlen);
	}
	QByteArray readAll()
	{
		return reader_.read(reader_.bytesAvailable());
	}
};

#endif // MYAUDIOINPUT6_H
