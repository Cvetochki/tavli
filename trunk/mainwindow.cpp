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
#include <QtGui>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>



#include "mainwindow.h"
#include "settings.h"

#include <iostream>

MainWindow::MainWindow()
	:m_blockSize(0),
	m_activeConnection(0)
{
    m_board = new board(this);
	m_server = new QTcpServer(this);
	int port=1971;
	while(!m_server->listen(QHostAddress::Any,port)) {
		std::cerr << "Can't listen on port #" <<port<<std::endl;
		++port;
	}
	
	
	connect(m_server,SIGNAL(newConnection()),this,SLOT(gotConnection()));
	createBoard();
	
    

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
	statusBar()->showMessage(tr("Listening on port #%1").arg(port), 10000);
	
    readSettings();

    //connect(msgDisplay->document(), SIGNAL(contentsChanged()),
    //        this, SLOT(documentWasModified()));
	QWidget *center=new QWidget(this);
	QWidget *right=new QWidget(this);
//#if QT_VERSION >= 0x040300
	center->setContentsMargins(0,0,0,0);
//#endif
	QLabel *label=new QLabel(right);
	msgInput = new QLineEdit(right);
	msgDisplay = new QTextEdit(right);
	msgDisplay->setReadOnly(true);
	msgDisplay->setFocusPolicy(Qt::NoFocus);
	msgDisplay->setTextInteractionFlags (Qt::LinksAccessibleByMouse);
	msgDisplay->setMinimumWidth(300);
	QHBoxLayout *mainLayout = new QHBoxLayout(center);
	QVBoxLayout *secLayout = new QVBoxLayout(right);
#if QT_VERSION >= 0x040300
	mainLayout->setContentsMargins(0,0,0,0);
#endif
	mainLayout->addWidget(m_board);
	secLayout->addWidget(label);
	secLayout->addWidget(msgDisplay);
	secLayout->addWidget(msgInput);
	mainLayout->addWidget(right);
	
	label->setText("<a href=\"http://redlumf.blogspot.com\">Check the blog</a>");
	label->setOpenExternalLinks(true);
	
	setWindowIcon(QIcon(":/images/tavli.png"));
    
    setCentralWidget(center);
	QString boardID=getPositionID(board::Plakoto,m_anBoard);
	msgDisplay->append(boardID);
	msgDisplay->append("$Revision$");
	msgDisplay->append(tr("Welcome to Tavli\nOh, and good luck...you'll actually need it ;-)\n"));
	//msgDisplay->append("http://redlumf.blogspot.com");
	//msgDisplay->append("<a href=\"http://en.wikipedia.org/wiki/Cognate\"><span style=\" text-decoration: underline; color:#0000ff;\">cognate</span></a>");
// 	QString t;
// 	for(int i=0; i<100; ++i) {
// 		t=QString::number( i );
// 		msgDisplay->setTextColor(QColor(rand()%255,rand()%255,rand()%255));
// 		msgDisplay->append(t);
// 	}
	msgDisplay->moveCursor(QTextCursor::End);
	msgDisplay->ensureCursorVisible ();
	msgInput->hide();
	setWindowTitle(tr("tavli"));
	setBoardFromPositionID("dummy");
}

void MainWindow::gotConnection(void)
{
	m_client=m_server->nextPendingConnection();
	connect(m_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError()));
	connect(m_client,SIGNAL(readyRead()), this, SLOT(readNet()));
	connect(m_client,SIGNAL(disconnected()),this,SLOT(lostConnection()));
	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(this, tr("Tavli"),
					tr("A remote host (at %1) is trying to connect...\n"
					"Do you want to allow connection?").arg(m_client->peerAddress().toString()),
					QMessageBox::Yes | QMessageBox::No);
	if (ret != QMessageBox::Yes) {
	    disconnect(m_client, 0, 0, 0);
		m_client->disconnectFromHost();
		m_client->deleteLater();
	} else {
		//NetSend("Bonjour!\n");
		msgInput->show();
		connect(msgInput,SIGNAL(returnPressed()),this,SLOT(sendNetMsg()));
		m_activeConnection=1;
	}
}

void MainWindow::socketError()
{
	msgDisplay->append(m_client->errorString());
}
void MainWindow::sendNetMsg(void)
{
	static QString str;
	if (msgInput->text()=="")
		return;
	str=msgInput->text();
	//str+="\n";
	NetSend(str);//+"\n");
	msgInput->setText("");
	str="You say: "+str;
	msgDisplay->append(str);
}

void MainWindow::NetSend(QString str)
{
	 QByteArray block;
     QDataStream out(&block, QIODevice::WriteOnly);
     out.setVersion(QDataStream::Qt_4_0);
     out << (quint16)0;
     out << str;
     out.device()->seek(0);
     out << (quint16)(block.size() - sizeof(quint16));
	 m_client->write(block);
	 m_client->flush();
	 //std::cout << "Sent \"" << str.data() <<"\"" << std::endl;
}

void MainWindow::readNet(void)
{
	QDataStream in(m_client);
	in.setVersion(QDataStream::Qt_4_0);
	if (m_blockSize == 0) {
         if (m_client->bytesAvailable() < (int)sizeof(quint16))
             return;
         in >> m_blockSize;
     }
     if (m_client->bytesAvailable() < m_blockSize)
         return;
     QString str;
     in >> str;
     msgDisplay->append(str);
	 m_blockSize=0;
}
void MainWindow::lostConnection(void)
{
	m_client->deleteLater();
	QMessageBox::about(this, tr("Lost connection"),
            tr("Yeap, I <b>lost</b> it."));
	m_activeConnection=0;
}


void MainWindow::createBoard(void)
{
	for(int i=0; i<25; ++i)
		m_anBoard[0][i]=m_anBoard[1][i]=0;
	//m_anBoard[0][23]=m_anBoard[1][23]=15;
	m_anBoard[0][5]=m_anBoard[1][5]=5;
	m_anBoard[0][7]=m_anBoard[1][7]=3;
	m_anBoard[0][12]=m_anBoard[1][12]=5;
	m_anBoard[0][23]=m_anBoard[1][23]=2;
	m_board->setBoard(m_anBoard);
	m_board->setGame(board::Plakoto);
	
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    SettingsDialog foo(this);
	if (foo.exec()==QDialog::Accepted) {
		int n=foo.matchLength->value();
		QString name=foo.player1Name->text();
		std::cout << "Accepted" <<std::endl;
		msgInput->show();
		QString rIP=foo.remoteIP->text();
		msgDisplay->append(rIP);
		if (rIP.isEmpty())
			return;
		m_client= new QTcpSocket(this);
		connect(m_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError()));
		m_client->connectToHost(rIP,1971);
		if (m_client->waitForConnected()) {
			connect(m_client,SIGNAL(readyRead()), this, SLOT(readNet()));
			connect(m_client,SIGNAL(disconnected()),this,SLOT(lostConnection()));
			connect(msgInput,SIGNAL(returnPressed()),this,SLOT(sendNetMsg()));
			m_activeConnection=1;
		}
	} else
		std::cout << "Not Accepted" <<std::endl;
}

void MainWindow::open()
{
	QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty())
		loadFile(fileName);
}

bool MainWindow::save()
{
	/*
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
	*/
	return true;
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Tavli"),
            tr("An application to play the three greek variations "
               "of backgammon.\n\n"
               "Programmed by Alkis <a href=\"http://redlumf.blogspot.com\">here</a>"));
               //"Programmed by Alkis http://redlumf.blogspot.com"));
}

void MainWindow::documentWasModified()
{
    //setWindowModified(msgDisplay->document()->isModified());
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New Match..."), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Start a new match"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	
    undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    undoAct->setStatusTip(tr("Takes back the last move (when not completed) "));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(QIcon(":/images/redo.png"), tr("Re&do"), this);
    redoAct->setShortcut(tr("Ctrl+Shift+Z"));
    redoAct->setStatusTip(tr("Repeats the partial move that you took back"));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    rollAct = new QAction(QIcon(":/images/roll.png"), tr("&Roll dice"), this);
    rollAct->setShortcut(tr("Ctrl+R"));
    rollAct->setStatusTip(tr("Rolls the dice"));
    connect(rollAct, SIGNAL(triggered()), this, SLOT(roll()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	//TODO: Make signals on board for undo/redo
    undoAct->setEnabled(false);
    redoAct->setEnabled(false);
	rollAct->setEnabled(true);
	/*
    connect(msgDisplay, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(msgDisplay, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
	*/
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Game"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

	
    moveMenu = menuBar()->addMenu(tr("&Move"));
    moveMenu->addAction(undoAct);
    moveMenu->addAction(redoAct);
    moveMenu->addSeparator();
    moveMenu->addAction(rollAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    gameToolBar = addToolBar(tr("Game"));
    gameToolBar->addAction(newAct);
    gameToolBar->addAction(openAct);
    gameToolBar->addAction(saveAct);

	
    moveToolBar = addToolBar(tr("Move"));
    moveToolBar->addAction(undoAct);
    moveToolBar->addAction(redoAct);
    moveToolBar->addAction(rollAct);
    
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("Alkis", "Tavli");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("Alkis", "Tavli");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
	
    if (m_activeConnection) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Tavli"),
                     tr("You are currently connected.\n"
                        "Do you really want to quit?"),
                     QMessageBox::Yes | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes)
            return true;
        else 
            return false;
    }
    return true;
}

void MainWindow::loadFile(const QString &)//fileName)
{
	/*
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    msgDisplay->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    */
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &)//fileName)
{
	/*
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << msgDisplay->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    */
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &)//fileName)
{
	/*
    curFile = fileName;
    msgDisplay->document()->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(curFile);
	*/
    //setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Application")));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::roll()
{
	int d1=rand()%6+1;
	int d2=rand()%6+1;
	m_board->setRoll(d1,d2);
}


QString MainWindow::getPositionID(board::GameType game,int board[2][25])
{
	QString str="";

	if (game==board::Plakoto) {
		for(int j=0; j<2; ++j)
			for(int i=0; i<25; ++i) {
				int t=board[j][i];
				if (t>64) t-=63; // this is -64+1
				while(t--)
					str+="1";
				str+="0";
			}
	}
	msgDisplay->append(str);
	int len=str.length();
	while(len%8) {
		str+="0";
		++len;
	}

	if (len%8)
		len=len/8+1;
	else
		len=len/8;
	QByteArray ar(len,0);
	
	int start=0;
	int end=str.length();
	unsigned char c;
	int i;
	int pos=0;
	QString rev="";
	QString hex="";
	do {
		i=c=0;
		while(i<8) {
			if (str[start+7-i]=='1')
				c|=1<<(7-i);
			rev+=str[start+7-i];
			++i;
		}
		rev+=" ";
		QString tmp;
		tmp.sprintf("%0.2X",c);
		hex+=tmp+" ";
		ar[pos++]=c;
		start+=8;
	} while(start<end);
	msgDisplay->append(rev);
	msgDisplay->append(hex);
	rev=ar.toBase64();
	rev=rev.left(rev.length()-2);
	return rev;
}
//4HPwATDgc/ABMA
void MainWindow::setBoardFromPositionID(QString positionID)
{
	//QByteArray ar = QByteArray::fromBase64("4HPwATDgc/ABMA");
	
	QByteArray ar = QByteArray::fromBase64("XwAAgPceAAAAAA");

	int size=ar.size();
	QString str="";
	for (int byte=0; byte<size; ++byte) {
		unsigned char c=ar[byte];
		for(int bit=0; bit<8; ++bit)
			if (c & 1<<bit)
				str+="1";
			else
				str+="0";
		//str+=" ";
	}
	msgDisplay->append("Decoding:\n"+str);

	for(int j=0; j<2; ++j)
		for(int i=0; i<25; ++i)
			m_anBoard[j][i]=0;

//	if (game==board::Plakoto) {
		int side=0;
		int index=0;
		int pos=0;
		int count=0;
		while(1) {
			while(str[pos]=='0') {
				pos++;
				m_anBoard[side][index++]=0;
				if (index==25) {
					index=0;
					if (side==1) goto done;
					side=1;
				}
			}
			
			count=0;
			while(str[pos++]=='1')
				++count;
			//pos++;
			m_anBoard[side][index++]=count;
			if (index==25) {
				index=0;
				if (side==1) goto done;
				side=1;
			}
		}
done:		
//	}
	m_board->setBoard(m_anBoard);
}