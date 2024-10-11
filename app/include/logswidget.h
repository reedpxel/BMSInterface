#ifndef LOGSWIDGET_H
#define LOGSWIDGET_H
#include "qcustomplot.h"
#include "maininfoparser.h"
#include <QWidget>
#include <QTime>
#include <QFile>
#include <QFileDialog>
#include <QList>
#include <vector>

namespace Ui {
class LogsWidget;
}

class LogsWidget : public QWidget
{
Q_OBJECT
    QCustomPlot* customPlot;
    QList<std::pair<double, QTime>> totalVoltageDots;
    QList<std::pair<double, QTime>> currentDots;
    QList<std::pair<double, QTime>> currentCapacityDots;
    std::vector<QList<std::pair<double, QTime>>> temperatureDots;
    std::vector<QList<std::pair<double, QTime>>> lineVoltageDots;
    QList<std::pair<double, QTime>> differenceDots;
    bool logsKept;
    QFile logsFile;
    int graphDataListSize;
    std::vector<bool> radioButtonsState;

    int getCurrentGraph();
    void repaintGraph();
public:
    explicit LogsWidget(QWidget* parent = nullptr);
    ~LogsWidget();
    bool logsAreKept();
    void stopLogging();

public slots:
    void on_keepLogsButton_clicked();
    void slotWriteMainDataInFile(const MainInfo&);
    void slotUpdate03Data(const MainInfo&);
    void slotUpdate04Data(const MainInfo&);
    void on_radioButtonTotalVoltage_clicked();
    void on_radioButtonCurrent_clicked();
    void on_radioButtonCurrentCapacity_clicked();
    void on_radioButtonTemperatures_clicked();
    void on_radioButtonLineVoltage_clicked();
    void on_radioButtonDifference_clicked();

private:
    Ui::LogsWidget *ui;
};

#endif // LOGSWIDGET_H
