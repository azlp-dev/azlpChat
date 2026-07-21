#include "conuseritem.h"
#include "ui_conuseritem.h"

ConUserItem::ConUserItem(QWidget *parent)
    : ItemBase(parent)
    , ui(new Ui::ConUserItem)
{
    ui->setupUi(this);
    this->SetType(ListItemType::CONTACT_USER_ITEM);
    ui->ci_redpoint_label->raise();
    showRedPoint(true);
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

void ConUserItem::showRedPoint(bool b)
{
    if(b){
        ui->ci_redpoint_label->show();
    }else{
        ui->ci_redpoint_label->hide();
    }
}

void ConUserItem::SetInfo(std::shared_ptr<AuthInfo> auth_info)
{
    _info = std::make_shared<UserInfo>(auth_info);
    // 加载图片
    QPixmap pixmap(_info->_icon);

    // 设置图片自动缩放
    ui->ci_icon_label->setPixmap(pixmap.scaled(ui->ci_icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->ci_icon_label->setScaledContents(true);

    ui->ci_name_label->setText(_info->_name);
}


void ConUserItem::SetInfo(std::shared_ptr<AuthRsp> auth_rsp)
{
    _info = std::make_shared<UserInfo>(auth_rsp);
    // 加载图片
    QPixmap pixmap(_info->_icon);

    // 设置图片自动缩放
    ui->ci_icon_label->setPixmap(pixmap.scaled(ui->ci_icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->ci_icon_label->setScaledContents(true);

    ui->ci_name_label->setText(_info->_name);
}

void ConUserItem::SetInfo(std::shared_ptr<UserInfo> info)
{
    _info = info;
    // 加载图片
    QPixmap pixmap(info->_icon);

    // 设置图片自动缩放
    ui->ci_icon_label->setPixmap(pixmap.scaled(ui->ci_icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->ci_icon_label->setScaledContents(true);

    ui->ci_name_label->setText(info->_name);
}

void ConUserItem::SetInfo(int uid, QString name, QString icon)
{
    _info = std::make_shared<UserInfo>(uid,name,"",icon,1,"","");
    QPixmap pix(icon);
    pix = pix.scaled(ui->ci_icon_label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);

    ui->ci_icon_label->setPixmap(pix);
    ui->ci_name_label->setText(name);
}

QSize ConUserItem::sizeHint() const
{
    return QSize(250,70);
}
