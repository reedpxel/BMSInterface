#ifndef LOGSWIDGET_H
#define LOGSWIDGET_H
#include "graphplotter.h"
#include "maininfoparser.h"
#include <QWidget>
#include <QFile>
#include <QFileDialog>

/* TO DO LIST на завтра:
    - сделать, чтобы прога не ложилась, если что-то из доп. параметров не
прочиталось
    - пофиксить баг, от которого прога ложится, дебаггером
    - если успеешь, начать рисовать графики
*/

namespace Ui {
class LogsWidget;
}

class LogsWidget : public QWidget
{
Q_OBJECT
    GraphPlotter* plotter;
    bool logsKept;
    QFile logsFile;

public:
    explicit LogsWidget(QWidget *parent = nullptr);
    ~LogsWidget();
    bool logsAreKept();
    void stopLogging();

public slots:
    void on_keepLogsButton_clicked();
    void slotWriteMainDataInFile(const MainInfo&);

private:
    Ui::LogsWidget *ui;
};

#endif // LOGSWIDGET_H
