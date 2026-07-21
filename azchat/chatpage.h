#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include "userinfo.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
    virtual void paintEvent(QPaintEvent *event) override;
    void SetUserInfo(std::shared_ptr<UserInfo>);
    void AppendChatMsg(std::shared_ptr<TextChatData> msg);

signals:
    void sig_append_send_chat_msg(std::shared_ptr<TextChatData>);

private slots:
    void on_send_pushButton_clicked();
//    void slot_append_send_chat_msg(std::shared_ptr<TextChatData>);
private:
    Ui::ChatPage *ui;

    std::shared_ptr<UserInfo> _userinfo{nullptr};
};

#endif // CHATPAGE_H
