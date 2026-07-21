#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include "userinfo.h"
#include <QListWidgetItem>
namespace Ui {
class ChatDialog;
}
class StateWidget;
class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void shwoSearch(bool b_search);
    void addChatUserList();
    void ClearLabelState(StateWidget* w);
    void AddLBGroup(StateWidget *lb)
    {
        _lb_list.push_back(lb);
    }
    void handlePress(QMouseEvent* event);
    void SetSelectedChatItem(int);
    void SetSelectedChatPage(int);
    void loadMoreCon();
    void slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
    void UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>>);

protected:
    bool eventFilter(QObject *, QEvent *) override;

public slots:
    void slot_load_chatUser();
    void slot_load_conuser();
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(QString str);
    void slot_turnto_contact();
    void slot_apply_friend(std::shared_ptr<AddFriendApply> apply);
    void slot_refresh_friendapply();
    void slot_add_auth_friend(std::shared_ptr<AuthInfo>);
    void slot_add_friend_authrsp(std::shared_ptr<AuthRsp>);
    void slot_jump_chat(std::shared_ptr<UserInfo>);
    void slot_friendinfo_page(std::shared_ptr<UserInfo>);
    void slot_item_clicked(QListWidgetItem* item);
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData>);

private:
    Ui::ChatDialog *ui;

    QMap<int,QListWidgetItem*> _chat_item_added;
    QWidget* _last_widget;
    bool _b_loading{false};
    ChatUIMode _state{ChatUIMode::ChatMode};
    ChatUIMode _mode{ChatUIMode::ChatMode};
    QList<StateWidget*> _lb_list;
    int _cur_chat_uid{0};

};

#endif // CHATDIALOG_H
