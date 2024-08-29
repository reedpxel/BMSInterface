#include "batteryslider.h"

void BatterySlider::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QColor color_;
    if (value <= 0) color_ = QColor(255, 0, 0);
    else if (value >= 100) color_ = QColor(0, 255, 0);
    else color_ = QColor(255 * pow((100 - value) / 100., 0.1),
        255 * pow((value / 100.), 0.6), 30);
    QBrush brush_(color_);
    painter.fillRect(QRect(0, 0, width(), height()), brush_);
    painter.setPen(QPen(QBrush(QColor(0, 0, 0)), 3));
    painter.drawRect(0, 0, width(), height());
    painter.setPen(QPen(QBrush(QColor(255, 255, 255)), 10));
    QFont f("Arial", 26, QFont::Bold);
    painter.setFont(f);
    QLabel label(QString::number(value) + " %");
    label.setFont(f);
    painter.drawText((width() -
        label.fontMetrics().boundingRect(label.text()).width()) / 2,
        (height() + 13) / 2, QString::number(value) + " %");
}

BatterySlider::BatterySlider(QWidget* parent) : QWidget(parent), value(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void BatterySlider::slotSetPercent(int percent)
{
    value = percent;
    repaint();
}

int BatterySlider::percent() { return value; }
