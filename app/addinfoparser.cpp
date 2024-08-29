#include "addinfoparser.h"

void AddInfoParser::setCrc(uint8_t* begin, uint8_t* end, uint8_t* crc)
{
    uint16_t tmp = 0x0000;
    for (uint8_t* pointer = begin; pointer <= end; ++pointer)
    {
        tmp -= *pointer;
    }
    *crc = tmp >> 8;
    *(crc + 1) = tmp;
}

uint16_t AddInfoParser::getCrc(const QByteArray& array)
{
    uint16_t tmp = 0x0000;
    for (int i = 2; i < array.size() - 3; ++i)
    {
        tmp -= (array[i] & 0x00ff); // how it works ???
        // (uint8_t)0xab -> (uint16_t)0xffab, а не (uint16_t)0x00ab
    }
    return tmp;
}

QByteArray AddInfoParser::getMessageReadRegister(uint8_t register_)
{
    uint8_t data[7] = {0xdd, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x77};
    data[2] = register_;
    setCrc(data + 2, data + 3, data + 4);
    return QByteArray(reinterpret_cast<const char*>(data), 7);
}

QByteArray AddInfoParser::getMessageWriteRegister(uint8_t register_,
    std::vector<uint8_t> data_)
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
    // QByteArray arr(reinterpret_cast<char*>(msg_data.data()),
    //     msg_data.size());
    // for (int i = 0; i < arr.size(); ++i)
    // {
    //     std::cout << std::hex << static_cast<uint16_t>(arr[i] & 0xff) << ' ';
    // }
    // std::cout << '\n';
    // return arr;
    return QByteArray(reinterpret_cast<char*>(msg_data.data()),
        msg_data.size());
}

bool AddInfoParser::messageIsViable(const QByteArray& array)
{
    if (array.size() < 7) return false;
    const uint8_t* arr_data = reinterpret_cast<const uint8_t*>(array.data());
    if (arr_data[0] != 0xdd || arr_data[array.size() - 1] != 0x77)
    {
        return false;
    }
    if (arr_data[2]) return false;
    if (twoBytesToUInt(arr_data[array.size() - 3], arr_data[array.size() - 2])
        != getCrc(array))
    {
        std::cout << "actual CRC: " <<
twoBytesToUInt(arr_data[array.size() - 3], arr_data[array.size() - 2]) <<
            " counted CRC: " << getCrc(array) << '\n';
        return false;
    }
    return true;
}

QByteArray AddInfoParser::getMessageUsefulData(const QByteArray& array)
{
    return QByteArray(array.data() + 4, array[3]);
}

uint8_t AddInfoParser::getRegister(const QByteArray &array) { return array[1]; }

uint16_t AddInfoParser::twoBytesToUInt(uint8_t high, uint8_t low)
{
    uint16_t res = (high << 8) + low;
    return res;
}

AddInfoParser::AddInfoParser(COMPortReader* reader) :
    QObject(),
    reader(reader),
    querySequence(),
    registersToRead({0x00, 0x03, 0x05, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
        0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22,
        0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2e, 0x32,
        0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
        0x3f, 0xa0, 0xa1, 0xa2, 0xaa, 0x01}),
    registersContent(registersToRead.size() - 2),
    currentQuery(0),
    attempt(1)
{
    querySequence.reserve(registersToRead.size());
    for (int i = 0; i < registersToRead.size(); ++i)
    {
        if (!i)
        {
            querySequence.push_back(getMessageWriteRegister(
                registersToRead[i], {0x56, 0x78}));
        } else if (i == registersToRead.size() - 1) {
            querySequence.push_back(getMessageWriteRegister(
                registersToRead[i], {0x00, 0x00}));
        } else {
            querySequence.push_back(getMessageReadRegister(registersToRead[i]));
        }
    }
    QObject::connect(this, SIGNAL(sgnSetManualMode()), reader,
        SLOT(slotSetManualMode()));
    QObject::connect(reader, SIGNAL(sgnManualModeIsSet()),
        SLOT(slotSendNextMessageOrExit()));
    QObject::connect(this, SIGNAL(sgnSend(const QByteArray&)), reader,
        SLOT(slotWriteManually(const QByteArray&)));
    QObject::connect(reader, SIGNAL(sgnDataGotManual(const QByteArray&)),
        SLOT(slotParseMessage(const QByteArray&)));
    QObject::connect(this, SIGNAL(sgnSetAutomaticMode()), reader,
        SLOT(slotSetAutomaticMode()));
}

void AddInfoParser::slotParseMessage(const QByteArray& array)
{
    std::cout << "A message is got: ";
    for (int i = 0; i < array.size(); ++i)
    {
        std::cout << std::hex <<
            (static_cast<uint16_t>(array[i]) & 0x00ff) << ' ';
    }
    std::cout << '\n';
    if (messageIsViable(array) &&
        getRegister(array) == registersToRead[currentQuery])
    {
        if (currentQuery && currentQuery != registersToRead.size() - 1)
        {
            attempt = 1;
            registersContent[currentQuery - 1] = getMessageUsefulData(array);
        }
        ++currentQuery;
    } else {
        std::cout << "not viable or wrong register, attempt " << attempt <<
            '\n';
        ++attempt;
        if (attempt == 4) ++currentQuery;
    }
    slotSendNextMessageOrExit();
}

void AddInfoParser::slotSendNextMessageOrExit()
{
    if (currentQuery == querySequence.size())
    {
        std::cout << "All queries are got, the content is:\n";
        for (int i = 0; i < registersContent.size(); ++i)
        {
            std::cout << std::hex <<
                static_cast<uint16_t>(registersToRead[i + 1]) << "   ";
            for (int j = 0; j < registersContent[i].size(); ++j)
            {
                std::cout << std::hex <<
                    (static_cast<uint16_t>(registersContent[i][j]) & 0xff) <<
                    ' ';
            }
            std::cout << '\n';
        }
        emit sgnSendDataToGUI(registersContent);
        emit sgnSetAutomaticMode();
        currentQuery = 0; // to read additional data again
    } else {
        emit sgnSend(querySequence[currentQuery]);
    }
}
