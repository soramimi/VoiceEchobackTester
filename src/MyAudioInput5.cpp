#include "MyAudioInput5.h"



void MyAudioInput::start(AudioDevice const &dev, QAudioFormat const &format)
{
	stop();
	input_ = std::shared_ptr<QAudioInput>(new QAudioInput(format));
	reader_ = input_->start();
}

void MyAudioInput::stop()
{
	reader_ = nullptr;
	input_.reset();
}
