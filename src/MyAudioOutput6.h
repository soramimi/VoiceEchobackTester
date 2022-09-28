#ifndef MYAUDIOOUTPUT6_H
#define MYAUDIOOUTPUT6_H

#include <QAudioDevice>
#include <QAudioSink>
#include <deque>
#include <memory>

class QAudioOutput;
class QAudioSink;

class OutputStream : public QIODevice {
public:
	std::deque<uint8_t> queue_;
	qint64 readData(char *data, qint64 len) override;
	qint64 writeData(const char *data, qint64 len) override;
	qint64 bytesAvailable() const override;
	qint64 size() const override;

	int writeInput(uint8_t const *ptr, int len);
};


class MyAudioOutput {
public:
	QAudioDevice device_;
	std::shared_ptr<QAudioOutput> output_;
	std::shared_ptr<QAudioSink> sink_;
	OutputStream out;

	void start(const QAudioFormat &format);
	int bytesFree() const
	{
		return (int)sink_->bytesFree();
	}
	void process(std::deque<uint8_t> *q);


	size_t bufferedBytes() const
	{
		return out.queue_.size();
	}
};

#endif // MYAUDIOOUTPUT6_H
