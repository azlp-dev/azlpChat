#include "contactlist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "conuseritem.h"
#include <QListWidgetItem>
#include "itembase.h"
#include "grouptip_item.h"
#include "conuseritem.h"
#include <QRandomGenerator>
#include "tcpmgr.h"
#include "usermgr.h"
#include <QRandomGenerator>

ContactList::ContactList(QWidget *parent)
    :QListWidget(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->viewport()->installEventFilter(this);
    addContactUserList();
    connect(this,&QListWidget::itemClicked,this,&ContactList::slot_item_clicked);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_add_auth_friend,this,&ContactList::slot_add_auth_firend);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_auth_rsp,this,&ContactList::slot_auth_rsp);
    //qDebug() << "ContactList ctor =" << this;

}

void ContactList::ShowRedPoint(bool bshow)
{

}

bool ContactList::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this->viewport()&&event->type() == QEvent::Enter){
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }else if(watched == this->viewport()&&event->type() == QEvent::Leave){
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    if(watched == this->viewport()&& event->type() == QEvent::Wheel){
       QWheelEvent* ev = static_cast<QWheelEvent*>(event);
        int numDegree = ev->angleDelta().y()/8;
        int step = numDegree/15;

        QScrollBar* sb = this->verticalScrollBar();
        sb->setValue(sb->value() - step);
        int maxm = sb->maximum();
        if(sb->value()>=maxm){
            //qDebug()<<"emit sig_load_con"<<Qt::endl;
            if(UserMgr::Get_instance()->isLoadConFin()){
                return true;
            }
            emit sig_loading_contact_user();
        }
        return true;
    }
    return QListWidget::eventFilter(watched,event);
}

void ContactList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget* w = this->itemWidget(item);
    if(!w){
        //qDebug()<<"slot_item_clicked 1"<<Qt::endl;
        return;
    }
    ItemBase* i = static_cast<ItemBase*>(w);
    if(i->GetType() == ListItemType::INVALID_ITEM||i->GetType() == ListItemType::GROUP_TIP_ITEM){
        //qDebug()<<"slot_item_clicked 2"<<Qt::endl;
        return;
    }
    if(i->GetType() == ListItemType::APPLY_FRIEND_ITEM){
        //qDebug()<<"slot_item_clicked 3"<<Qt::endl;
        emit sig_switch_apply_friend_page();
        return;
    }
    if(i->GetType() == ListItemType::CONTACT_USER_ITEM){
        //qDebug()<<"slot_item_clicked 4"<<Qt::endl;
        auto itemtemp = qobject_cast<ConUserItem*>(w);
        emit sig_switch_friend_info_page(itemtemp->GetInfo());
        return;
    }
}

void ContactList::slot_add_auth_firend(std::shared_ptr<AuthInfo> info)
{
    //qDebug()<<"slot slot_add_auth_firend"<<Qt::endl;
    bool isFriend = UserMgr::Get_instance()->checkUserByUid(info->_uid);
    if(isFriend){
        //qDebug()<<"error here"<<Qt::endl;
        return;
    }
    int random = QRandomGenerator::global()->bounded(0,100);
    int head_i = random%heads.size();
    int str_i = random%msgs.size();
    info->_icon = heads[head_i];
    ConUserItem* item = new ConUserItem();
    item->SetInfo(info);
    QListWidgetItem* temp = new QListWidgetItem();
    temp->setSizeHint(item->sizeHint());
    this->addItem(temp);
    this->setItemWidget(temp,item);
}

void ContactList::slot_auth_rsp(std::shared_ptr<AuthRsp> info)
{
    //qDebug()<<"enter slot_auth_rsp"<<Qt::endl;
    bool isFriend = UserMgr::Get_instance()->checkUserByUid(info->_uid);
    if(isFriend){
        //qDebug()<<"error here"<<Qt::endl;
        return;
    }
    int random = QRandomGenerator::global()->bounded(0,100);
    int head_i = random%heads.size();
    int str_i = random%msgs.size();
    info->_icon = heads[head_i];
    ConUserItem* item = new ConUserItem();
    item->SetInfo(info);
    QListWidgetItem* temp = new QListWidgetItem();
    temp->setSizeHint(item->sizeHint());
    this->addItem(temp);
    this->setItemWidget(temp,item);
    //qDebug() << "slot_auth_rsp =" << this;
}

void ContactList::addContactUserList()
{
    auto* first = new Grouptip_Item();
    first->setText("新的朋友");
    QListWidgetItem* new_friend = new QListWidgetItem();
    new_friend->setSizeHint(first->sizeHint());
    this->addItem(new_friend);
    this->setItemWidget(new_friend,first);
    new_friend->setFlags(new_friend->flags() & ~Qt::ItemIsSelectable);

    _add_friend_item = new ConUserItem();
    _add_friend_item->setObjectName("add_friend_item");
    _add_friend_item->SetInfo(-1,"新的朋友",":/res/person_add.svg");
    QListWidgetItem* new_friend1 = new QListWidgetItem();
    new_friend1->setSizeHint(_add_friend_item->sizeHint());
    this->addItem(new_friend1);
    this->setItemWidget(new_friend1,_add_friend_item);

    auto second = new Grouptip_Item();
    second->setText("联系人");
    QListWidgetItem* contact = new QListWidgetItem();
    contact->setSizeHint(second->sizeHint());
    this->addItem(contact);
    this->setItemWidget(contact,second);

    // for(int i = 0;i<13;i++){
    //     int random = QRandomGenerator::global()->bounded(100);
    //     QString head = heads[random%heads.size()];
    //     QString name = names[random%heads.size()];
    //     ConUserItem* item = new ConUserItem();
    //     item->SetInfo(-1,name,head);
    //     QListWidgetItem* temp = new QListWidgetItem();
    //     temp->setSizeHint(item->sizeHint());
    //     this->addItem(temp);
    //     this->setItemWidget(temp,item);
    // }
    auto con_list = UserMgr::Get_instance()->GetConListPerPage();
    for(auto& ele:con_list){
        int random = QRandomGenerator::global()->bounded(100);
            QString head = heads[random%heads.size()];
            ConUserItem* item = new ConUserItem();
            auto userinfo = std::make_shared<UserInfo>(ele);
            userinfo->_icon = head;
            item->SetInfo(userinfo);
            QListWidgetItem* temp = new QListWidgetItem();
            temp->setSizeHint(item->sizeHint());
            this->addItem(temp);
            this->setItemWidget(temp,item);
    }
    UserMgr::Get_instance()->UpdateConLoadedCount();
}


