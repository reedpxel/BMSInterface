#include "mainwindow.h"
#include "comportreader.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QFile styleFile(":/stylesheet.qss");
    styleFile.open(QFile::ReadOnly);
    if (styleFile.isOpen())
    {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }
    COMPortReader* reader = new COMPortReader;
    MainWindow* mw = new MainWindow(reader);
    mw->show();
    return app.exec();
}
