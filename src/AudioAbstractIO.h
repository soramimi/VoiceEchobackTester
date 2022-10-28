#ifndef AUDIOABSTRACTIO_H
#define AUDIOABSTRACTIO_H

#include <QString>
#include <cstdint>
#include <deque>

class AudioDevice;
class QAudioFormat;
class QByteArray;

class AbstractAudioInput {
public:
	const int RECOMMENDED_BUFFER_SIZE = 9600;
	virtual ~AbstractAudioInput() = default;
	virtual QString description() = 0;
	virtual void start(const AudioDevice &dev, const QAudioFormat &format) = 0;
	virtual void stop() = 0;
	virtual int bytesAvailable() const = 0;
	virtual int read(char *data, int maxlen) = 0;
	virtual QByteArray readAll() = 0;
};

class AbstractAudioOutput {
public:
	const int RECOMMENDED_BUFFER_SIZE = 9600;
	virtual ~AbstractAudioOutput() = default;
	virtual QString description() = 0;
	virtual void start(const AudioDevice &dev, const QAudioFormat &format) = 0;
	virtual void stop() = 0;
	virtual int bytesFree() const = 0;
	virtual void process(std::deque<uint8_t> *source) = 0;
};

#endif // AUDIOABSTRACTIO_H
