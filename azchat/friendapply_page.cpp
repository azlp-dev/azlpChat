#include "friendapply_page.h"
#include "ui_friendapply_page.h"
#include "friendapply_list.h"
#include "friendappy_item.h"
#include "tcpmgr.h"
#include "usermgr.h"
#include <QRandomGenerator>
#include "authenfriend.h"
FriendApply_Page::FriendApply_Page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FriendApply_Page)
{
    ui->setupUi(this);
    connect(ui->listWidget,&FriendApply_List::sig_show_search,this,&FriendApply_Page::sig_show_search);
    loadApplyList();
    //接受tcp传递的authrsp信号处理
    connect(TcpMgr::Get_instance().get(), &TcpMgr::sig_auth_rsp, this, &FriendApply_Page::slot_auth_rsp);
    this->setAttribute(Qt::WA_StyledBackground);
}

FriendApply_Page::~FriendApply_Page()
{
    delete ui;
}

void FriendApply_Page::AddNewApply(std::shared_ptr<AddFriendApply> apply)
{
    //qDebug()<<"debug2"<<Qt::endl;
    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    int head_i = randomValue % heads.size();
    auto* apply_item = new FriendAppy_Item();
    auto apply_info = std::make_shared<ApplyInfo>(apply->_from_uid,
                                                  apply->_name, apply->_desc,heads[head_i], apply->_name, 0, 0);
    apply_item->SetInfo( apply_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(apply_item->sizeHint());
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    ui->listWidget->insertItem(0,item);
    ui->listWidget->setItemWidget(item, apply_item);
    apply_item->showbutton(true);
    //收到审核好友信号
    connect(apply_item, &FriendAppy_Item::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info) {
        auto* authFriend = new AuthenFriend(this);
        authFriend->setModal(true);
        authFriend->SetApplyInfo(apply_info);
        authFriend->show();
    });
}

void FriendApply_Page::loadApplyList()
{
    auto apply_list = UserMgr::Get_instance()->GetApplyList();
    for(auto &apply: apply_list){
        qDebug()<<"apply_list have ele"<<Qt::endl;
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int head_i = randomValue % heads.size();
        auto* apply_item = new FriendAppy_Item();
        apply->SetIcon(heads[head_i]);
        apply_item->SetInfo(apply);
        QListWidgetItem* item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui->listWidget->insertItem(0,item);
        ui->listWidget->setItemWidget(item, apply_item);
        if(apply->_status){
            apply_item->showbutton(false);
        }else{
            apply_item->showbutton(true);
            auto uid = apply_item->GetUid();
            _unauth_items[uid] = apply_item;
        }

        //收到审核好友信号
        connect(apply_item, &FriendAppy_Item::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info) {
                       auto* authFriend = new AuthenFriend(this);
                       authFriend->setModal(true);
                       authFriend->SetApplyInfo(apply_info);
                       authFriend->show();
        });
    }

    // 模拟假数据，创建QListWidgetItem，并设置自定义的widget
    // for(int i = 0; i < 13; i++){
    //     int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    //     int str_i = randomValue%msgs.size();
    //     int head_i = randomValue%heads.size();
    //     int name_i = randomValue%names.size();

    //     auto *apply_item = new FriendAppy_Item();
    //     auto apply = std::make_shared<ApplyInfo>(0, names[name_i], msgs[str_i],
    //                                              heads[head_i], names[name_i], 0, 1);
    //     apply_item->SetInfo(apply);
    //     QListWidgetItem *item = new QListWidgetItem;
    //     //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    //     item->setSizeHint(apply_item->sizeHint());
    //     item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    //     ui->listWidget->addItem(item);
    //     ui->listWidget->setItemWidget(item, apply_item);
    //     //收到审核好友信号
    //     connect(apply_item, &FriendAppy_Item::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info){
    //         //            auto *authFriend =  new AuthenFriend(this);
    //         //            authFriend->setModal(true);
    //         //            authFriend->SetApplyInfo(apply_info);
    //         //            authFriend->show();
    //     });
    // }
}

void FriendApply_Page::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto uid = auth_rsp->_uid;
    auto find_iter = _unauth_items.find(uid);
    if (find_iter == _unauth_items.end()) {
        return;
    }

    find_iter->second->showbutton(false);
}
