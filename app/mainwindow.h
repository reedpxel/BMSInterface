#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtWidgets>
#include <QTime>
#include "maininfowidget.h"
#include "addinfowidget.h"
#include "logswidget.h"

class MainInfoWidget;
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

    QMenuBar* menubar_;
    MainInfoWidget* mainInfoWidget;
    AddInfoWidget* addInfoWidget;
    LogsWidget* logsWidget;

    COMPortReader* reader;

    QWidget* noConnectionWidget;
    QPixmap* noConnectionPixmap;
    QProgressBar* addInfoReadingProgress;

    void drawNoConnectionWindow();
    void drawConnectedWindow();
    void drawMenuBar();
public:
    MainWindow(COMPortReader* reader);
    ~MainWindow();
    COMPortReader* getReader();
signals:
    void sgnMessageGot(const QByteArray&);
public slots:
    void slotNoPortsFound(size_t);
    void slotPortOpened(const QString&);
    void slotPortDidNotOpen(const QString&);
    void slotPortClosed();
    void slotAddInfoReadBegun();
    void slotAddInfoUpdated(unsigned currentValue);
    void slotAddInfoReadEnded();
    void slotSetStatusBarMessage(const QByteArray&);
private slots:
    void slotShowReceivedData(bool checked);
    void slotWaitReply();
    void slotLightAction();
    void slotDarkAction();
    void slotCreateChangeDeviceWindow();
};

#endif // MAINWINDOW_H
