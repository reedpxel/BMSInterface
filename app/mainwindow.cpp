#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::drawNoConnectionWindow()
{
    delete ui;
    ui = nullptr; // if "delete ui" in ~MainWindow() called, delete nullptr
//is ok, delete some value that is already deleted is UB
    noConnectionWidget = new QWidget;
    QLabel* pixelLabel = new QLabel;
    pixelLabel->setPixmap(*noConnectionPixmap);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addStretch();
    layout->addWidget(pixelLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    layout->addSpacerItem(new QSpacerItem(0, 25));
    layout->addWidget(new QLabel(
        "No fitting COM ports found. Make sure the USB-UART converter is "
        "connected and is not used by other processes."), 0, Qt::AlignHCenter |
        Qt::AlignTop);
    layout->addStretch();
    noConnectionWidget->setLayout(layout);
    setCentralWidget(noConnectionWidget);
}

void MainWindow::drawConnectedWindow()
{
    delete noConnectionWidget;
    noConnectionWidget = nullptr;
    ui = new Ui::MainWindow;
    ui->setupUi(this);
    MainInfoWidget* mainInfoWidget = new MainInfoWidget;
    QGridLayout* mainInfoLayout = new QGridLayout;
    mainInfoLayout->addWidget(mainInfoWidget);
    ui->mainInfoTab->setLayout(mainInfoLayout);
    addInfoWidget = new AddInfoWidget(this);
    ui->scrollArea->setWidget(addInfoWidget);
    logsWidget = new LogsWidget(this);
    QGridLayout* logsLayout = new QGridLayout;
    logsLayout->addWidget(logsWidget);
    ui->logsTab->setLayout(logsLayout);
    QObject::connect(addInfoWidget->getParser(),
        SIGNAL(sgnReadingBegun()),
        SLOT(slotAddInfoReadBegun()));
    QObject::connect(addInfoWidget->getParser(),
        SIGNAL(sgnReadingUpdate(unsigned)), SLOT(slotAddInfoUpdated(unsigned)));
    QObject::connect(addInfoWidget->getParser(),
        SIGNAL(sgnReadingEnded()), SLOT(slotAddInfoReadEnded()));
    // main info widget
    connect(&reader, SIGNAL(sgnDataGotAutomatic(const QByteArray&)),
        mainInfoWidget->getParser(), SLOT(slotParseMessage(const QByteArray&)));
    // connections that send messages to status bar
    connect(mainInfoWidget->getParser(),
        SIGNAL(sgnMessageGot(const QByteArray&)),
        SLOT(slotSetStatusBarMessage(const QByteArray&)));
    connect(addInfoWidget->getParser(),
        SIGNAL(sgnUncheckedMessageGot(const QByteArray&)),
        SLOT(slotSetStatusBarMessage(const QByteArray&)));
    // handling absense of BMS
    connect(&reader, SIGNAL(sgnNoBMS()), mainInfoWidget, SLOT(slotNoAnswer()));
    connect(&reader, &COMPortReader::sgnNoBMS,
        [this](){ statusBar()->showMessage("No reply from BMS"); });
    // sending main data to logsWidget
    connect(mainInfoWidget->getParser(),
        SIGNAL(sgnData03Updated(const MainInfo&)),
        logsWidget, SLOT(slotWriteMainDataInFile(const MainInfo&)));
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    ui(nullptr),
    reader(),
    noConnectionPixmap(nullptr),
    noConnectionWidget(nullptr),
    addInfoWidget(nullptr)
{
    noConnectionPixmap = new QPixmap(
        QPixmap(":/new/images/no-connection.png").scaled(QSize(300, 300),
        Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setMinimumSize(1150, 650);
    setWindowTitle("BMSInterface");

    drawNoConnectionWindow();

    statusBar()->showMessage("");
    connect(&reader, SIGNAL(sgnNoPortsFound(size_t)),
        SLOT(slotNoPortsFound(size_t)));
    connect(&reader, SIGNAL(sgnPortOpened(const QString&)),
        SLOT(slotPortOpened(const QString&)));
    connect(&reader, SIGNAL(sgnPortDidNotOpen(const QString&)),
        SLOT(slotPortDidNotOpen(const QString&)));
    connect(&reader, SIGNAL(sgnPortClosed()),
        SLOT(slotPortClosed()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete noConnectionWidget;
}

COMPortReader* MainWindow::getReader() { return &reader; }

void MainWindow::slotNoPortsFound(size_t attempt)
{
    statusBar()->showMessage("Attempt " + QString::number(attempt) +
        " : no fitting COM ports found", 2000);
}

void MainWindow::slotPortOpened(const QString& str)
{
    drawConnectedWindow();
    statusBar()->showMessage("COM port " + str + " opened", 2000);
}

void MainWindow::slotPortDidNotOpen(const QString& str)
{
    statusBar()->showMessage("Error when opening COM port " + str, 2000);
}

void MainWindow::slotPortClosed()
{
    drawNoConnectionWindow();
    statusBar()->showMessage("No reply from device, COM port closed", 2000);
}

void MainWindow::slotAddInfoReadBegun()
{
    addInfoReadingProgress = new QProgressBar(ui->statusBar);
    addInfoReadingProgress->setStyleSheet("QProgressBar{text-align : middle;}"
        " QProgressBar::chunk{background-color : #00ff00;}");
    addInfoReadingProgress->setMaximumSize(200, 20);
    ui->statusBar->addPermanentWidget(addInfoReadingProgress);
    addInfoReadingProgress->setRange(0, addInfoWidget->getParser()->
        getAmountOfQueries());
}

void MainWindow::slotAddInfoUpdated(unsigned currentValue)
{
    addInfoReadingProgress->setValue(currentValue);
}

void MainWindow::slotAddInfoReadEnded()
{
    delete addInfoReadingProgress;
    addInfoReadingProgress = nullptr; // is it necessary?
}

void MainWindow::slotSetStatusBarMessage(const QByteArray& ba)
{
    statusBar()->showMessage("Recieved: " + ba.toHex(' '), 2000);
}

void MainWindow::on_actionShow_received_data_triggered(bool checked)
{
    if (checked)
    {
        connect(mainInfoWidget->getParser(),
            SIGNAL(sgnMessageGot(const QByteArray&)),
            SLOT(slotSetStatusBarMessage(const QByteArray&)));
        connect(addInfoWidget->getParser(),
            SIGNAL(sgnUncheckedMessageGot(const QByteArray&)),
            SLOT(slotSetStatusBarMessage(const QByteArray&)));
    } else {
        QObject::disconnect(mainInfoWidget->getParser(),
            SIGNAL(sgnMessageGot(const QByteArray&)),
            this, SLOT(slotSetStatusBarMessage(const QByteArray&)));
        QObject::disconnect(addInfoWidget->getParser(),
            SIGNAL(sgnUncheckedMessageGot(const QByteArray&)), this,
            SLOT(slotSetStatusBarMessage(const QByteArray&)));
    }
}
