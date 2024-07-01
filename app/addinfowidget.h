#ifndef ADDINFO_H
#define ADDINFO_H
#include <QWidget>

namespace Ui {
class AddInfoWidget;
}

class AddInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddInfoWidget(QWidget *parent = nullptr);
    ~AddInfoWidget();

private:
    Ui::AddInfoWidget *ui;
};

#endif // ADDINFO_H
