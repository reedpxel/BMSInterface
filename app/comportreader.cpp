#include "comportreader.h"

void COMPortReader::openPort(const QString& portName_)
{
    port->setPortName(portName_);
    port->setBaudRate(QSerialPort::Baud9600);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    if (port->open(QIODevice::ReadWrite)) {
        emit sgnPortOpened(port->portName());
        isConnected = true;
        killTimer(timerId);
        connect(writeTimer, SIGNAL(timeout()), SLOT(slotWriteAutomatic()));
        writeTimer->start(MAIN_INFO_TIMER_STEP);
    } else {
        emit sgnPortDidNotOpen(port->portName());
    }
}

COMPortReader::COMPortReader() : QObject(nullptr),
    port(new QSerialPort),
    waitingReply(false),
    modeIsAutomatic(true),
    lastQueryAnswered(false),
    writeTimer(new QTimer),
    timerId(startTimer(1000)),
    isConnected(false),
    attempt(1),
    writeMsgFlag(false)
{
    uint8_t arr03[7] = {0xdd, 0xa5, 0x03, 0x00, 0xff, 0xfd, 0x77};
    uint8_t arr04[7] = {0xdd, 0xa5, 0x04, 0x00, 0xff, 0xfc, 0x77};
    msgToSend03 = QByteArray(reinterpret_cast<char*>(arr03), 7);
    msgToSend04 = QByteArray(reinterpret_cast<char*>(arr04), 7);
    connect(port, SIGNAL(readyRead()), SLOT(slotReadyReadAutomatic()));
}

COMPortReader::~COMPortReader()
{
    std::cout << "~COMPortReader()\n";
    if (!isConnected)
    {
        port->close();
        emit sgnPortClosed();
    }
}

void COMPortReader::timerEvent(QTimerEvent*) { slotSearchForPorts(); }

bool COMPortReader::slotSearchForPorts()
{
    auto ports = QSerialPortInfo::availablePorts();
    for (auto port_ : ports)
    {
        QString port_description = port_.description();
        if (port_description == "USB Serial")
        {
            attempt = 1;
            try
            {
                openPort(port_.portName());
                return true;
            } catch (...) {
                return false;
            }
        }
    }
    emit sgnNoPortsFound(attempt);
    ++attempt;
    return false;
}

void COMPortReader::slotReadyReadAutomatic()
{
    QTest::qWait(MAIN_INFO_AUTOMATIC_DELAY);
    QByteArray ba = port->readAll();
    lastQueryAnswered = true;
    emit sgnDataGotAutomatic(ba);
    std::cout << "Received automatically: ";
    for (int i = 0; i < ba.size(); ++i)
    {
        std::cout << std::hex << (static_cast<uint16_t>(ba[i]) & 0xff) << ' ';
    }
    std::cout << '\n';
}

void COMPortReader::slotReadyReadManual()
{
    QTest::qWait(100);
    QByteArray ba = port->readAll();
    lastQueryAnswered = true;
    emit sgnDataGotManual(ba);
    waitingReply = false;
    std::cout << "Recieved manually: ";
    for (int i = 0; i < ba.size(); ++i)
    {
        std::cout << std::hex << (static_cast<uint16_t>(ba[i]) & 0xff) << ' ';
    }
    std::cout << '\n';
}

void COMPortReader::slotNoAnswer()
{
    if (port->isOpen())
    {
        port->close();
        emit sgnPortClosed();
        startTimer(NO_PORTS_TIMER_DELAY);
    }
}

void COMPortReader::slotWriteAutomatic()
{
    if (!lastQueryAnswered) emit sgnNoBMS();
    lastQueryAnswered = false;
    QByteArray& msg = writeMsgFlag ? msgToSend03 : msgToSend04;
    writeMsgFlag = writeMsgFlag ? false : true;
    waitingReply = true;
    port->write(msg);
    std::cout << "Sent automatically: ";
    for (int i = 0; i < msg.size(); ++i)
    {
        std::cout << std::hex << (static_cast<uint16_t>(msg[i]) & 0xff) << ' ';
    }
    std::cout << '\n';
}

void COMPortReader::slotSetAutomaticMode()
{
    writeTimer->start(MAIN_INFO_TIMER_STEP);
    modeIsAutomatic = true;
    QObject::connect(port, SIGNAL(readyRead()),
        SLOT(slotReadyReadAutomatic()));
    QObject::disconnect(port, SIGNAL(readyRead()), this,
        SLOT(slotReadyReadManual()));
}

void COMPortReader::slotSetManualMode()
{
    writeTimer->stop();
    if (waitingReply)
    {
        // waiting until reply got
        QSignalSpy spy(this, SIGNAL(sgnDataGotAutomatic(const QByteArray&)));
        spy.wait(SET_MANUAL_MODE_WAITING_LIMIT);
    }
    QObject::disconnect(port, SIGNAL(readyRead()), this,
        SLOT(slotReadyReadAutomatic()));
    QObject::connect(port, SIGNAL(readyRead()),
        SLOT(slotReadyReadManual()));
    modeIsAutomatic = false;
    emit sgnManualModeIsSet();
}

void COMPortReader::slotWriteManually(const QByteArray& message)
{
    if (!lastQueryAnswered) emit sgnNoBMS();
    lastQueryAnswered = false;
    port->write(message);
    std::cout << "Sent manually: ";
    for (int i = 0; i < message.size(); ++i)
    {
        std::cout << std::hex << (static_cast<uint16_t>(message[i]) & 0xff) <<
            ' ';
    }
    std::cout << '\n';
}

void COMPortReader::slotWriteQueries(const std::vector<QByteArray>& queries)
{
    slotSetManualMode();
    port->readAll();
    for (auto& query : queries)
    {
        slotWriteManually(query);
        QSignalSpy spy(port, SIGNAL(readyRead()));
        spy.wait(MAIN_INFO_MANUAL_DELAY);
        // TO DO: check if the incoming byte is 0x77, chech CRC and consider a
        // message to be got if crc is correct
        QTest::qWait(70);
        slotReadyReadManual();
    }
    slotSetAutomaticMode();
}
