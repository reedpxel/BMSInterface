#include "maininfoparser.h"

/*
    0 - no error
    1 - no messages got
    2 - wrong message length
    3 - message does not begin at 0xDD or does not end at 0x77
    4 - error from BMS
    5 - wrong crc
*/

int MainInfoParser::twoBytesToInt(const char* pHigh)
{
    int res = (*pHigh << 8) + *(pHigh + 1);
    return res;
}

unsigned short MainInfoParser::twoBytesToUShort(const char* pHigh)
{
    unsigned short res = (*pHigh << 8) + *(pHigh + 1);
    return res;
}

MainInfoParser::MainInfoParser() : QObject(),
    mainInfo({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        std::vector<int>(3, 0), std::vector<int>(13, 0), 0}) {}

void MainInfoParser::parseMessage(const QByteArray &message)
{
    if (message.size() < 7)
    {
        mainInfo.error = 2;
        return;
    }
    if (static_cast<uint8_t>(message.front()) != 0xDD ||
        static_cast<uint8_t>(message.back()) != 0x77)
    {
        mainInfo.error = 3;
        return;
    }
    if (message[2])
    {
        mainInfo.error = 4;
        return;
    }
    unsigned crc = 0x10000;
    for (int i = 3; i < message.size() - 3; ++i)
    {
        crc -= static_cast<uint8_t>(message[i]);
    }
    if (static_cast<uint8_t>(message[message.size() - 3]) != static_cast<uint8_t>(crc >> 8) ||
        static_cast<uint8_t>(message[message.size() - 2]) != static_cast<uint8_t>(crc))
    {
        mainInfo.error = 5;
        return;
    }
    if (static_cast<uint8_t>(message[1]) == 0x03)
    {
        parseMainInfoMessage(message);
    } else if (static_cast<uint8_t>(message[1]) == 0x04) {
        parseLineVoltageMessage(message);
    }
}

void MainInfoParser::parseMainInfoMessage(const QByteArray& message)
{
    mainInfo.error = 0;
    mainInfo.totalVoltage = twoBytesToInt(message.data() + 4);
    mainInfo.current = twoBytesToInt(message.data() + 6);
    mainInfo.currentCapacity = twoBytesToInt(message.data() + 8);
    mainInfo.maximumCapacity = twoBytesToInt(message.data() + 10);
    mainInfo.cycles = twoBytesToInt(message.data() + 12);
    mainInfo.linesBalanceStatus = twoBytesToUShort(message.data() + 16);
    mainInfo._BMSErrors = twoBytesToUShort(message.data() + 20);
    mainInfo.capacityInPercents = message[23];
    mainInfo.chargeFETState = message[24] & 0b10 ? true : false;
    mainInfo.dischargeFETState = message[24] & 0b1 ? true : false;
    mainInfo.lines = message[25];
    if (mainInfo.lines > 32) mainInfo.lines = 32;
    mainInfo.temperatures.resize(message[26]);
    for (int i = 0; i < mainInfo.temperatures.size(); ++i)
    {
        // here may be an error
        mainInfo.temperatures[i] = twoBytesToInt(message.data() + 27 + 2 * i) -
            2731;
    }
    emit sgnDataReceived(mainInfo);
}

void MainInfoParser::parseLineVoltageMessage(const QByteArray& message)
{
    mainInfo.linesVoltage.resize((message.size() - 7) / 2);
    int _max = INT_MIN;
    int _min = INT_MAX;
    for (int i = 0; i < mainInfo.linesVoltage.size(); ++i)
    {
        mainInfo.linesVoltage[i] = twoBytesToInt(message.data() + 4 + 2 * i);
        if (mainInfo.linesVoltage[i] > _max) _max = mainInfo.linesVoltage[i];
        if (mainInfo.linesVoltage[i] < _min) _min = mainInfo.linesVoltage[i];
    }
    mainInfo.diff = _max - _min;
    emit sgnDataReceived(mainInfo);
}

