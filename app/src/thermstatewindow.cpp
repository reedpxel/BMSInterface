#include "thermstatewindow.h"

ThermStateWindow::ThermStateWindow(const std::vector<bool>& thermState,
    QWidget* parent) : QDialog(parent),
    saveChangesButton(new QPushButton("Save changes")),
    cancelButton(new QPushButton("Cancel"))
{
    for (int i = 0; i < thermState.size(); ++i)
    {
        checkBoxes.push_back(
            new QCheckBox("Thermoresistor " + QString::number(i + 1)));
        checkBoxes[i]->setChecked(thermState[i]);
    }
    QVBoxLayout* vLayout = new QVBoxLayout;
    for (auto pCheckBox : checkBoxes) vLayout->addWidget(pCheckBox);
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(saveChangesButton);
    hLayout->addWidget(cancelButton);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);
    // seems that it is impossible to disable maximize and minimize buttons
    // setWindowFlags does not change anything
    connect(saveChangesButton, SIGNAL(clicked()),
        SLOT(slotOnSaveChangesButtonClicked()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(close()));
}

void ThermStateWindow::slotOnSaveChangesButtonClicked()
{
    std::vector<bool> state;
    for (auto pCheckBox : checkBoxes)
    {
        state.push_back(pCheckBox->isChecked());
    }
    emit sgnSendInfo(state);
    close();
}
