#include "searchlist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include "tcpmgr.h"
#include "adduseritem.h"
#include "itembase.h"
#include "find_success_dlg.h"
#include "find_failed_dlg.h"
#include <customizeedit.h>
#include "usermgr.h"
SearchList::SearchList(QWidget *parent)
    :QListWidget(parent),_b_pendiong(false),_find_dlg(nullptr),_search_edit()
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->viewport()->installEventFilter(this);
    connect(this,&QListWidget::itemClicked,this,&SearchList::slot_item_clicked);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_user_search,this,&SearchList::slot_search_recved);
    test_addItem();
}

void SearchList::closeFindDlg()
{
    if(_find_dlg){
        _find_dlg->hide();
        _find_dlg = nullptr;
    }
}

void SearchList::setSeacrhEdit(QWidget *w)
{
    _search_edit = w;
}

bool SearchList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched, event);
}

void SearchList::waitPending(bool b)
{
    if(b){
        _b_pendiong = true;
        load_dlg = new Loading_Dialog(this,this->parentWidget()->size());
        //qDebug()<<"tick"<<load_dlg->size()<<Qt::endl;
        load_dlg->setModal(true);
        load_dlg->show();
    }else{
        _b_pendiong = false;
        load_dlg->deleteLater();
    }
}



void SearchList::test_addItem()
{
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item_tmp->setSizeHint(QSize(250,10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);


    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}


void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); //获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ItemBase *customItem = qobject_cast<ItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetType();
    if(itemType == ListItemType::INVALID_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }

    if(itemType == ListItemType::ADD_USER_TIP_ITEM){
        if(_b_pendiong){
            return;
        }
        waitPending(true);

        auto edit = dynamic_cast<CustomizeEdit*>(_search_edit);
        auto uidstr = edit->text();
        QJsonObject jsobj;
        jsobj["uid"] = uidstr;
        QJsonDocument doc(jsobj);
        QByteArray data = doc.toJson(QJsonDocument::Compact);
        emit TcpMgr::Get_instance()->sig_send_data(Reqids::ID_SEARCH_USER_REQ,data);
        return;
    }

    closeFindDlg();
}

void SearchList::slot_search_recved(std::shared_ptr<SearchInfo> info)
{
    if(info == nullptr){
        qDebug() << "failed";
    }
    else{
        qDebug() << "success";
    }
    waitPending(false);
    if(info == nullptr){
        _find_dlg = std::make_shared<find_failed_dlg>(this);
        //qDebug()<<"find_failed_dlg"<<Qt::endl;
    }
    else{
        auto self = UserMgr::Get_instance()->getuid();
        if(self == info->uid){
            return;
        }
        bool b_check = UserMgr::Get_instance()->checkUserByUid(info->uid);
        auto temp = std::make_shared<UserInfo>(info);
        if(b_check){
            emit sig_jump_chat(temp);
            return;
        }

        _find_dlg = std::make_shared<Find_Success_Dlg>(this);
        std::dynamic_pointer_cast<Find_Success_Dlg>(_find_dlg)->setSearchInfo(info);
        //qDebug()<<"debug3"<<Qt::endl;
    }
    _find_dlg->show();
}
