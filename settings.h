#include "ui_settings.h"

class SettingsDialog : public QDialog, public Ui::settingsDlg
{
     Q_OBJECT

 public:
     SettingsDialog(QWidget *parent = 0);
};
