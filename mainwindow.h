/****************************************************************************
**
** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "board.h"

class QVBoxLayout;
class QAction;
class QMenu;
class QTextEdit;
class QLineEdit;
class Network;
class QLabel;

//class board;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();

	void LogMsg(QString str);
    void socketError(QString str);
	void gotConnection(QString host);
	void lostConnection();
	void sendTextMsg();
	void rcvMsg(QString str);
	void roll();
	
private:
    void createBoard(void);
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
	QString getPositionID(board::GameType game,int board[2][25]);
	void setBoardFromPositionID(QString positionID);

    board *m_board;
	int    m_anBoard[2][25];

	Network *m_network;
    QString curFile;

	QVBoxLayout *mainLayout;
    QMenu *fileMenu;
    QMenu *moveMenu;
    QMenu *helpMenu;
    QToolBar *gameToolBar;
    QToolBar *moveToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *rollAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QLineEdit *msgInput;
	QTextEdit *msgDisplay;
	QLabel	*m_statusLabel;
	
	int		m_activeConnection;
};

#endif
