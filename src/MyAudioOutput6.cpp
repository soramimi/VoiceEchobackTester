#include "MyAudioOutput6.h"
#include <QAudioOutput>
#include <QAudioSink>
#include <QMediaDevices>

qint64 OutputStream::readData(char *data, qint64 len)
{
	const int N = sizeof(int16_t) * 2;
	if (len >= N && queue_.size() >= N) {
		std::copy(queue_.begin(), queue_.begin() + N, data);
		queue_.erase(queue_.begin(), queue_.begin() + N);
	}
	return N;
}

qint64 OutputStream::writeData(const char *data, qint64 len)
{
	return 0;
}

qint64 OutputStream::bytesAvailable() const
{
	return sizeof(int16_t) * 2;
}

qint64 OutputStream::size() const
{
	return sizeof(int16_t) * 2;
}

int OutputStream::writeInput(uint8_t const *ptr, int len)
{
	uint8_t const *begin = (uint8_t const *)ptr;
	uint8_t const *end = begin + len;
	queue_.insert(queue_.end(), begin, end);
	return len;
}



void MyAudioOutput::start(AudioDevice const &dev, const QAudioFormat &format)
{
	stop();
	device_ = QMediaDevices::defaultAudioOutput();
	output_ = std::make_shared<QAudioOutput>(device_);
	sink_ = std::make_shared<QAudioSink>(device_, format);
	out.open(QIODevice::ReadOnly);
	sink_->start(&out);
}

void MyAudioOutput::stop()
{
	device_ = {};
	output_.reset();
	sink_.reset();
	out.close();
}

void MyAudioOutput::process(std::deque<uint8_t> *q)
{
	int n = std::min((int)q->size(), (int)sink_->bytesFree());
	std::vector<uint8_t> buf;
	buf.insert(buf.end(), q->begin(), q->begin() + n);
	out.writeInput(buf.data(), buf.size());
	q->erase(q->begin(), q->begin() + n);
}


