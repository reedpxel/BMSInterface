#ifndef MAININFOPARSER_H
#define MAININFOPARSER_H
#include <QObject>
#include <QByteArray>
#include <climits>
#include "jbdparser.h"
#include "comportreader.h"
#include <QMessageBox>
#include "maininfo.h"

// struct MainInfo
// {
//     int error;
//     int totalVoltage;
//     int current;
//     int currentCapacity;
//     int maximumCapacity;
//     int cycles;
//     unsigned short linesBalanceStatus;
//     unsigned short _BMSErrors;
//     int capacityInPercents;
//     bool chargeFETState; // 1 - unlocked, 0 - locked
//     bool dischargeFETState;
//     int lines;
//     std::vector<int> temperatures;
//     std::vector<int> linesVoltage;
//     int diff;

//     // MainInfo();
//     MainInfo() :
//     error(1),
//     totalVoltage(0),
//     current(0),
//     currentCapacity(0),
//     maximumCapacity(0),
//     cycles(0),
//     linesBalanceStatus(0),
//     _BMSErrors(0),
//     capacityInPercents(0),
//     chargeFETState(false),
//     dischargeFETState(false),
//     lines(0),
//     temperatures(3, 0),
//     linesVoltage(13, 0),
//     diff(0) {}
// };

class MainInfoParser : public QObject, public JBDParser
{
Q_OBJECT
    void parseMainInfoMessage(const QByteArray& message);
    void parseLineVoltageMessage(const QByteArray& message);
public:
    COMPortReader* reader;
    MainInfo mainInfo;
    MainInfoParser(COMPortReader* reader);
public slots:
    void slotParseMessage(const QByteArray& message);
    void slotParseFETStateMessage(const QByteArray&);
    void slotOnFETChargeButtonClicked();
    void slotOnFETDischargeButtonClicked();

signals:
    void sgnMessageGot(const QByteArray&);
    void sgnData03Updated(const MainInfo&);
    void sgnData04Updated(const MainInfo&);
};

#endif // MAININFOPARSER_H
