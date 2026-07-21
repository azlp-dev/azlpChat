#ifndef HTTPMGR_H
#define HTTPMGR_H

#include "singleton.h"
#include <QNetworkAccessManager>
#include <QString>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>

class HttpMgr:public QObject,public Singleton<HttpMgr>,public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
    friend class Singleton<HttpMgr>;
public:
    ~HttpMgr();
    void PostHttpReq(QUrl url,QJsonObject json,Moudles mod,Reqids id);

private:
    HttpMgr();

    QNetworkAccessManager _manager;

signals:
    void sig_http_finish(Reqids reqid,QString result,Moudles mod,Errids errid);

    void sig_reg_finish(Reqids reqid,QString result,Errids errid);
    void sig_Reset_finish(Reqids reqid,QString result,Errids errid);
    void sig_Login_finish(Reqids reqid,QString result,Errids errid);

public slots:
    void slot_http_finish(Reqids reqid,QString result,Moudles mod,Errids errid);

};

#endif // HTTPMGR_H
