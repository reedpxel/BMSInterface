#ifndef ADDINFOWIDGET_H
#define ADDINFOWIDGET_H
#include <vector>
#include <iostream>
#include <functional>
#include <memory>
#include "addinfoparser.h"
#include "mainwindow.h"
#include "thermstatewindow.h"

namespace Ui {
class AddInfoWidget;
}

class AddInfoWidget : public QWidget
{
Q_OBJECT

    AddInfoParser parser_;
    std::vector<uint8_t> expectedRegisterSizes;
    std::vector<std::function<void(const QByteArray&)>>
        guiSetters;
    std::vector<std::function<void()>> guiCleaners;
    std::vector<QWidget*> editableRegistersObjects;
    std::vector<std::function<void()>> onFocusSet;
    std::vector<std::function<void()>> onEnterPressed;
    std::vector<std::function<void(const QByteArray&)>> setPreviousBuffer;

    QByteArray buffer_;
    int64_t previousWidgetIndex;
    uint8_t awaitedRegisterIndex; // as enter pressed, here stored index of the
    // widget, where written data must be written

    uint8_t amountOfThermoresistors; // to read 0x2e, got in 0x03, byte 22
    bool amountOfThermoresistorsGot;
    std::vector<bool> thermoresistorsState; // to enable and disable
                                            // thermoresistors

    bool dataRead; // initially false, gets true as widget is opened
                   // after reading of data
    bool instantlyChangeParameters;

    Ui::AddInfoWidget* ui;

    uint16_t getUInt16InArray(const QByteArray& array, int index);
    QString getSoftwareVersion(char ch);
    QString getTemperature(const QByteArray& array);
    void uncheckRadioButton(QRadioButton* radioButton);
    using it_t = std::vector<std::pair<QWidget*, std::function<void(
        const QByteArray&)>>>::iterator;
    bool binarySearch(it_t begin_, it_t end_, QWidget* value);

    void warningReadOnly(QLineEdit* lineEdit);
    void setTextUint16T(QLineEdit* lineEdit, uint8_t register_,
        unsigned signsAfterComma = 2);
    void setTextJBDString(QLineEdit* lineEdit, uint8_t register_);
    void setTextTemperature(QLineEdit* lineEdit, uint8_t register_);
    void setTextDoubleUint8T(QLineEdit* lineEdit0, QLineEdit* lineEdit1,
        uint8_t register_);
    QString setTextSoftwareVersion(const QByteArray& array);
    int findEditableWidgetIndex(QWidget* widget);
    QWidget* findEditableWidgetIndex(int index);

public:
    explicit AddInfoWidget(QWidget* parent = nullptr);
    AddInfoParser* getParser();
    ~AddInfoWidget();

public slots:
    void slotShowDataOnGUI(const std::vector<QByteArray>& data_);
    void slotOnFocusChanged(QWidget* old, QWidget* now);
    void slotOnWritingSuccess(const QByteArray& array);
    void slotOnWritingError();
    void slotOnTabChosen(int);
    void slotChangeInstantlyChangeParameters();
    void slotChangeThermoresistorsState(const std::vector<bool>&);
private slots:
    void on_changeThermStateButton_clicked();
};

#endif // ADDINFOWIDGET_H
