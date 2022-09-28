#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MyAudio.h"
#include "MySettings.h"
#include <QAudioInput>
#include <QAudioOutput>
#include <QBuffer>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <deque>
#include <memory>
#include <utility>


struct MainWindow::Private {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	QList<QAudioDeviceInfo> audio_input_devices;
#else
#endif
	QAudioFormat audio_format;
	MyAudioInput input;
	MyAudioOutput output;
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

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	m->audio_format.setByteOrder(QAudioFormat::LittleEndian);
	m->audio_format.setChannelCount(2);
	m->audio_format.setCodec("audio/pcm");
	m->audio_format.setSampleRate(48000);
	m->audio_format.setSampleSize(16);
	m->audio_format.setSampleType(QAudioFormat::SignedInt);
	m->audio_input_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
#else
	m->audio_format.setChannelCount(2);
	m->audio_format.setSampleRate(48000);
	m->audio_format.setSampleFormat(QAudioFormat::Int16);
#endif

//	m->audio_output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
//	for (QAudioDeviceInfo const &dev : m->audio_input_devices) {
//		QString name = dev.deviceName();
//		ui->comboBox_input->addItem(name, name);
//	}
//	for (QAudioDeviceInfo const &dev : m->audio_output_devices) {
//		QString name = dev.deviceName();
//		ui->comboBox_output->addItem(name, name);
//	}

	m->input.start(m->audio_format);
	m->output.start(m->audio_format);

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	connect(m->input.io_, &QIODevice::readyRead, this, &MainWindow::onReadyRead);
#endif

//	m->audio_output = std::shared_ptr<QAudioOutput>(new QAudioOutput(m->audio_format));
//	m->output_device = m->audio_output->start();


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
	m->max_record_size = 48000 * 4 * seconds;
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

void MainWindow::onReadyRead()
{
	if (m->state == State::Recording) {
		if (m->recorded_bytes < m->max_record_size) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
			int n = m->input.input_->bytesReady();
			n = std::min(n, int(m->recording_buffer.size() - m->recorded_bytes));
			if (n > 0) {
				n = m->input.io_->read((char *)m->recording_buffer.data() + m->recorded_bytes, n);
				setInputLevel((int16_t const *)(m->recording_buffer.data() + m->recorded_bytes), n / 2);
				m->recorded_bytes += n;
			}
#else
			int n = m->input.reader_.bytesAvailable();
			n = std::min(n, int(m->recording_buffer.size() - m->recorded_bytes));
			if (n > 0) {
				n = m->input.reader_.read((char *)m->recording_buffer.data() + m->recorded_bytes, n);
				setInputLevel((int16_t const *)(m->recording_buffer.data() + m->recorded_bytes), n / 2);
				m->recorded_bytes += n;
			}
#endif
			float percent = 100 * m->recorded_bytes / m->max_record_size;
			ui->progressBar_recording->setValue(int(percent * 10));
			goto done;
		} else {
			setState(State::Playing);
		}
	}
	{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
		QByteArray ba = m->input.io_->readAll();
		if (ba.size() >= 2) {
			setInputLevel((int16_t const *)ba.data(), ba.size() / 2);
		}
	}
#else
		auto len = m->input.reader_.bytesAvailable();
		QByteArray ba = m->input.reader_.read(len);
		if (ba.size() >= 2) {
			setInputLevel((int16_t const *)ba.data(), ba.size() / 2);
		}
	}
#endif
done:;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	onReadyRead();

	if (m->state == State::Playing) {
		int bytes = m->output.bytesFree();
		if (m->play_bytes < m->recorded_bytes) {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			int bytes2 = std::max(0, 4800 - (int)m->output.bufferedBytes());
			bytes = std::min(bytes, bytes2);
#endif
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
//	if (m->input.audio_input) {
//		disconnect(m->input.input_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
//		m->input.audio_input->stop();
//		m->input.audio_input.reset();
//	}

//	if (index >= 0 && index < m->audio_input_devices.size()) {
//		m->input.audio_input = std::shared_ptr<QAudioInput>(new QAudioInput(m->audio_input_devices[index], m->audio_format));
//		m->input.input_device = m->input.audio_input->start();
//		connect(m->input.input_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
//	}
}

void MainWindow::on_comboBox_output_currentIndexChanged(int index)
{
//	if (m->audio_output) {
//		m->audio_output->stop();
//		m->audio_output.reset();
//	}

//	m->audio_output = std::shared_ptr<QAudioOutput>(new QAudioOutput(m->audio_output_devices[index], m->audio_format));
//	m->output_device = m->audio_output->start();
}
