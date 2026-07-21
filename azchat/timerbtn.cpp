#include "timerbtn.h"
#include <QTimer>
#include <QEvent>
#include <QMouseEvent>
TimerBtn::TimerBtn(QWidget *parent,int count)
    :QPushButton(parent),_counter(count)
{
    _timer = new QTimer(this);
    connect(_timer,&QTimer::timeout,[this,count](){
        _counter--;
        if(_counter<0) {
            _timer->stop();
            _counter = count;
            this->setEnabled(true);
            this->setText("获取");
            return;
        }
        this->setText(QString::number(_counter));
    });
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *e){
    QPushButton::mouseReleaseEvent(e);
    if(e->button() == Qt::LeftButton){
        this->setEnabled(false);
        this->setText(QString::number(_counter));
        _timer->start(1000);
        //emit clicked();
    }

}