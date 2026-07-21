#include "friendappy_item.h"
#include "ui_friendappy_item.h"

FriendAppy_Item::FriendAppy_Item(QWidget *parent)
    :ItemBase(parent)
    ,ui(new Ui::FriendAppy_Item)
{
    ui->setupUi(this);
    this->SetType(ListItemType::APPLY_FRIEND_ITEM);
    ui->fa_add_pushButton->SetState("normal","hover","press");
    connect(ui->fa_add_pushButton,&ClickedBtn::clicked,[this](){
        emit sig_auth_friend(_apply_info);
        ui->fa_add_pushButton->setText("已添加");
        ui->fa_add_pushButton->setEnabled(false);
    });
}

FriendAppy_Item::~FriendAppy_Item()
{
    delete ui;
}

void FriendAppy_Item::SetInfo(std::shared_ptr<ApplyInfo> apply_info)
{
    _apply_info = apply_info;
    QPixmap pix(apply_info->_icon);
    pix = pix.scaled(ui->label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->label->setPixmap(pix);
    ui->fa_name_label->setText(apply_info->_name);
    ui->fa_msglabel->setText(_apply_info->_desc);

}

void FriendAppy_Item::showbutton(bool b)
{
    if(!b){
        ui->fa_add_pushButton->setText("已添加");
    }
    ui->fa_add_pushButton->setEnabled(b);
}

// void FriendAppy_Item::on_fa_add_pushButton_clicked()
// {

// }




