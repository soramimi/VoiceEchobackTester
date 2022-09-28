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
	void start(QAudioFormat const &format);
};

#endif // MYAUDIOINPUT6_H
