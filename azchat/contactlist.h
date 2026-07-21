#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QListWidget>
#include "userinfo.h"

class ConUserItem;

class ContactList : public QListWidget
{
    Q_OBJECT
public:
    ContactList(QWidget* parent = nullptr);
    void ShowRedPoint(bool bshow = true);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    void slot_item_clicked(QListWidgetItem *item);
    void slot_add_auth_firend(std::shared_ptr<AuthInfo>);
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);
signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page(std::shared_ptr<UserInfo>);

private:
    void addContactUserList();

private:
    ConUserItem* _add_friend_item;
    QListWidgetItem * _groupitem;
};

#endif // CONTACTLIST_H
