#ifndef MAININFOPARSER_H
#define MAININFOPARSER_H
#include <QObject>
#include <vector>
#include <QByteArray>
#include <climits>
#include "jbdparser.h"
#include "comportreader.h"

struct MainInfo
{
    int error;
    int totalVoltage;
    int current;
    int currentCapacity;
    int maximumCapacity;
    int cycles;
    unsigned short linesBalanceStatus;
    unsigned short _BMSErrors;
    int capacityInPercents;
    bool chargeFETState; // 1 - unlocked, 0 - locked
    bool dischargeFETState;
    int lines;
    std::vector<int> temperatures;
    std::vector<int> linesVoltage;
    int diff;
};

class MainInfoParser : public QObject, public JBDParser
{
Q_OBJECT
    COMPortReader* reader;
    MainInfo mainInfo;

    void parseMainInfoMessage(const QByteArray& message);
    void parseLineVoltageMessage(const QByteArray& message);
public:
    MainInfoParser(COMPortReader* reader);
public slots:
    void slotParseMessage(const QByteArray& message);
signals:
    void sgnMessageGot(const QByteArray&);
    void sgnData03Updated(const MainInfo&);
    void sgnData04Updated(const MainInfo&);
};

#endif // MAININFOPARSER_H
