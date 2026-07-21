#include "tcpmgr.h"
#include "usermgr.h"
#include <QJsonArray>
#include <fstream>
TcpMgr::TcpMgr()
    :_host(""),_port(""),_b_reading(false),_msg_id(0),_msg_len(0),_sock(new QTcpSocket(this))
{
    //auto self = shared_from_this();
    QObject::connect(_sock,&QTcpSocket::connected,this,[this](){
        //qDebug()<<"tcp 连接成功"<<Qt::endl;
        emit sig_tcpcon_success(true);
    });
    QObject::connect(_sock,&QTcpSocket::readyRead,this,[this](){
        _buffer.append(_sock->readAll());
        QDataStream stream(&_buffer,QDataStream::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_0);
        forever{
            if(!_b_reading){
                if(_buffer.size()<static_cast<int>(sizeof(quint16)*2)){
                    return;
                }
                stream>>_msg_id>>_msg_len;
                qDebug()<<"msg id:"<<_msg_id<<" msg_len:"<<_msg_len<<Qt::endl;
                _buffer = _buffer.mid(sizeof(quint16)*2);
            }
            if(_buffer.length()<_msg_len){
                _b_reading = true;
                return;
            }else{
                _b_reading = false;
                QByteArray msgbody = _buffer.mid(0,_msg_len);
                handleMsg(Reqids(_msg_id),_msg_len,msgbody);
                _buffer = _buffer.mid(_msg_len);
                qDebug()<<_buffer.data()<<Qt::endl;
                return;
            }

        }
    });
    QObject::connect(_sock,&QTcpSocket::errorOccurred,this,[this](QAbstractSocket::SocketError socketError){
        qDebug()<<"TcpSock error:"<<_sock->errorString()<<Qt::endl;
        switch (socketError) {
        case QAbstractSocket::ConnectionRefusedError:
            qDebug() << "Connection Refused!";
            emit sig_tcpcon_success(false);
            break;

        case QAbstractSocket::RemoteHostClosedError:
            qDebug() << "Remote Host Closed!";
            break;

        case QAbstractSocket::HostNotFoundError:
            qDebug() << "Host Not Found!";
            emit sig_tcpcon_success(false);
            break;

        case QAbstractSocket::SocketTimeoutError:
            qDebug() << "Connection Timeout!";
            emit sig_tcpcon_success(false);
            break;

        case QAbstractSocket::NetworkError:
            qDebug() << "Network Error!";
            break;

        default:
            qDebug() << "Other Error!";
            break;
        }
    });
    QObject::connect(_sock,&QTcpSocket::disconnected,this,[](){
        qDebug()<<"server closed"<<Qt::endl;
    });
    QObject::connect(this,&TcpMgr::sig_send_data,this,&TcpMgr::slot_send_data);
    initHandlers();
}

void TcpMgr::initHandlers()
{
    _handlers.insert(Reqids::ID_CHAT_LOGIN_RSP,[this](Reqids id,int len,QByteArray data){
        Q_UNUSED(len);
        QJsonDocument jsdoc = QJsonDocument::fromJson(data);
        if(jsdoc.isNull()){
            qDebug()<<"QJsonDocument 创建失败"<<Qt::endl;
            return;
        }
        QJsonObject jsobj = jsdoc.object();
        if(!jsobj.contains("error")){
            qDebug()<<"返回数据缺失error字段"<<Qt::endl;
            emit sig_login_failed(Errids::ERR_JSON);
            return;
        }
        Errids error = (Errids)(jsobj["error"].toInt());
        if(error!=Errids::SUCCESS){
            qDebug()<<"登录失败："<<error<<Qt::endl;
            emit sig_login_failed(error);
            return;
        }
        //
        //qDebug()<<"test:"<<jsobj["uid"].toInt()<<" token"<<jsobj["token"].toString()<<Qt::endl;
        //qDebug()<<"label5: login success,client received rsp"<<Qt::endl;
        auto uid = jsobj["uid"].toInt();
        auto name = jsobj["name"].toString();
        auto nick = jsobj["nick"].toString();
        auto icon = jsobj["icon"].toString();
        auto sex = jsobj["sex"].toInt();
        auto desc = jsobj["desc"].toString();
        auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex,"",desc);

        UserMgr::Get_instance()->SetUserInfo(user_info);
        UserMgr::Get_instance()->setutoken(jsobj["token"].toString());
        if(jsobj.contains("apply_list")){
            //qDebug()<<"*debug1"<<Qt::endl;
            UserMgr::Get_instance()->AppendApplyList(jsobj["apply_list"].toArray());
            //emit sig_refresh_friendapply();
        }
        if(jsobj.contains("friend_list")){

            UserMgr::Get_instance()->AppendFriendList(jsobj["friend_list"].toArray());
        }


        emit switchChatDlg();
    });
    _handlers.insert(Reqids::ID_SEARCH_USER_RSP,[this](Reqids id,int len,QByteArray data){
        Q_UNUSED(len);
        //qDebug()<<"debug2"<<Qt::endl;
        QJsonDocument jsdoc = QJsonDocument::fromJson(data);
        if(jsdoc.isNull()){
            qDebug()<<"QJsonDocument 创建失败"<<Qt::endl;
            emit sig_user_search(nullptr);
            return;
        }
        QJsonObject jsobj = jsdoc.object();
        if(!jsobj.contains("error")){
            qDebug()<<"返回数据缺失error字段"<<Qt::endl;
            emit sig_user_search(nullptr);
            return;
        }
        Errids error = (Errids)(jsobj["error"].toInt());
        if(error!=Errids::SUCCESS){
            qDebug()<<"搜索失败"<<error<<Qt::endl;
            emit sig_user_search(nullptr);
            return;
        }
        //
        //nt uid,int sex,QString name,QString nick,QString desc,QString icon
        auto searchinfo = std::make_shared<SearchInfo>(jsobj["uid"].toInt(),jsobj["sex"].toInt(),
                                                       jsobj["name"].toString(),jsobj["nick"].toString(),jsobj["desc"].toString(),jsobj["icon"].toString());
        emit sig_user_search(searchinfo);
    });
    _handlers.insert(Reqids::ID_NOTIFY_ADD_FRIEND_REQ,[this](Reqids id,int len,QByteArray data){
        Q_UNUSED(len);
        //qDebug()<<"debug2"<<Qt::endl;
        QJsonDocument jsdoc = QJsonDocument::fromJson(data);
        if(jsdoc.isNull()){
            qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ"<<Qt::endl;
            //emit sig_friend_apply(nullptr);
            return;
        }
        QJsonObject jsobj = jsdoc.object();
        if(!jsobj.contains("error")){
            qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ返回数据缺失error字段"<<Qt::endl;
            //emit sig_friend_apply(nullptr);
            return;
        }
        Errids error = (Errids)(jsobj["error"].toInt());
        if(error!=Errids::SUCCESS){
            qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ请求失败"<<error<<Qt::endl;
            //emit sig_friend_apply(nullptr);
            return;
        }
        //
        //nt uid,int sex,QString name,QString nick,QString desc,QString icon
        //qDebug()<<"receive notify"<<Qt::endl;
        int from_uid = jsobj["applyuid"].toInt();
        QString name = jsobj["name"].toString();
        QString desc = jsobj["desc"].toString();
        QString icon = jsobj["icon"].toString();
        QString nick = jsobj["nick"].toString();
        int sex = jsobj["sex"].toInt();
        auto apply_info = std::make_shared<AddFriendApply>(
            from_uid, name, desc,
            icon, nick, sex);

        emit sig_friend_apply(apply_info);
    });
    _handlers.insert(Reqids::ID_NOTIFY_AUTH_FRIEND_REQ,[this](Reqids id,int len,QByteArray data){
        Q_UNUSED(len);
        std::ofstream log("debug.log", std::ios::app);

        log << "receive notify_auth"
            << std::endl;
        QJsonDocument jsdoc = QJsonDocument::fromJson(data);
        if(jsdoc.isNull()){
            qDebug()<<"QJsonDocument 创建失败"<<Qt::endl;
            return;
        }
        QJsonObject jsobj = jsdoc.object();
        if(!jsobj.contains("error")){
            qDebug()<<"返回数据缺失error字段"<<Qt::endl;
            return;
        }
        Errids error = (Errids)(jsobj["error"].toInt());
        if(error!=Errids::SUCCESS){
            return;
        }
        int fromuid = jsobj["fromuid"].toInt();
        QString name = jsobj["name"].toString();
        QString nick = jsobj["nick"].toString();
        QString icon = jsobj["icon"].toString();
        int sex = jsobj["sex"].toInt();
        auto authinfo = std::make_shared<AuthInfo>(fromuid,name,nick,icon,sex);
        emit sig_add_auth_friend(authinfo);
    });

    _handlers.insert(Reqids::ID_AUTH_FRIEND_RSP,[this](Reqids id,int len,QByteArray data){
        Q_UNUSED(len);
        qDebug()<<"receive auth rsp"<<Qt::endl;
        QJsonDocument jsdoc = QJsonDocument::fromJson(data);
        if(jsdoc.isNull()){
            qDebug()<<"QJsonDocument 创建失败"<<Qt::endl;
            return;
        }
        QJsonObject jsobj = jsdoc.object();
        if(!jsobj.contains("error")){
            qDebug()<<"返回数据缺失error字段"<<Qt::endl;
            return;
        }
        Errids error = (Errids)(jsobj["error"].toInt());
        if(error!=Errids::SUCCESS){
            return;
        }
        auto name = jsobj["name"].toString();
        auto nick = jsobj["nick"].toString();
        auto icon = jsobj["icon"].toString();
        auto sex = jsobj["sex"].toInt();
        auto uid = jsobj["uid"].toInt();
        auto authrsp = std::make_shared<AuthRsp>(uid,name,nick,icon,sex);
        emit sig_auth_rsp(authrsp);
        //qDebug()<<"send sig_auth_rsp"<<Qt::endl;
    });

    _handlers.insert(ID_TEXT_CHAT_MSG_RSP, [this](Reqids id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = Errids::ERR_JSON;
            qDebug() << "Chat Msg Rsp Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != Errids::SUCCESS) {
            qDebug() << "Chat Msg Rsp Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Rsp Success " ;
        //ui设置送达等标记 todo...
    });

    _handlers.insert(ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](Reqids id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = Errids::ERR_JSON;
            qDebug() << "Notify Chat Msg Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != Errids::SUCCESS) {
            qDebug() << "Notify Chat Msg Failed, err is " << err;
            return;
        }

        //qDebug() << "Receive Text Chat Notify Success " ;
        auto msg_ptr = std::make_shared<TextChatMsg>(jsonObj["fromuid"].toInt(),
                                                     jsonObj["touid"].toInt(),jsonObj["text_array"].toArray());
        emit sig_text_chat_msg(msg_ptr);
    });
    _handlers.insert(Reqids::ID_ADD_FRIEND_RSP, [this](Reqids id, int len, QByteArray data){
        //UserMgr::Get_instance()->AddFriend()
    });
}

void TcpMgr::handleMsg(Reqids id, int len, QByteArray data)
{
    auto find_iter = _handlers.find(id);
    if(find_iter == _handlers.end()){
        qDebug()<<"unkonwn reqiid"<<Qt::endl;
        return;
    }
    find_iter.value()(id,len,data);
}

void TcpMgr::slot_tcp_con(ServerInfo si)
{
    auto host = si.host;
    quint16 port = static_cast<quint16>(si.port.toUInt());
    qDebug() << "host =" << host;
    qDebug() << "port =" << port;


    _sock->connectToHost(host,port);

}

void TcpMgr::slot_send_data(Reqids rid, QByteArray data)
{
    quint16 id = rid;
    //qDebug()<<"label2: the client successfully connect chatserver now send msg "<<rid<<Qt::endl;
    quint16 len = static_cast<quint16>(data.size());

    QByteArray senddata;
    QDataStream out(&senddata,QDataStream::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out<<id<<len;
    senddata.append(data);
    _sock->write(senddata);
    //qDebug()<<"tick1"<<Qt::endl;
}
