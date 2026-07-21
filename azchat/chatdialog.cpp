#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QRandomGenerator>
#include "chatuserwid.h"
#include "loading_dialog.h"
#include <QTimer>
#include "statewidget.h"
#include <QMouseEvent>
#include "clickedlable.h"
#include "tcpmgr.h"
#include "usermgr.h"
#include "userinfo.h"
#include "conuseritem.h"
#include "contactlist.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog),_last_widget(nullptr)
{
    ui->setupUi(this);
    ui->add_pushButton->SetState("normal","hover","press");
    ui->search_pushButton->SetState("normal","hover","press");
    QAction* search_action = new QAction(ui->search_lineEdit);
    search_action->setIcon(QIcon(":res/chat-search-normal.svg"));
    ui->search_lineEdit->addAction(search_action,QLineEdit::LeadingPosition);
    ui->search_lineEdit->setPlaceholderText("搜索");
    QPixmap pix(":/res/head_1.jpg");
    pix = pix.scaled(ui->head_label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->head_label->setPixmap(pix);

    ui->chat_label->setState("normal","hover","press","selected","sel_hover","sel_press");
    ui->friends_label->setState("normal","hover","press","selected","sel_hover","sel_press");
    AddLBGroup(ui->chat_label);
    AddLBGroup(ui->friends_label);
    connect(ui->chat_label, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->friends_label, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);
    QAction* clear_action = new QAction(ui->search_lineEdit);
    clear_action->setIcon(QIcon(":res/chat-clear-input.svg"));
    ui->search_lineEdit->addAction(clear_action,QLineEdit::TrailingPosition);
    clear_action->setVisible(false);
    connect(ui->search_lineEdit,&QLineEdit::textChanged,[clear_action](const QString& text){
        if(!text.isEmpty()){
            clear_action->setVisible(true);
        }
    });
    connect(clear_action,&QAction::triggered,[this,clear_action](){
        ui->search_lineEdit->clear();
        clear_action->setVisible(false);
        ui->search_lineEdit->clearFocus();
        shwoSearch(false);
    });
    connect(ui->friends_listView,&ChatUserList::sig_load_chatUser,this,&ChatDialog::slot_load_chatUser);
    connect(ui->con_listWidget,&ContactList::sig_loading_contact_user,this,&ChatDialog::slot_load_conuser);
    shwoSearch(false);
    addChatUserList();
    connect(ui->search_lineEdit,&QLineEdit::textChanged,this,&ChatDialog::slot_text_changed);
    this->installEventFilter(this);
    ui->chat_label->SetSelected(true);
    connect(ui->friends_label,&StateWidget::clicked,this,&ChatDialog::slot_turnto_contact);
    ui->search_listView->setSeacrhEdit(ui->search_lineEdit);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_friend_apply,this,&ChatDialog::slot_apply_friend);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_refresh_friendapply,this,&ChatDialog::slot_refresh_friendapply);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_add_auth_friend,this,&ChatDialog::slot_add_auth_friend);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_auth_rsp,this,&ChatDialog::slot_add_friend_authrsp);
    connect(ui->search_listView,&SearchList::sig_jump_chat,this,&ChatDialog::slot_jump_chat);
    connect(ui->friend_info_page,&FriendInfoPage::sig_jump_chat_item,this,&ChatDialog::slot_jump_chat);
    connect(ui->friends_listView,&QListWidget::itemClicked,this,&ChatDialog::slot_item_clicked);

    connect(ui->con_listWidget,&ContactList::sig_switch_friend_info_page,this,&ChatDialog::slot_friendinfo_page);
    SetSelectedChatItem(0);
    SetSelectedChatPage(0);

    connect(ui->chatpage,&ChatPage::sig_append_send_chat_msg,this,&ChatDialog::slot_append_send_chat_msg);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_text_chat_msg,this,&ChatDialog::slot_text_chat_msg);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::shwoSearch(bool b_search)
{
    if(b_search){
        ui->friends_listView->hide();
        ui->con_listWidget->hide();
        ui->search_listView->show();
        _mode = ChatUIMode::SearchMode;
    }else{
        if(_state == ChatUIMode::ChatMode){
            ui->search_listView->hide();
            ui->con_listWidget->hide();
            ui->friends_listView->show();
            _mode = ChatUIMode::ChatMode;
        }
        if(_state == ChatUIMode::ContactMode){
            ui->search_listView->hide();
            ui->friends_listView->hide();
            ui->con_listWidget->show();
            _mode = ChatUIMode::ContactMode;
        }
    }
}


void ChatDialog::addChatUserList()
{
    // for(int i = 0;i<13;i++){
    //     int random = QRandomGenerator::global()->bounded(0,99);
    //     int head_i = random%heads.size();
    //     int name_i = random%names.size();
    //     int msg_i = random%msgs.size();

    //     auto *chat_user_wid = new ChatUserWid();
    //     chat_user_wid->setInfo(heads[head_i],names[name_i],msgs[msg_i]);

    //     QListWidgetItem* item = new QListWidgetItem();
    //     item->setSizeHint(chat_user_wid->sizeHint());
    //     ui->friends_listView->addItem(item);
    //     ui->friends_listView->setItemWidget(item,chat_user_wid);
    // }


    // std::vector<std::shared_ptr<FriendInfo>> vec;
    // vec = UserMgr::Get_instance()->GetFriendList();
    // for(auto ele:vec){
    //     int random = QRandomGenerator::global()->bounded(0,99);
    //     int head_i = random%heads.size();
    //     auto *chatuserwid = new ChatUserWid();
    //     chatuserwid->setInfo(heads[head_i],ele->_name,"");
    //     QListWidgetItem* item = new QListWidgetItem();
    //     item->setSizeHint(chatuserwid->sizeHint());
    //     ui->friends_listView->addItem(item);
    //     ui->friends_listView->setItemWidget(item,chatuserwid);
    // }
    auto friend_list = UserMgr::Get_instance()->GetChatListPerPage();
    if(!friend_list.empty()){
        for(auto ele:friend_list){
            auto find_iter = _chat_item_added.find(ele->_uid);
            if(find_iter != _chat_item_added.end()){
                continue;
            }
            int random = QRandomGenerator::global()->bounded(0,99);
            int head_i = random%heads.size();
            auto *chatuserwid = new ChatUserWid();
            auto userinfo = std::make_shared<UserInfo>(ele);
            userinfo->_icon = heads[head_i];
            chatuserwid->setInfo(userinfo);
            QListWidgetItem* item = new QListWidgetItem();
            item->setSizeHint(chatuserwid->sizeHint());
            ui->friends_listView->addItem(item);
            ui->friends_listView->setItemWidget(item,chatuserwid);
            _chat_item_added.insert(ele->_uid,item);
        }
        UserMgr::Get_instance()->UpdateChatLoadedCount();
    }
}

void ChatDialog::ClearLabelState(StateWidget *w)
{
    for(auto ele:_lb_list){
        if(ele == w){
            continue;
        }
        ele->ClearState();
    }
}

void ChatDialog::handlePress(QMouseEvent* ev)
{
    //qDebug()<<"label1"<<Qt::endl;
    if( _mode != ChatUIMode::SearchMode){
        return;
    }
    //qDebug()<<"label2"<<Qt::endl;
    // 将鼠标点击位置转换为搜索列表坐标系中的位置
    QPoint posInSearchList = ui->search_listView->mapFromGlobal(ev->globalPos());
    // 判断点击位置是否在聊天列表的范围内
    if (!ui->search_listView->rect().contains(posInSearchList)) {
        // 如果不在聊天列表内，清空输入框
        ui->search_lineEdit->clear();
        shwoSearch(false);
    }
}

void ChatDialog::SetSelectedChatItem(int uid)
{
    if(ui->friends_listView->count()<0){
        return;
    }
    if(uid == 0){
        ui->friends_listView->setCurrentRow(0);
        QListWidgetItem *first = ui->friends_listView->item(0);
        if(!first){
            return;
        }
        QWidget* wid = ui->friends_listView->itemWidget(first);
        if(!wid){
            return;
        }
        auto userw = qobject_cast<ChatUserWid*>(wid);
        if(!userw){
            return;
        }
        _cur_chat_uid = userw->GetUserInfo()->_uid;
        return;
    }
    auto item = _chat_item_added.find(uid);
    if(item == _chat_item_added.end()){
        ui->friends_listView->setCurrentRow(0);
        QListWidgetItem *first = ui->friends_listView->item(0);
        if(!first){
            return;
        }
        QWidget* wid = ui->friends_listView->itemWidget(first);
        if(!wid){
            return;
        }
        auto userw = qobject_cast<ChatUserWid*>(wid);
        if(!userw){
            return;
        }
        _cur_chat_uid = userw->GetUserInfo()->_uid;
        return;
    }
    else{
        ui->friends_listView->setCurrentItem(item.value());
        QWidget* wid = ui->friends_listView->itemWidget(item.value());
         auto userw = qobject_cast<ChatUserWid*>(wid);
        _cur_chat_uid = userw->GetUserInfo()->_uid;
        return;
    }


}

void ChatDialog::SetSelectedChatPage(int uid)
{
    if(ui->friends_listView->count()<0){
        return;
    }
    if(uid == 0){
        auto item = ui->friends_listView->item(0);
        QWidget* widget = ui->friends_listView->itemWidget(item);
        if(!widget){
            return;
        }
        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if(!con_item ){
            return;
        }
        auto userinfo = con_item->GetUserInfo();
        ui->chatpage->SetUserInfo(userinfo);
        return;
    }else{
        auto find_iter = _chat_item_added.find(uid);
        if(find_iter == _chat_item_added.end()){
            return;
        }
        QWidget* widget = ui->friends_listView->itemWidget(find_iter.value());
        if(!widget){
            return;
        }
        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if(!con_item ){
            return;
        }
        auto userinfo = con_item->GetUserInfo();
        ui->chatpage->SetUserInfo(userinfo);
        return;

    }
}

void ChatDialog::loadMoreCon()
{
    //qDebug()<<"loadmoreCon"<<Qt::endl;
    auto friend_list = UserMgr::Get_instance()->GetConListPerPage();
    if (friend_list.empty() == false) {
        for (auto& friend_ele : friend_list) {
            auto* con_user_wid = new ConUserItem();
            std::shared_ptr<UserInfo> info = std::make_shared<UserInfo>(friend_ele);
            int random = QRandomGenerator::global()->bounded(0,99);
            int head_i = random%heads.size();
            info->_icon = heads[head_i];
            con_user_wid->SetInfo(info);

            QListWidgetItem* item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(con_user_wid->sizeHint());
            ui->con_listWidget->addItem(item);
            ui->con_listWidget->setItemWidget(item, con_user_wid);
        }

        //更新已加载条目
        UserMgr::Get_instance()->UpdateConLoadedCount();
    }
}

bool ChatDialog::eventFilter(QObject * watched, QEvent * event)
{
    if(event->type() == QEvent::MouseButtonPress){
        //qDebug()<<"label3"<<Qt::endl;
        QMouseEvent* ev = static_cast<QMouseEvent*>(event);
        handlePress(ev);
    }
    return QDialog::eventFilter(watched,event);
}

void ChatDialog::slot_load_chatUser()
{
    if(_b_loading){
        return;
    }
    _b_loading = true;
    Loading_Dialog* load_dlg = new Loading_Dialog(this);
    load_dlg->setAttribute(Qt::WA_DeleteOnClose);
    int x = (this->width() - load_dlg->width()) / 2;
    int y = (this->height() - load_dlg->height()) / 2;
    load_dlg->move(x, y);
    load_dlg->show();
    load_dlg->raise();

    addChatUserList();
    load_dlg->deleteLater();

    _b_loading = false;
}

void ChatDialog::slot_load_conuser()
{
    if(_b_loading){
        return;
    }
    _b_loading = true;
    Loading_Dialog* load_dlg = new Loading_Dialog(this);
    load_dlg->setAttribute(Qt::WA_DeleteOnClose);
    int x = (this->width() - load_dlg->width()) / 2;
    int y = (this->height() - load_dlg->height()) / 2;
    load_dlg->move(x, y);
    load_dlg->show();
    load_dlg->raise();

    loadMoreCon();
    load_dlg->deleteLater();

    _b_loading = false;
}

void ChatDialog::slot_side_chat()
{
    //qDebug()<< "receive side chat clicked";
    ClearLabelState(ui->chat_label);
    ui->stackedWidget->setCurrentWidget(ui->chatpage);
    _state = ChatUIMode::ChatMode;
    shwoSearch(false);
}

void ChatDialog::slot_side_contact()
{
    //qDebug()<< "receive side chat clicked";
    ClearLabelState(ui->friends_label);
}

void ChatDialog::slot_text_changed(QString str)
{
    if(!str.isEmpty()){
        shwoSearch(true);
    }
    else{
        shwoSearch(false);
    }
}

void ChatDialog::slot_turnto_contact()
{
    if (_last_widget == nullptr) {
        ui->stackedWidget->setCurrentWidget(ui->friendapply_page);
        _last_widget = ui->friendapply_page;
    }
    else {
        ui->stackedWidget->setCurrentWidget(_last_widget);
    }
    _state = ChatUIMode::ContactMode;
    shwoSearch(false);
}

void ChatDialog::slot_apply_friend(std::shared_ptr<AddFriendApply> apply)
{
    // qDebug() << "receive apply friend slot, applyuid is " << apply->_from_uid << " name is "
    //          << apply->_name << " desc is " << apply->_desc;

    // bool b_already = UserMgr::Get_instance()->AlreadyApply(apply->_from_uid);
    // if(b_already){
    //     return;
    // }

    // UserMgr::Get_instance()->AddApplyList(std::make_shared<ApplyInfo>(apply));

   // qDebug()<<"change chatdlg"<<Qt::endl;
   // qDebug()<<"test:"<<apply->_from_uid<<apply->_name<<Qt::endl;
    bool b_already = UserMgr::Get_instance()->AlreadyApply(apply->_from_uid);
    if(b_already){
        return;
    }
    //qDebug()<<"debug1"<<Qt::endl;
    UserMgr::Get_instance()->AddApplyList(std::make_shared<ApplyInfo>(apply));
    ui->friends_label->ShowRedPoint(true);
    ui->con_listWidget->ShowRedPoint(true);
    ui->friendapply_page->AddNewApply(apply);
}

void ChatDialog::slot_refresh_friendapply()
{
    std::vector<std::shared_ptr<ApplyInfo>> vec = UserMgr::Get_instance()->GetApplyList();
    for(auto ele:vec){
        std::shared_ptr<AddFriendApply> temp = std::make_shared<AddFriendApply>(ele->_uid, ele->_name, ele->_desc,
                                              ele->_icon, ele->_nick, ele->_sex);
        ui->friendapply_page->AddNewApply(temp);
    }
}

void ChatDialog::slot_add_auth_friend(std::shared_ptr<AuthInfo> info)
{
    bool isFriend = UserMgr::Get_instance()->checkUserByUid(info->_uid);
    if(isFriend){
        return;
    }
    int random = QRandomGenerator::global()->bounded(0,100);
    int head_i = random%heads.size();
    int str_i = random%msgs.size();
    info->_icon = heads[head_i];

    auto *chat_user_wid = new ChatUserWid();
    chat_user_wid->setInfo(std::make_shared<UserInfo>(info));

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->friends_listView->addItem(item);
    ui->friends_listView->setItemWidget(item,chat_user_wid);
    _chat_item_added.insert(info->_uid,item);
    UserMgr::Get_instance()->AddFriend(info);
}

void ChatDialog::slot_add_friend_authrsp(std::shared_ptr<AuthRsp> info)
{
    bool isFriend = UserMgr::Get_instance()->checkUserByUid(info->_uid);
    if(isFriend){
        return;
    }
    int random = QRandomGenerator::global()->bounded(0,100);
    int head_i = random%heads.size();
    int str_i = random%msgs.size();
    info->_icon = heads[head_i];

    auto *chat_user_wid = new ChatUserWid();
    chat_user_wid->setInfo(std::make_shared<UserInfo>(info));

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->friends_listView->addItem(item);
    ui->friends_listView->setItemWidget(item,chat_user_wid);
    _chat_item_added.insert(info->_uid,item);
    UserMgr::Get_instance()->AddFriend(info);
}

void ChatDialog::slot_jump_chat(std::shared_ptr<UserInfo> info)
{
    //qDebug() << "slot jump chat item " << Qt::endl;
    auto finditer = _chat_item_added.find(info->_uid);
    _cur_chat_uid = info->_uid;
    if(finditer != _chat_item_added.end()){
        ui->friends_listView->scrollToItem(finditer.value());
        ui->chat_label->SetSelected(true);
        SetSelectedChatItem(info->_uid);
        SetSelectedChatPage(info->_uid);
        slot_side_chat();
        return;
    }
    auto chat_user_wid = new ChatUserWid();
    chat_user_wid->setInfo(info);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->friends_listView->insertItem(0,item);
    ui->friends_listView->setItemWidget(item,chat_user_wid);
    _chat_item_added.insert(info->_uid,item);
    SetSelectedChatItem(info->_uid);
    SetSelectedChatPage(info->_uid);
    slot_side_chat();
    return;
}

void ChatDialog::slot_friendinfo_page(std::shared_ptr<UserInfo> info)
{
    _last_widget = ui->friend_info_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(info);
}

void ChatDialog::slot_item_clicked(QListWidgetItem *item)
{
    QWidget* widget = ui->friends_listView->itemWidget(item); // 获取自定义widget对象
    if (!widget) {
        //qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ItemBase* customItem = qobject_cast<ItemBase*>(widget);
    if (!customItem) {
        //qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetType();
    if (itemType == ListItemType::INVALID_ITEM
        || itemType == ListItemType::GROUP_TIP_ITEM) {
        //qDebug() << "slot invalid item clicked ";
        return;
    }


    if (itemType == ListItemType::CHAT_USER_ITEM) {
        // 创建对话框，提示用户
        //qDebug() << "contact user item clicked ";

        auto chat_wid = qobject_cast<ChatUserWid*>(customItem);
        auto userinfo = chat_wid->GetUserInfo();
        ui->chatpage->SetUserInfo(userinfo);
        //auto chat_data = chat_wid->GetChatData();
        //跳转到聊天界面
        //ui->chatpage->SetChatData(chat_data);
        _cur_chat_uid = userinfo->_uid;
        return;
    }
}

void ChatDialog::slot_append_send_chat_msg(std::shared_ptr<TextChatData> msg)
{
    if(_cur_chat_uid == 0){
        return;
    }
    auto finditer = _chat_item_added.find(_cur_chat_uid);
    if(finditer == _chat_item_added.end()){
        return;
    }
    QWidget* wid = ui->friends_listView->itemWidget(finditer.value());
    if(!wid){
        return;
    }
    ItemBase* item  = qobject_cast<ItemBase*>(wid);
    if(!item){
        return;
    }

    auto itemtype = item->GetType();
    if(itemtype == ListItemType::CHAT_USER_ITEM){
        auto chatuserwid = qobject_cast<ChatUserWid*>(item);
        if(!item){
            return;
        }
        auto userinfo = chatuserwid->GetUserInfo();
        userinfo->_chat_msgs.push_back(msg);
        std::vector<std::shared_ptr<TextChatData>> msg_vec;
        msg_vec.push_back(msg);
        UserMgr::Get_instance()->AppendFriendChatMsg(_cur_chat_uid,msg_vec);
    }
}

void ChatDialog::slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg)
{
    auto find_iter = _chat_item_added.find(msg->_from_uid);
    if(find_iter != _chat_item_added.end()){
        //qDebug() << "set chat item msg, uid is " << msg->_from_uid;
        QWidget *widget = ui->friends_listView->itemWidget(find_iter.value());
        auto chat_wid = qobject_cast<ChatUserWid*>(widget);
        if(!chat_wid){
            return;
        }
        chat_wid->updateLastMsg(msg->_vec);
        //更新当前聊天页面记录
        UpdateChatMsg(msg->_vec);
        UserMgr::Get_instance()->AppendFriendChatMsg(msg->_from_uid,msg->_vec);
        return;
    }

    //如果没找到，则创建新的插入listwidget

    auto* chat_user_wid = new ChatUserWid();
    //查询好友信息
    auto fi_ptr = UserMgr::Get_instance()->GetFriendByUid(msg->_from_uid);
    chat_user_wid->setInfo(fi_ptr);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    chat_user_wid->updateLastMsg(msg->_vec);
    UserMgr::Get_instance()->AppendFriendChatMsg(msg->_from_uid,msg->_vec);
    ui->friends_listView->insertItem(0, item);
    ui->friends_listView->setItemWidget(item, chat_user_wid);
    _chat_item_added.insert(msg->_from_uid, item);
}

void ChatDialog::UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>> msgs)
{
    for(auto& msg:msgs){
        if(msg->_fromuid!=_cur_chat_uid){
            break;
        }
        ui->chatpage->AppendChatMsg(msg);
    }
}








