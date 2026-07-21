#ifndef FRIENDAPPY_ITEM_H
#define FRIENDAPPY_ITEM_H

#include <QWidget>
#include "userinfo.h"
#include "itembase.h"

namespace Ui {
class FriendAppy_Item;
}

class FriendAppy_Item : public ItemBase
{
    Q_OBJECT

public:
    explicit FriendAppy_Item(QWidget *parent = nullptr);

    ~FriendAppy_Item();
    void SetInfo(std::shared_ptr<ApplyInfo> apply_info);
    //void ShowAddBtn(bool bshow);
    QSize sizeHint() const override {
        return QSize(250, 80); // 返回自定义的尺寸
    }
    int GetUid(){
        return _apply_info->_uid;
    }
    void showbutton(bool);
private:
    Ui::FriendAppy_Item *ui;
    std::shared_ptr<ApplyInfo> _apply_info;
    bool _added;
signals:
    void sig_auth_friend(std::shared_ptr<ApplyInfo> apply_info);
private slots:
    //void on_fa_add_pushButton_clicked();
    //void on_fa_add_pushButton_clicked();
};

#endif // FRIENDAPPY_ITEM_H
