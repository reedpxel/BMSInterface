#include <vector>

struct MainInfo
{
    int error;
    int totalVoltage;
    int current;
    int currentCapacity;
    int maximumCapacity;
    int cycles;
    unsigned short linesBalanceStatus;
    unsigned short _BMSErrors;
    int capacityInPercents;
    bool chargeFETState; // 1 - unlocked, 0 - locked
    bool dischargeFETState;
    int lines;
    std::vector<int> temperatures;
    std::vector<int> linesVoltage;
    int diff;

    MainInfo();
};
