#include "batteryslider.h"

BatterySlider::BatterySlider(QWidget* parent) : QWidget(parent), percent_(50),
    isActive(false), batteryPixmap(new QPixmap(":/images/battery.png"))
    {}

void BatterySlider::paintEvent(QPaintEvent*)
{
    QPainter painter_(this);
    painter_.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QRectF greenRect;
    if (isActive)
    {
        if (percent_ <= 0)
        {
            greenRect = QRectF(
                0.07 * width(), // 0.0678
                0.913 * height(),
                0.83 * width(), // 0.864
                0);
        } else if (percent_ >= 100) {
            greenRect = QRectF(
                0.07 * width(),
                0.127 * height(),
                0.83 * width(),
                0.786 * height());
        } else {
            greenRect = QRectF(
                0.07 * width(),
                (0.786 * (100 - percent_) / 100. + 0.127)* height(),
                0.83 * width(),
                0.786 * percent_ / 100. * height());
        }
        painter_.fillRect(greenRect, QBrush(QColor(0, 255, 0)));
        painter_.setPen(QPen(QColor(0, 0, 0), 20));
        painter_.setFont(QFont("Times new roman", 20));
        QFont f("Arial", 26, QFont::Bold);
        painter_.setFont(f);
        QLabel label(QString::number(percent_) + " %");
        label.setFont(f);
        painter_.drawText((width() -
            label.fontMetrics().boundingRect(label.text()).width()) / 2,
            (height() + 13) / 2, QString::number(percent_) + " %");
    }
    painter_.drawPixmap(QRect(0, 0, width(), height()), *batteryPixmap);
}

void BatterySlider::slotSetValue(int percent)
{
    percent_ = percent;
    repaint();
}

void BatterySlider::slotSetActive(bool isActive_)
{
    isActive = isActive_;
    repaint();
}
