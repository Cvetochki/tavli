/***************************************************************************
*   Copyright (C) 2005 by Alex Argiropoulos   *
*   alkis_@hotmail.com   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include "board.h"
#include <QPainter>
#include <QCursor>
#include <QDateTime>
#include <QMouseEvent>
#include <QTimer>

#include <iostream>

board::board(QWidget *parent)
: QWidget(parent),
  m_boardMsgActive(0)
{
	m_sideToPlay=None;
	m_d[0]=m_d[1]=-1;
	m_showdrag=false;
	mousepos=QPoint(0,0);
	setMouseTracking (true);
	if (m_iboard.load("images/board.png", 0))
		std::cout << "Loaded image...\n" << std::endl;
	else
		std::cout << "Couldn't load image...\n" << std::endl;

	//setPaletteBackgroundPixmap(bg);
	m_iwhite.load("images/white_pawn.png");
	m_ired.load("images/red_pawn.png");

	//setBackgroundMode(NoBackground);
	setAutoFillBackground(false);
	//mp.resize(32,32);
	//mp.setMask(mp.createHeuristicMask());

	//QCursor c(mp);

	//setCursor(Qt::BlankCursor);

	//qpixmap=0;
	setFixedSize(700,600);
	QResizeEvent foo(QSize(width(),height()),QSize(width(),height()));
	resizeEvent(&foo);

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(boardMsgTimeOut()));
}


board::~board()
{}

void board::setBoard(int b[2][25])
{
	for(int i=0; i<25; ++i) {
		anBoard[0][i]=b[0][i];
		anBoard[1][i]=b[1][i];
	}

	update();
}

void board::setGame(GameType n)
{
	m_gameType=n;
	update();
	std::cout << "Game set to " << n <<std::endl;
}
//#include "board.moc"

/*!
\fn board::paintEvent(QPaintEvent *)
*/
void board::paintEvent(QPaintEvent *)
{
	int t;
	QString s;
	static int paintCounter;

	//std::cout << ++paintCounter << std::endl;
	++paintCounter;
	QPainter qpainter(this);
	qpainter.drawPixmap(0,0,m_pboard,0,0,width(),height());

	int offsy=46-16;
	int bottom=512+16;
	if (m_gameType==Portes) {//Portes

	} else if (m_gameType==Plakoto) { //Plakoto
		//std::cout << "ok, it gets run..." << m_pboard.width() <<std::endl;
		bool capture;
		int cy;
		for(int j=0; j<5; ++j) {
			int offsx=32;
			for(int i=0; i<12; ++i) {
				if (i==6) offsx+=30;
				if ((t=anBoard[1][23-i])) {
					if (t>64) {
						t-=64;
						capture=true;
						cy=m_pawnsize;
					} else {
						capture=false;
						cy=0;
					}
					--t;
					if (t>=j) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,offsy+j*m_pawnsize+cy,m_pred,0,0,width(),height());
					}
					if (j==0 && capture) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,offsy+m_pawnsize,m_pred,0,0,width(),height());
						qpainter.drawPixmap(offsx+i*m_pawnsize,offsy,m_pwhite,0,0,width(),height());
					}
					if (j==0 && (t>=5)) {
						//std::cout <<std::setw(2)<<t<< " ";
						qpainter.setPen( Qt::white );
						s = QString::number( t+1 );
						qpainter.drawText( offsx+i*m_pawnsize+m_pawnsize/2-8, offsy+cy+m_pawnsize/2+4, s );
					}

				}
				if ((t=anBoard[0][i])) {
					if (t>64) {
						t-=64;
						capture=true;
						cy=m_pawnsize;
					} else {
						capture=false;
						cy=0;
					}
					--t;
					if (t>=j) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,offsy+j*m_pawnsize+cy,m_pwhite,0,0,width(),height());
					}
					if (j==0 && capture) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,offsy+m_pawnsize,m_pwhite,0,0,width(),height());
						qpainter.drawPixmap(offsx+i*m_pawnsize,offsy,m_pred,0,0,width(),height());
					}
					if (j==0 && (t>=5)) {
						qpainter.setPen( Qt::black );
						s = QString::number( t+1 );
						qpainter.drawText( offsx+i*m_pawnsize+m_pawnsize/2-8, offsy+cy+m_pawnsize/2+4, s );
					} 
				}
				if ((t=anBoard[1][i])) {
					if (t>64) {
						t-=64;
						capture=true;
						cy=m_pawnsize;
					} else {
						capture=false;
						cy=0;
					}
					--t;
					if (t>=j) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,bottom-cy-j*m_pawnsize,m_pred,0,0,width(),height());
					}
					if (j==0 && capture) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,bottom-m_pawnsize,m_pred,0,0,width(),height());
						qpainter.drawPixmap(offsx+i*m_pawnsize,bottom,m_pwhite,0,0,width(),height());
					}
					if (j==0 && (t>=5) ) {
						qpainter.setPen( Qt::white );
						s = QString::number( t+1 );
						qpainter.drawText( offsx+i*m_pawnsize+m_pawnsize/2-8, bottom-cy+m_pawnsize/2+4, s );
					}
				} 
				if ((t=anBoard[0][23-i])) {
					if (t>64) {
						t-=64;
						capture=true;
						cy=m_pawnsize;
					} else {
						capture=false;
						cy=0;
					}
					--t;
					if (t>=j) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,bottom-cy-j*m_pawnsize,m_pwhite,0,0,width(),height());
					}
					if (j==0 && capture) { 					
						qpainter.drawPixmap(offsx+i*m_pawnsize,bottom-m_pawnsize,m_pwhite,0,0,width(),height());
						qpainter.drawPixmap(offsx+i*m_pawnsize,bottom,m_pred,0,0,width(),height());
					}
					if (j==0 && (t>=5) ) {

						s = QString::number( t+1 );
						int tx=offsx+i*m_pawnsize+m_pawnsize/2-8;
						int ty=bottom-cy+m_pawnsize/2+4;

						//qpainter.setPen( Qt::white );
						//qpainter.drawText( tx+1, ty+1, s );
						qpainter.setPen( Qt::black );
						qpainter.drawText( tx, ty, s );
					}
				}
			}
		}


	} else {	//Fevga
		for(int j=0; j<5; ++j) {
			int offsx=32;
			for(int i=0; i<12; ++i) {
				if (i==6) offsx+=m_pawnsize;
				if ((t=anBoard[1][12+i])) {
					if (t>=j) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,offsy+j*m_pawnsize,m_pred,0,0,width(),height());
					}
					if (j==0 && (t>5)) {
						//std::cout <<std::setw(2)<<t<< " ";
						qpainter.setPen( Qt::white );
						s = QString::number( t );
						qpainter.drawText( offsx+i*m_pawnsize+m_pawnsize/2-8, offsy+m_pawnsize/2+4, s );
					}

				} else if ((t=anBoard[0][i])) {
					if (t>=j) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,offsy+j*m_pawnsize,m_pwhite,0,0,width(),height());
					}
					if (j==0 && (t>5)) {
						qpainter.setPen( Qt::black );
						s = QString::number( t );
						qpainter.drawText( offsx+i*m_pawnsize+m_pawnsize/2-8, offsy+m_pawnsize/2+4, s );
					} 
				}
				if ((t=anBoard[1][11-i])) {
					if (t>=j) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,bottom-j*m_pawnsize,m_pred,0,0,width(),height());
					}
					if (j==0 && (t>5) ) {
						qpainter.setPen( Qt::white );
						s = QString::number( t );
						qpainter.drawText( offsx+i*m_pawnsize+m_pawnsize/2-8, bottom+m_pawnsize/2+4, s );
					}
				} else if ((t=anBoard[0][23-i])) {
					if (t>=j) {
						qpainter.drawPixmap(offsx+i*m_pawnsize,bottom-j*m_pawnsize,m_pwhite,0,0,width(),height());
					}
					if (j==0 && (t>5) ) {

						s = QString::number( t );
						int tx=offsx+i*m_pawnsize+m_pawnsize/2-8;
						int ty=bottom+m_pawnsize/2+4;

						//qpainter.setPen( Qt::white );
						//qpainter.drawText( tx+1, ty+1, s );
						qpainter.setPen( Qt::black );
						qpainter.drawText( tx, ty, s );
					}
				}
			}
		}

	}          
	if (m_showdrag) {
		anBoard[1][23]=14;
		qpainter.drawPixmap(mousepos.x()-m_pawnsize/2,mousepos.y()-m_pawnsize/2,m_pred);//,0,0,width(),height());
	}

	qpainter.setPen( Qt::white );
	qpainter.drawText(10,10,"paintEvent #"+QString::number(paintCounter));


	QString dis="X: "+QString::number(mousepos.x()) + "  -  Y: "+QString::number(mousepos.y());
	qpainter.drawText(0,600-4,dis);

	/*
	QPen mypen(QColor(180, 20, 20, 55));
	mypen.setWidth(10);
	qpainter.setPen( mypen );

	//qpainter.setPen(QColor(255, 255, 255, 255));
	qpainter.setBrush(QColor(255, 255, 0, 120));

	qpainter.setRenderHint(QPainter::Antialiasing);
	//qpainter.drawEllipse(QRectF(mousepos.x() - 100,
	//								mousepos.y() - 100,
	//								100*2, 100*2));
	//painter->setPen(QPen(Qt::lightGray, 0, Qt::SolidLine));
	//painter->setBrush(Qt::NoBrush);
	//painter->drawPolyline(m_points);
	qpainter.drawLine(10,10,100,100);
	*/

	if (m_d[0]!=-1) {
		qpainter.drawPixmap(610,300-16,m_dices[m_d[0]],0,0,width(),height());
		qpainter.drawPixmap(610+48,300-16,m_dices[m_d[1]],0,0,width(),height());
	}

	if (m_boardMsgActive) {
		QFont font("times", 24);
		QFontMetrics fm(font);
		int pixelsWide = fm.width(m_msg);
		int pixelsHigh = fm.lineSpacing();// .height();
		int x=(600-pixelsWide)/2;
		int y=(height()-pixelsHigh)/2;
		qpainter.setBrush(QColor(200, 200, 200, 200));
		QPen mypen(QColor(255, 255, 255, 200));
		mypen.setWidth(4);
		qpainter.setPen( mypen );
		qpainter.drawRect(x-10,y-10,pixelsWide+20,pixelsHigh+20);
		qpainter.setFont(font);
		qpainter.setPen( Qt::black );
		qpainter.drawText(x+2,y+2+3*pixelsHigh/4,m_msg);
		//QPen mytextpen(QColor(255, 255, 255, 100));
		//qpainter.setPen( mytextpen );
		qpainter.setPen( Qt::white );
		qpainter.drawText(x,y+3*pixelsHigh/4,m_msg);
	}
}


/*!
\fn board::mouseMoveEvent(QMouseEvent *qmouseevent)
*/
void board::mouseMoveEvent(QMouseEvent *qmouseevent)
{
	mousepos = qmouseevent->pos();

	if (m_showdrag) {

		update();
		//repaint();
	}
}


void board::wheelEvent ( QWheelEvent * ev )
{
	int numDegrees = ev->delta() / 8;
    int numSteps = numDegrees / 15;
	QString msg=QString::number(numSteps);
	boardMsg(msg);

}
void board::mousePressEvent ( QMouseEvent *me)
{
	if (me->button()==Qt::RightButton) {
		boardMsg("We could roll the dice here...");
		return;
	}
	if (m_sideToPlay==None) {
		boardMsg(tr("You are not currently in a game"));
		return;
	}
	setCursor(Qt::BlankCursor);
	m_showdrag=true;
	update();
}

void board::mouseReleaseEvent ( QMouseEvent *)
{
	m_showdrag=false;
	anBoard[1][23]=15;
	setCursor(Qt::ArrowCursor);

	update();
}

void board::setRoll(int d1, int d2)
{
	m_d[0]=d1;
	m_d[1]=d2;
	if (d1==d2)
		m_d[2]=m_d[3]=d1;
	update();
}
void board::resizeEvent ( QResizeEvent *re )
{
	QImage tmp;

	int w=re->size().width();
	int h=re->size().height();
	std::cout << "W=" << w << " H=" << h << std::endl;
	tmp=m_iboard.scaled(w,h,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	std::cout << "image width=" << tmp.width() << " image height=" << tmp.height() << std::endl;
	m_pboard=QPixmap::fromImage(tmp);
	std::cout << "pixmap width=" << m_pboard.width() << " pixmap height=" << m_pboard.height() << std::endl;

	m_pawnsize=42;
	tmp=m_ired.scaled(m_pawnsize,m_pawnsize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	m_pred=QPixmap::fromImage(tmp);
	std::cout << "pixmap width=" << m_pred.width() << " pixmap height=" << m_pred.height() << std::endl;
	tmp=m_iwhite.scaled(m_pawnsize,m_pawnsize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
	m_pwhite=QPixmap::fromImage(tmp);
	std::cout << "pixmap width=" << m_pwhite.width() << " pixmap height=" << m_pwhite.height() << std::endl;

	QString strdice;
	for(int i=1; i<7; ++i) {
		strdice=":/images/"+strdice.setNum(i)+".png";
		m_dices[i]=QPixmap(strdice);
	}

	update();
}

void board::boardMsg(QString msg, int seconds)
{
    m_timer->start(seconds*1000);
	m_boardMsgActive=1;
	m_msg=msg;
	update();
}

void board::boardMsgTimeOut()
{
	m_timer->stop();
	m_boardMsgActive=0;
	update();
}