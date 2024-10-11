#ifndef THERMSTATEWINDOW_H
#define THERMSTATEWINDOW_H
#include <QtWidgets>
#include <vector>

class ThermStateWindow : public QDialog
{
Q_OBJECT
    std::vector<QCheckBox*> checkBoxes;
    QPushButton* saveChangesButton;
    QPushButton* cancelButton;
signals:
    void sgnSendInfo(const std::vector<bool>&);
public:
    ThermStateWindow(const std::vector<bool>& thermState,
        QWidget* parent = nullptr);
public slots:
    void slotOnSaveChangesButtonClicked();
};

#endif // THERMSTATEWINDOW_H
