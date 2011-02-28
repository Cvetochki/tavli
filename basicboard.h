#ifndef BASICBOARD_H
#define BASICBOARD_H

#include "tavli.h"
#include <QWidget>

class basicBoard : public QWidget
{
    Q_OBJECT
    int m_board[2][25];
    GameType m_gameType;
    int m_d1,m_d2;

    void log(QString str);
public:
    explicit basicBoard(QWidget *parent = 0);
    void setBoard(int board[2][25]);
    void setGame(GameType gameType);
    void setRoll(int d1, int d2);


signals:
    void Log(QString str);

public slots:
    void netMove(int x, int y);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    QSizePolicy sizePolicy() const;
    QSize sizeHint() const;
    int heightForWidth(int w) const;
};

#endif // BASICBOARD_H
