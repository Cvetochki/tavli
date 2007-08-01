#include "settings.h"

#include <QLineEdit>

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