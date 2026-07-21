#include "logindialog.h"
#include "ui_logindialog.h"
#include <QPainter>
#include <QPainterPath>
#include "httpmgr.h"
#include <QJsonDocument>
#include "tcpmgr.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->reg_pushButton,&QPushButton::clicked,this,&LoginDialog::switchRegister);
    connect(HttpMgr::Get_instance().get(),&HttpMgr::sig_Login_finish,this,&LoginDialog::slot_Login_finished);
    connect(this,&LoginDialog::sig_con_server,TcpMgr::Get_instance().get(),&TcpMgr::slot_tcp_con);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_tcpcon_success,this,&LoginDialog::slot_tcpcon_success);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_login_failed,this,&LoginDialog::slot_login_failed);
    initHead();
    initHandler();
    connectCheck();
}

LoginDialog::~LoginDialog()
{
    //qDebug()<<"logindlg destructed"<<Qt::endl;
    delete ui;
}

void LoginDialog::initHead(){
    //qDebug()<<ui->pixmap_label->size()<<ui->pixmap_label->pixmap().size()<<Qt::endl;

    QPixmap originalpixmap(":/res/head_1.jpg");
    originalpixmap = originalpixmap.scaled(ui->pixmap_label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);

    QPixmap roundpix(originalpixmap.size());
    roundpix.fill(Qt::transparent);
    QPainter painter(&roundpix);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPainterPath path;
    path.addRoundedRect(0,0,roundpix.width(),roundpix.height(),10,10);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, originalpixmap);

    // 设置绘制好的圆角图片到QLabel上
    ui->pixmap_label->setPixmap(roundpix);
    // qDebug() << "label size:"
    //          << ui->pixmap_label->size();

    // qDebug() << "pixmap size:"
    //          << originalpixmap.size();

    // qDebug() << "roundpix size:"
    //          << roundpix.size();
    // qDebug() << ui->pixmap_label->pixmap().size();
    ui->pixmap_label->setFixedSize(roundpix.size());
}

void LoginDialog::on_reset_pushButton_clicked()
{
    //qDebug()<<"slot forget pwd";
    emit switchReset();
}

void LoginDialog::slot_Login_finished(Reqids reqid,QString result,Errids errid)
{
    if(errid != Errids::SUCCESS){
        //qDebug()<<"标记1"<<Qt::endl;
        showTip("参数错误",false);
        return;
    }
    QJsonDocument jsdoc = QJsonDocument::fromJson(result.toUtf8());
    if(jsdoc.isNull()){
        //qDebug()<<"标记2"<<Qt::endl;
        showTip("参数错误",false);
        return;
    }
    if(!jsdoc.isObject()){
        //qDebug()<<"标记3"<<Qt::endl;
        showTip("参数错误",false);
        return;
    }
    _handlers[reqid](jsdoc.object());
    return;
}

void LoginDialog::slot_tcpcon_success(bool success)
{
    if(success){
        showTip("聊天服务器连接成功",true);
        QJsonObject jsobj;
        jsobj["uid"] = _uid;
        jsobj["token"] = _token;
        QJsonDocument doc(jsobj);
        QByteArray data = doc.toJson(QJsonDocument::Compact);
        emit TcpMgr::Get_instance()->sig_send_data(Reqids::ID_CHAT_LOGIN,data);


    }else{
        showTip("网络异常",false);
        return;
    }

}

void LoginDialog::slot_login_failed(Errids eid)
{
    showTip("登录服务器失败",false);
    //qDebug()<<"err in login to server"<<eid<<Qt::endl;
}

void LoginDialog::showTip(QString str,bool is_ok){
    if(is_ok){
        ui->tips_label->setProperty("state","normal");
    }else{
        ui->tips_label->setProperty("state","err");

    }

    ui->tips_label->setText(str);
    polish(ui->tips_label);
}

void LoginDialog::addErrTip(ErrTips eid,const QString& tip){
    _tips_map[eid] = tip;
    showTip(tip,false);
}

void LoginDialog::DelErrtip(ErrTips eid){
    _tips_map.remove(eid);
    if(_tips_map.empty()){
        showTip("No error",true);
        return;
    }
    showTip(_tips_map.first(),false);
    return;
}

void LoginDialog::checkUserEdit(){
    if(ui->user_lineEdit->text() == ""){
        addErrTip(ErrTips::UserNameErr,"用户名不可为空");
        return;
    }
    DelErrtip(ErrTips::UserNameErr);
}

void LoginDialog::checkPassEdit(){
    if(ui->pass_lineEdit->text() == ""){
        addErrTip(ErrTips::PassErr,"密码不可为空");
        return;
    }
    DelErrtip(ErrTips::PassErr);
}

void LoginDialog::connectCheck()
{
    connect(ui->user_lineEdit,&QLineEdit::editingFinished,[this](){
        checkUserEdit();
    });
    connect(ui->pass_lineEdit,&QLineEdit::editingFinished,[this](){
        checkPassEdit();
    });
}

void LoginDialog::initHandler()
{
    _handlers.insert(Reqids::ID_USER_LOGIN,[this](QJsonObject obj){
        if(obj["error"]!=Errids::SUCCESS){
            showTip("参数错误",false);
            //qDebug()<<"标记4"<<Qt::endl;

            return;
        }
        showTip("登录成功",true);
        auto user = obj["user"];
        ServerInfo si;
        si.host = obj["host"].toString();
        si.port = obj["port"].toString();
        si.uid = obj["uid"].toInt();
        si.token = obj["token"].toString();

        _uid = obj["uid"].toInt();
        _token = obj["token"].toString();
        emit sig_con_server(si);
        return;
    });
}

void LoginDialog::on_loginButton_clicked()
{
    if(!_tips_map.empty()){
        return;
    }else{
        if(ui->user_lineEdit->text() == ""){
            return;
        }
        if(ui->pass_lineEdit->text() == ""){
            return;
        }
        QJsonObject jsobj;
        jsobj["user"] = ui->user_lineEdit->text();
        jsobj["pass"] = Enc_sha256(ui->pass_lineEdit->text());
        HttpMgr::Get_instance()->
            PostHttpReq(QUrl(gate_url_prefix+"/userlogin"),jsobj,Moudles::LOGIN_MOD,Reqids::ID_USER_LOGIN);
    }
}

