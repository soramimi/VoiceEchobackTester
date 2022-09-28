#ifndef MYAUDIOINPUT5_H
#define MYAUDIOINPUT5_H

#include <QAudioInput>
#include <QIODevice>
#include <memory>


class MyAudioInput {
public:
	std::shared_ptr<QAudioInput> input_;
	QIODevice *io_ = nullptr;
	void start(const QAudioFormat &format);
};

#endif // MYAUDIOINPUT5_H
