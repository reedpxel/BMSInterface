#ifndef CHANGEPASSWORDWINDOW_H
#define CHANGEPASSWORDWINDOW_H
#include <QtWidgets>

class ChangePasswordWindow : public QDialog
{
Q_OBJECT
    QLineEdit* oldPasswordLineEdit;
    QLineEdit* newPasswordLineEdit;
    QPushButton* changePushButton;
    QPushButton* cancelPushButton;
signals:
    void sgnPasswordsGot(const QByteArray&);
public:
    ChangePasswordWindow(QWidget* parent = nullptr);
public slots:
    void slotOnChangeButtonClicked();
    void slotOnCancelButtonClicked();
};

#endif // CHANGEPASSWORDWINDOW_H
