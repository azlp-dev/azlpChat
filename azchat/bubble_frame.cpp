#include "bubble_frame.h"
#include <QPainter>

const int SANJIAO = 8;
Bubble_Frame::Bubble_Frame(chat_role role, QWidget *parent)
    :QFrame(parent),_margin(3),m_role(role)
{
    _hl = new QHBoxLayout();
    if(role == chat_role::self)
        _hl->setContentsMargins(_margin,_margin,_margin+SANJIAO,_margin);
    else
        _hl->setContentsMargins(SANJIAO+_margin,_margin,_margin,_margin);
    this->setLayout(_hl);
}

void Bubble_Frame::setMargin(int margin)
{
    _margin = margin;
}

void Bubble_Frame::setWidget(QWidget *w)
{
    if(_hl->count()>0){
        return;
    }else{
        _hl->addWidget(w);
    }
}

void Bubble_Frame::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    if(m_role == chat_role::other)
    {
        //画气泡
        QColor bk_color(Qt::white);
        painter.setBrush(QBrush(bk_color));
        QRect bk_rect = QRect(SANJIAO, 0, this->width()-SANJIAO, this->height());
        painter.drawRoundedRect(bk_rect,5,5);
        //画小三角
        QPointF points[3] = {
            QPointF(bk_rect.x(), 12),
            QPointF(bk_rect.x(), 10+SANJIAO +2),
            QPointF(bk_rect.x()-SANJIAO, 10+SANJIAO-SANJIAO/2),
        };
        painter.drawPolygon(points, 3);
    }
    else
    {
        QColor bk_color(158,234,106);
        painter.setBrush(QBrush(bk_color));
        //画气泡
        QRect bk_rect = QRect(0, 0, this->width()-SANJIAO, this->height());
        painter.drawRoundedRect(bk_rect,5,5);
        //画三角
        QPointF points[3] = {
            QPointF(bk_rect.x()+bk_rect.width(), 12),
            QPointF(bk_rect.x()+bk_rect.width(), 12+SANJIAO +2),
            QPointF(bk_rect.x()+bk_rect.width()+SANJIAO, 10+SANJIAO-SANJIAO/2),
        };
        painter.drawPolygon(points, 3);

    }

    return QFrame::paintEvent(e);
}


