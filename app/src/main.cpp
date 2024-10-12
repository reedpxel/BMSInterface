#include "mainwindow.h"
#include "comportreader.h"
#include <QApplication>
#include "changepasswordwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    COMPortReader* reader = new COMPortReader;
    MainWindow* mw = new MainWindow(reader);
    mw->show();
    return app.exec();
}
