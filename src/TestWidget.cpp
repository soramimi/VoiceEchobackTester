#include "TestWidget.h"

#include <QPainter>

TestWidget::TestWidget(QWidget *parent)
	: QWidget(parent)
{
}

void TestWidget::paintEvent(QPaintEvent *event)
{
	QPainter pr(this);
	QRect r = rect();
	pr.fillRect(r, Qt::black);
	r.adjust(1, 1, -1, -1);
	r.setWidth(r.width() * percent_);
	pr.fillRect(r, Qt::green);
}

void TestWidget::setPercent(float percent)
{
	percent_ = percent;
	update();
}
