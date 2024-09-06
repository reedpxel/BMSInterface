#ifndef COMPORTREADER_H
#define COMPORTREADER_H
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QTest>
#include <iostream>

#define MAIN_INFO_TIMER_STEP 400
#define MAIN_INFO_AUTOMATIC_DELAY 150
#define MAIN_INFO_MANUAL_DELAY 50
#define NO_PORTS_TIMER_DELAY 1000
#define MANUAL_READ_DELAY 25
// COMPortReader works in 2 mods: manual and automatic
// In automatic mode it sends 0x03 and 0x04 queries every
// MAIN_INFO_TIMER_STEP ms.

class COMPortReader : public QObject
{
Q_OBJECT
    QSerialPort* port;
    bool modeIsAutomatic;
    QTimer* writeTimer;
    int timerId;
    bool isConnected;
    size_t attempt;
    bool writeMsgFlag;
    QByteArray msgToSend03;
    QByteArray msgToSend04;

    void openPort(const QString&);
public:
    COMPortReader();
    ~COMPortReader();
    virtual void timerEvent(QTimerEvent*);
signals:
    void sgnNoPortsFound(size_t);
    void sgnPortOpened(const QString&);
    void sgnPortDidNotOpen(const QString&);
    void sgnPortClosed();
    void sgnDataGotAutomatic(const QByteArray&);
    void sgnDataGotManual(const QByteArray&);
    void sgnManualModeIsSet();
public slots:
    bool slotSearchForPorts();
    void slotReadyReadAutomatic();
    void slotReadyReadManual();
    void slotNoAnswer();
    void slotWriteAutomatic();
    void slotSetAutomaticMode();
    void slotSetManualMode();
    void slotWriteManually(const QByteArray&);
};

#endif // COMPORTREADER_H
