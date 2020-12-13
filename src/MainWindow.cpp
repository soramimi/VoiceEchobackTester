#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MySettings.h"
#include <QAudioInput>
#include <QAudioOutput>
#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <utility>

enum class State {
	Stop,
	Recording,
	Playing
};

struct MainWindow::Private {
	std::shared_ptr<QAudioInput> audio_input;
	std::shared_ptr<QAudioOutput> audio_output;
	QIODevice *input_device;
	QIODevice *output_device;

	State state = State::Stop;
	std::vector<uint8_t> buffer;
	int record_bytes = 0;
	int play_bytes = 0;

	int count = 0;
};

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::MainWindow)
	, m(new Private)
{
	ui->setupUi(this);

	QAudioFormat format;
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setChannelCount(2);
	format.setCodec("audio/pcm");
	format.setSampleRate(48000);
	format.setSampleSize(16);
	format.setSampleType(QAudioFormat::SignedInt);

	m->audio_input = std::shared_ptr<QAudioInput>(new QAudioInput(format));
	m->input_device = m->audio_input->start();

	m->audio_output = std::shared_ptr<QAudioOutput>(new QAudioOutput(format));
	m->output_device = m->audio_output->start();

	m->buffer.resize(48000 * 4 * 5);

	connect(m->input_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

	startTimer(10);
	start();
}

MainWindow::~MainWindow()
{
	delete m;
	delete ui;
}

void MainWindow::start()
{
	m->record_bytes = 0;
	m->play_bytes = 0;
	m->state = State::Recording;
}

void MainWindow::onReadyRead()
{
	if (m->state == State::Recording) {
		if (m->record_bytes < m->buffer.size()) {
			int n = m->audio_input->bytesReady();
			n = std::min(n, int(m->buffer.size() - m->record_bytes));
			if (n > 0) {
				n = m->input_device->read((char *)m->buffer.data() + m->record_bytes, n);
				m->record_bytes += n;
			}
		} else {
			m->state = State::Playing;
		}
	}
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	if (m->state == State::Playing) {
		int bytes = m->audio_output->bytesFree();
		if (m->play_bytes < m->record_bytes) {
			int n = std::min(bytes, m->record_bytes - m->play_bytes);
			n = m->output_device->write((char const *)m->buffer.data() + m->play_bytes, n);
			m->play_bytes += n;
			bytes -= n;
		} else {
			m->state = State::Stop;
		}
	}
}
