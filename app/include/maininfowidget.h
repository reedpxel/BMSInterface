#ifndef MAININFO_H
#define MAININFO_H
#include "batteryslider.h"
#include "maininfoparser.h"
#include "mainwindow.h"

namespace Ui {
class MainInfoWidget;
}

class MainInfoWidget : public QWidget
{
Q_OBJECT
    MainInfoParser parser_;

    BatterySlider* batterySlider;
    bool isActive;
    bool instantlyChangeParameters;

public:
    explicit MainInfoWidget(QWidget* parent = nullptr);
    virtual void resizeEvent(QResizeEvent*);
    ~MainInfoWidget();
    MainInfoParser* getParser();
    void makeReadOnly(QCheckBox* checkBox);
private:
    Ui::MainInfoWidget* ui;
public slots:
    void slotDataUpdated(const MainInfo&);
    void slotData03Updated(const MainInfo&);
    void slotData04Updated(const MainInfo&);
    void slotNoAnswer();
    void slotChangeInstantlyChangeParameters();
};

#endif // MAININFO_H
