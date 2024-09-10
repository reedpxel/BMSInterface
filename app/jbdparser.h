#ifndef JBDPARSER_H
#define JBDPARSER_H
#include <iostream>
#include <cstdint>
#include <vector>
#include <QByteArray>

struct JBDParser
{
    void setCrc(uint8_t* begin, uint8_t* end, uint8_t* crc);
    uint16_t getCrc(const QByteArray& array); // returns the CRC that must be
    // according to BMS protocol, not the CRC that actually is in the array
    uint16_t twoBytesToUInt(const char* high);
    QByteArray getMessageReadRegister(uint8_t register_);
    QByteArray getMessageWriteRegister(uint8_t register_,
        const std::vector<uint8_t>& data_);
    bool messageIsViable(const QByteArray& message,
        uint8_t* errorNumber = nullptr);
    QByteArray getUsefulData(const QByteArray& message);
    uint8_t getRegister(const QByteArray& message);
};

#endif // JBDPARSER_H
