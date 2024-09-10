#include "maininfoparser.h"

/*
    0 - no error
    1 - no messages got
    2 - wrong message length
    3 - message does not begin at 0xDD or does not end at 0x77
    4 - error from BMS
    5 - wrong crc
*/

MainInfoParser::MainInfoParser(COMPortReader* reader) : QObject(),
    reader(reader),
    mainInfo({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        std::vector<int>(3, 0), std::vector<int>(13, 0), 0})
{
    QObject::connect(reader, SIGNAL(sgnDataGotAutomatic(const QByteArray&)),
        SLOT(slotParseMessage(const QByteArray&)));
    QObject::connect(reader, SIGNAL(sgnDataGotManual(const QByteArray&)),
        SLOT(slotParseFETStateMessage(const QByteArray&)));
}

void MainInfoParser::slotParseMessage(const QByteArray& message)
{
    emit sgnMessageGot(message);
    uint8_t error_ = 0x00;
    if (!messageIsViable(message, &error_))
    {
        // TO DO: display error code on status bar
        mainInfo.error = error_;
        return;
    }
    if (getRegister(message) == 0x03)
    {
        parseMainInfoMessage(message);
    } else if (getRegister(message) == 0x04) {
        parseLineVoltageMessage(message);
    }
}

void MainInfoParser::slotParseFETStateMessage(const QByteArray& message)
{
    if (message.isEmpty())
    {
        QMessageBox::warning(nullptr, "Error", "No reply from BMS");
    }
    if (messageIsViable(message) && getRegister(message) == 0xe1) return;
    QMessageBox::warning(nullptr, "Error", "The query to change FET state was "
        "sent, but the reply contents error");
}

void MainInfoParser::slotOnFETChargeButtonClicked()
{
    uint8_t state = 0;
    if (!mainInfo.chargeFETState)
    {
        state |= 0b1;
    }
    if (mainInfo.dischargeFETState)
    {
        state |= 0b10;
    }
    QByteArray ba = getMessageWriteRegister(0xe1, {0x00, state});
    reader->slotWriteQueries({ba});
    // std::vector<QByteArray> reply = reader->getReplies({ba});
    // for (auto ba_ : reply)
    // {
    //     std::cout << "Recieved array for MainInfoParser: ";
    //     for (int i = 0; i < ba_.size(); ++i)
    //     {
    //         std::cout << std::hex << static_cast<uint16_t>(
    //             ba_[i] & 0xff) << ' ';
    //     }
    //     std::cout << '\n';
    // }
}

void MainInfoParser::slotOnFETDischargeButtonClicked()
{
    uint8_t state = 0;
    if (mainInfo.chargeFETState)
    {
        state |= 0b1;
    }
    if (!mainInfo.dischargeFETState)
    {
        state |= 0b10;
    }
    QByteArray ba = getMessageWriteRegister(0xe1, {0x00, state});
    reader->slotWriteQueries({ba});
    // /*QByteArray ba = */getMessageWriteRegister(0xe1, {0x00, state});
    // std::vector<QByteArray> reply = reader->getReplies({ba});
    // for (auto ba_ : reply)
    // {
    //     std::cout << "Recieved array for MainInfoParser: ";
    //     for (int i = 0; i < ba_.size(); ++i)
    //     {
    //         std::cout << std::hex << static_cast<uint16_t>(
    //             ba_[i] & 0xff) << ' ';
    //     }
    //     std::cout << '\n';
    // }
}

void MainInfoParser::parseMainInfoMessage(const QByteArray& message)
{
    mainInfo.error = 0;
    mainInfo.totalVoltage = twoBytesToUInt(message.data() + 4);
    // TO DO: current may be negative
    mainInfo.current = twoBytesToUInt(message.data() + 6);
    mainInfo.currentCapacity = twoBytesToUInt(message.data() + 8);
    mainInfo.maximumCapacity = twoBytesToUInt(message.data() + 10);
    mainInfo.cycles = twoBytesToUInt(message.data() + 12);
    mainInfo.linesBalanceStatus = twoBytesToUInt(message.data() + 16);
    mainInfo._BMSErrors = twoBytesToUInt(message.data() + 20);
    mainInfo.capacityInPercents = message[23];
    mainInfo.chargeFETState = message[24] & 0b1 ? true : false;
    mainInfo.dischargeFETState = message[24] & 0b10 ? true : false;
    mainInfo.lines = message[25];
    if (mainInfo.lines > 32) mainInfo.lines = 32;
    mainInfo.temperatures.resize(message[26]);
    for (int i = 0; i < mainInfo.temperatures.size(); ++i)
    {
        // here may be an error
        mainInfo.temperatures[i] = twoBytesToUInt(message.data() + 27 + 2 * i) -
            2731;
    }
    emit sgnData03Updated(mainInfo);
}

void MainInfoParser::parseLineVoltageMessage(const QByteArray& message)
{
    mainInfo.linesVoltage.resize((message.size() - 7) / 2);
    int _max = INT_MIN;
    int _min = INT_MAX;
    for (int i = 0; i < mainInfo.linesVoltage.size(); ++i)
    {
        mainInfo.linesVoltage[i] = twoBytesToUInt(message.data() + 4 + 2 * i);
        if (mainInfo.linesVoltage[i] > _max) _max = mainInfo.linesVoltage[i];
        if (mainInfo.linesVoltage[i] < _min) _min = mainInfo.linesVoltage[i];
    }
    mainInfo.diff = _max - _min;
    emit sgnData04Updated(mainInfo);
}

