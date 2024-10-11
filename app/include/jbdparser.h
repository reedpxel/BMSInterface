#ifndef JBDPARSER_H
#define JBDPARSER_H
#include <iostream>
#include <cmath>
#include <cstdint>
#include <vector>
#include <QByteArray>
#include <QString>
#include <QDate>

struct JBDParser
{
    void setCrc(uint8_t* begin, uint8_t* end, uint8_t* crc);
    uint16_t getCrc(const QByteArray& array); // returns the CRC that must be
    // according to BMS protocol, not the CRC that actually is in the array
    uint16_t twoBytesToUInt(const char* high);
    short twoBytesToSignedInt(const char* high);
    QByteArray getMessageReadRegister(uint8_t register_);
    QByteArray getMessageWriteRegister(uint8_t register_,
        const std::vector<uint8_t>& data_);
    bool messageIsViable(const QByteArray& message,
        uint8_t* errorNumber = nullptr);
    QByteArray getUsefulData(const QByteArray& message);
    uint8_t getRegister(const QByteArray& message);
    uint8_t getError(const QByteArray& message);

    // TO DO: add error handling
    QByteArray uint16_tToArray(const QString& str,
        unsigned signsAfterComma = 0);
    QByteArray QStringToJBDString(const QString& str);
    QString JBDStringToQString(const QByteArray& array);
    QByteArray temperatureToArray(const QString& str);
    QString QByteArrayToDate(const QByteArray& array);
    QByteArray dateToQByteArray(const QString& str);
};

#endif // JBDPARSER_H
