#include "jbdparser.h"

void JBDParser::setCrc(uint8_t* begin, uint8_t* end, uint8_t* crc)
{
    uint16_t tmp = 0x0000;
    for (uint8_t* pointer = begin; pointer <= end; ++pointer)
    {
        tmp -= *pointer;
    }
    *crc = tmp >> 8;
    *(crc + 1) = tmp;
}

uint16_t JBDParser::getCrc(const QByteArray& array)
{
    uint16_t tmp = 0x0000;
    for (int i = 2; i < array.size() - 3; ++i)
    {
        tmp -= (array[i] & 0x00ff); // how it works ???
        // (uint8_t)0xab -> (uint16_t)0xffab, а не (uint16_t)0x00ab
        // answer: reinterpret_cast<uint16_t*>(uint8_t*) is forbidden according
        // to strict aliasing rule, the result is UB
    }
    return tmp;
}

uint16_t JBDParser::twoBytesToUInt(const char* high)
{
    const uint8_t* pUHigh = reinterpret_cast<const uint8_t*>(high);
    uint16_t res = (*pUHigh << 8) + *(pUHigh + 1);
    return res;
}

QByteArray JBDParser::getMessageReadRegister(uint8_t register_)
{
    uint8_t data[7] = {0xdd, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x77};
    data[2] = register_;
    setCrc(data + 2, data + 3, data + 4);
    return QByteArray(reinterpret_cast<const char*>(data), 7);
}

QByteArray JBDParser::getMessageWriteRegister(uint8_t register_,
    const std::vector<uint8_t>& data_)
{
    std::vector<uint8_t> msg_data(7 + data_.size());
    msg_data[0] = 0xdd;
    msg_data[1] = 0x5a;
    msg_data[2] = register_;
    msg_data[3] = data_.size();
    msg_data.back() = 0x77;
    memcpy(msg_data.data() + 4, data_.data(), data_.size());
    setCrc(msg_data.data() + 2, msg_data.data() + data_.size() + 3,
        msg_data.data() + data_.size() + 4);
    return QByteArray(reinterpret_cast<char*>(msg_data.data()),
        msg_data.size());
}

bool JBDParser::messageIsViable(const QByteArray& message, uint8_t* errorNumber)
{
    /*
        0 - no error
        1 - no messages got
        2 - wrong message length
        3 - message does not begin at 0xDD or does not end at 0x77
        4 - error from BMS
        5 - wrong crc
    */
    if (message.isEmpty())
    {
        if (errorNumber) *errorNumber = 1;
        return false;
    }
    if (message.size() < 7)
    {
        if (errorNumber) *errorNumber = 2;
        return false;
    }
    const uint8_t* arr_data = reinterpret_cast<const uint8_t*>(message.data());
    if (arr_data[0] != 0xdd || arr_data[message.size() - 1] != 0x77)
    {
        if (errorNumber) *errorNumber = 3;
        return false;
    }
    if (arr_data[2])
    {
        if (errorNumber) *errorNumber = 4;
        return false;
    }
    if (twoBytesToUInt(message.data() + message.size() - 3) != getCrc(message))
    {
        std::cout << "actual CRC: " <<
            twoBytesToUInt(message.data() + message.size() - 3) <<
            " counted CRC: " << getCrc(message) << '\n';
        if (errorNumber) *errorNumber = 5;
        return false;
    }
    return true;
}

QByteArray JBDParser::getUsefulData(const QByteArray& message)
{
    return QByteArray(message.data() + 4, message[3]);
}

uint8_t JBDParser::getRegister(const QByteArray& message) { return message[1]; }
