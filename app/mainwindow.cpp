#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow), reader(), parser()
{
    ui->setupUi(this);
    MainInfoWidget* mi = new MainInfoWidget;
    connect(&parser, SIGNAL(dataReceived(const MainInfo&)),
        mi, SLOT(slotDataReceived(const MainInfo&)));
    QGridLayout* mainInfoLayout = new QGridLayout;
    mainInfoLayout->addWidget(mi);
    ui->mainInfoTab->setLayout(mainInfoLayout);
    AddInfoWidget* ai = new AddInfoWidget;
    ui->scrollArea->setWidget(ai);

    connect(&reader, SIGNAL(noPortsFound(size_t)),
        SLOT(slotNoPortsFound(size_t)));
    connect(&reader, SIGNAL(portOpened(const QString&)),
        SLOT(slotPortOpened(const QString&)));
    connect(&reader, SIGNAL(portDidNotOpen(const QString&)),
        SLOT(slotPortDidNotOpen(const QString&)));
    connect(&reader, SIGNAL(portClosed()),
        SLOT(slotPortClosed()));
    connect(&reader, SIGNAL(dataGot(const QByteArray&)),
        SLOT(slotDataGot(const QByteArray&)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotNoPortsFound(size_t attempt)
{
    statusBar()->showMessage("Attempt " + QString::number(attempt) +
        " : no fitting COM ports found", 2000);
}

void MainWindow::slotPortOpened(const QString& str)
{
    statusBar()->showMessage("COM port " + str + " opened", 2000);
}

void MainWindow::slotPortDidNotOpen(const QString& str)
{
    statusBar()->showMessage("Error when opening COM port " + str, 2000);
}

void MainWindow::slotPortClosed()
{
    statusBar()->showMessage("No reply from device, COM port closed", 2000);
}

void MainWindow::slotDataGot(const QByteArray& arr)
{

    statusBar()->showMessage("Received: " + arr.toHex(' '), 500);
    parser.parseMessage(arr);
}
