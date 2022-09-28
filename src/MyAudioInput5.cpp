#include "MyAudioInput5.h"



void MyAudioInput::start(QAudioFormat const &format)
{
	input_ = std::shared_ptr<QAudioInput>(new QAudioInput(format));
	io_ = input_->start();
}
