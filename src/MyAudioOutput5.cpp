#include "MyAudioOutput5.h"
#include <QAudioFormat>
#include <QAudioOutput>

void MyAudioOutput::start(AudioDevice const &dev, QAudioFormat const &format)
{
	stop();
	output_ = std::make_shared<QAudioOutput>(format);
	output_->setBufferSize(2000);
	device_ = output_->start();
}

void MyAudioOutput::stop()
{
	output_.reset();
	device_ = nullptr;
}

void MyAudioOutput::process(std::deque<uint8_t> *source)
{
	std::vector<uint8_t> buf;
	int n = std::min((int)source->size(), output_->bytesFree());
	buf.insert(buf.end(), source->begin(), source->begin() + n);
	device_->write((char const *)buf.data(), buf.size());
	source->erase(source->begin(), source->begin() + n);
}

