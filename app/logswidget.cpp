#include "logswidget.h"
#include "ui_logswidget.h"

LogsWidget::LogsWidget(QWidget *parent) : QWidget(parent),
    ui(new Ui::LogsWidget), plotter(new GraphPlotter), logsKept(false),
    logsFile()
{
    ui->setupUi(this);
    QGridLayout* plotterLayout = new QGridLayout;
    plotterLayout->addWidget(plotter);
    ui->plotterBase->setLayout(plotterLayout);
}

LogsWidget::~LogsWidget()
{
    if (logsKept) stopLogging();
    delete ui;
}

bool LogsWidget::logsAreKept() { return logsKept; }

void LogsWidget::stopLogging()
{
    logsFile.flush();
    logsFile.close();
}

void LogsWidget::on_keepLogsButton_clicked()
{
    if (logsKept)
    {
        logsFile.close();
        logsKept = false;
        ui->keepLogsButton->setText("Keep logs");
        ui->stateLabel->setText("No logs are kept");
    } else {
        QString fileName = QFileDialog::getSaveFileName(this,
            "Select a text file", "", "Text files(*.txt)", 0,
            QFileDialog::DontConfirmOverwrite);
        if (fileName.isEmpty()) return;
        logsFile.setFileName(fileName);
        if (!logsFile.open(QIODevice::ReadWrite | QIODevice::Append)) return;
        ui->keepLogsButton->setText("Stop keeping logs");
        ui->stateLabel->setText("Logs are kept");
        logsKept = true;
    }
}

void LogsWidget::slotWriteMainDataInFile(const MainInfo& mainInfo_)
{
    if (!logsKept) return;
    QTextStream stream(&logsFile);
    QString dataToWrite = QDateTime::currentDateTime().toString(
        "dd/MM/yyyy hh:mm:ss.zzz ");
    if (ui->checkBoxTotalVoltage->isChecked())
    {
        dataToWrite.append("Total voltage: " +
            QString::number(mainInfo_.totalVoltage / 100., 'f', 2) + " ");
    }
    if (ui->checkBoxCurrent->isChecked())
    {
        dataToWrite.append("Current: " +
            QString::number(mainInfo_.current / 100., 'f', 2) + " ");
    }
    if (ui->checkBoxCurrentCapacity->isChecked())
    {
        dataToWrite.append("Current capacity: " +
            QString::number(mainInfo_.currentCapacity / 100., 'f', 2) + " ");
    }
    if (ui->checkBoxAmountOfCycles->isChecked())
    {
        dataToWrite.append("Amount of cycles: " +
            QString::number(mainInfo_.cycles) + " ");
    }
    if (ui->checkBoxTemperatures->isChecked())
    {
        QString stringTemps = "Temperatures: ";
        for (auto temp_ : mainInfo_.temperatures)
        {
            stringTemps += QString::number(temp_ / 10., 'f', 1) + " ";
        }
        dataToWrite += stringTemps;
    }
    if (ui->checkBoxErrors->isChecked())
    {
    // TO DO
        if (!mainInfo_._BMSErrors) dataToWrite.append("No errors ");
        else dataToWrite.append("Error code: " +
            QString::number(mainInfo_._BMSErrors, 2) + " ");
    }
    if (ui->checkBoxLineBalanceStatus->isChecked())
    {
    // TO DO
        dataToWrite.append("Line balance state: " +
            QString::number(mainInfo_.linesBalanceStatus, 2) + " ");
    }
    if (ui->checkBoxFETState->isChecked())
    {
        QString chargeState = "on charge: ";
        chargeState += mainInfo_.chargeFETState ? "unlocked" : "locked";
        QString dischargeState = ", on discharge: ";
        dischargeState += mainInfo_.dischargeFETState ? "unlocked " : "locked ";
        dataToWrite.append("FET state " + chargeState + dischargeState);
    }
    if (ui->checkBoxLineVoltage->isChecked())
    {
        QString stringLineVoltages = "Lines' voltages: ";
        for (auto voltage : mainInfo_.linesVoltage)
        {
            stringLineVoltages += QString::number(voltage / 1000., 'f', 3) +
                " ";
        }
        dataToWrite += stringLineVoltages;
    }
    if (ui->checkBoxDifference->isChecked())
    {
        dataToWrite += "Difference: " +
            QString::number(mainInfo_.diff / 1000., 'f', 3) + " ";
    }
    dataToWrite.append("\n");
    stream << dataToWrite;
}
