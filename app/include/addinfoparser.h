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

    // to write in a register
    std::vector<uint8_t> registersSequence;
    std::vector<QByteArray> querySequenceWriteRegister;
    QByteArray recievedRegisterContent;
    int registerToWriteIn;

    virtual void timerEvent(QTimerEvent*);

signals:
    void sgnSetAutomaticMode();
    void sgnSetManualMode();
    void sgninfoUpdated();
    void sgnSend(const QByteArray&);
    void sgnSendDataToGUI(const std::vector<QByteArray>&);
    void sgnReadingBegun();
    void sgnReadingUpdate(unsigned);
    void sgnReadingEnded();
    void sgnWritingSuccess(const QByteArray&);
    void sgnWritingError();
public:
    AddInfoParser(COMPortReader* reader);
    size_t getAmountOfQueries();
    COMPortReader* getReader();

    /*
        - writes {0x00, 0x00} in 0x00
        - writes data_ in register_
        - writes {0x28, 0x28} in 0x01
    */
    void setRegisterValue(uint8_t register_, const QByteArray& data_);
    // simply writes data_ in register_
    void setNotEEPROMRegisterValue(uint8_t register_, const QByteArray& data_);
public slots:
    void slotParseMessage(const QByteArray&);
    void slotSendNextMessageOrExit();
    void slotPrepareReadAddData();
};

#endif // ADDINFOPARSER_H
