#include "basicboard.h"
#include <QPainter>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QDebug>

basicBoard::basicBoard(QWidget *parent) :
    QWidget(parent)
{
    QSizePolicy size_policy;
    size_policy.setHeightForWidth( true );
    setSizePolicy( size_policy );
}


void basicBoard::setBoard(int board[2][25])
{
    for(int i=0; i<25; ++i) {
        m_board[0][i]=board[0][i];
        m_board[1][i]=board[1][i];
    }
}

void basicBoard::setGame(GameType gameType)
{
    m_gameType=gameType;
}

void basicBoard::setRoll(int d1,int d2)
{
    m_d1=d1;
    m_d2=d2;
}

void basicBoard::log(QString str)
{
    emit Log(str);
}

void basicBoard::netMove(int x, int y)
{

}

void basicBoard::paintEvent(QPaintEvent *)
{

    QColor background = QColor(130,100,100);
    QPainter qpainter(this);
    qpainter.setRenderHint(QPainter::Antialiasing,true);
    //qpainter.drawRect(0,0,width()-1,height()-1);
    qpainter.fillRect(0,0,width()-1,height()-1, background);

    QColor lightcolor = QColor(255,255,255);
    qpainter.setPen(lightcolor);
    qpainter.drawEllipse(QPoint(width()/2,height()/2),width()/2,height()/2);
}

QSize basicBoard::sizeHint() const
{
    //return QWidget::sizeHint();
    return QSize(400,300);
}

void basicBoard::resizeEvent(QResizeEvent *ev)
{
    qDebug() << ev->size();
}

QSizePolicy basicBoard::sizePolicy() const
{
    QSizePolicy ret=this->sizePolicy();
    ret.setHeightForWidth(true);
    return ret;
}

int basicBoard::heightForWidth(int w) const
{
    qDebug() << "width = " << w;
    return w;
}
