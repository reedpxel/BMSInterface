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
    modeIsAutomatic(true),
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
    if (!isConnected)
    {
        port->close();
        emit sgnPortClosed();
    }
}

void COMPortReader::timerEvent(QTimerEvent*)
{
    slotSearchForPorts();
}

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
    emit sgnDataGotAutomatic(ba);
}

void COMPortReader::slotReadyReadManual()
{
    QTest::qWait(100);
    QByteArray ba = port->readAll();
    emit sgnDataGotManual(ba);
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
    QByteArray& msg = writeMsgFlag ? msgToSend03 : msgToSend04;
    writeMsgFlag = writeMsgFlag ? false : true;
    port->write(msg);
}

void COMPortReader::slotSetAutomaticMode()
{
    writeTimer->start(MAIN_INFO_TIMER_STEP);
    modeIsAutomatic = true;
    QObject::connect(port, SIGNAL(readyRead()), this,
        SLOT(slotReadyReadAutomatic()));
    QObject::disconnect(port, SIGNAL(readyRead()), this,
        SLOT(slotReadyReadManual()));
}

void COMPortReader::slotSetManualMode()
{
    QObject::connect(port, SIGNAL(readyRead()), this,
        SLOT(slotReadyReadManual()));
    QObject::disconnect(port, SIGNAL(readyRead()), this,
        SLOT(slotReadyReadAutomatic()));
    writeTimer->stop();
    modeIsAutomatic = false;
    emit sgnManualModeIsSet();
}

void COMPortReader::slotWriteManually(const QByteArray& message)
{
    port->write(message);
}

