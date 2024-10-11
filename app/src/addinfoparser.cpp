#include "addinfoparser.h"

void AddInfoParser::timerEvent(QTimerEvent*)
{
    killTimer(timerId);
    ++timeouts;
    std::cout << "timeout " << timeouts << '\n';
    if (timeouts == TIMEOUTS_LIMIT)
    {
        currentQuery = querySequence.size();
    }
    slotSendNextMessageOrExit();
}

AddInfoParser::AddInfoParser(COMPortReader* reader) :
    QObject(),
    reader(reader),
    querySequence(),
    registersToRead({0x00, 0x03, 0x05, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
        0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22,
        0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2e, 0x32,
        0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
        0x3f, 0xa0, 0xa1, 0xa2, 0xaa, /*0xe2,*/ 0x01}),
    registersContent(registersToRead.size() - 2),
    currentQuery(0),
    attempt(1),
    timeouts(0),
    registersSequence({0x00, 0x00, 0x01, 0x00}),
    querySequenceWriteRegister(),
    recievedRegisterContent(),
    registerToWriteIn(0)
{
    querySequence.reserve(registersToRead.size());
    for (unsigned i = 0; i < registersToRead.size(); ++i)
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
    QObject::connect(this, SIGNAL(sgnSend(const QByteArray&)), reader,
        SLOT(slotWriteManually(const QByteArray&)));
    QObject::connect(this, SIGNAL(sgnSetAutomaticMode()), reader,
        SLOT(slotSetAutomaticMode()));
}

size_t AddInfoParser::getAmountOfQueries() { return registersToRead.size(); }

COMPortReader* AddInfoParser::getReader() { return reader; }

void AddInfoParser::setRegisterValue(uint8_t register_, const QByteArray& data_)
{
    for (int i = 0; i < data_.size(); ++i)
    {
        std::cout << std::hex << (static_cast<uint16_t>(data_[i]) & 0xff) << ' ';
    }
    std::cout << '\n';
    registerToWriteIn = register_;
    registersSequence[1] = register_;
    registersSequence[3] = register_;
    std::vector<uint8_t> usefulInfo(data_.data(), data_.data() + data_.size());
    querySequenceWriteRegister = {
        getMessageWriteRegister(0x00, {0x56, 0x78}),
        getMessageWriteRegister(register_, usefulInfo),
        getMessageWriteRegister(0x01, {0x28, 0x28})
    };
    recievedRegisterContent = data_;
    slotPrepareReadAddData();
}

void AddInfoParser::slotParseMessage(const QByteArray& array)
{
    killTimer(timerId);
    if (registerToWriteIn)
    {
        if (messageIsViable(array) &&
            getRegister(array) == registersSequence[currentQuery])
        {
            ++currentQuery;
        } else {
            std::cout << "not viable or wrong register, cancelling\n";
            emit sgnWritingError();
        }
    } else {
        if (messageIsViable(array) &&
            getRegister(array) == registersToRead[currentQuery])
        {
            if (currentQuery && currentQuery != registersToRead.size() - 1)
            {
                attempt = 1;
                registersContent[currentQuery - 1] = getUsefulData(array);
            }
            ++currentQuery;
            emit sgnReadingUpdate(currentQuery);
        } else {
            std::cout << "not viable or wrong register, attempt " << attempt <<
                '\n';
            ++attempt;
            if (attempt == 4) ++currentQuery;
        }
    }
    slotSendNextMessageOrExit();
}

void AddInfoParser::slotSendNextMessageOrExit()
{
    if (registerToWriteIn)
    {
        if (currentQuery >= querySequenceWriteRegister.size())
        {
            QObject::disconnect(reader,
                SIGNAL(sgnDataGotManual(const QByteArray&)),
                this, SLOT(slotParseMessage(const QByteArray&)));
            QObject::disconnect(reader,
                SIGNAL(sgnManualModeIsSet()),
                this, SLOT(slotSendNextMessageOrExit()));
            emit sgnWritingSuccess(recievedRegisterContent);
            emit sgnSetAutomaticMode();
            registerToWriteIn = 0;
            currentQuery = 0;
            timeouts = 0;
        } else {
            timerId = startTimer(TIMER_INTERVAL);
            emit sgnSend(querySequenceWriteRegister[currentQuery]);
        }
    } else {
        if (currentQuery == querySequence.size())
        {
            QObject::disconnect(reader,
                SIGNAL(sgnDataGotManual(const QByteArray&)),
                this, SLOT(slotParseMessage(const QByteArray&)));
            QObject::disconnect(reader,
                SIGNAL(sgnManualModeIsSet()),
                this, SLOT(slotSendNextMessageOrExit()));
            emit sgnReadingEnded();
            std::cout << "All queries got, the content is:\n";
            for (unsigned i = 0; i < registersContent.size(); ++i)
            {
                std::cout << std::hex <<
                    static_cast<uint16_t>(registersToRead[i + 1]) << "   ";
                for (int j = 0; j < registersContent[i].size(); ++j)
                {
                    std::cout << std::hex <<
                        (static_cast<uint16_t>(registersContent[i][j]) & 0xff)
                        << ' ';
                }
                std::cout << ' ' << std::dec <<
                    registersContent[i].size() << '\n';
            }
            emit sgnSendDataToGUI(registersContent);
            registersContent =
                std::vector<QByteArray>(registersToRead.size() - 2);
            // otherwise previous data will be shown, if it was not read
            // in current reading
            emit sgnSetAutomaticMode();
            currentQuery = 0;
            timeouts = 0; // to read additional data again
        } else {
            timerId = startTimer(TIMER_INTERVAL);
            emit sgnSend(querySequence[currentQuery]);
        }
    }
}

void AddInfoParser::slotPrepareReadAddData()
{
    QObject::connect(reader, SIGNAL(sgnDataGotManual(const QByteArray&)),
        SLOT(slotParseMessage(const QByteArray&)));
    QObject::connect(reader, SIGNAL(sgnManualModeIsSet()),
        SLOT(slotSendNextMessageOrExit()));
    emit sgnSetManualMode();
}
