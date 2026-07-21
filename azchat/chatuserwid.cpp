#include "chatuserwid.h"
#include "ui_chatuserwid.h"

ChatUserWid::ChatUserWid(QWidget *parent)
    : ItemBase(parent)
    , ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    this->SetType(ListItemType::CHAT_USER_ITEM);
}

ChatUserWid::~ChatUserWid()
{
    delete ui;
}

void ChatUserWid::setInfo(QString head, QString name, QString msg)
{
    _head = head;
    _name = name;
    _msg = msg;

    QPixmap pixmap(_head);
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(),Qt::KeepAspectRatioByExpanding,Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(false);

    ui->name_label->setText(name);
    ui->msg_label->setText(msg);
}

void ChatUserWid::setInfo(std::shared_ptr<UserInfo> info)
{
    _friend_info = info;
    QPixmap pix(_friend_info->_icon);
    ui->icon_label->setPixmap(pix.scaled(ui->icon_label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->name_label->setText(info->_name);
    ui->msg_label->setText(info->_last_msg);
}

void ChatUserWid::updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs)
{
    QString lastmsg = "";
    for(auto& msg:msgs){
        lastmsg = msg->_msg_content;
        _friend_info->_chat_msgs.push_back(msg);
    }
    _friend_info->_last_msg = lastmsg;
    ui->msg_label->setText(lastmsg);
}


