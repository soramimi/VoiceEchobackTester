#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include "main.h"
#include <stdint.h>

class Video;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LevelMeterWidget;

enum class State {
	Stop,
	Recording,
	Playing
};

class MainWindow : public QDialog {
	Q_OBJECT
	friend class ImageWidget;
	friend class InternalImageView;
	friend class ThumbnailCache;
private:
	Ui::MainWindow *ui;
    struct Private;
	Private *m;
	void start();
	void setLevel(const int16_t *p, int n, LevelMeterWidget *w);
	void setInputLevel(const int16_t *p, int n);
	void setOutputLevel(const int16_t *p, int n);
	void setLength(int seconds);
	void setState(State state);
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
private slots:
	void onReadyRead();

	// QObject interface
	void on_pushButton_start_clicked();

	void on_comboBox_input_currentIndexChanged(int index);

	void on_comboBox_output_currentIndexChanged(int index);

protected:
	void timerEvent(QTimerEvent *event);
};
#endif // MAINWINDOW_H
