#ifndef AUDIOOUTPUTQT6_H
#define AUDIOOUTPUTQT6_H

#include "AudioAbstractIO.h"

class AudioOutput : public AbstractAudioOutput {
private:
	struct Private;
	Private *m;
public:
	AudioOutput();
	~AudioOutput() override;
	void start(const AudioDevice &dev, const QAudioFormat &format) override;
	void stop() override;
	int bytesFree() const override;
	void process(std::deque<uint8_t> *source) override;
};

#endif // AUDIOOUTPUTQT6_H
