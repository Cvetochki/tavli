#include "settings.h"

#include <QLineEdit>

#ifdef __GNUC__
#include <pwd.h> 
#else
#include <windows.h>
#endif

SettingsDialog::SettingsDialog(QWidget *parent)
     : QDialog(parent)
 {
	 QString user;

     setupUi(this);
/*
     colorDepthCombo->addItem(tr("2 colors (1 bit per pixel)"));
     colorDepthCombo->addItem(tr("4 colors (2 bits per pixel)"));
     colorDepthCombo->addItem(tr("16 colors (4 bits per pixel)"));
     colorDepthCombo->addItem(tr("256 colors (8 bits per pixel)"));
     colorDepthCombo->addItem(tr("65536 colors (16 bits per pixel)"));
     colorDepthCombo->addItem(tr("16 million colors (24 bits per pixel)"));
*/
#ifdef __GNUC__
	 struct passwd *userinfo;
	 
	 userinfo=getpwuid(getuid());
	 user = userinfo -> pw_name;
#else

	 DWORD dwBuffer = 256;       
    TCHAR strUserName[255];
	GetUserName(strUserName, &dwBuffer); 
QT_WA ( 
{
    user = QString::fromWCharArray (reinterpret_cast<ushort *>(strUserName));
} , 
{
	user = QString::fromLocal8Bit (reinterpret_cast<const char *>(&(strUserName[0])));
} ); // QT_WA

#endif
	QString firstLetter=user.left(1);
	QString rest=user.right(user.length()-1);
	user=firstLetter.toUpper()+rest;

	player1Name->setText(user);
	//remoteIP->setText("127.0.0.1");
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