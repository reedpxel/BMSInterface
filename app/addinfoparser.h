#ifndef ADDINFOPARSER_H
#define ADDINFOPARSER_H
#include <iostream>
#include <vector>
#include <stdint.h>
#include <QObject>
#include <QDebug>
#include <QByteArray>
#include "comportreader.h"
#include "jbdparser.h"

#define TIMEOUTS_LIMIT 7
#define TIMER_INTERVAL 300

class AddInfoParser : public QObject, public JBDParser
{
Q_OBJECT
    COMPortReader* reader;

    std::vector<QByteArray> querySequence;
    std::vector<uint8_t> registersToRead;
    std::vector<QByteArray> registersContent;
    unsigned currentQuery;
    unsigned attempt;
    unsigned timeouts;
    int timerId;

    virtual void timerEvent(QTimerEvent*);

signals:
    void sgnUncheckedMessageGot(const QByteArray&);
    void sgnSetAutomaticMode();
    void sgnSetManualMode();
    void sgninfoUpdated();
    void sgnSend(const QByteArray&);
    void sgnSendDataToGUI(const std::vector<QByteArray>&);
    void sgnReadingBegun();
    void sgnReadingUpdate(unsigned);
    void sgnReadingEnded();
public:
    AddInfoParser(COMPortReader* reader);
    size_t getAmountOfQueries();
public slots:
    void slotParseMessage(const QByteArray&);
    void slotSendNextMessageOrExit();
    void slotPrepareReadAddData();
};

#endif // ADDINFOPARSER_H
