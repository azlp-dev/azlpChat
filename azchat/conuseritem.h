#ifndef CONUSERITEM_H
#define CONUSERITEM_H

#include <QWidget>
#include "itembase.h"
#include "userinfo.h"

namespace Ui {
class ConUserItem;
}

class ConUserItem : public ItemBase
{
    Q_OBJECT

public:
    explicit ConUserItem(QWidget *parent = nullptr);
    ~ConUserItem();
    void showRedPoint(bool);
    void SetInfo(std::shared_ptr<AuthInfo> auth_info);
    void SetInfo(std::shared_ptr<AuthRsp> auth_rsp);
    void SetInfo(std::shared_ptr<UserInfo> info);
    void SetInfo(int uid, QString name, QString icon);
    std::shared_ptr<UserInfo> GetInfo(){
        return _info;
    }
    virtual QSize sizeHint() const;

private:
    Ui::ConUserItem *ui;
    std::shared_ptr<UserInfo> _info;
};

#endif // CONUSERITEM_H
