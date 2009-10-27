#include "ui_settings.h"

class SettingsDialog : public QDialog, public Ui::settingsDlg
{
     Q_OBJECT

	int m_matchLength;
	int m_portes;
	int m_plakoto;
	int m_fevga;
 public:
     SettingsDialog(QWidget *parent = 0);
	 void markState(void);
	 bool isChanged(void);
      int isValidIP(QString str);
private slots:
         void network_textchanged(QString str);
};

