#include "AudioOutputQt5.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <memory>

// MyAudioOutput

struct AudioOutput::Private {
	int volume = 5000;
	int sample_fq = 48000;
	std::shared_ptr<QAudioOutput> output;
	QIODevice *device = nullptr;
};

AudioOutput::AudioOutput()
	: m(new Private)
{
}

AudioOutput::~AudioOutput()
{
	delete m;
}

void AudioOutput::start(AudioDevice const &dev, QAudioFormat const &format)
{
	stop();
	m->output = std::make_shared<QAudioOutput>(format);
	m->output->setBufferSize(RECOMMENDED_BUFFER_SIZE);
	m->device = m->output->start();
}

void AudioOutput::stop()
{
	m->output.reset();
	m->device = nullptr;
}

int AudioOutput::bytesFree() const
{
	return m->output->bytesFree();
}

void AudioOutput::process(std::deque<uint8_t> *source)
{
	std::vector<uint8_t> buf;
	int n = std::min((int)source->size(), m->output->bytesFree());
	buf.insert(buf.end(), source->begin(), source->begin() + n);
	m->device->write((char const *)buf.data(), buf.size());
	source->erase(source->begin(), source->begin() + n);
}

