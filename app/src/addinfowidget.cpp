#include "addinfowidget.h"
#include "ui_addinfowidget.h"

uint16_t AddInfoWidget::getUInt16InArray(const QByteArray& array, int index)
{
    uint16_t res = (static_cast<uint16_t>(array[index]) << 8) +
        *reinterpret_cast<const uint8_t*>(array.data() + index + 1);
    return res;
}

QString AddInfoWidget::getSoftwareVersion(char ch)
{
    uint8_t low = *(reinterpret_cast<uint8_t*>(&ch)) % 16;
    uint8_t high = *(reinterpret_cast<uint8_t*>(&ch)) / 16;
    QString res = QString::number(static_cast<uint16_t>(high) & 0xff) +
        '.' +
        QString::number(static_cast<uint16_t>(low) & 0xff);
    return res;
}

QString AddInfoWidget::getTemperature(const QByteArray& array)
{
    int temp = static_cast<int>(getUInt16InArray(array, 0));
    temp -= 2731;
    return QString::number(temp / 10., 'f', 1);
}

void AddInfoWidget::uncheckRadioButton(QRadioButton* radioButton)
{
    radioButton->setChecked(true);
    radioButton->setAutoExclusive(false);
    radioButton->setChecked(false);
    radioButton->setAutoExclusive(true);
}

bool AddInfoWidget::binarySearch(it_t begin_, it_t end_, QWidget* value)
{
    if (value < (*begin_).first || value > (*end_).first) return false;
    if (value == (*begin_).first) return true;
    if (value == (*end_).first) return true;
    auto left_ = begin_;
    auto right_ = end_;
    while (right_ - left_ > 1)
    {
        auto middle_ = left_ + (right_ - left_) / 2;
        std::cout << reinterpret_cast<void*>((*middle_).first) << '\n';
        if (value == (*middle_).first) return true;
        if (value > (*middle_).first) left_ = middle_;
        else right_ = middle_;
    }
    return false;
}

void AddInfoWidget::warningReadOnly(QLineEdit* lineEdit)
{
    QMessageBox::warning(this, "Error", "This parameter is read-only");
    lineEdit->setText(buffer_);
    lineEdit->clearFocus();
}

void AddInfoWidget::setTextUint16T(QLineEdit* lineEdit, uint8_t register_,
    unsigned int signsAfterComma)
{
    QByteArray value =
        parser_.uint16_tToArray(lineEdit->text(), signsAfterComma);
    if (buffer_ != value) parser_.setRegisterValue(register_, value);
    if (instantlyChangeParameters) buffer_ = value;
    lineEdit->clearFocus();
}

void AddInfoWidget::setTextJBDString(QLineEdit* lineEdit, uint8_t register_)
{
    QByteArray value = parser_.QStringToJBDString(lineEdit->text());
    if (buffer_ != value) parser_.setRegisterValue(register_, value);
    if (instantlyChangeParameters) buffer_ = value;
    lineEdit->clearFocus();
}

void AddInfoWidget::setTextTemperature(QLineEdit* lineEdit, uint8_t register_)
{
    QByteArray value = parser_.temperatureToArray(lineEdit->text());
    if (buffer_ != value) parser_.setRegisterValue(register_, value);
    if (instantlyChangeParameters) buffer_ = value;
    lineEdit->clearFocus();
}

void AddInfoWidget::setTextDoubleUint8T(QLineEdit* lineEdit0,
    QLineEdit* lineEdit1, uint8_t register_)
{
    uint8_t first_ = lineEdit0->text().toInt();
    uint8_t second_ = lineEdit1->text().toInt();
    uint8_t data_[2] = {first_, second_};
    QByteArray value(reinterpret_cast<char*>(data_), 2);
    if (instantlyChangeParameters) buffer_ = value;
    if (buffer_ != value) parser_.setRegisterValue(register_, value);
    lineEdit0->clearFocus();
    lineEdit1->clearFocus();
}

QString AddInfoWidget::setTextSoftwareVersion(const QByteArray& array)
{
    return QString(reinterpret_cast<const char*>(array.data()));
}

int AddInfoWidget::findEditableWidgetIndex(QWidget* widget)
{
    for (unsigned i = 0; i < editableRegistersObjects.size(); ++i)
    {
        if (widget == editableRegistersObjects[i]) return i;
    }
    return -1;
}

QWidget* AddInfoWidget::findEditableWidgetIndex(int index)
{
    if (index < 0 || index >= editableRegistersObjects.size()) return nullptr;
    return editableRegistersObjects[index];
}

AddInfoWidget::AddInfoWidget(QWidget* parent) : QWidget(parent),
    parser_(qobject_cast<MainWindow*>(parent)->getReader()),
    expectedRegisterSizes({27, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 1, 1, 1, 22/*, 2*/}),
    guiSetters({
        [this](const QByteArray& data_)
        {
            amountOfThermoresistors = data_[22];
            amountOfThermoresistorsGot = true;
            ui->lineEdit_0x03->setText(getSoftwareVersion(data_[18]));
        }, // 0x03
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x05->setText(QString(data_));
        }, // 0x05
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x10->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x10
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x11->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x11
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x12->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x12
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x13->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x13
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x14->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x14
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x15->setText(parser_.QByteArrayToDate(data_));
        }, // 0x15
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x16->setText(QString::number(getUInt16InArray(
                data_, 0)));
        }, // 0x16
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x17->setText(QString::number(getUInt16InArray(
                data_, 0)));
        }, // 0x17
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x18->setText(getTemperature(data_));
        }, // 0x18
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x19->setText(getTemperature(data_));
        }, // 0x19
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1a->setText(getTemperature(data_));
        }, // 0x1a
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1b->setText(getTemperature(data_));
        }, // 0x1b
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1c->setText(getTemperature(data_));
        }, // 0x1c
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1d->setText(getTemperature(data_));
        }, // 0x1d
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1e->setText(getTemperature(data_));
        }, // 0x1e
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1f->setText(getTemperature(data_));
        }, // 0x1f
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x20->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x20
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x21->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x21
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x22->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x22
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x23->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x23
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x24->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x24
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x25->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x25
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x26->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x26
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x27->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x27
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x28->setText(QString::number(
                parser_.twoBytesToSignedInt(data_.data()) / 100., 'f', 2));
        }, // 0x28
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x29->setText(QString::number(
                parser_.twoBytesToSignedInt(data_.data()) / 100., 'f', 2));
        }, // 0x29
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x2a->setText(QString::number(getUInt16InArray(
                data_, 0) / 1000., 'f', 3));
        }, // 0x2a
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x2b->setText(QString::number(getUInt16InArray(
                data_, 0) / 1000., 'f', 3));
        }, // 0x2b
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x2c->setText(QString::number(getUInt16InArray(
                data_, 0)));
        }, // 0x2c
        [this](const QByteArray& data_)
        {
            if (!amountOfThermoresistorsGot) return;
            uint16_t value = (data_[0] << 8) + data_[1];
            QString res;
            thermoresistorsState.clear();
            for (unsigned i = 0; i < amountOfThermoresistors; ++i)
            {
                if ((value >> i) & 0b1) res += "On ";
                else res += "Off ";
                thermoresistorsState.push_back((value >> i) & 0b1);
            }
            ui->thermStateLabel->setText(res);
            ui->changeThermStateButton->setEnabled(true);
        }, // 0x2e
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x32->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x32
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x33->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x33
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x34->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x34
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x35->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x35
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x36->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x36
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x37->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x37
        [this](const QByteArray& data_)
        {
            ui->radioButton_0x38_07->setChecked((data_[0] & 0b10000000) >>
                7);
            uint8_t shortCurcuitValue = (data_[0] >> 3) & 0b00000111;
            uint8_t shortCurcuitTime = data_[0] & 0b00000011;
            uint8_t secOvercurrentValue = (data_[1] >> 3) & 0b00000111;
            uint8_t secOvercurrentTime = (data_[1]) & 0b00001111;
            switch (shortCurcuitValue)
            {
                case 0:
                    ui->radioButton_0x38_02100->setChecked(true);
                break;
                case 1:
                    ui->radioButton_0x38_02101->setChecked(true);
                break;
                case 2:
                    ui->radioButton_0x38_02102->setChecked(true);
                break;
                case 3:
                    ui->radioButton_0x38_02103->setChecked(true);
                break;
                case 4:
                    ui->radioButton_0x38_02104->setChecked(true);
                break;
                case 5:
                    ui->radioButton_0x38_02105->setChecked(true);
                break;
                case 6:
                    ui->radioButton_0x38_02106->setChecked(true);
                break;
                case 7:
                    ui->radioButton_0x38_02107->setChecked(true);
            }
            switch (shortCurcuitTime)
            {
                case 0:
                    ui->radioButton_0x38_0430->setChecked(true);
                break;
                case 1:
                    ui->radioButton_0x38_0431->setChecked(true);
                break;
                case 2:
                    ui->radioButton_0x38_0432->setChecked(true);
                break;
                case 3:
                    ui->radioButton_0x38_0433->setChecked(true);
            }
            switch (secOvercurrentValue)
            {
                case 0:
                    ui->radioButton_0x38_032100->setChecked(true);
                break;
                case 1:
                    ui->radioButton_0x38_032101->setChecked(true);
                break;
                case 2:
                    ui->radioButton_0x38_032102->setChecked(true);
                break;
                case 3:
                    ui->radioButton_0x38_032103->setChecked(true);
                break;
                case 4:
                    ui->radioButton_0x38_032104->setChecked(true);
                break;
                case 5:
                    ui->radioButton_0x38_032105->setChecked(true);
                break;
                case 6:
                    ui->radioButton_0x38_032106->setChecked(true);
                break;
                case 7:
                    ui->radioButton_0x38_032107->setChecked(true);
                break;
                case 8:
                    ui->radioButton_0x38_032108->setChecked(true);
                break;
                case 9:
                    ui->radioButton_0x38_032109->setChecked(true);
                break;
                case 10:
                    ui->radioButton_0x38_03210a->setChecked(true);
                break;
                case 11:
                    ui->radioButton_0x38_03210b->setChecked(true);
                break;
                case 12:
                    ui->radioButton_0x38_03210c->setChecked(true);
                break;
                case 13:
                    ui->radioButton_0x38_03210d->setChecked(true);
                break;
                case 14:
                    ui->radioButton_0x38_03210e->setChecked(true);
                break;
                case 15:
                    ui->radioButton_0x38_03210f->setChecked(true);
            }
            switch (secOvercurrentTime)
            {
                case 0:
                    ui->radioButton_0x38_06540->setChecked(true);
                break;
                case 1:
                    ui->radioButton_0x38_06541->setChecked(true);
                break;
                case 2:
                    ui->radioButton_0x38_06542->setChecked(true);
                break;
                case 3:
                    ui->radioButton_0x38_06543->setChecked(true);
                break;
                case 4:
                    ui->radioButton_0x38_06544->setChecked(true);
                break;
                case 5:
                    ui->radioButton_0x38_06545->setChecked(true);
                break;
                case 6:
                    ui->radioButton_0x38_06546->setChecked(true);
                break;
                case 7:
                    ui->radioButton_0x38_06547  ->setChecked(true);
            }
        }, // 0x38
        [this](const QByteArray& data_)
        {
            uint8_t undervoltageDelay = data_[0] >> 6;
            uint8_t overvoltageDelay = (data_[0] >> 4) & 0b00000011;
            switch (undervoltageDelay)
            {
                case 0:
                    ui->radioButton_0x39_0760->setChecked(true);
                break;
                case 1:
                    ui->radioButton_0x39_0761->setChecked(true);
                break;
                case 2:
                    ui->radioButton_0x39_0762->setChecked(true);
                break;
                case 3:
                    ui->radioButton_0x39_0763->setChecked(true);
            }
            switch (overvoltageDelay)
            {
                case 0:
                    ui->radioButton_0x39_0540->setChecked(true);
                break;
                case 1:
                    ui->radioButton_0x39_0541->setChecked(true);
                break;
                case 2:
                    ui->radioButton_0x39_0542->setChecked(true);
                break;
                case 3:
                    ui->radioButton_0x39_0543->setChecked(true);
            }
            ui->lineEdit_0x39_1->setText(QString::number(data_[1]));
        }, // 0x39
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3a_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3a_1->setText(QString::number(data_[1]));
        }, // 0x3a
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3b_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3b_1->setText(QString::number(data_[1]));
        }, // 0x3b
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3c_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3c_1->setText(QString::number(data_[1]));
        }, // 0x3c
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3d_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3d_1->setText(QString::number(data_[1]));
        }, // 0x3d
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3e_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3e_1->setText(QString::number(data_[1]));
        }, // 0x3e
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3f_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3f_1->setText(QString::number(data_[1]));
        }, // 0x3f
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0xa0->setText(parser_.JBDStringToQString(data_));
        }, // 0xa0
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0xa1->setText(parser_.JBDStringToQString(data_));
        }, // 0xa1
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0xa2->setText(parser_.JBDStringToQString(data_));
        }, // 0xa2
        [this](const QByteArray& data_)
        {
            ui->label_0xaa_0->setText(QString::number(getUInt16InArray(
                data_, 0)));
            ui->label_0xaa_1->setText(QString::number(getUInt16InArray(
                data_, 2)));
            ui->label_0xaa_2->setText(QString::number(getUInt16InArray(
                data_, 4)));
            ui->label_0xaa_3->setText(QString::number(getUInt16InArray(
                data_, 6)));
            ui->label_0xaa_4->setText(QString::number(getUInt16InArray(
                data_, 8)));
            ui->label_0xaa_5->setText(QString::number(getUInt16InArray(
                data_, 10)));
            ui->label_0xaa_6->setText(QString::number(getUInt16InArray(
                data_, 12)));
            ui->label_0xaa_7->setText(QString::number(getUInt16InArray(
                data_, 14)));
            ui->label_0xaa_8->setText(QString::number(getUInt16InArray(
                data_, 16)));
            ui->label_0xaa_9->setText(QString::number(getUInt16InArray(
                data_, 18)));
            ui->label_0xaa_10->setText(QString::number(getUInt16InArray(
                data_, 20)));
        }/*, */// 0xaa
        // [this](const QByteArray& data_)
        // {
        //     switch (data_[1])
        //     {
        //         case 0:
        //             ui->radioButton_0xe2_0->setChecked(true);
        //         break;
        //         case 1:
        //             ui->radioButton_0xe2_1->setChecked(true);
        //         break;
        //         case 2:
        //             ui->radioButton_0xe2_2->setChecked(true);
        //         break;
        //         case 3:
        //             ui->radioButton_0xe2_3->setChecked(true);
        //     }
        // }
    }),
    guiCleaners({
        [this](){ ui->lineEdit_0x03->setText(""); }, // 0x03
        [this](){ ui->lineEdit_0x05->setText(""); }, // 0x05
        [this](){ ui->lineEdit_0x10->setText(""); }, // 0x10
        [this](){ ui->lineEdit_0x11->setText(""); }, // 0x11
        [this](){ ui->lineEdit_0x12->setText(""); }, // 0x12
        [this](){ ui->lineEdit_0x13->setText(""); }, // 0x13
        [this](){ ui->lineEdit_0x14->setText(""); }, // 0x14
        [this](){ ui->lineEdit_0x15->setText(""); }, // 0x15
        [this](){ ui->lineEdit_0x16->setText(""); }, // 0x16
        [this](){ ui->lineEdit_0x17->setText(""); }, // 0x17
        [this](){ ui->lineEdit_0x18->setText(""); }, // 0x18
        [this](){ ui->lineEdit_0x19->setText(""); }, // 0x19
        [this](){ ui->lineEdit_0x1a->setText(""); }, // 0x1a
        [this](){ ui->lineEdit_0x1b->setText(""); }, // 0x1b
        [this](){ ui->lineEdit_0x1c->setText(""); }, // 0x1c
        [this](){ ui->lineEdit_0x1d->setText(""); }, // 0x1d
        [this](){ ui->lineEdit_0x1e->setText(""); }, // 0x1e
        [this](){ ui->lineEdit_0x1f->setText(""); }, // 0x1f
        [this](){ ui->lineEdit_0x20->setText(""); }, // 0x20
        [this](){ ui->lineEdit_0x21->setText(""); }, // 0x21
        [this](){ ui->lineEdit_0x22->setText(""); }, // 0x22
        [this](){ ui->lineEdit_0x23->setText(""); }, // 0x23
        [this](){ ui->lineEdit_0x24->setText(""); }, // 0x24
        [this](){ ui->lineEdit_0x25->setText(""); }, // 0x25
        [this](){ ui->lineEdit_0x26->setText(""); }, // 0x26
        [this](){ ui->lineEdit_0x27->setText(""); }, // 0x27
        [this](){ ui->lineEdit_0x28->setText(""); }, // 0x28
        [this](){ ui->lineEdit_0x29->setText(""); }, // 0x29
        [this](){ ui->lineEdit_0x2a->setText(""); }, // 0x2a
        [this](){ ui->lineEdit_0x2b->setText(""); }, // 0x2b
        [this](){ ui->lineEdit_0x2c->setText(""); }, // 0x2c
        [this]()
        {
            ui->thermStateLabel->setText("");
            ui->changeThermStateButton->setEnabled(false);
        }, // 0x2e
        [this](){ ui->lineEdit_0x32->setText(""); }, // 0x32
        [this](){ ui->lineEdit_0x33->setText(""); }, // 0x33
        [this](){ ui->lineEdit_0x34->setText(""); }, // 0x34
        [this](){ ui->lineEdit_0x35->setText(""); }, // 0x35
        [this](){ ui->lineEdit_0x36->setText(""); }, // 0x36
        [this](){ ui->lineEdit_0x37->setText(""); }, // 0x37
        [this]()
        {
            uncheckRadioButton(ui->radioButton_0x38_07);
            uncheckRadioButton(ui->radioButton_0x38_02100);
            uncheckRadioButton(ui->radioButton_0x38_0430);
            uncheckRadioButton(ui->radioButton_0x38_032100);
            uncheckRadioButton(ui->radioButton_0x38_06540);
        }, // 0x38
        [this]()
        {
            uncheckRadioButton(ui->radioButton_0x39_0760);
            uncheckRadioButton(ui->radioButton_0x39_0540);
            ui->lineEdit_0x39_1->setText("");
        }, // 0x39
        [this]()
        {
            ui->lineEdit_0x3a_0->setText("");
            ui->lineEdit_0x3a_1->setText("");
        }, // 0x3a
        [this]()
        {
            ui->lineEdit_0x3b_0->setText("");
            ui->lineEdit_0x3b_1->setText("");
        }, // 0x3b
        [this]()
        {
            ui->lineEdit_0x3c_0->setText("");
            ui->lineEdit_0x3c_1->setText("");
        }, // 0x3c
        [this]()
        {
            ui->lineEdit_0x3d_0->setText("");
            ui->lineEdit_0x3d_1->setText("");
        }, // 0x3d
        [this]()
        {
            ui->lineEdit_0x3e_0->setText("");
            ui->lineEdit_0x3e_1->setText("");
        }, // 0x3e
        [this]()
        {
            ui->lineEdit_0x3f_0->setText("");
            ui->lineEdit_0x3f_1->setText("");
        }, // 0x3f
        [this](){ ui->lineEdit_0xa0->setText(""); }, // 0xa0
        [this](){ ui->lineEdit_0xa1->setText(""); }, // 0xa1
        [this](){ ui->lineEdit_0xa2->setText(""); }, // 0xa2
        [this]()
        {
            ui->label_0xaa_0->setText("");
            ui->label_0xaa_1->setText("");
            ui->label_0xaa_2->setText("");
            ui->label_0xaa_3->setText("");
            ui->label_0xaa_4->setText("");
            ui->label_0xaa_5->setText("");
            ui->label_0xaa_6->setText("");
            ui->label_0xaa_7->setText("");
            ui->label_0xaa_8->setText("");
            ui->label_0xaa_9->setText("");
            ui->label_0xaa_10->setText("");
        }, // 0xaa
        // [this]() { uncheckRadioButton(ui->radioButton_0xe2_0); } // 0xe2
    }),
    editableRegistersObjects(),
    onFocusSet({
        [this]()
        {
            buffer_ = QByteArray(ui->lineEdit_0x03->text().toUtf8());
        }, // 0x03, read only
        [this]()
        {
            buffer_ = QByteArray(ui->lineEdit_0x05->text().toUtf8());
        }, // 0x05, read only
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x10->text(), 2);
        }, // 0x10
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x11->text(), 2);
        }, // 0x11
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x12->text(), 2);
        }, // 0x12
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x13->text(), 2);
        }, // 0x13
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x14->text(), 2);
        }, // 0x14
        [this]()
        {
            buffer_ = parser_.dateToQByteArray(ui->lineEdit_0x15->text());
        }, // 0x15
        [this]()
        {
            buffer_ = parser_.QStringToJBDString(ui->lineEdit_0x16->text());
        }, // 0x16
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x17->text());
        }, // 0x17
        [this]()
        {
            buffer_ = parser_.temperatureToArray(ui->lineEdit_0x18->text());
        }, // 0x18
        [this]()
        {
            buffer_ = parser_.temperatureToArray(ui->lineEdit_0x19->text());
        }, // 0x19
        [this]()
        {
            buffer_ = parser_.temperatureToArray(ui->lineEdit_0x1a->text());
        }, // 0x1a
        [this]()
        {
            buffer_ = parser_.temperatureToArray(ui->lineEdit_0x1b->text());
        }, // 0x1b
        [this]()
        {
            buffer_ = parser_.temperatureToArray(ui->lineEdit_0x1c->text());
        }, // 0x1c
        [this]()
        {
            buffer_ = parser_.temperatureToArray(ui->lineEdit_0x1d->text());
        }, // 0x1d
        [this]()
        {
            buffer_ = parser_.temperatureToArray(ui->lineEdit_0x1e->text());
        }, // 0x1e
        [this]()
        {
            buffer_ = parser_.temperatureToArray(ui->lineEdit_0x1f->text());
        }, // 0x1f
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x20->text(), 2);
        }, // 0x20
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x21->text(), 2);
        }, // 0x21
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x22->text(), 2);
        }, // 0x22
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x23->text(), 2);
        }, // 0x23
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x24->text(), 2);
        }, // 0x24
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x25->text(), 2);
        }, // 0x25
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x26->text(), 2);
        }, // 0x26
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x27->text(), 2);
        }, // 0x27
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x28->text(), 2);
        }, // 0x28
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x29->text(), 2);
        }, // 0x29
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x2a->text(), 3);
        }, // 0x2a
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x2b->text(), 3);
        }, // 0x2b
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x2c->text());
        }, // 0x2c
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x32->text(), 2);
        }, // 0x32
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x33->text(), 2);
        }, // 0x33
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x34->text(), 2);
        }, // 0x34
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x35->text(), 2);
        }, // 0x35
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x36->text(), 2);
        }, // 0x36
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x37->text(), 2);
        }, // 0x37
        [this]()
        {
            buffer_ = parser_.uint16_tToArray(ui->lineEdit_0x39_1->text());
        }, // 0x39_1
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3a_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3a_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3a_0
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3a_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3a_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3a_1
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3b_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3b_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3b_0
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3b_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3b_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3b_1
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3c_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3c_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3c_0
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3c_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3c_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3c_1
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3d_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3d_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3d_0
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3d_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3d_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3d_1
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3e_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3e_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3e_0
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3e_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3e_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3e_1
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3f_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3f_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3f_0
        [this]()
        {
            uint8_t first_ = ui->lineEdit_0x3f_0->text().toInt();
            uint8_t second_ = ui->lineEdit_0x3f_1->text().toInt();
            uint8_t data_[2] = {first_, second_};
            buffer_ = QByteArray(reinterpret_cast<char*>(data_), 2);
        }, // 0x3f_1
        [this]()
        {
            buffer_ = parser_.QStringToJBDString(ui->lineEdit_0xa0->text());
        }, // 0xa0
        [this]()
        {
            buffer_ = parser_.QStringToJBDString(ui->lineEdit_0xa1->text());
        }, // 0xa1
        [this]()
        {
            buffer_ = parser_.QStringToJBDString(ui->lineEdit_0xa2->text());
        } // 0xa2
    }),
    onEnterPressed({
        [this](){ warningReadOnly(ui->lineEdit_0x03); }, // 0x03
        [this](){ warningReadOnly(ui->lineEdit_0x05); }, // 0x05
        [this](){ setTextUint16T(ui->lineEdit_0x10, 0x10); }, // 0x10
        [this](){ setTextUint16T(ui->lineEdit_0x11, 0x11); }, // 0x11
        [this](){ setTextUint16T(ui->lineEdit_0x12, 0x12); }, // 0x12
        [this](){ setTextUint16T(ui->lineEdit_0x13, 0x13); }, // 0x13
        [this](){ setTextUint16T(ui->lineEdit_0x14, 0x14); }, // 0x14
        [this]()
        {
            QByteArray array =
                parser_.dateToQByteArray(ui->lineEdit_0x15->text());
            if (buffer_ != array) parser_.setRegisterValue(0x15, array);
            if (instantlyChangeParameters) buffer_ = array;
            ui->lineEdit_0x15->clearFocus();
        }, // 0x15
        [this](){ setTextJBDString(ui->lineEdit_0x16, 0x16); }, // 0x16
        [this](){ setTextUint16T(ui->lineEdit_0x17, 0x17, 0); }, // 0x17
        [this](){ setTextTemperature(ui->lineEdit_0x18, 0x18); }, // 0x18
        [this](){ setTextTemperature(ui->lineEdit_0x19, 0x19); }, // 0x19
        [this](){ setTextTemperature(ui->lineEdit_0x1a, 0x1a); }, // 0x1a
        [this](){ setTextTemperature(ui->lineEdit_0x1b, 0x1b); }, // 0x1b
        [this](){ setTextTemperature(ui->lineEdit_0x1c, 0x1c); }, // 0x1c
        [this](){ setTextTemperature(ui->lineEdit_0x1d, 0x1d); }, // 0x1d
        [this](){ setTextTemperature(ui->lineEdit_0x1e, 0x1e); }, // 0x1e
        [this](){ setTextTemperature(ui->lineEdit_0x1f, 0x1f); }, // 0x1f
        [this](){ setTextUint16T(ui->lineEdit_0x20, 0x20); }, // 0x20
        [this](){ setTextUint16T(ui->lineEdit_0x21, 0x21); }, // 0x21
        [this](){ setTextUint16T(ui->lineEdit_0x22, 0x22); }, // 0x22
        [this](){ setTextUint16T(ui->lineEdit_0x23, 0x23); }, // 0x23
        [this](){ setTextUint16T(ui->lineEdit_0x24, 0x24); }, // 0x24
        [this](){ setTextUint16T(ui->lineEdit_0x25, 0x25); }, // 0x25
        [this](){ setTextUint16T(ui->lineEdit_0x26, 0x26); }, // 0x26
        [this](){ setTextUint16T(ui->lineEdit_0x27, 0x27); }, // 0x27
        [this](){ setTextUint16T(ui->lineEdit_0x28, 0x28); }, // 0x28
        [this](){ setTextUint16T(ui->lineEdit_0x29, 0x29); }, // 0x29
        [this](){ setTextUint16T(ui->lineEdit_0x2a, 0x2a, 3); }, // 0x2a
        [this](){ setTextUint16T(ui->lineEdit_0x2b, 0x2b, 3); }, // 0x2b
        [this](){ setTextUint16T(ui->lineEdit_0x2c, 0x2c, 0); }, // 0x2c
        [this](){ setTextUint16T(ui->lineEdit_0x32, 0x32); }, // 0x32
        [this](){ setTextUint16T(ui->lineEdit_0x33, 0x33); }, // 0x33
        [this](){ setTextUint16T(ui->lineEdit_0x34, 0x34); }, // 0x34
        [this](){ setTextUint16T(ui->lineEdit_0x35, 0x35); }, // 0x35
        [this](){ setTextUint16T(ui->lineEdit_0x36, 0x36); }, // 0x36
        [this](){ setTextUint16T(ui->lineEdit_0x37, 0x37); }, // 0x37
        [this](){ setTextUint16T(ui->lineEdit_0x39_1, 0x39, 0); }, // 0x39_1
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3a_0, ui->lineEdit_0x3a_1,
            0x3a); }, // 0x3a_0
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3a_0, ui->lineEdit_0x3a_1,
            0x3a); }, // 0x3a_1
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3b_0, ui->lineEdit_0x3b_1,
            0x3b); }, // 0x3b_0
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3b_0, ui->lineEdit_0x3b_1,
            0x3b); }, // 0x3b_1
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3c_0, ui->lineEdit_0x3c_1,
            0x3c); }, // 0x3c_0
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3c_0, ui->lineEdit_0x3c_1,
            0x3c); }, // 0x3c_1
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3d_0, ui->lineEdit_0x3d_1,
            0x3d); }, // 0x3d_0
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3d_0, ui->lineEdit_0x3d_1,
            0x3d); }, // 0x3d_1
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3e_0, ui->lineEdit_0x3e_1,
            0x3e); }, // 0x3e_0
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3e_0, ui->lineEdit_0x3e_1,
            0x3e); }, // 0x3e_1
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3f_0, ui->lineEdit_0x3f_1,
            0x3f); }, // 0x3f_0
        [this](){ setTextDoubleUint8T(ui->lineEdit_0x3f_0, ui->lineEdit_0x3f_1,
            0x3f); }, // 0x3f_1
        [this](){ setTextJBDString(ui->lineEdit_0xa0, 0xa0); }, // 0xa0
        [this](){ setTextJBDString(ui->lineEdit_0xa1, 0xa1); }, // 0xa1
        [this](){ setTextJBDString(ui->lineEdit_0xa2, 0xa2); } // 0xa2
    }),
    setPreviousBuffer({
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x03->setText(setTextSoftwareVersion(data_));
        }, // 0x03
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x05->setText(data_);
        }, // 0x05
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x10->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x10
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x11->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x11
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x12->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x12
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x13->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x13
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x14->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x14
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x15->setText(parser_.QByteArrayToDate(data_));
        }, // 0x15
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x16->setText(parser_.JBDStringToQString(data_));
        }, // 0x16
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x17->setText(QString::number(getUInt16InArray(
                data_, 0)));
        }, // 0x17
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x18->setText(getTemperature(data_));
        }, // 0x18
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x19->setText(getTemperature(data_));
        }, // 0x19
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1a->setText(getTemperature(data_));
        }, // 0x1a
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1b->setText(getTemperature(data_));
        }, // 0x1b
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1c->setText(getTemperature(data_));
        }, // 0x1c
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1d->setText(getTemperature(data_));
        }, // 0x1d
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1e->setText(getTemperature(data_));
        }, // 0x1e
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x1f->setText(getTemperature(data_));
        }, // 0x1f
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x20->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x20
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x21->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x21
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x22->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x22
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x23->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x23
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x24->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x24
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x25->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x25
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x26->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x26
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x27->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x27
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x28->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x28
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x29->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x29
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x2a->setText(QString::number(getUInt16InArray(
                data_, 0) / 1000., 'f', 3));
        }, // 0x2a
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x2b->setText(QString::number(getUInt16InArray(
                data_, 0) / 1000., 'f', 3));
        }, // 0x2b
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x2c->setText(QString::number(getUInt16InArray(
                data_, 0)));
        }, // 0x2c
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x32->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x32
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x33->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x33
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x34->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x34
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x35->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x35
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x36->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x36
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x37->setText(QString::number(getUInt16InArray(
                data_, 0) / 100., 'f', 2));
        }, // 0x37
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x39_1->setText(QString::number(data_[1]));
        }, // 0x39
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3a_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3a_1->setText(QString::number(data_[1]));
        }, // 0x3a
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3a_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3a_1->setText(QString::number(data_[1]));
        }, // 0x3a
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3b_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3b_1->setText(QString::number(data_[1]));
        }, // 0x3b
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3b_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3b_1->setText(QString::number(data_[1]));
        }, // 0x3b
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3c_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3c_1->setText(QString::number(data_[1]));
        }, // 0x3c
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3c_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3c_1->setText(QString::number(data_[1]));
        }, // 0x3c
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3d_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3d_1->setText(QString::number(data_[1]));
        }, // 0x3d
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3d_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3d_1->setText(QString::number(data_[1]));
        }, // 0x3d
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3e_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3e_1->setText(QString::number(data_[1]));
        }, // 0x3e
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3e_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3e_1->setText(QString::number(data_[1]));
        }, // 0x3e
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3f_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3f_1->setText(QString::number(data_[1]));
        }, // 0x3f
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0x3f_0->setText(QString::number(data_[0]));
            ui->lineEdit_0x3f_1->setText(QString::number(data_[1]));
        }, // 0x3f
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0xa0->setText(parser_.JBDStringToQString(data_));
        }, // 0xa0
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0xa1->setText(parser_.JBDStringToQString(data_));
        }, // 0xa1
        [this](const QByteArray& data_)
        {
            ui->lineEdit_0xa2->setText(parser_.JBDStringToQString(data_));
        } // 0xa2
    }),
    buffer_(),
    previousWidgetIndex(-1),
    awaitedRegisterIndex(0),
    amountOfThermoresistors(0),
    amountOfThermoresistorsGot(false),
    thermoresistorsState(),
    dataRead(false),
    instantlyChangeParameters(false),
    ui(new Ui::AddInfoWidget)
{
    ui->setupUi(this);

    // setting up editableObjects
    editableRegistersObjects = {
        ui->lineEdit_0x03,
        ui->lineEdit_0x05,
        ui->lineEdit_0x10,
        ui->lineEdit_0x11,
        ui->lineEdit_0x12,
        ui->lineEdit_0x13,
        ui->lineEdit_0x14,
        ui->lineEdit_0x15,
        ui->lineEdit_0x16,
        ui->lineEdit_0x17,
        ui->lineEdit_0x18,
        ui->lineEdit_0x19,
        ui->lineEdit_0x1a,
        ui->lineEdit_0x1b,
        ui->lineEdit_0x1c,
        ui->lineEdit_0x1d,
        ui->lineEdit_0x1e,
        ui->lineEdit_0x1f,
        ui->lineEdit_0x20,
        ui->lineEdit_0x21,
        ui->lineEdit_0x22,
        ui->lineEdit_0x23,
        ui->lineEdit_0x24,
        ui->lineEdit_0x25,
        ui->lineEdit_0x26,
        ui->lineEdit_0x27,
        ui->lineEdit_0x28,
        ui->lineEdit_0x29,
        ui->lineEdit_0x2a,
        ui->lineEdit_0x2b,
        ui->lineEdit_0x2c, // 30 no 0x26 if (i == 30 || i == 37) ++i
        ui->lineEdit_0x32, // 32
        ui->lineEdit_0x33, // 33
        ui->lineEdit_0x34, // 34
        ui->lineEdit_0x35, // 35
        ui->lineEdit_0x36, // 36
        ui->lineEdit_0x37, // 37
        ui->lineEdit_0x39_1, // 39 no 0x38
        ui->lineEdit_0x3a_0, // 40
        ui->lineEdit_0x3a_1, // 40
        ui->lineEdit_0x3b_0, // 41
        ui->lineEdit_0x3b_1, // 41
        ui->lineEdit_0x3c_0, // 42
        ui->lineEdit_0x3c_1, // 42
        ui->lineEdit_0x3d_0, // 43
        ui->lineEdit_0x3d_1, // 43
        ui->lineEdit_0x3e_0, // 44
        ui->lineEdit_0x3e_1, // 44
        ui->lineEdit_0x3f_0, // 45
        ui->lineEdit_0x3f_1, // 45
        ui->lineEdit_0xa0, // 46
        ui->lineEdit_0xa1, // 47
        ui->lineEdit_0xa2 // 48
    };

    QObject::connect(ui->updateButton, SIGNAL(clicked()), &parser_,
        SLOT(slotPrepareReadAddData()));
    QObject::connect(ui->updateButton, &QPushButton::clicked,
        ui->updateButton, [this](){ ui->updateButton->setEnabled(false); });
    QObject::connect(ui->updateButton, SIGNAL(clicked()), &parser_,
        SIGNAL(sgnReadingBegun()));
    // updating information whether amount of thermoresistor got or not
    QObject::connect(ui->updateButton, &QPushButton::clicked,
        [this](){ amountOfThermoresistorsGot = false; });
    QObject::connect(&parser_,
        SIGNAL(sgnSendDataToGUI(const std::vector<QByteArray>&)),
        SLOT(slotShowDataOnGUI(const std::vector<QByteArray>&)));
    // handling QApplication::focusChanged signal
    QObject::connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
        SLOT(slotOnFocusChanged(QWidget*, QWidget*)));
    // connecting signals ans lambdas that send new data to BMS
    for (unsigned i = 0; i < editableRegistersObjects.size(); ++i)
    {
        QObject::connect(qobject_cast<QLineEdit*>(editableRegistersObjects[i]),
            &QLineEdit::returnPressed, onEnterPressed[i]);
        QObject::connect(qobject_cast<QLineEdit*>(editableRegistersObjects[i]),
            &QLineEdit::returnPressed, [this, i](){ awaitedRegisterIndex = i; });
    }
    // updating GUI as data written in registers
    connect(&parser_, SIGNAL(sgnWritingSuccess(const QByteArray&)),
        SLOT(slotOnWritingSuccess(const QByteArray&)));
    connect(&parser_, SIGNAL(sgnWritingError()),
        SLOT(slotOnWritingError()));
}

AddInfoParser* AddInfoWidget::getParser() { return &parser_; }

AddInfoWidget::~AddInfoWidget() { delete ui; }

void AddInfoWidget::slotShowDataOnGUI(const std::vector<QByteArray>& data_)
{
    std::vector<uint8_t> unreadRegisters;
    for (unsigned i = 0; i < guiSetters.size(); ++i)
    {
        if (data_[i].size() < expectedRegisterSizes[i])
        {
            unreadRegisters.push_back(i);
            guiCleaners[i]();
        } else {
            std::invoke(guiSetters[i], data_[i]);
        }
    }
    if (!unreadRegisters.empty())
    {
        QString msg;
        if (unreadRegisters.size() == expectedRegisterSizes.size())
        {
            msg = "No reply from BMS got";
        } else {
            msg = "The following registers were failed to read: ";
            for (unsigned i = 0; i < unreadRegisters.size() - 1; ++i)
            {
                msg += "0x" + QString::number(unreadRegisters[i], 16) + ", ";
            }
            msg += "0x" + QString::number(unreadRegisters.back(), 16);
        }
        QMessageBox::information(this, "Reading result", msg);
    }
    ui->updateButton->setEnabled(true);
}

void AddInfoWidget::slotOnFocusChanged(QWidget*, QWidget* now)
{
    if (previousWidgetIndex >= 0)
    {
        std::invoke(setPreviousBuffer[previousWidgetIndex], buffer_);
    }
    // searching whether a newly focused widget is among those, with which
    // data can be written in BMS and updating previousWidgetIndex
    previousWidgetIndex = findEditableWidgetIndex(now);
    // if widget is not among writing to BMS, there is no need to do anything
    if (previousWidgetIndex < 0) return;

    onFocusSet[previousWidgetIndex]();
}

void AddInfoWidget::slotOnWritingSuccess(const QByteArray& array)
{
    std::cout << "new buffer: ";
    for (int i = 0; i < array.size(); ++i)
    {
        std::cout << std::hex <<
            (static_cast<uint16_t>(array[i]) & 0xff) << ' ';
    }
    std::cout << '\n';
    std::invoke(setPreviousBuffer[awaitedRegisterIndex], array);
}

void AddInfoWidget::slotOnWritingError()
{
    std::invoke(setPreviousBuffer[awaitedRegisterIndex], buffer_);
    QMessageBox::warning(this, "Error", "Writing error");
}

void AddInfoWidget::slotOnTabChosen(int currentTab)
{
    if (currentTab == 1 && !dataRead)
    {
        dataRead = true;
        ui->updateButton->click();
    }
}

void AddInfoWidget::slotChangeInstantlyChangeParameters()
{
    instantlyChangeParameters = instantlyChangeParameters ? false : true;
    std::cout << instantlyChangeParameters << '\n';
}

void AddInfoWidget::slotChangeThermoresistorsState(
    const std::vector<bool>& state)
{
    uint16_t data_ = 0;
    for (int i = 0; i < (state.size() > 16 ? 16 : state.size()); ++i)
    {
        if (state[i]) data_ |= 1 << i;
    }
    QByteArray array(2, '\0');
    array[0] = data_ >> 8;
    array[1] = data_;
    parser_.setRegisterValue(0x2e, array);
    guiSetters[31](array);
}

void AddInfoWidget::on_changeThermStateButton_clicked()
{
    if (!amountOfThermoresistorsGot)
    {
        QMessageBox::warning(this, "Error",
            "Amount of thermoresistors is not got");
        return;
    }
    ThermStateWindow* thermStateWindow =
        new ThermStateWindow(thermoresistorsState, this);
    connect(thermStateWindow,
        SIGNAL(sgnSendInfo(const std::vector<bool>&)),
        SLOT(slotChangeThermoresistorsState(const std::vector<bool>&)));
    thermStateWindow->show();
}
