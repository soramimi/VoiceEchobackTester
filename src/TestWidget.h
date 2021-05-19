#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QWidget>

class TestWidget : public QWidget {
	Q_OBJECT
private:
	float percent_ = 0;
protected:
	void paintEvent(QPaintEvent *event);
public:
	explicit TestWidget(QWidget *parent = nullptr);
	void setPercent(float percent);
};

#endif // TESTWIDGET_H
