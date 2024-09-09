#ifndef MAININFO_H
#define MAININFO_H
#include "batteryslider.h"
#include "maininfoparser.h"
#include "mainwindow.h"
#include <QWidget>

namespace Ui {
class MainInfoWidget;
}

class MainInfoWidget : public QWidget
{
Q_OBJECT
    MainInfoParser parser_;

    BatterySlider* batterySlider;
    bool isActive;

public:
    explicit MainInfoWidget(QWidget* parent = nullptr);
    virtual void resizeEvent(QResizeEvent*);
    ~MainInfoWidget();
    MainInfoParser* getParser();

private:
    Ui::MainInfoWidget* ui;
public slots:
    void slotDataUpdated(const MainInfo&);
    void slotData03Updated(const MainInfo&);
    void slotData04Updated(const MainInfo&);
    void slotNoAnswer();
private slots:
    void on_FETChargeButton_clicked();
    void on_FETDischargeButton_clicked();
};

#endif // MAININFO_H
