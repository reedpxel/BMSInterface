#include "maininfo.h"

MainInfo::MainInfo() :
    error(1),
    totalVoltage(0),
    current(0),
    currentCapacity(0),
    maximumCapacity(0),
    cycles(0),
    linesBalanceStatus(0),
    _BMSErrors(0),
    capacityInPercents(0),
    chargeFETState(false),
    dischargeFETState(false),
    lines(0),
    temperatures(3, 0),
    linesVoltage(13, 0),
    diff(0) {}
