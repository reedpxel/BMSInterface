#ifndef BMSREADER_H
#define BMSREADER_H
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QDebug>
#include <deque>
#include <iostream>
#include <QTest>
#include "maininforeader.h"
#include "addinforeader.h"

class BMSReader : public QObject
{
Q_OBJECT
    QSerialPort* port;
    QTimer* openTimer;
    bool register03Read;
    MainInfoParser mainInfoParser;
    // AddInfoReader addInfo;

    int charsToNum(const char*);
    QByteArray writeRegisterMessage(uint8_t register_,
        std::vector<uint8_t> value);
    QByteArray readRegisterMessage(uint8_t register_);
    QByteArray getMessage(std::vector<uint8_t>& arr);
    void openPort(const QString&);
    QByteArray numToChar(char ch);
    bool arrayIsViableAnswer(const QByteArray& arr);

public:
    BMSReader();
    ~BMSReader();
    virtual void timerEvent(QTimerEvent*);
signals:
    void noControllerFound();
    void noBMSFound();
    void portOpened(const QString&);
    void mainData();
public slots:
    void slotReadMainData();
    void slotReadAddData();
    bool slotSearchForPorts();
    void slotReadyRead();
    void slotError();
};

#endif // BMSREADER_H
