#ifndef MAININFO_H
#define MAININFO_H
#include "batteryslider.h"
#include "maininfoparser.h"
#include <QWidget>

namespace Ui {
class MainInfoWidget;
}

class MainInfoWidget : public QWidget
{
Q_OBJECT
    BatterySlider* batterySlider;

public:
    explicit MainInfoWidget(QWidget *parent = nullptr);
    virtual void resizeEvent(QResizeEvent*);
    ~MainInfoWidget();

private:
    Ui::MainInfoWidget *ui;
public slots:
    void slotDataReceived(const MainInfo&);
};

#endif // MAININFO_H
