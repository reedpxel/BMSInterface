#include "maininfowidget.h"
#include "ui_maininfowidget.h"

MainInfoWidget::MainInfoWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainInfoWidget)
{
    ui->setupUi(this);
    batterySlider = new BatterySlider(ui->batterySliderWidget);
    batterySlider->resize(ui->batterySliderWidget->size());
    ui->tempsTable->setColumnCount(2);
    ui->linesVoltageTable->setColumnCount(2);
}

void MainInfoWidget::resizeEvent(QResizeEvent*)
{
    batterySlider->resize(ui->batterySliderWidget->size());
}

MainInfoWidget::~MainInfoWidget()
{
    delete ui;
}

void MainInfoWidget::slotDataReceived(const MainInfo& mainInfo)
{
    // lines' balance status
    if (!mainInfo.linesBalanceStatus)
    {
        ui->linesBalanceLabel->setText(QString(mainInfo.lines, '+'));
    } else {
        QString str(mainInfo.lines, '+');
        for (int i = 0; i < mainInfo.lines; ++i)
        {
            if (mainInfo.linesBalanceStatus & (0b1 << (mainInfo.lines - i)))
            {
                str[i] = '-';
            }
        }
        ui->linesBalanceLabel->setText(str);
    }

    // FET status
    if (mainInfo.chargeFETState) ui->FETChargeLabel->setText("Opened");
    else ui->FETChargeLabel->setText("Closed");
    if (mainInfo.dischargeFETState) ui->FETDischargeLabel->setText("Opened");
    else ui->FETDischargeLabel->setText("Closed");

    // voltage, current, capacity, cycles
    ui->totalVoltageLabel->setText(
        QString::number(mainInfo.totalVoltage / 100.));
    ui->currentLabel->setText(QString::number(mainInfo.current / 100.));
    ui->currentCapacityLabel->setText(
        QString::number(mainInfo.currentCapacity / 100.));
    ui->maxCapacityLabel->setText(
        QString::number(mainInfo.maximumCapacity / 100.));
    ui->cyclesLabel->setText(QString::number(mainInfo.cycles));

    // battery slider
    batterySlider->slotSetPercent(mainInfo.capacityInPercents);

    // temperatures
    if (ui->tempsTable->rowCount() != mainInfo.temperatures.size())
    {
        ui->tempsTable->setRowCount(mainInfo.temperatures.size());
        for (int i = 0; i < mainInfo.temperatures.size(); ++i)
        {
            QTableWidgetItem* item0 = new QTableWidgetItem;
            item0->setText(QString::number(i + 1));
            ui->tempsTable->setItem(i, 0, item0);
            QTableWidgetItem* item1 = new QTableWidgetItem;
            ui->tempsTable->setItem(i, 1, item1);
            item1->setText(QString::number(mainInfo.temperatures[i]));
        }
    } else {
        for (int i = 0; i < mainInfo.temperatures.size(); ++i)
        {
            ui->tempsTable->item(i, 1)->setText(
                QString::number(mainInfo.temperatures[i] / 10.));
        }
    }

    // errors
    ui->lineOvervoltageChB->setChecked(mainInfo._BMSErrors & 1);
    ui->lineUndervoltageChB->setChecked(mainInfo._BMSErrors & (1 << 1));
    ui->packOvervoltageChB->setChecked(mainInfo._BMSErrors & (1 << 2));
    ui->packUndervoltageChB->setChecked(mainInfo._BMSErrors & (1 << 3));
    ui->chargeOverheatingChB->setChecked(mainInfo._BMSErrors & (1 << 4));
    ui->chargeUnderheatingChB->setChecked(mainInfo._BMSErrors & (1 << 5));
    ui->dischargeOverheatingChB->setChecked(mainInfo._BMSErrors & (1 << 6));
    ui->dischargeUnderheatingChB->setChecked(mainInfo._BMSErrors & (1 << 7));
    ui->chargeOvercurrentChB->setChecked(mainInfo._BMSErrors & (1 << 8));
    ui->dischargeOvercurrentChB->setChecked(mainInfo._BMSErrors & (1 << 9));
    ui->shortCurcuitChB->setChecked(mainInfo._BMSErrors & (1 << 10));
    ui->frontendChB->setChecked(mainInfo._BMSErrors & (1 << 11));
    ui->ConfigChB->setChecked(mainInfo._BMSErrors & (1 << 12));

    // lines' voltages, difference
    if (ui->linesVoltageTable->rowCount() != mainInfo.lines)
    {
        ui->linesVoltageTable->setRowCount(mainInfo.lines);
        for (int i = 0; i < mainInfo.lines; ++i)
        {
            QTableWidgetItem* item0 = new QTableWidgetItem;
            item0->setText(QString::number(i + 1));
            ui->linesVoltageTable->setItem(i, 0, item0);
            QTableWidgetItem* item1 = new QTableWidgetItem;
            ui->linesVoltageTable->setItem(i, 1, item1);
            item1->setText(QString::number(mainInfo.linesVoltage[i] / 1000.));
        }
    } else {
        for (int i = 0; i < mainInfo.lines; ++i)
        {
            ui->linesVoltageTable->item(i, 1)->setText(
                QString::number(mainInfo.linesVoltage[i] / 1000.));
        }
    }
    ui->differenceLabel->setText(QString::number(mainInfo.diff / 1000.));
}
