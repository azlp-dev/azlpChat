#include "httpmgr.h"

HttpMgr::~HttpMgr(){


}

HttpMgr::HttpMgr()
{
    connect(this,&HttpMgr::sig_http_finish,this,&HttpMgr::slot_http_finish);
}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, Moudles mod, Reqids id)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(data.length()));
    auto self = shared_from_this();
    QNetworkReply* reply = _manager.post(request,data);
    connect(reply,&QNetworkReply::finished,[self,reply,mod,id](){
        if(reply->error()!=QNetworkReply::NoError){
            //qDebug()<<reply->errorString()<<Qt::endl;
            emit self->sig_http_finish(id,"",mod,ERR_NETWORK);
            reply->deleteLater();
            return;
        }
        //qDebug()<<"请求收到回复"<<Qt::endl;
        emit self->sig_http_finish(id,reply->readAll(),mod,SUCCESS);
        reply->deleteLater();
    });

}

void HttpMgr::slot_http_finish(Reqids reqid, QString result, Moudles mod, Errids errid)
{
    if(mod == REGISTER_MOD){
        emit sig_reg_finish(reqid,result,errid);
    }
    if(mod == Moudles::RESET_MOD){
        emit sig_Reset_finish(reqid,result,errid);
    }
    if(mod == Moudles::LOGIN_MOD){
        emit sig_Login_finish(reqid,result,errid);
    }
}


