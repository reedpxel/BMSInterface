#include "changepasswordwindow.h"

ChangePasswordWindow::ChangePasswordWindow(QWidget* parent) :
    QDialog(parent)
{
    QLabel* oldPasswordLabel = new QLabel("Old password:");
    oldPasswordLineEdit = new QLineEdit;
    QLabel* newPasswordLabel = new QLabel("New password:");
    newPasswordLineEdit = new QLineEdit;
    changePushButton = new QPushButton("Change");
    cancelPushButton = new QPushButton("Cancel");
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(changePushButton);
    hLayout->addWidget(cancelPushButton);
    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(oldPasswordLabel);
    vLayout->addWidget(oldPasswordLineEdit);
    vLayout->addWidget(newPasswordLabel);
    vLayout->addWidget(newPasswordLineEdit);
    vLayout->addLayout(hLayout);
    connect(changePushButton, SIGNAL(clicked()),
        SLOT(slotOnChangeButtonClicked()));
    connect(cancelPushButton, SIGNAL(clicked()),
        SLOT(slotOnCancelButtonClicked()));
    setLayout(vLayout);
}

void ChangePasswordWindow::slotOnChangeButtonClicked()
{
    std::string oldPassword = oldPasswordLineEdit->text().toStdString();
    std::string newPassword = newPasswordLineEdit->text().toStdString();
    if (oldPassword.size() > 6 || newPassword.size() > 6)
    {
        QMessageBox::warning(this, "Error", "Password must be 6 "
            "characters long");
        return;
    }
    QByteArray res(13, '\0');
    res[0] = 12;
    for (int i = 1; i < oldPassword.size() + 1; ++i)
    {
        res[i] = oldPassword[i - 1];
    }
    for (int i = 6; i < oldPassword.size() + 6; ++i)
    {
        res[i] = newPassword[i - 6];
    }
    emit sgnPasswordsGot(res);
    close();
}

void ChangePasswordWindow::slotOnCancelButtonClicked() { close(); }
