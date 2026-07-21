#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H
#include <QListWidget>

class ChatUserList:public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget* parent = nullptr);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void sig_load_chatUser();

private:

};

#endif // CHATUSERLIST_H
