#ifndef ADDINFOPARSER_H
#define ADDINFOPARSER_H
#include <iostream>
#include <vector>
#include <stdint.h>
#include <QObject>
#include <QDebug>
#include <QByteArray>
#include "comportreader.h"
#include "maininfowidget.h"

class AddInfoParser : public QObject
{
Q_OBJECT
    COMPortReader* reader;

    std::vector<QByteArray> querySequence;
    std::vector<uint8_t> registersToRead;
    std::vector<QByteArray> registersContent;
    unsigned currentQuery;
    unsigned attempt;

    void setCrc(uint8_t* begin, uint8_t* end, uint8_t* crc);
    uint16_t getCrc(const QByteArray& array); // returns the CRC that must be
    // according to BMS protocol, not the CRC that actually is in the array
    QByteArray getMessageReadRegister(uint8_t register_);
    QByteArray getMessageWriteRegister(uint8_t register_,
        std::vector<uint8_t> data);
    bool messageIsViable(const QByteArray& array);
    QByteArray getMessageUsefulData(const QByteArray& array);
    inline uint8_t getRegister(const QByteArray& array);
    inline uint16_t twoBytesToUInt(uint8_t high, uint8_t low);
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
};

#endif // ADDINFOPARSER_H
