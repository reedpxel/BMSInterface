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

    mainInfoWidget = new MainInfoWidget(this);
    QGridLayout* mainInfoLayout = new QGridLayout;
    mainInfoLayout->addWidget(mainInfoWidget);
    ui->mainInfoTab->setLayout(mainInfoLayout);

    addInfoWidget = new AddInfoWidget(this);
    ui->scrollArea->setWidget(addInfoWidget);

    logsWidget = new LogsWidget(this);
    QGridLayout* logsLayout = new QGridLayout;
    logsLayout->addWidget(logsWidget);
    ui->logsTab->setLayout(logsLayout);

    drawMenuBar();

    // reading additional data if addInfoWidget tab clicked
    QObject::connect(ui->tabWidget, SIGNAL(currentChanged(int)), addInfoWidget,
        SLOT(slotOnTabChosen(int)));

    // drawing a progress bar while reading of add data proceeds
    QObject::connect(addInfoWidget->getParser(), SIGNAL(sgnReadingBegun()),
        SLOT(slotAddInfoReadBegun()));
    QObject::connect(addInfoWidget->getParser(),
        SIGNAL(sgnReadingUpdate(unsigned)), SLOT(slotAddInfoUpdated(unsigned)));
    QObject::connect(addInfoWidget->getParser(),
        SIGNAL(sgnReadingEnded()), SLOT(slotAddInfoReadEnded()));
    // main info widget
    // connect(&reader, SIGNAL(sgnDataGotAutomatic(const QByteArray&)),
    //     mainInfoWidget->getParser(), SLOT(slotParseMessage(const QByteArray&)));

    // connections that send messages to status bar
    connect(reader, SIGNAL(sgnDataGotManual(const QByteArray&)),
        SLOT(slotSetStatusBarMessage(QByteArray)));
    connect(reader, SIGNAL(sgnDataGotAutomatic(QByteArray)),
        SLOT(slotSetStatusBarMessage(const QByteArray&)));

    // handling absense of BMS
    connect(reader, SIGNAL(sgnNoBMS()), mainInfoWidget, SLOT(slotNoAnswer()));
    connect(reader, &COMPortReader::sgnNoBMS,
        [this](){ statusBar()->showMessage("No reply from BMS"); });
    // sending main data to logsWidget
    connect(mainInfoWidget->getParser(),
        SIGNAL(sgnData03Updated(const MainInfo&)),
        logsWidget, SLOT(slotWriteMainDataInFile(const MainInfo&)));
    // setting data from mainInfoParser on graph
    connect(mainInfoWidget->getParser(),
        SIGNAL(sgnData03Updated(const MainInfo&)),
        logsWidget,
        SLOT(slotUpdate03Data(const MainInfo&)));
    connect(mainInfoWidget->getParser(),
        SIGNAL(sgnData04Updated(const MainInfo&)),
        logsWidget,
        SLOT(slotUpdate04Data(const MainInfo&)));
}

void MainWindow::drawMenuBar()
{
    menubar_ = new QMenuBar(this);
    QMenu* comPortMenu = new QMenu("&COM port");
    QMenu* viewMenu = new QMenu("&View");

    // disconnect
    QAction* disconnectAction = new QAction("Disconnect");
    QObject::connect(disconnectAction, SIGNAL(triggered()),
        reader, SLOT(slotDisconnect()));
    comPortMenu->addAction(disconnectAction);

    // change USB-Serial device name
    QAction* changeDeviceNameAction = new QAction("Change device name");
    QObject::connect(changeDeviceNameAction, SIGNAL(triggered(bool)),
        SLOT(slotCreateChangeDeviceWindow()));
    comPortMenu->addAction(changeDeviceNameAction);

    // show received data
    QAction* showReceivedDataAction = new QAction("Show received data");
    showReceivedDataAction->setCheckable(true);
    showReceivedDataAction->setChecked(true);
    QObject::connect(showReceivedDataAction, SIGNAL(triggered(bool)),
        SLOT(slotShowReceivedData(bool)));
    viewMenu->addAction(showReceivedDataAction);

    // wait for reply before changing text
    QAction* waitReplyAction = new QAction("Wait for reply before "
        "changing text");
    waitReplyAction->setCheckable(true);
    waitReplyAction->setChecked(true);
    QObject::connect(waitReplyAction, SIGNAL(triggered()),
        SLOT(slotWaitReply()));
    viewMenu->addAction(waitReplyAction);

    menubar_->addMenu(comPortMenu);
    menubar_->addMenu(viewMenu);
    setMenuBar(menubar_);
}

MainWindow::MainWindow(COMPortReader* reader) : QMainWindow(nullptr),
    ui(nullptr),
    mainInfoWidget(nullptr),
    addInfoWidget(nullptr),
    logsWidget(nullptr),
    reader(reader),
    noConnectionWidget(nullptr),
    noConnectionPixmap(nullptr),
    addInfoReadingProgress(nullptr)
{
    noConnectionPixmap = new QPixmap(
        QPixmap(":/images/no-connection.png").scaled(QSize(300, 300),
        Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setMinimumSize(1150, 650);
    setWindowTitle("BMSInterface");

    drawNoConnectionWindow();
    drawMenuBar();

    statusBar()->showMessage("");
    connect(reader, SIGNAL(sgnNoPortsFound(size_t)),
        SLOT(slotNoPortsFound(size_t)));
    connect(reader, SIGNAL(sgnPortOpened(const QString&)),
        SLOT(slotPortOpened(const QString&)));
    connect(reader, SIGNAL(sgnPortDidNotOpen(const QString&)),
        SLOT(slotPortDidNotOpen(const QString&)));
    connect(reader, SIGNAL(sgnPortClosed()),
        SLOT(slotPortClosed()));
}

MainWindow::~MainWindow()
{
    delete reader;
    delete ui;
    delete noConnectionWidget;
}

COMPortReader* MainWindow::getReader() { return reader; }

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
    statusBar()->showMessage("COM port closed", 2000);
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

void MainWindow::slotShowReceivedData(bool checked)
{
    if (checked)
    {
        connect(reader, SIGNAL(sgnDataGotManual(const QByteArray&)),
            SLOT(slotSetStatusBarMessage(QByteArray)));
        connect(reader, SIGNAL(sgnDataGotAutomatic(QByteArray)),
            SLOT(slotSetStatusBarMessage(const QByteArray&)));
    } else {
        QObject::disconnect(reader,
            SIGNAL(sgnDataGotManual(const QByteArray&)),
            this, SLOT(slotSetStatusBarMessage(QByteArray)));
        QObject::disconnect(reader, SIGNAL(sgnDataGotAutomatic(QByteArray)),
            this, SLOT(slotSetStatusBarMessage(const QByteArray&)));
    }
}

void MainWindow::slotWaitReply()
{
    if (mainInfoWidget) mainInfoWidget->slotChangeInstantlyChangeParameters();
    if (addInfoWidget) addInfoWidget->slotChangeInstantlyChangeParameters();
}

void MainWindow::slotLightAction()
{
    std::cout << "light\n";
}

void MainWindow::slotDarkAction()
{
    std::cout << "dark\n";
}

void MainWindow::slotCreateChangeDeviceWindow()
{
    bool ok;
    QString newName = QInputDialog::getText(this, tr("Set new device name"),
        tr("Device name: "), QLineEdit::Normal, reader->getDeviceName(), &ok);
    if (ok) reader->slotSetDeviceName(newName);
}
