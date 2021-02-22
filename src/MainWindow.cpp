#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MySettings.h"
#include <QAudioInput>
#include <QAudioOutput>
#include <QClipboard>
#include <QDebug>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <utility>
#include <memory>

struct MainWindow::Private {
	QList<QAudioDeviceInfo> audio_input_devices;
	QList<QAudioDeviceInfo> audio_output_devices;
	QAudioFormat audio_format;
	std::shared_ptr<QAudioInput> audio_input;
	std::shared_ptr<QAudioOutput> audio_output;
	QIODevice *input_device = nullptr;
	QIODevice *output_device = nullptr;

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

	auto flags = windowFlags();
	flags &= ~Qt::WindowContextHelpButtonHint;
	setWindowFlags(flags);

	ui->comboBox_length->addItem("3", QVariant(3));
	ui->comboBox_length->addItem("5", QVariant(5));
	ui->comboBox_length->addItem("10", QVariant(10));

	m->audio_format.setByteOrder(QAudioFormat::LittleEndian);
	m->audio_format.setChannelCount(2);
	m->audio_format.setCodec("audio/pcm");
	m->audio_format.setSampleRate(48000);
	m->audio_format.setSampleSize(16);
	m->audio_format.setSampleType(QAudioFormat::SignedInt);

	m->audio_input_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
	m->audio_output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	for (QAudioDeviceInfo const &dev : m->audio_input_devices) {
		QString name = dev.deviceName();
		ui->comboBox_input->addItem(name, name);
	}
	for (QAudioDeviceInfo const &dev : m->audio_output_devices) {
		QString name = dev.deviceName();
		ui->comboBox_output->addItem(name, name);
	}

	m->audio_input = std::shared_ptr<QAudioInput>(new QAudioInput(m->audio_format));
	m->input_device = m->audio_input->start();

	m->audio_output = std::shared_ptr<QAudioOutput>(new QAudioOutput(m->audio_format));
	m->output_device = m->audio_output->start();

	connect(m->input_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

	ui->comboBox_length->setCurrentIndex(0);


	startTimer(10);
	setState(State::Stop);
}

MainWindow::~MainWindow()
{
	delete m;
	delete ui;
}

void MainWindow::setLength(int seconds)
{
	m->buffer.resize(48000 * 4 * seconds);
}

void MainWindow::setState(State state)
{
	m->state = state;
	if (m->state == State::Stop) {
		ui->comboBox_input->setEnabled(true);
		ui->comboBox_output->setEnabled(true);
		ui->comboBox_length->setEnabled(true);
		ui->pushButton_start->setText(tr("Start"));
		ui->progressBar_recording->setValue(0);
		ui->progressBar_playback->setValue(0);
	} else {
		ui->comboBox_input->setEnabled(false);
		ui->comboBox_output->setEnabled(false);
		ui->comboBox_length->setEnabled(false);
		ui->pushButton_start->setText(tr("Stop"));
	}
}

void MainWindow::start()
{
	int len = ui->comboBox_length->currentData().toInt();
	setLength(len);
	ui->progressBar_recording->setValue(0);
	ui->progressBar_playback->setValue(0);
	m->record_bytes = 0;
	m->play_bytes = 0;
	setState(State::Recording);
}

void MainWindow::setLevel(int16_t const *p, int n, LevelMeterWidget *w)
{
	int v = 0;
	for (int i = 0; i < n; i++) {
		v = std::max(v, abs(p[i]));
	}
	float percent = v / 32768.0;
	w->setPercent(percent);
}

void MainWindow::setInputLevel(int16_t const *p, int n)
{
	setLevel(p, n, ui->widget_input_level);
}

void MainWindow::setOutputLevel(int16_t const *p, int n)
{
	setLevel(p, n, ui->widget_output_level);
}

void MainWindow::onReadyRead()
{
	if (m->state == State::Recording) {
        if (m->record_bytes < (int)m->buffer.size()) {
			int n = m->audio_input->bytesReady();
			n = std::min(n, int(m->buffer.size() - m->record_bytes));
			if (n > 0) {
				n = m->input_device->read((char *)m->buffer.data() + m->record_bytes, n);
				setInputLevel((int16_t const *)(m->buffer.data() + m->record_bytes), n / 2);
				m->record_bytes += n;
			}
			float percent = 100 * m->record_bytes / m->buffer.size();
			ui->progressBar_recording->setValue(int(percent * 10));
			goto done;
		} else {
			setState(State::Playing);
		}
	}
	{
		QByteArray ba = m->input_device->readAll();
		if (ba.size() >= 2) {
			setInputLevel((int16_t const *)ba.data(), ba.size() / 2);
		}
	}
done:;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	if (m->state == State::Playing) {
		int bytes = m->audio_output->bytesFree();
		if (m->play_bytes < m->record_bytes) {
			int n = std::min(bytes, m->record_bytes - m->play_bytes);
			if (n >= 2) {
				n = m->output_device->write((char const *)m->buffer.data() + m->play_bytes, n);
				setOutputLevel((int16_t const *)(m->buffer.data() + m->play_bytes), n / 2);
				m->play_bytes += n;
				bytes -= n;
				float percent = 100 * m->play_bytes / m->record_bytes;
				ui->progressBar_playback->setValue(int(percent * 10));
			}
			goto done;
		} else {
			setState(State::Stop);
		}
	}
	ui->widget_output_level->setPercent(0);
done:;
}

void MainWindow::on_pushButton_start_clicked()
{
	if (m->state == State::Stop) {
		start();
	} else {
		setState(State::Stop);
	}
}

void MainWindow::on_comboBox_input_currentIndexChanged(int index)
{
	if (m->audio_input) {
		disconnect(m->input_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
		m->audio_input->stop();
		m->audio_input.reset();
	}

	if (index >= 0 && index < m->audio_input_devices.size()) {
		m->audio_input = std::shared_ptr<QAudioInput>(new QAudioInput(m->audio_input_devices[index], m->audio_format));
		m->input_device = m->audio_input->start();
		connect(m->input_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	}
}

void MainWindow::on_comboBox_output_currentIndexChanged(int index)
{
	if (m->audio_output) {
		m->audio_output->stop();
		m->audio_output.reset();
	}

	m->audio_output = std::shared_ptr<QAudioOutput>(new QAudioOutput(m->audio_output_devices[index], m->audio_format));
	m->output_device = m->audio_output->start();
}
