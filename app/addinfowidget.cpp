#include "addinfowidget.h"
#include "ui_addinfowidget.h"

uint16_t AddInfoWidget::getUInt16InArray(const QByteArray& array, int index)
{
    uint16_t res = (static_cast<uint16_t>(array[index]) << 8) +
        *reinterpret_cast<const uint8_t*>(array.data() + index + 1);
    return res;
}

QString AddInfoWidget::JBDStringToQString(const QByteArray &array)
{
    uint8_t sz = array[0] < 11 ? array[0] : 11;
    char str[sz + 1];
    memcpy(str, array.data() + 1, sz);
    str[sz] = '\0';
    return QString(str);
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

QString AddInfoWidget::QByteArrayToDate(const QByteArray &array)
{
    uint16_t year = (array[0] >> 1) + 2000;
    uint8_t month = array[1] >> 5;
    month |= ((array[0] & 0b1) << 3);
    uint8_t day = array[1] & 0b00011111;
    QString res = QString::number(day) + '.' + QString::number(month) +
        '.' + QString::number(year);
    return res;
}

void AddInfoWidget::uncheckRadioButton(QRadioButton* radioButton)
{
    radioButton->setChecked(true);
    radioButton->setAutoExclusive(false);
    radioButton->setChecked(false);
    radioButton->setAutoExclusive(true);
}

AddInfoWidget::AddInfoWidget(QWidget* parent) : QWidget(parent),
    parser_(qobject_cast<MainWindow*>(parent)->getReader()),
    expectedRegisterSizes({29, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 1, 1, 1, 22}),
    dataSetters({
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x03->setText(getSoftwareVersion(data_[0][18]));
        }, // 0x03
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x05->setText(JBDStringToQString(data_[1]));
        }, // 0x05
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x10->setText(QString::number(getUInt16InArray(
                data_[2], 0) / 100., 'f', 2));
        }, // 0x10
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x11->setText(QString::number(getUInt16InArray(
                data_[3], 0) / 100., 'f', 2));
        }, // 0x11
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x12->setText(QString::number(getUInt16InArray(
                data_[4], 0) / 100., 'f', 2));
        }, // 0x12
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x13->setText(QString::number(getUInt16InArray(
                data_[5], 0) / 100., 'f', 2));
        }, // 0x13
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x14->setText(QString::number(getUInt16InArray(
                data_[6], 0) / 100., 'f', 2));
        }, // 0x14
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x15->setText(QByteArrayToDate(data_[7]));
        }, // 0x15
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x16->setText(QString::number(getUInt16InArray(
                data_[8], 0)));
        }, // 0x16
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x17->setText(QString::number(getUInt16InArray(
                data_[9], 0)));
        }, // 0x17
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x18->setText(getTemperature(data_[10]));
        }, // 0x18
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x19->setText(getTemperature(data_[11]));
        }, // 0x19
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x1a->setText(getTemperature(data_[12]));
        }, // 0x1a
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x1b->setText(getTemperature(data_[13]));
        }, // 0x1b
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x1c->setText(getTemperature(data_[14]));
        }, // 0x1c
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x1d->setText(getTemperature(data_[15]));
        }, // 0x1d
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x1e->setText(getTemperature(data_[16]));
        }, // 0x1e
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x1f->setText(getTemperature(data_[17]));
        }, // 0x1f
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x20->setText(QString::number(getUInt16InArray(
                data_[18], 0) / 100., 'f', 2));
        }, // 0x20
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x21->setText(QString::number(getUInt16InArray(
                data_[19], 0) / 100., 'f', 2));
        }, // 0x21
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x22->setText(QString::number(getUInt16InArray(
                data_[20], 0) / 100., 'f', 2));
        }, // 0x22
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x23->setText(QString::number(getUInt16InArray(
                data_[21], 0) / 100., 'f', 2));
        }, // 0x23
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x24->setText(QString::number(getUInt16InArray(
                data_[22], 0) / 100., 'f', 2));
        }, // 0x24
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x25->setText(QString::number(getUInt16InArray(
                data_[23], 0) / 100., 'f', 2));
        }, // 0x25
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x26->setText(QString::number(getUInt16InArray(
                data_[24], 0) / 100., 'f', 2));
        }, // 0x26
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x27->setText(QString::number(getUInt16InArray(
                data_[25], 0) / 100., 'f', 2));
        }, // 0x27
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x28->setText(QString::number(getUInt16InArray(
                data_[26], 0) / 100., 'f', 2));
        }, // 0x28
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x29->setText(QString::number(getUInt16InArray(
                data_[27], 0) / 100., 'f', 2));
        }, // 0x29
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x2a->setText(QString::number(getUInt16InArray(
                data_[28], 0) / 1000., 'f', 3));
        }, // 0x2a
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x2b->setText(QString::number(getUInt16InArray(
                data_[29], 0) / 1000., 'f', 2));
        }, // 0x2b
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x2c->setText(QString::number(getUInt16InArray(
                data_[30], 0)));
        }, // 0x2c
        [this](const std::vector<QByteArray>& data_)
        {
        // thermoresistors state - TO DO
        }, // 0x2e
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x32->setText(QString::number(getUInt16InArray(
                data_[32], 0) / 100., 'f', 2));
        }, // 0x32
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x33->setText(QString::number(getUInt16InArray(
                data_[33], 0) / 100., 'f', 2));
        }, // 0x33
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x34->setText(QString::number(getUInt16InArray(
                data_[34], 0) / 100., 'f', 2));
        }, // 0x34
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x35->setText(QString::number(getUInt16InArray(
                data_[35], 0) / 100., 'f', 2));
        }, // 0x35
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x36->setText(QString::number(getUInt16InArray(
                data_[36], 0) / 100., 'f', 2));
        }, // 0x36
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x37->setText(QString::number(getUInt16InArray(
                data_[37], 0) / 100., 'f', 2));
        }, // 0x37
        [this](const std::vector<QByteArray>& data_)
        {
            ui->radioButton_0x38_07->setChecked((data_[38][0] & 0b10000000) >>
                7);
            uint8_t shortCurcuitValue = (data_[38][0] >> 3) & 0b00000111;
            uint8_t shortCurcuitTime = data_[38][0] & 0b00000011;
            uint8_t secOvercurrentValue = (data_[38][1] >> 3) & 0b00000111;
            uint8_t secOvercurrentTime = (data_[38][1]) & 0b00001111;
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
        [this](const std::vector<QByteArray>& data_)
        {
            uint8_t undervoltageDelay = data_[39][0] >> 6;
            uint8_t overvoltageDelay = (data_[39][0] >> 4) & 0b00000011;
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
            ui->lineEdit_0x39_1->setText(QString::number(data_[39][1]));
        }, // 0x39
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x3a_0->setText(QString::number(data_[40][0]));
            ui->lineEdit_0x3a_1->setText(QString::number(data_[40][1]));
        }, // 0x3a
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x3b_0->setText(QString::number(data_[41][0]));
            ui->lineEdit_0x3b_1->setText(QString::number(data_[41][1]));
        }, // 0x3b
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x3c_0->setText(QString::number(data_[42][0]));
            ui->lineEdit_0x3c_1->setText(QString::number(data_[42][1]));
        }, // 0x3c
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x3d_0->setText(QString::number(data_[43][0]));
            ui->lineEdit_0x3d_1->setText(QString::number(data_[43][1]));
        }, // 0x3d
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x3e_0->setText(QString::number(data_[44][0]));
            ui->lineEdit_0x3e_1->setText(QString::number(data_[44][1]));
        }, // 0x3e
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0x3f_0->setText(QString::number(data_[45][0]));
            ui->lineEdit_0x3f_1->setText(QString::number(data_[45][1]));
        }, // 0x3f
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0xa0->setText(JBDStringToQString(data_[46]));
        }, // 0xa0
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0xa1->setText(JBDStringToQString(data_[47]));
        }, // 0xa1
        [this](const std::vector<QByteArray>& data_)
        {
            ui->lineEdit_0xa2->setText(JBDStringToQString(data_[48]));
        }, // 0xa2
        [this](const std::vector<QByteArray>& data_)
        {
            ui->label_0xaa_0->setText(QString::number(getUInt16InArray(
                data_[49], 0)));
            ui->label_0xaa_1->setText(QString::number(getUInt16InArray(
                data_[49], 2)));
            ui->label_0xaa_2->setText(QString::number(getUInt16InArray(
                data_[49], 4)));
            ui->label_0xaa_3->setText(QString::number(getUInt16InArray(
                data_[49], 6)));
            ui->label_0xaa_4->setText(QString::number(getUInt16InArray(
                data_[49], 8)));
            ui->label_0xaa_5->setText(QString::number(getUInt16InArray(
                data_[49], 10)));
            ui->label_0xaa_6->setText(QString::number(getUInt16InArray(
                data_[49], 12)));
            ui->label_0xaa_7->setText(QString::number(getUInt16InArray(
                data_[49], 14)));
            ui->label_0xaa_8->setText(QString::number(getUInt16InArray(
                data_[49], 16)));
            ui->label_0xaa_9->setText(QString::number(getUInt16InArray(
                data_[49], 18)));
            ui->label_0xaa_10->setText(QString::number(getUInt16InArray(
                data_[49], 20)));
        } // 0xaa
    }),
    dataCleaners({
        [this]()
        {
            ui->lineEdit_0x03->setText("");
        }, // 0x03
        [this]()
        {
            ui->lineEdit_0x05->setText("");
        }, // 0x05
        [this]()
        {
            ui->lineEdit_0x10->setText("");
        }, // 0x10
        [this]()
        {
            ui->lineEdit_0x11->setText("");
        }, // 0x11
        [this]()
        {
            ui->lineEdit_0x12->setText("");
        }, // 0x12
        [this]()
        {
            ui->lineEdit_0x13->setText("");
        }, // 0x13
        [this]()
        {
            ui->lineEdit_0x14->setText("");
        }, // 0x14
        [this]()
        {
            ui->lineEdit_0x15->setText("");
        }, // 0x15
        [this]()
        {
            ui->lineEdit_0x16->setText("");
        }, // 0x16
        [this]()
        {
            ui->lineEdit_0x17->setText("");
        }, // 0x17
        [this]()
        {
            ui->lineEdit_0x18->setText("");
        }, // 0x18
        [this]()
        {
            ui->lineEdit_0x19->setText("");
        }, // 0x19
        [this]()
        {
            ui->lineEdit_0x1a->setText("");
        }, // 0x1a
        [this]()
        {
            ui->lineEdit_0x1b->setText("");
        }, // 0x1b
        [this]()
        {
            ui->lineEdit_0x1c->setText("");
        }, // 0x1c
        [this]()
        {
            ui->lineEdit_0x1d->setText("");
        }, // 0x1d
        [this]()
        {
            ui->lineEdit_0x1e->setText("");
        }, // 0x1e
        [this]()
        {
            ui->lineEdit_0x1f->setText("");
        }, // 0x1f
        [this]()
        {
            ui->lineEdit_0x20->setText("");
        }, // 0x20
        [this]()
        {
            ui->lineEdit_0x21->setText("");
        }, // 0x21
        [this]()
        {
            ui->lineEdit_0x22->setText("");
        }, // 0x22
        [this]()
        {
            ui->lineEdit_0x23->setText("");
        }, // 0x23
        [this]()
        {
            ui->lineEdit_0x24->setText("");
        }, // 0x24
        [this]()
        {
            ui->lineEdit_0x25->setText("");
        }, // 0x25
        [this]()
        {
            ui->lineEdit_0x26->setText("");
        }, // 0x26
        [this]()
        {
            ui->lineEdit_0x27->setText("");
        }, // 0x27
        [this]()
        {
            ui->lineEdit_0x28->setText("");
        }, // 0x28
        [this]()
        {
            ui->lineEdit_0x29->setText("");
        }, // 0x29
        [this]()
        {
            ui->lineEdit_0x2a->setText("");
        }, // 0x2a
        [this]()
        {
            ui->lineEdit_0x2b->setText("");
        }, // 0x2b
        [this]()
        {
            ui->lineEdit_0x2c->setText("");
        }, // 0x2c
        [this]()
        {
        // thermoresistors state - TO DO
        }, // 0x2e
        [this]()
        {
            ui->lineEdit_0x32->setText("");
        }, // 0x32
        [this]()
        {
            ui->lineEdit_0x33->setText("");
        }, // 0x33
        [this]()
        {
            ui->lineEdit_0x34->setText("");
        }, // 0x34
        [this]()
        {
            ui->lineEdit_0x35->setText("");
        }, // 0x35
        [this]()
        {
            ui->lineEdit_0x36->setText("");
        }, // 0x36
        [this]()
        {
            ui->lineEdit_0x37->setText("");
        }, // 0x37
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
        [this]()
        {
            ui->lineEdit_0xa0->setText("");
        }, // 0xa0
        [this]()
        {
            ui->lineEdit_0xa1->setText("");
        }, // 0xa1
        [this]()
        {
            ui->lineEdit_0xa2->setText("");
        }, // 0xa2
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
        }
    }),
    ui(new Ui::AddInfoWidget)
{
    ui->setupUi(this);
    QObject::connect(ui->updateButton, SIGNAL(clicked()), &parser_,
        SIGNAL(sgnSetManualMode()));
    QObject::connect(ui->updateButton, &QPushButton::clicked,
        ui->updateButton, [this](){ ui->updateButton->setEnabled(false); });
    QObject::connect(ui->updateButton, SIGNAL(clicked()), &parser_,
        SIGNAL(sgnReadingBegun()));
    QObject::connect(&parser_,
        SIGNAL(sgnSendDataToGUI(const std::vector<QByteArray>&)),
        SLOT(slotShowDataOnGUI(const std::vector<QByteArray>&)));
}

AddInfoParser* AddInfoWidget::getParser() { return &parser_; }

AddInfoWidget::~AddInfoWidget() { delete ui; }

void AddInfoWidget::slotShowDataOnGUI(const std::vector<QByteArray>& data_)
{
    std::vector<uint8_t> unreadRegisters;
    for (int i = 0; i < dataSetters.size(); ++i)
    {
        if (data_[i].size() < expectedRegisterSizes[i])
        {
            unreadRegisters.push_back(i);
            dataCleaners[i]();
        } else {
            std::invoke(dataSetters[i], data_);
        }
    }
    if (!unreadRegisters.empty())
    {
    // TO DO: чтобы выводило названия данных, а не номера регистров
        QString msg;
        if (unreadRegisters.size() == expectedRegisterSizes.size())
        {
            msg = "No reply from BMS got";
        } else {
            msg = "The following registers were failed to read: ";
            for (int i = 0; i < unreadRegisters.size() - 1; ++i)
            {
                msg += "0x" + QString::number(unreadRegisters[i], 16) + ", ";
            }
            msg += "0x" + QString::number(unreadRegisters.back(), 16);
        }
        QMessageBox::information(this, "Reading result", msg);
    }
    ui->updateButton->setEnabled(true);
}
