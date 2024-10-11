#ifndef BATTERYSLIDER_H
#define BATTERYSLIDER_H
#include <QtWidgets>

class BatterySlider : public QWidget
{
Q_OBJECT
    int percent_;
    bool isActive;
    QPixmap* batteryPixmap;
public:
    BatterySlider(QWidget* parent = nullptr);
    virtual void paintEvent(QPaintEvent*);
public slots:
    void slotSetValue(int percent);
    void slotSetActive(bool);
};

#endif // BATTERYSLIDER_H
