#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "maininfowidget.h"
#include "addinfowidget.h"
#include "comportreader.h"
#include "maininfoparser.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT
    Ui::MainWindow *ui;
    COMPortReader reader;
    MainInfoParser parser;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void slotNoPortsFound(size_t);
    void slotPortOpened(const QString&);
    void slotPortDidNotOpen(const QString&);
    void slotPortClosed();
    void slotDataGot(const QByteArray&);
};
#endif // MAINWINDOW_H
