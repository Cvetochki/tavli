#include "settings.h"

#include <QLineEdit>
#include <QHostAddress>
#include <QHostInfo>

#include <iostream>
SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void SettingsDialog::markState(void)
{
    m_matchLength=matchLength->value();
    m_portes  = checkBox_portes->isChecked() ? 1:0;
    m_plakoto = checkBox_plakoto->isChecked() ? 1:0;
    m_fevga   = checkBox_fevga->isChecked() ? 1:0;
}

bool SettingsDialog::isChanged(void)
{
    int cmatchLength,cportes,cplakoto,cfevga;

    cmatchLength=matchLength->value();
    cportes  = checkBox_portes->isChecked() ? 1:0;
    cplakoto = checkBox_plakoto->isChecked() ? 1:0;
    cfevga   = checkBox_fevga->isChecked() ? 1:0;

    if (m_matchLength!=cmatchLength || cportes!=m_portes
        || cplakoto!=m_plakoto || cfevga!=m_fevga)
        return true;
    else
        return false;
}

void SettingsDialog::network_textchanged(QString str)
{
    if (isValidIP(remoteIP->text()))
        okButton->setEnabled(true);
    else
        okButton->setEnabled(false);
}

int SettingsDialog::isValidIP(QString str)
{
    QHostAddress addr;
    if (addr.setAddress(str))
        return 1;

    if (isNumericOnly(str)) {
        std::cout << "ok, numeric and not passing through...plain invalid." << std::endl;
        return 0;
    }

    std::cout << "Passing through to dns lookup." << std::endl;
    QHostInfo info = QHostInfo::fromName(str);
    if (!info.addresses().isEmpty()) {
         QHostAddress address = info.addresses().first();
         // use the first IP address
         Ui_settingsDlg::remoteIP->setToolTip(address.toString());
         return 1;
    }
    return 0;
}

int SettingsDialog::isNumericOnly(QString str)
{
    static QString numerical = "0123456789.";

    for(int i=0; i<str.length(); ++i)
        if (numerical.indexOf(str.at(i))==-1)
            return 0;
    return 1;
}
