#ifndef TCPMGR_H
#define TCPMGR_H
#include "global.h"
#include "singleton.h"
#include <QObject>
#include "userinfo.h"

class TcpMgr:public QObject, public Singleton<TcpMgr>
    ,public std::enable_shared_from_this<TcpMgr>
{
    friend class Singleton<TcpMgr>;
    Q_OBJECT
public:
    void handleMsg(Reqids,int len,QByteArray);

signals:
    void sig_tcpcon_success(bool success);
    void sig_switch_chatdlg();
    void sig_login_failed(Errids);
    void sig_send_data(Reqids id,QByteArray data);
    void switchChatDlg();
    void sig_user_search(std::shared_ptr<SearchInfo>);
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
    void sig_friend_apply(std::shared_ptr<AddFriendApply>);
    void sig_refresh_friendapply();
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);
    void sig_text_chat_msg(std::shared_ptr<TextChatMsg>);

public slots:
    void slot_tcp_con(ServerInfo);
    void slot_send_data(Reqids rid,QByteArray data);

private:
    TcpMgr();
    void initHandlers();

    QTcpSocket* _sock;  //一个客户端只需要一个socket连接chatserver即可
    QByteArray _buffer;
    bool _b_reading;
    quint16 _msg_id;
    quint16 _msg_len;
    QMap<Reqids,std::function<void(Reqids,int len,QByteArray)>> _handlers;

    QString _host;
    QString _port;
};

#endif // TCPMGR_H
