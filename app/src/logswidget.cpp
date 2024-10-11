#include "logswidget.h"
#include "ui_logswidget.h"

int LogsWidget::getCurrentGraph()
{
    int res = -1;
    for (int i = 0; i < radioButtonsState.size(); ++i)
    {
        if (radioButtonsState[i])
        {
            res = i;
            break;
        }
    }
    return res;
}

void LogsWidget::repaintGraph()
{
    int currentGraph = getCurrentGraph();
    if (currentGraph < 0) return;
    for (int i = 0; i < customPlot->graphCount(); ++i)
    {
        customPlot->graph(i)->data()->clear();
    }
    switch (currentGraph)
    {
        case 3:
            for (auto& list_ : temperatureDots)
            {
                customPlot->addGraph();
            }
        break;
        case 4:
            for (auto& list_ : lineVoltageDots)
            {
                customPlot->addGraph();
            }
        break;
        default:
            customPlot->addGraph();
    }
    QVector<double> data_;
    QVector<double> x_;
    switch (currentGraph)
    {
        case 0:
        {
            if (totalVoltageDots.empty()) return;
            for (int i = 0; i < totalVoltageDots.size(); ++i)
            {
                data_.push_back(totalVoltageDots[i].first);
                x_.push_back(totalVoltageDots[i].second.msecsSinceStartOfDay());
            }
            customPlot->graph(0)->setData(x_, data_);
            double max_ = INT_MIN;
            double min_ = INT_MAX;
            for (int i = 0; i < data_.size(); ++i)
            {
                if (data_[i] > max_) max_ = data_[i];
                if (data_[i] < min_) min_ = data_[i];
            }
            customPlot->xAxis->setRange(x_.front(), x_.back());
            customPlot->yAxis->setRange(max_ + 1, min_ - 1);
        }
        break;
        case 1:
        {
            if (currentDots.empty()) return;
            for (int i = 0; i < currentDots.size(); ++i)
            {
                data_.push_back(currentDots[i].first);
                x_.push_back(currentDots[i].second.msecsSinceStartOfDay());
            }
            customPlot->graph(0)->setData(x_, data_);
            double max_ = INT_MIN;
            double min_ = INT_MAX;
            for (int i = 0; i < data_.size(); ++i)
            {
                if (data_[i] > max_) max_ = data_[i];
                if (data_[i] < min_) min_ = data_[i];
            }
            customPlot->xAxis->setRange(x_.front(), x_.back());
            customPlot->yAxis->setRange(max_ + 1, min_ - 1);
        }
        break;
        case 2:
        {
            if (currentCapacityDots.empty()) return;
            for (int i = 0; i < currentCapacityDots.size(); ++i)
            {
                data_.push_back(currentCapacityDots[i].first);
                x_.push_back(
                    currentCapacityDots[i].second.msecsSinceStartOfDay());
            }
            customPlot->graph(0)->setData(x_, data_);
            double max_ = INT_MIN;
            double min_ = INT_MAX;
            for (int i = 0; i < data_.size(); ++i)
            {
                if (data_[i] > max_) max_ = data_[i];
                if (data_[i] < min_) min_ = data_[i];
            }
            customPlot->xAxis->setRange(x_.front(), x_.back());
            customPlot->yAxis->setRange(max_ + 1, min_ - 1);
        }
        break;
        case 3:
        {
            if (temperatureDots.empty()) return;
            for (auto& list_ : temperatureDots)
            {
                if (list_.empty()) return;
            }
            QVector<QVector<double>> vectorsData_(temperatureDots.size());
            for (int i = 0; i < temperatureDots[0].size(); ++i)
            {
                x_.push_back(
                    temperatureDots[0][i].second.msecsSinceStartOfDay());
            }
            double max_ = INT_MIN;
            double min_ = INT_MAX;
            for (int i = 0; i < temperatureDots.size(); ++i)
            {
                for (int j = 0; j < temperatureDots[i].size(); ++j)
                {
                    double value = temperatureDots[i][j].first;
                    vectorsData_[i].push_back(value);
                    if (value > max_) max_ = value;
                    if (value < min_) min_ = value;
                }
                customPlot->graph(i)->setData(x_, vectorsData_[i]);
            }
            customPlot->xAxis->setRange(x_.front(), x_.back());
            customPlot->yAxis->setRange(max_ + 1, min_ - 1);
        }
        break;
        case 4:
        {
            if (lineVoltageDots.empty()) return;
            for (auto& list_ : lineVoltageDots)
            {
                if (list_.empty()) return;
            }
            QVector<QVector<double>> vectorsData_(lineVoltageDots.size());
            for (int i = 0; i < lineVoltageDots[0].size(); ++i)
            {
                x_.push_back(
                    lineVoltageDots[0][i].second.msecsSinceStartOfDay());
            }
            double max_ = INT_MIN;
            double min_ = INT_MAX;
            for (int i = 0; i < lineVoltageDots.size(); ++i)
            {
                for (int j = 0; j < lineVoltageDots[i].size(); ++j)
                {
                    double value = lineVoltageDots[i][j].first;
                    vectorsData_[i].push_back(value);
                    if (value > max_) max_ = value;
                    if (value < min_) min_ = value;
                }
                customPlot->graph(i)->setData(x_, vectorsData_[i]);
            }
            customPlot->xAxis->setRange(x_.front(), x_.back());
            customPlot->yAxis->setRange(max_ + 0.01, min_ - 0.01);
        }
        break;
        case 5:
        {
            if (differenceDots.empty()) return;
            for (int i = 0; i < differenceDots.size(); ++i)
            {
                data_.push_back(differenceDots[i].first);
                x_.push_back(differenceDots[i].second.msecsSinceStartOfDay());
            }
            customPlot->graph(0)->setData(x_, data_);
            double max_ = INT_MIN;
            double min_ = INT_MAX;
            for (int i = 0; i < data_.size(); ++i)
            {
                if (data_[i] > max_) max_ = data_[i];
                if (data_[i] < min_) min_ = data_[i];
            }
            customPlot->xAxis->setRange(x_.front(), x_.back());
            customPlot->yAxis->setRange(max_ + 0.01, min_ - 0.01);
        }
        break;
    }
    customPlot->replot();
}

LogsWidget::LogsWidget(QWidget *parent) : QWidget(parent),
    customPlot(new QCustomPlot),
    totalVoltageDots(),
    currentDots(),
    currentCapacityDots(),
    temperatureDots(),
    lineVoltageDots(),
    differenceDots(),
    logsKept(false),
    logsFile(),
    graphDataListSize(25),
    ui(new Ui::LogsWidget)
{
    ui->setupUi(this);
    radioButtonsState = {
        ui->radioButtonTotalVoltage->isChecked(),
        ui->radioButtonCurrent->isChecked(),
        ui->radioButtonCurrentCapacity->isChecked(),
        ui->radioButtonTemperatures->isChecked(),
        ui->radioButtonLineVoltage->isChecked(),
        ui->radioButtonDifference->isChecked()
    };
    customPlot->xAxis->setTickLabels(false);
    customPlot->yAxis->setLabel("Total voltage, V");
    QGridLayout* plotterLayout = new QGridLayout;
    plotterLayout->addWidget(customPlot);
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
        if (!mainInfo_._BMSErrors) dataToWrite.append("No errors ");
        else dataToWrite.append("Error code: " +
            QString::number(mainInfo_._BMSErrors, 2) + " ");
    }
    if (ui->checkBoxLineBalanceStatus->isChecked())
    {
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

void LogsWidget::slotUpdate03Data(const MainInfo& mainInfo)
{
    totalVoltageDots.push_back(
        std::make_pair(mainInfo.totalVoltage / 100., QTime::currentTime()));
    if (totalVoltageDots.size() > graphDataListSize)
    {
        totalVoltageDots.pop_front();
    }
    currentDots.push_back(
        std::make_pair(mainInfo.current / 100., QTime::currentTime()));
    if (currentDots.size() > graphDataListSize)
    {
        currentDots.pop_front();
    }
    currentCapacityDots.push_back(
        std::make_pair(mainInfo.currentCapacity / 100., QTime::currentTime()));
    if (currentCapacityDots.size() > graphDataListSize)
    {
        currentCapacityDots.pop_front();
    }
    if (temperatureDots.size() != mainInfo.temperatures.size())
    {
        temperatureDots.resize(mainInfo.temperatures.size());
    }
    for (int i = 0; i < mainInfo.temperatures.size(); ++i)
    {
        temperatureDots[i].push_back(
            std::make_pair(mainInfo.temperatures[i] / 10.,
            QTime::currentTime()));
        if (temperatureDots[i].size() > graphDataListSize)
        {
            temperatureDots[i].pop_front();
        }
    }
    int currentGraph = getCurrentGraph();
    if (currentGraph >= 0 && currentGraph <= 2 || currentGraph == 5)
    {
        repaintGraph();
    }
}

void LogsWidget::slotUpdate04Data(const MainInfo& mainInfo)
{
    if (lineVoltageDots.size() != mainInfo.linesVoltage.size())
    {
        lineVoltageDots.resize(mainInfo.linesVoltage.size());
    }
    for (int i = 0; i < lineVoltageDots.size(); ++i)
    {
        lineVoltageDots[i].push_back(
            std::make_pair(mainInfo.linesVoltage[i]/ 1000.,
            QTime::currentTime()));
        if (lineVoltageDots[i].size() > graphDataListSize)
        {
            lineVoltageDots[i].pop_front();
        }
    }
    differenceDots.push_back(
        std::make_pair(mainInfo.diff / 1000., QTime::currentTime()));
    if (differenceDots.size() > graphDataListSize)
    {
        differenceDots.pop_front();
    }
    int currentGraph = getCurrentGraph();
    if (currentGraph == 3 || currentGraph == 4) repaintGraph();
}

void LogsWidget::on_radioButtonTotalVoltage_clicked()
{
    radioButtonsState = {true, false, false, false, false, false};
    customPlot->yAxis->setLabel("Total voltage, V");
    repaintGraph();
}

void LogsWidget::on_radioButtonCurrent_clicked()
{
    radioButtonsState = {false, true, false, false, false, false};
    customPlot->yAxis->setLabel("Current, A");
    repaintGraph();
}

void LogsWidget::on_radioButtonCurrentCapacity_clicked()
{
    radioButtonsState = {false, false, true, false, false, false};
    customPlot->yAxis->setLabel("Current capacity, A*h");
    repaintGraph();
}

void LogsWidget::on_radioButtonTemperatures_clicked()
{
    radioButtonsState = {false, false, false, true, false, false};
    customPlot->yAxis->setLabel("Temperatures, °С");
    repaintGraph();
}

void LogsWidget::on_radioButtonLineVoltage_clicked()
{
    radioButtonsState = {false, false, false, false, true, false};
    customPlot->yAxis->setLabel("Line voltages, V");
    repaintGraph();
}

void LogsWidget::on_radioButtonDifference_clicked()
{
    radioButtonsState = {false, false, false, false, false, true};
    customPlot->yAxis->setLabel("Line voltage difference, V");
    repaintGraph();
}
