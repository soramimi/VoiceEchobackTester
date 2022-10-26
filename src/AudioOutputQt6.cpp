#include "Audio.h"
#include "AudioOutputQt6.h"
#include <QAudioOutput>
#include <QAudioSink>
#include <QMediaDevices>
#include <memory>

namespace {

// OutputStream

class OutputStream : public QIODevice {
public:
	std::deque<uint8_t> queue_;
	qint64 readData(char *data, qint64 len) override
	{
		const int N = sizeof(int16_t) * 2;
		if (len >= N && queue_.size() >= N) {
			std::copy(queue_.begin(), queue_.begin() + N, data);
			queue_.erase(queue_.begin(), queue_.begin() + N);
		}
		return N;
	}
	qint64 writeData(const char *data, qint64 len) override
	{
		return 0;
	}
	qint64 bytesAvailable() const override
	{
		return sizeof(int16_t) * 2;
	}
	qint64 size() const override
	{
		return sizeof(int16_t) * 2;
	}
	int writeInput(uint8_t const *ptr, int len)
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
	int n = m->out.queue_.size();
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
	m->out.writeInput(buf.data(), buf.size());
	source->erase(source->begin(), source->begin() + n);
}


