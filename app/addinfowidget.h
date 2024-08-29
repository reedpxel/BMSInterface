#ifndef ADDINFOWIDGET_H
#define ADDINFOWIDGET_H
#include <QWidget>
#include <vector>
#include <iostream>
#include "addinfoparser.h"
#include "mainwindow.h"

// struct GeneralInfo
// {
//     int maximumCapacity; // 0x10
//     int maximumCyclicCapacity; // 0x11
//     int chargedLineVoltage; // 0x12
//     int charged80PercentVoltage;
//     int charged60PercentVoltage;
//     int charged40PercentVoltage;
//     int charged20PercentVoltage;
//     int dischargedLineVoltage; // 0x13
//     int selfDischargeRate;
//     int cellBalanceVoltage;
//     int balanceWindow;
//     int shunt;
//     std::vector<bool> thermoresistorsState;
// };

// struct DeviceInfo
// {
//     int manufactureDate;
//     std::vector<uint8_t> serialNumber;
//     int cycles;
//     int softwareVersion;
//     std::vector<uint8_t> manufacturer;
//     std::vector<uint8_t> deviceName;
//     std::vector<uint8_t> barcode;
// };

// struct ErrorCount
// {
//     unsigned shortCurcuits;
//     unsigned chargeOvercurrents;
//     unsigned dischargeOvercurrents;
//     unsigned lineOvervoltages;
//     unsigned lineUndervoltages;
//     unsigned chargeOverheatings;
//     unsigned chargeUnderheatings;
//     unsigned dischargeOverheatings;
//     unsigned dischargeUnderheatings;
//     unsigned blockOvervoltages;
//     unsigned blockUndervoltages;
// };

// struct OverheatingProtection
// {
//     int overheatingChargeShutdownTemp;
//     int overheatingChargeReturnTemp;
//     int overheatingChargeReturnTime;

//     int overheatingDischargeShutdownTemp;
//     int overheatingDischargeReturnTemp;
//     int overheatingDischargeReturnTime;
// };

// struct UnderheatingProtection
// {
//     int underheatingChargeShutdownTemp;
//     int underheatingChargeReturnTemp;
//     int underheatingChargeReturnTime;

//     int underheatingDischargeShutdownTemp;
//     int underheatingDischargeReturnTemp;
//     int underheatingDischargeReturnTime;
// };

// struct VoltageShutdown
// {
//     int aboveShutdownVoltage;
//     int aboveReturnVoltage;
//     int aboveReturnTime;

//     int beyondShutdownVoltage;
//     int beyondReturnVoltage;
//     int beyondReturnTime;
// };

// struct LineVoltageShutdown
// {
//     int aboveShutdownLineVoltage;
//     int aboveReturnLineVoltage;
//     int aboveReturnTime_line;

//     int beyondShutdownLineVoltage;
//     int beyondReturnLineVoltage;
//     int beyondReturnTime_line;
// };

// struct ShortCurcuit
// {
//     int chargeShutdownCurrent;
//     int chargeBreakDelay;
//     int chargeReturnTime;

//     int dischargeShutdownCurrent;
//     int dischargeBreakDelay;
//     int dischargeReturnTime;

//     int scReleaseTime;
//     int scDelay;
//     int scValue;

//     int dischargeOvercurrentValue;
//     int dischargeOvercurrentDelay;
// };

// struct CellProtection
// {
//     int overvoltageProtectionValue;
//     int undervoltageProtectionValue;
//     int overvoltageProtectionDelay;
//     int undervoltageProtectionDelay;
// };

// struct Other
// {
//     bool enableLineBalancing;
//     bool enableLineChargeBalancing;
//     unsigned short linesStatus;
// };

// struct AddInfoStruct
// {
//     int error;
//     GeneralInfo generalInfo;
//     DeviceInfo deviceInfo;
//     ErrorCount errorCount;
//     OverheatingProtection overheatingProtection;
//     UnderheatingProtection underheatingProtection;
//     VoltageShutdown voltageShutdown;
//     LineVoltageShutdown lineVoltageShutdown;
//     ShortCurcuit shortCurcuit;
//     CellProtection cellProtection;
//     Other other;
// };

namespace Ui {
class AddInfoWidget;
}

class AddInfoWidget : public QWidget
{
Q_OBJECT
// public:
    // AddInfoStruct addInfoStruct;
private:
    AddInfoParser parser_;

    Ui::AddInfoWidget *ui;

    uint16_t getUInt16InArray(const QByteArray& array, int index);
    QString JBDStringToQString(const QByteArray& array);
    QString getSoftwareVersion(char ch);
    QString getTemperature(const QByteArray& array);
    QString QByteArrayToDate(const QByteArray& array);
public:
    explicit AddInfoWidget(QWidget* parent = nullptr);
    ~AddInfoWidget();

public slots:
    void slotShowDataOnGUI(const std::vector<QByteArray>& data_);
};

#endif // ADDINFOWIDGET_H
