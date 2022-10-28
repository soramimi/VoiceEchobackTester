#ifndef AUDIOOUTPUTQT5_H
#define AUDIOOUTPUTQT5_H

#include "AudioAbstractIO.h"

class AudioOutput : public AbstractAudioOutput {
private:
	struct Private;
	Private *m;
public:
	AudioOutput();
	~AudioOutput() override;
	QString description();
	void start(const AudioDevice &dev, const QAudioFormat &format) override;
	void stop() override;
	int bytesFree() const override;
	void process(std::deque<uint8_t> *source) override;

	// AbstractAudioOutput interface
public:
};

#endif // AUDIOOUTPUTQT5_H
