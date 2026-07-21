#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include <QWidget>
#include "itembase.h"
#include "userinfo.h"
namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid();

    virtual QSize sizeHint() const override{
        return QSize(250,70);
    }

    void setInfo(QString head,QString name,QString msg);
    void setInfo(std::shared_ptr<UserInfo>);
    std::shared_ptr<UserInfo> GetUserInfo(){
        return _friend_info;
    }
    void updateLastMsg(std::vector<std::shared_ptr<TextChatData>>);
private:
    Ui::ChatUserWid *ui;

    QString _name;
    QString _head;
    QString _msg;
    std::shared_ptr<UserInfo> _friend_info;
};

#endif // CHATUSERWID_H
