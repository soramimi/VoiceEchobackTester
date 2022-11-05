#include "Audio.h"
#include "AudioOutputQt6.h"
#include <QAudioOutput>
#include <QAudioSink>
#include <QDebug>
#include <QMediaDevices>
#include <memory>

namespace {

// OutputStream

class OutputStream : public QIODevice {
private:
	const int MINSAMPLES = 4;
	const int MINSIZE = int(sizeof(int16_t) * 2 * MINSAMPLES);
public:
	std::deque<uint8_t> queue_;
	qint64 readData(char *data, qint64 len) override
	{
		int n = std::min((int)len, (int)queue_.size());
		n = std::min(n, MINSIZE);
		if (n > 0) {
			std::copy(queue_.begin(), queue_.begin() + (int)n, data);
			queue_.erase(queue_.begin(), queue_.begin() + (int)n);
		}
		if (queue_.empty()) {
			if (n & 1) {
				data[n] = 0;
				n++;
			}
			while (n < MINSIZE) {
				*(int16_t *)(data + n) = 0;
				n += sizeof(int16_t);
			}
		}
		return (qint64)n;
	}
	qint64 writeData(const char *data, qint64 len) override
	{
		(void)data;
		(void)len;
		return 0;
	}
	qint64 bytesAvailable() const override
	{
		return std::min(MINSIZE, (int)queue_.size());
	}
	qint64 size() const override
	{
		return std::min(MINSIZE, (int)queue_.size());
	}
	int write(uint8_t const *ptr, int len)
	{
		uint8_t const *begin = (uint8_t const *)ptr;
		uint8_t const *end = begin + len;
		queue_.insert(queue_.end(), begin, end);
		return len;
	}
};

} // namespace

// MyAudioOutput

struct AudioOutput::Private {
	QAudioDevice device;
	std::shared_ptr<QAudioOutput> output;
	std::shared_ptr<QAudioSink> sink;
	OutputStream out;
};


AudioOutput::AudioOutput()
	: m(new Private)
{
}

AudioOutput::~AudioOutput()
{
	delete m;
}

QString AudioOutput::description()
{
	return m->device.description();
}

void AudioOutput::start(AudioDevice const &dev, const QAudioFormat &format)
{
	stop();
	m->device = dev.device_;
	m->output = std::make_shared<QAudioOutput>(m->device);
	m->sink = std::make_shared<QAudioSink>(m->device, format);
	m->out.open(QIODevice::ReadOnly);
	m->sink->start(&m->out);
}

void AudioOutput::stop()
{
	m->device = {};
	m->output.reset();
	m->sink.reset();
	m->out.close();
}

int AudioOutput::bytesFree() const
{
	int n = (int)m->out.queue_.size();
	if (n < RECOMMENDED_BUFFER_SIZE) {
		n = RECOMMENDED_BUFFER_SIZE - n;
		n = std::min(n, (int)m->sink->bytesFree());
		return n;
	}
	return 0;
}

void AudioOutput::process(std::deque<uint8_t> *source)
{
	int n = std::min((int)source->size(), (int)m->sink->bytesFree());
	std::vector<uint8_t> buf;
	buf.insert(buf.end(), source->begin(), source->begin() + n);
	m->out.write(buf.data(), (int)buf.size());
	source->erase(source->begin(), source->begin() + n);
}


