#include "maininfowidget.h"
#include "ui_maininfowidget.h"

MainInfoWidget::MainInfoWidget(QWidget* parent) : QWidget(parent),
    parser_(qobject_cast<MainWindow*>(parent)->getReader()),
    isActive(false),
    instantlyChangeParameters(false),
    ui(new Ui::MainInfoWidget)
{
    ui->setupUi(this);
    makeReadOnly(ui->ConfigChB);
    makeReadOnly(ui->chargeOvercurrentChB);
    makeReadOnly(ui->chargeOverheatingChB);
    makeReadOnly(ui->chargeUnderheatingChB);
    makeReadOnly(ui->dischargeOvercurrentChB);
    makeReadOnly(ui->dischargeOverheatingChB);
    makeReadOnly(ui->dischargeUnderheatingChB);
    makeReadOnly(ui->frontendChB);
    makeReadOnly(ui->lineOvervoltageChB);
    makeReadOnly(ui->lineUndervoltageChB);
    makeReadOnly(ui->packOvervoltageChB);
    makeReadOnly(ui->packUndervoltageChB);
    makeReadOnly(ui->shortCurcuitChB);
    batterySlider = new BatterySlider(ui->batterySliderWidget);
    ui->tempsTable->setFocusPolicy(Qt::NoFocus);
    ui->linesVoltageTable->setFocusPolicy(Qt::NoFocus);
    // main data setters
    QObject::connect(&parser_, SIGNAL(sgnData03Updated(const MainInfo&)),
        SLOT(slotData03Updated(const MainInfo&)));
    QObject::connect(&parser_, SIGNAL(sgnData04Updated(const MainInfo&)),
        SLOT(slotData04Updated(const MainInfo&)));
    // slots to set opposite text if according option chosen in menu
    connect(ui->FETChargeButton, &QPushButton::clicked,
        [this]()
        {
            if (instantlyChangeParameters)
            {
                ui->FETChargeLabel->setText(
                    parser_.mainInfo.chargeFETState ? "Closed" : "Opened");
            }
        });
    connect(ui->FETDischargeButton, &QPushButton::clicked,
        [this]()
        {
            if (instantlyChangeParameters)
            {
                ui->FETDischargeLabel->setText(
                    parser_.mainInfo.dischargeFETState ? "Closed" : "Opened");
            }
        });
    // change FET state buttons
    connect(ui->FETChargeButton, SIGNAL(clicked()),
        &parser_, SLOT(slotOnFETChargeButtonClicked()));
    connect(ui->FETDischargeButton, SIGNAL(clicked()),
        &parser_, SLOT(slotOnFETDischargeButtonClicked()));
}

void MainInfoWidget::resizeEvent(QResizeEvent*)
{
    int expectedWidth = ui->batterySliderWidget->width();
    int expectedHeight = expectedWidth * 1.3356;
    if (expectedHeight > ui->batterySliderWidget->height())
    {
        expectedWidth = ui->batterySliderWidget->height() / 1.3356;
        expectedHeight = ui->batterySliderWidget->height();
    }
    batterySlider->resize(expectedWidth, expectedHeight);
    batterySlider->move((ui->batterySliderWidget->width() - batterySlider->width()) / 2,
        (ui->batterySliderWidget->height() - batterySlider->height()) / 2);
}

MainInfoWidget::~MainInfoWidget() { delete ui; }

MainInfoParser* MainInfoWidget::getParser() { return &parser_; }

void MainInfoWidget::makeReadOnly(QCheckBox* checkBox)
{
    checkBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    checkBox->setFocusPolicy(Qt::NoFocus);
}

void MainInfoWidget::slotDataUpdated(const MainInfo& mainInfo)
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
        QString::number(mainInfo.totalVoltage / 100., 'f', 2));
    ui->currentLabel->setText(QString::number(mainInfo.current / 100., 'f', 2));
    ui->currentCapacityLabel->setText(
        QString::number(mainInfo.currentCapacity / 100., 'f', 2));
    ui->maxCapacityLabel->setText(
        QString::number(mainInfo.maximumCapacity / 100., 'f', 2));
    ui->cyclesLabel->setText(QString::number(mainInfo.cycles));

    // battery slider
    batterySlider->slotSetActive(true);
    batterySlider->slotSetValue(mainInfo.capacityInPercents);

    // temperatures
    if (ui->tempsTable->rowCount() != mainInfo.temperatures.size())
    {
        ui->tempsTable->setRowCount(mainInfo.temperatures.size());
        for (int i = 0; i < mainInfo.temperatures.size(); ++i)
        {
            QTableWidgetItem* item0 = new QTableWidgetItem;
            item0->setText("Thermoresistor " + QString::number(i + 1) + ":");
            ui->tempsTable->setItem(i, 0, item0);
            QTableWidgetItem* item1 = new QTableWidgetItem;
            ui->tempsTable->setItem(i, 1, item1);
            item1->setText(QString::number(mainInfo.temperatures[i], 'f', 1));
        }
    } else {
        for (int i = 0; i < mainInfo.temperatures.size(); ++i)
        {
            ui->tempsTable->item(i, 1)->setText(
                QString::number(mainInfo.temperatures[i] / 10., 'f', 1));
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
            item1->setText(QString::number(
                mainInfo.linesVoltage[i] / 1000., 'f', 3));
        }
    } else {
        for (int i = 0; i < mainInfo.lines; ++i)
        {
            ui->linesVoltageTable->item(i, 1)->setText(
                QString::number(mainInfo.linesVoltage[i] / 1000., 'f', 3));
        }
    }
    ui->differenceLabel->setText(QString::number(mainInfo.diff / 1000.,
        'f', 3));
}

void MainInfoWidget::slotData03Updated(const MainInfo& mainInfo)
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
        QString::number(mainInfo.totalVoltage / 100., 'f', 2));
    ui->currentLabel->setText(QString::number(mainInfo.current / 100., 'f', 2));
    ui->currentCapacityLabel->setText(
        QString::number(mainInfo.currentCapacity / 100., 'f', 2));
    ui->maxCapacityLabel->setText(
        QString::number(mainInfo.maximumCapacity / 100., 'f', 2));
    ui->cyclesLabel->setText(QString::number(mainInfo.cycles));

    // battery slider
    batterySlider->slotSetActive(true);
    batterySlider->slotSetValue(mainInfo.capacityInPercents);

    // temperatures
    if (ui->tempsTable->rowCount() != mainInfo.temperatures.size())
    {
        ui->tempsTable->setRowCount(mainInfo.temperatures.size());
        for (int i = 0; i < mainInfo.temperatures.size(); ++i)
        {
            QTableWidgetItem* item0 = new QTableWidgetItem;
            item0->setText("Thermoresistor " + QString::number(i + 1) + ":");
            ui->tempsTable->setItem(i, 0, item0);
            QTableWidgetItem* item1 = new QTableWidgetItem;
            ui->tempsTable->setItem(i, 1, item1);
            item1->setText(QString::number(mainInfo.temperatures[i], 'f', 1));
        }
    } else {
        for (int i = 0; i < mainInfo.temperatures.size(); ++i)
        {
            ui->tempsTable->item(i, 1)->setText(
                QString::number(mainInfo.temperatures[i] / 10., 'f', 1));
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
}

void MainInfoWidget::slotData04Updated(const MainInfo& mainInfo)
{
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
            item1->setText(QString::number(
                mainInfo.linesVoltage[i] / 1000., 'f', 3));
        }
    } else {
        for (int i = 0; i < mainInfo.lines; ++i)
        {
            ui->linesVoltageTable->item(i, 1)->setText(
                QString::number(mainInfo.linesVoltage[i] / 1000., 'f', 3));
        }
    }
    ui->differenceLabel->setText(QString::number(mainInfo.diff / 1000.,
        'f', 3));
}

void MainInfoWidget::slotNoAnswer()
{
    batterySlider->slotSetActive(false);
    ui->linesBalanceLabel->setText("");
    ui->FETChargeLabel->setText("");
    ui->FETDischargeLabel->setText("");
    ui->totalVoltageLabel->setText("");
    ui->currentLabel->setText("");
    ui->currentCapacityLabel->setText("");
    ui->maxCapacityLabel->setText("");
    ui->cyclesLabel->setText("");
    ui->tempsTable->setRowCount(0);
    ui->lineOvervoltageChB->setChecked(false);
    ui->lineUndervoltageChB->setChecked(false);
    ui->packOvervoltageChB->setChecked(false);
    ui->packUndervoltageChB->setChecked(false);
    ui->chargeOverheatingChB->setChecked(false);
    ui->chargeUnderheatingChB->setChecked(false);
    ui->dischargeOverheatingChB->setChecked(false);
    ui->dischargeUnderheatingChB->setChecked(false);
    ui->chargeOvercurrentChB->setChecked(false);
    ui->dischargeOvercurrentChB->setChecked(false);
    ui->shortCurcuitChB->setChecked(false);
    ui->frontendChB->setChecked(false);
    ui->ConfigChB->setChecked(false);
    ui->linesVoltageTable->setRowCount(0);
    ui->differenceLabel->setText("");
}

void MainInfoWidget::slotChangeInstantlyChangeParameters()
{
    instantlyChangeParameters = instantlyChangeParameters ? false : true;
}
