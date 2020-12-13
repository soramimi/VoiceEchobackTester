#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include "main.h"

class Video;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
private slots:
	void onReadyRead();

	// QObject interface
protected:
	void timerEvent(QTimerEvent *event);
};
#endif // MAINWINDOW_H
