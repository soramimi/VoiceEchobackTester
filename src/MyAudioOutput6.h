#ifndef MYAUDIOOUTPUT6_H
#define MYAUDIOOUTPUT6_H

#include <QAudioDevice>
#include <QAudioSink>
#include <deque>
#include <memory>

class QAudioOutput;
class QAudioSink;
class AudioDevice;

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

	void start(const AudioDevice &dev, const QAudioFormat &format);
	void stop();
	int bytesFree() const
	{
		int n = bufferedBytes();
		if (n < 4800) {
			n = 4800 - n;
			n = std::min(n, (int)sink_->bytesFree());
			return n;
		}
		return 0;
	}
	void process(std::deque<uint8_t> *q);


	size_t bufferedBytes() const
	{
		return out.queue_.size();
	}
};

#endif // MYAUDIOOUTPUT6_H
