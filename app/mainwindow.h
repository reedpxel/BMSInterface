#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtWidgets>
#include "maininfowidget.h"
#include "addinfowidget.h"
#include "comportreader.h"
#include "logswidget.h"

class AddInfoWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT
    Ui::MainWindow* ui;

    MainInfoWidget* mainInfoWidget;
    AddInfoWidget* addInfoWidget;
    LogsWidget* logsWidget;

    COMPortReader reader;

    MainInfoParser parser; // объект этого класса должен быть в MainInfoWidget
    // ^ TO DO !!!
    QWidget* noConnectionWidget;
    QPixmap* noConnectionPixmap;
    QProgressBar* addInfoReadingProgress;

    void drawNoConnectionWindow();
    void drawConnectedWindow();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    COMPortReader* getReader();
signals:
    void sgnMessageGot(const QByteArray&);
public slots:
    void slotNoPortsFound(size_t);
    void slotPortOpened(const QString&);
    void slotPortDidNotOpen(const QString&);
    void slotPortClosed();
    void slotDataGot(const QByteArray&);
    void slotAddInfoReadBegun();
    void slotAddInfoUpdated(unsigned currentValue);
    void slotAddInfoReadEnded();
    void slotSetStatusBarMessage(const QByteArray&);
private slots:
    void on_actionShow_received_data_triggered(bool checked);
};

#endif // MAINWINDOW_H
