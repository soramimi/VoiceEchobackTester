#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MySettings.h"
#include "Audio.h"
#include <QBuffer>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <deque>
#include <memory>
#include <utility>


struct MainWindow::Private {
	AudioDevices input_devices;
	AudioDevices output_devices;
	QAudioFormat audio_format;
	AudioInput input;
	AudioOutput output;
	std::deque<uint8_t> output_buffer;

	int duration = 3;
	size_t max_record_size = 0;

	State state = State::Stop;
	std::vector<uint8_t> recording_buffer;
	int recorded_bytes = 0;
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

	m->audio_format = Audio::defaultAudioFormat();
	m->input.start(AudioDevices::defaultAudioInputDevice(), m->audio_format);
	m->output.start(AudioDevices::defaultAudioOutputDevice(), m->audio_format);

	m->input_devices.fetchDevices(AudioDevices::AudioInput);
	m->output_devices.fetchDevices(AudioDevices::AudioOutput);
	{
		int sel = -1;
		bool b = ui->comboBox_input->blockSignals(true);
		for (int i = 0; i < m->input_devices.size(); i++) {
			AudioDevice dev = m->input_devices.device(i);
			QString name = dev.text();
			ui->comboBox_input->addItem(name, name);
			if (name == m->input.description()) {
				sel = i;
			}
		}
		ui->comboBox_input->blockSignals(b);
		ui->comboBox_input->setCurrentIndex(sel);
	}
	{
		int sel = -1;
		bool b = ui->comboBox_output->blockSignals(true);
		for (int i = 0; i < m->output_devices.size(); i++) {
			AudioDevice dev = m->output_devices.device(i);
			QString name = dev.text();
			ui->comboBox_output->addItem(name, name);
			if (name == m->output.description()) {
				sel = i;
			}
		}
		ui->comboBox_output->blockSignals(b);
		ui->comboBox_output->setCurrentIndex(sel);
	}

	ui->comboBox_length->setCurrentIndex(0);

	ui->pushButton_start->setFocus();

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
	m->max_record_size = 48000 * sizeof(int16_t) * 2 * seconds;
	m->recording_buffer.resize(m->max_record_size + 20000);
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
	m->duration = ui->comboBox_length->currentData().toInt();
	setLength(m->duration + 1);
	ui->progressBar_recording->setValue(0);
	ui->progressBar_playback->setValue(0);
	m->recorded_bytes = 0;
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

void MainWindow::inputAudio()
{
	if (m->state == State::Recording) {
		if (m->recorded_bytes < m->max_record_size) {
			int n = m->input.bytesAvailable();
			n = std::min(n, int(m->recording_buffer.size() - m->recorded_bytes));
			if (n > 0) {
				n = m->input.read((char *)m->recording_buffer.data() + m->recorded_bytes, n);
				setInputLevel((int16_t const *)(m->recording_buffer.data() + m->recorded_bytes), n / 2);
				m->recorded_bytes += n;
			}
			float percent = 100 * m->recorded_bytes / m->max_record_size;
			ui->progressBar_recording->setValue(int(percent * 10));
			return;
		}
		setState(State::Playing);
	}
	{
		QByteArray ba = m->input.readAll();
		if (ba.size() >= 2) {
			setInputLevel((int16_t const *)ba.data(), ba.size() / 2);
		}
	}
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	inputAudio();

	if (m->state == State::Playing) {
		int bytes = m->output.bytesFree();
		if (m->play_bytes < m->recorded_bytes) {
			int n = std::min(bytes, m->recorded_bytes - m->play_bytes);
			if (n >= 2) {
				uint8_t const *begin = (uint8_t const *)m->recording_buffer.data() + m->play_bytes;
				uint8_t const *end = begin + n;
				m->output_buffer.insert(m->output_buffer.end(), begin, end);
				setOutputLevel((int16_t const *)(m->recording_buffer.data() + m->play_bytes), n / 2);
				m->play_bytes += n;
				bytes -= n;
				float percent = 100 * m->play_bytes / m->recorded_bytes;
				ui->progressBar_playback->setValue(int(percent * 10));
			}
			m->output.process(&m->output_buffer);
			return;
		}
		setState(State::Stop);
	}
	ui->widget_output_level->setPercent(0);
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
	m->input.start(m->input_devices.device(index), m->audio_format);
}

void MainWindow::on_comboBox_output_currentIndexChanged(int index)
{
	m->output.start(m->output_devices.device(index), m->audio_format);
}
