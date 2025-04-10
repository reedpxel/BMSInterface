#ifndef COMPORTREADER_H
#define COMPORTREADER_H
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QTest>
#include <QThread>
#include <QSignalSpy>
#include <iostream>

#define MAIN_INFO_TIMER_STEP 400
#define MAIN_INFO_AUTOMATIC_DELAY 150
#define MAIN_INFO_MANUAL_DELAY 200
#define NO_PORTS_TIMER_DELAY 1000
#define MANUAL_READ_DELAY 25
#define SET_MANUAL_MODE_WAITING_LIMIT 400
// COMPortReader works in 2 mods: manual and automatic
// In automatic mode it sends 0x03 and 0x04 queries every
// MAIN_INFO_TIMER_STEP ms.

class COMPortReader : public QObject
{
Q_OBJECT
    QSerialPort* port;
    QString deviceName;
    bool waitingReply;
    bool modeIsAutomatic;
    bool lastQueryAnswered; // this variable is checked before sending a query
    // if is false,  no reply got from BMS => BMS is absent
    QTimer* writeTimer; // timer that writes 0x03 and 0x04 messages
    int timerId; // id of a timer that searches for ports until a reading
                 // device found
    bool isConnected;
    size_t attempt;
    bool writeMsgFlag;
    QByteArray msgToSend03;
    QByteArray msgToSend04;

    void openPort(const QString&);
    void closePort();
    virtual void timerEvent(QTimerEvent*);
public:
    COMPortReader();
    ~COMPortReader();
    QString getDeviceName();
signals:
    void sgnNoPortsFound(size_t);
    void sgnPortOpened(const QString&);
    void sgnPortDidNotOpen(const QString&);
    void sgnPortClosed();
    void sgnDataGotAutomatic(const QByteArray&);
    void sgnDataGotManual(const QByteArray&);
    void sgnManualModeIsSet();
    void sgnNoBMS();
public slots:
    bool slotSearchForPorts();
    void slotReadyReadAutomatic();
    void slotReadyReadManual();
    void slotNoAnswer();
    void slotWriteAutomatic();
    void slotSetAutomaticMode();
    void slotSetManualMode();
    void slotWriteManually(const QByteArray&);
    void slotWriteQueries(const std::vector<QByteArray>&);
    void slotDisconnect();
    void slotSetDeviceName(const QString& str);
};

#endif // COMPORTREADER_H
