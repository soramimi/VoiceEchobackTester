#ifndef MYAUDIOOUTPUT5_H
#define MYAUDIOOUTPUT5_H

#include <QAudioOutput>
#include <deque>
#include <memory>

class QAudioOutput;
class QIODevice;
class AudioDevice;

class MyAudioOutput {
public:
	int volume_ = 5000;
	int sample_fq_ = 48000;
	std::shared_ptr<QAudioOutput> output_;
	QIODevice *device_ = nullptr;
	void start(const AudioDevice &dev, const QAudioFormat &format);
	void stop();
	int bytesFree() const
	{
		return output_->bytesFree();
	}
	int write(char const *p, int n)
	{
		device_->write(p, n);
	}
	void process(std::deque<uint8_t> *source);
};

#endif // MYAUDIOOUTPUT5_H
