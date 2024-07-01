#include "bmsreader.h"

QByteArray BMSReader::writeRegisterMessage(uint8_t register_,
    std::vector<uint8_t> value)
{
    std::vector<uint8_t> tmp(value.size() + 3, 0x5A);
    tmp[1] = register_;
    tmp[2] = value.size();
    for (int i = 0; i < value.size(); ++i)
    {
        tmp[i + 3] = value[i];
    }
    for (auto x : tmp) qDebug() << x;
    return getMessage(tmp);
}

QByteArray BMSReader::readRegisterMessage(uint8_t register_)
{
    std::vector<uint8_t> tmp(3, 0xA5);
    tmp[1] = register_;
    tmp[2] = '\0';
    return getMessage(tmp);
}

QByteArray BMSReader::getMessage(std::vector<uint8_t>& arr)// arr = {0xA5, 0x03, 0x00}
{
    std::vector<uint8_t> resVec(arr.size() + 4);
    resVec.front() = 0xDD;
    int crc = 0x10000;
    for (int i = 0; i < arr.size(); ++i)
    {
        resVec[i + 1] = arr[i];
        if (!i) continue;
        crc -= arr[i];
    }
    resVec[arr.size() + 1] = crc >> 8;
    resVec[arr.size() + 2] = crc;
    resVec[arr.size() + 3] = 0x77;
    QByteArray res = QByteArray(reinterpret_cast<const char*>(resVec.data()),
        resVec.size());
    return res;
}

void BMSReader::openPort(const QString& portName_)
{
    port->setPortName(portName_);
    port->setBaudRate(QSerialPort::Baud9600);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    if (port->open(QIODevice::ReadWrite)) {
        // std::cout << (port->portName()).toStdString() << "port opened\n";
        emit portOpened(QString(port->portName() + " port opened"));
        connect(port, SIGNAL(readyRead()), SLOT(slotReadyRead()));
        startTimer(1000);


    } else {
        throw std::invalid_argument("port didn't open\n");
    }
}

QByteArray BMSReader::numToChar(char ch)
{
    char low = ch & 0b00001111;
    char high = ch >> 4;
    QByteArray res(3, '\0');
    res[0] = high;
    res[1] = low;
    res[2] = ' ';
    return res;
}

bool BMSReader::arrayIsViableAnswer(const QByteArray& arr)
{
    if (arr.size() < 3) return false;
    const uint8_t* pArr = reinterpret_cast<const uint8_t*>(arr.data());
    if (pArr[0] != 0xDD || pArr[arr.size() - 1] != 0x77) return false;
    if (pArr[2] != 0x00) return false;
    int crc = 0x10000;
    for (int i = 3; i < arr.size() - 3; ++i)
    {
        crc -= pArr[i];
    }
    if (pArr[arr.size() - 3] != (crc >> 8) || pArr[arr.size() - 2] != crc)
    {
        return false;
    }
    return true;
}

BMSReader::BMSReader() : QObject(), port(new QSerialPort), register03Read(true),
    mainInfoParser()
{
    if (!slotSearchForPorts())
    {
        openTimer = new QTimer;
        connect(openTimer, SIGNAL(timeout()), SLOT(slotSearchForPorts()));
        openTimer->start(1000);
    }
}

BMSReader::~BMSReader()
{
    port->close();
    delete port;
}

void BMSReader::timerEvent(QTimerEvent*)
{
    slotReadMainData();
}

bool BMSReader::slotSearchForPorts()
{
    auto ports = QSerialPortInfo::availablePorts();
    QString portName_;
    for (auto port_ : ports)
    {
        if (port_.description() == "USB Serial")
        {
            try
            {
                openPort(port_.portName());
                if (openTimer)
                {
                    openTimer->stop();
                    delete openTimer;
                }
                connect(port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
                    SLOT(slotError()));
                return true;
            } catch (...) {
                std::cout << "port " << port_.portName().toStdString() << " didn't open";
                return false;
            }
        }
    }
    emit noControllerFound();
    std::cout << "no ports found";
    return false;
}

void BMSReader::slotReadMainData()
{
    QByteArray arr;
    if (register03Read)
    {
        arr = readRegisterMessage(0x03);
    } else {
        arr = readRegisterMessage(0x04);
    }
    register03Read = !register03Read;
    std::cout << "Sent: ";
    for (int i = 0; i < arr.size(); ++i) std::cout << std::hex << static_cast<unsigned>(arr[i]) % 256 << ' ';
    std::cout << '\n';
    port->write(arr);
}

void BMSReader::slotReadyRead()
{
    QTest::qWait(15); // to read an entire message as one QByteArray
    QByteArray arr = port->readAll();
    QByteArray arrReadable = arr;
    if (arr[0] == 'e')
    {
        std::cout << "Recieved: ";
        for (int i = 0; i < arrReadable.size(); ++i) std::cout << std::hex <<
            static_cast<unsigned>(arrReadable[i]) % 256 << ' ';
    } else {
        if (static_cast<uint8_t>(arr[1]) == 0x03)
        {
            mainInfoParser.parseMainInfoMessage(arr);
        } else if (static_cast<uint8_t>(arr[1]) == 0x04) {
            mainInfoParser.parseLineVoltageMessage(arr);
        }
        std::cout << "Error: " << mainInfoParser.error << std::dec <<
        " Total voltage: " << mainInfoParser.totalVoltage <<
        " Current: " << mainInfoParser.current <<
        " Current capacity: " << mainInfoParser.currentCapacity <<
        " Maximum capacity: " << mainInfoParser.maximumCapacity <<
        " Cycles: " << mainInfoParser.cycles <<
        " Lines balance status: " << mainInfoParser.linesBalanceStatus <<
        " BMS errors: " << mainInfoParser._BMSErrors <<
        " Charge FET state: " << mainInfoParser.chargeFETState <<
        " Discharge FET state: " << mainInfoParser.dischargeFETState <<
        " Temperatures: ";
        for (int i = 0; i < mainInfoParser.temperatures.size(); ++i)
        {
            std::cout << mainInfoParser.temperatures[i] << ' ';
        }
        std::cout << " Lines' voltages: ";
        for (int i = 0; i < mainInfoParser.linesVoltage.size(); ++i)
        {
            std::cout << mainInfoParser.linesVoltage[i] << ' ';
        }
        std::cout << " Difference: " << mainInfoParser.diff;
    }

    std::cout << '\n';
}

void BMSReader::slotError()
{
    std::cout << "some error\n";
}

void BMSReader::slotReadAddData()
{

}
