#ifndef ADDINFOWIDGET_H
#define ADDINFOWIDGET_H
#include <QWidget>
#include <vector>
#include <iostream>
#include <functional>
#include "addinfoparser.h"
#include "mainwindow.h"

namespace Ui {
class AddInfoWidget;
}

class AddInfoWidget : public QWidget
{
Q_OBJECT

    AddInfoParser parser_;
    std::vector<uint8_t> expectedRegisterSizes;
    std::vector<std::function<void(const std::vector<QByteArray>&)>>
        dataSetters;
    std::vector<std::function<void()>> dataCleaners;

    Ui::AddInfoWidget *ui;

    uint16_t getUInt16InArray(const QByteArray& array, int index);
    QString JBDStringToQString(const QByteArray& array);
    QString getSoftwareVersion(char ch);
    QString getTemperature(const QByteArray& array);
    QString QByteArrayToDate(const QByteArray& array);
    void uncheckRadioButton(QRadioButton* radioButton);
public:
    explicit AddInfoWidget(QWidget* parent = nullptr);
    AddInfoParser* getParser();
    ~AddInfoWidget();


public slots:
    void slotShowDataOnGUI(const std::vector<QByteArray>& data_);
};

#endif // ADDINFOWIDGET_H
