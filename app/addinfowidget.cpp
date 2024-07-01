#include "addinfowidget.h"
#include "ui_addinfowidget.h"

AddInfoWidget::AddInfoWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddInfoWidget)
{
    ui->setupUi(this);
}

AddInfoWidget::~AddInfoWidget()
{
    delete ui;
}
