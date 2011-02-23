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
#ifndef BOARD_H
#define BOARD_H

#include <QWidget>

class QImage;
class QPixmap;

class Network;

/**
This is the widget that implement the board (Tavliera)

@author Alex Argiropoulos
*/
class board : public QWidget
{
    Q_OBJECT
public:
    enum SideToPlay {White,Red,None};
    enum GameType {Portes, Plakoto, Fevga};

    board(QWidget *parent = 0);
    void setNetwork(Network *network);
    void setGame(GameType n);
    void setBoard(int b[2][25]);
    void setRoll(int d1, int d2);
    void boardMsg(QString msg,int seconds=4);

    ~board();

protected:
    void paintEvent(QPaintEvent *);
    void wheelEvent ( QWheelEvent * ev );
    void mouseMoveEvent(QMouseEvent *qmouseevent);
    virtual void resizeEvent ( QResizeEvent *re);
    void mousePressEvent ( QMouseEvent * e );
    void mouseReleaseEvent ( QMouseEvent * e );


private:
    Network *m_network;
    QPoint  mousepos;
    bool    m_showdrag;
    int     m_boardMsgActive;
    bool    m_netmove;
    int     m_pawnsize;
    QImage  m_iboard;
    QPixmap m_pboard;
    QImage  m_iwhite;
    QImage  m_ired;
    QPixmap m_pred; 
    QPixmap m_pwhite;

    QPixmap m_dices[7];
    int     m_d[4];

    //QPixmap *qpixmap;
    //bool bneedrecreate;

    int     anBoard[2][25];
    GameType m_gameType;
    SideToPlay m_sideToPlay;
    QTimer *m_timer;

    QString m_msg;
    bool    m_holdingPiece;
    int     m_grabFrom;
private slots:
    void boardMsgTimeOut();
    void netMove(int x, int y);

signals:
    void Log(QString str);
    void requestGrab(int from);
    void requestMove(int from, int to);
};
#endif
