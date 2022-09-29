#include "MyAudioInput6.h"
#include <QAudioInput>
#include <QMediaDevices>
#include <utility>

qint64 QueueWriter::readData(char *data, qint64 len)
{
	return 0;
}

qint64 QueueWriter::writeData(const char *data, qint64 len)
{
	uint8_t const *begin = (uint8_t const *)data;
	uint8_t const *end = begin + len;
	queue_->insert(queue_->end(), begin, end);
	return len;
}

qint64 QueueWriter::bytesAvailable() const
{
	return 0;
}

qint64 QueueWriter::size() const
{
	return 0;
}


qint64 QueueReader::readData(char *data, qint64 len)
{
	size_t n = (size_t)std::min(len, (qint64)queue_->size());
	std::copy(queue_->begin(), queue_->begin() + n, data);
	queue_->erase(queue_->begin(), queue_->begin() + n);
	return n;
}

qint64 QueueReader::writeData(const char *data, qint64 len)
{
	return 0;
}

qint64 QueueReader::bytesAvailable() const
{
	return queue_->size();
}

qint64 QueueReader::size() const
{
	return queue_->size();
}

void MyAudioInput::start(AudioDevice const &dev, const QAudioFormat &format)
{
	stop();
	device_ = QMediaDevices::defaultAudioInput();
	input_ = std::make_shared<QAudioInput>(device_);
	source_ = std::make_shared<QAudioSource>(device_, format);
	writer_.queue_ = &queue_;
	reader_.queue_ = &queue_;
	writer_.open(QueueWriter::WriteOnly);
	reader_.open(QueueWriter::ReadOnly);
	source_->start(&writer_);
}

void MyAudioInput::stop()
{
	device_ = {};
	input_.reset();
	source_.reset();
	queue_.clear();
	writer_.close();
	reader_.close();

}
