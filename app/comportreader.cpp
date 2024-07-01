#include "comportreader.h"

void COMPortReader::openPort(const QString& portName_)
{
    port->setPortName(portName_);
    port->setBaudRate(QSerialPort::Baud9600);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    if (port->open(QIODevice::ReadWrite)) {
        emit portOpened(port->portName());
        isConnected = true;
        killTimer(timerId);
    } else {
        emit portDidNotOpen(port->portName());
    }
}

COMPortReader::COMPortReader() : QObject(nullptr), port(new QSerialPort),
    timerId(startTimer(1000)), isConnected(false), attempt(1)
{
    connect(port, SIGNAL(readyRead()), SLOT(slotReadyRead()));
}

COMPortReader::~COMPortReader()
{
    if (!isConnected)
    {
        port->close();
        emit portClosed();
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
        if (port_.description() == "USB Serial")
        {
            attempt = 1;
            try
            {
                openPort(port_.portName());
                return true;
            } catch (...) {
                qDebug() << "port " << port_.portName() << " didn't open";
                return false;
            }
        }
    }
    emit noPortsFound(attempt);
    ++attempt;
    return false;
}

void COMPortReader::slotReadyRead()
{
    QTest::qWait(70);
    QByteArray ba = port->readAll();
    emit dataGot(ba);
}
