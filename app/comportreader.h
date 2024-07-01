#ifndef COMPORTREADER_H
#define COMPORTREADER_H
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QDebug>
#include <QTest>

class COMPortReader : public QObject
{
Q_OBJECT
    QSerialPort* port;
    int timerId;
    bool isConnected;
    size_t attempt;

    void openPort(const QString&);
public:
    COMPortReader();
    ~COMPortReader();
    virtual void timerEvent(QTimerEvent*);
signals:
    void noPortsFound(size_t);
    void portOpened(const QString&);
    void portDidNotOpen(const QString&);
    void portClosed();
    void dataGot(const QByteArray&);
public slots:
    bool slotSearchForPorts();
    void slotReadyRead();
};

#endif // COMPORTREADER_H
