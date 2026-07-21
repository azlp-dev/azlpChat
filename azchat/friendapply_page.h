#ifndef FRIENDAPPLY_PAGE_H
#define FRIENDAPPLY_PAGE_H

#include <QWidget>
#include "userinfo.h"

namespace Ui {
class FriendApply_Page;
}

class FriendAppy_Item;

class FriendApply_Page : public QWidget
{
    Q_OBJECT

public:
    explicit FriendApply_Page(QWidget *parent = nullptr);
    ~FriendApply_Page();
    void AddNewApply(std::shared_ptr<AddFriendApply> apply);

protected:
    //void paintEvent(QPaintEvent *event);

private:
    void loadApplyList();
    std::unordered_map<int, FriendAppy_Item *> _unauth_items;
public slots:
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);
signals:
    void sig_show_search(bool);

private:
    Ui::FriendApply_Page *ui;
};

#endif // FRIENDAPPLY_PAGE_H
