#ifndef BATTERYSLIDER_H
#define BATTERYSLIDER_H
#include <QtWidgets>

class BatterySlider : public QWidget
{
Q_OBJECT
    int value;

public:
    virtual void paintEvent(QPaintEvent*);
    BatterySlider(QWidget* parent = nullptr);
    int percent();
public slots:
    void setPercent(int percent);
};

#endif // BATTERYSLIDER_H
