#include "chatuserlist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "usermgr.h"
ChatUserList::ChatUserList(QWidget *parent):QListWidget(parent)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this->viewport()&&event->type() == QEvent::Enter){
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    if(watched == this->viewport()&&event->type() == QEvent::Leave){
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    if(watched == this->viewport()&&event->type() == QEvent::Wheel){
        QWheelEvent* wheel = static_cast<QWheelEvent*>(event);
        int numdegrees = wheel->angleDelta().y()/8;
        int step = numdegrees/15;
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value()-step);

        if(this->verticalScrollBar()->maximum()-this->verticalScrollBar()->value()<=0){
            if(UserMgr::Get_instance()->isLoadFin()){
                return true;
            }
            emit sig_load_chatUser();
        }
        return true;
    }

    return QListWidget::eventFilter(watched,event);;
}


