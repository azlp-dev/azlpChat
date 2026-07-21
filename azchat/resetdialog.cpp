#include "resetdialog.h"
#include "ui_resetdialog.h"
#include "httpmgr.h"

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);
    ui->newpass_lineEdit->setEchoMode(QLineEdit::Password);
    connect(HttpMgr::Get_instance().get(),&HttpMgr::sig_Reset_finish
            ,this,&ResetDialog::slot_reset_finished);
    Checkconnect();
    initHandler();
}

ResetDialog::~ResetDialog()
{
    delete ui;
}


void ResetDialog::showTip(QString str,bool is_ok){
    if(is_ok){
        ui->tips_label->setProperty("state","normal");
    }else{
        ui->tips_label->setProperty("state","err");

    }
    ui->tips_label->setText(str);
    polish(ui->tips_label);
}
void ResetDialog::addErrTip(ErrTips eid,const QString& tip){
    _tips_map[eid] = tip;
    showTip(tip,false);
}

void ResetDialog::DelErrtip(ErrTips eid){
    _tips_map.remove(eid);
    if(_tips_map.empty()){
        showTip("No error",true);
        return;
    }
    showTip(_tips_map.first(),false);
    return;
}
void ResetDialog::checkUserEdit(){
    if(ui->user_lineEdit->text() == ""){
        addErrTip(ErrTips::UserNameErr,"用户名不可为空");
        return;
    }
    DelErrtip(ErrTips::UserNameErr);
}
void ResetDialog::checkEmailEdit(){
    auto email = ui->email_lineEdit->text();
    QRegularExpression regex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");

    bool match = regex.match(email).hasMatch();
    if(!match){
        addErrTip(ErrTips::EmailErr,"请输入正确的邮箱");
        return;
    }
    DelErrtip(ErrTips::EmailErr);
}
void ResetDialog::checkCodeEdit(){
    if(ui->code_lineEdit->text() == ""){
        addErrTip(ErrTips::CodeErr,"请输入验证码");
        return;
    }
    DelErrtip(ErrTips::CodeErr);
}
void ResetDialog::checkNewPassEdit(){
    auto pass = ui->newpass_lineEdit->text();
    QRegularExpression regx(R"(^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)[A-Za-z\d]{8,20}$)");
    bool match = regx.match(pass).hasMatch();
    if(!match){
        addErrTip(ErrTips::PassErr,"请输入格式为8-20位包含大小写字母和数字的密码");
        return;
    }
    DelErrtip(ErrTips::PassErr);
}
void ResetDialog::Checkconnect(){
    connect(ui->user_lineEdit,&QLineEdit::editingFinished,[this](){
        checkUserEdit();
    });
    connect(ui->email_lineEdit,&QLineEdit::editingFinished,[this](){
        checkEmailEdit();
    });
    connect(ui->code_lineEdit,&QLineEdit::editingFinished,[this](){
        checkCodeEdit();
    });
    connect(ui->newpass_lineEdit,&QLineEdit::editingFinished,[this](){
        checkNewPassEdit();
    });
}

void ResetDialog::initHandler()
{
    _handlers.insert(Reqids::ID_GET_VARIFY_CODE,[this](const QJsonObject& json_obj){
        int errid = json_obj["error"].toInt();
        if(errid!=Errids::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = json_obj["email"].toString();
        showTip(tr("验证码已发送，请注意查收"),true);
        //qDebug()<<"email: "<<email<<Qt::endl;
    });

    _handlers.insert(Reqids::ID_PassWord_Reset,[this](const QJsonObject& json_obj){
        int errid = json_obj["error"].toInt();
        if(errid != Errids::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        else{
            showTip(tr("重置成功，点击返回"),true);
            return;
        }
    });
}

void ResetDialog::on_code_pushButton_clicked()
{
    auto email = ui->email_lineEdit->text();
    QRegularExpression regex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");

    bool match = regex.match(email).hasMatch();
    if(match){
        QJsonObject obj;
        obj["email"] = email;
        //qDebug()<<email<<Qt::endl;
        //qDebug()<<"即将发送请求"<<Qt::endl;
        HttpMgr::Get_instance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode")
                                             ,obj,Moudles::RESET_MOD,Reqids::ID_GET_VARIFY_CODE);

    }else{
        showTip(tr("邮箱格式不正确 请重新输入"),false);
    }
}

void ResetDialog::slot_reset_finished(Reqids id, QString result, Errids errid){
    //qDebug()<<"收到重置密码模块回包"<<Qt::endl;
    if(errid != SUCCESS){
        showTip(tr("网络请求失败"),false);
        return;
    }
    //qDebug()<<result<<Qt::endl;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(result.toUtf8());
    if(jsonDoc.isNull()){
        showTip(tr("json解析失败"),false);
        return;
    }
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }
    _handlers[id](jsonDoc.object());
    return;

}

void ResetDialog::on_cancel_pushButton_clicked()
{
    emit switchLogin();
    return;
}


void ResetDialog::on_yes_pushButton_clicked()
{
    //qDebug()<<"标记3"<<Qt::endl;
    if(!_tips_map.empty()){
        //qDebug()<<"标记1"<<Qt::endl;
        return;
    }
    QJsonObject jsObj;
    jsObj["user"] = ui->user_lineEdit->text();
    jsObj["email"] = ui->email_lineEdit->text();
    jsObj["code"] = ui->code_lineEdit->text();
    jsObj["newpass"] = Enc_sha256(ui->newpass_lineEdit->text());
    //qDebug()<<"标记2"<<Qt::endl;
    HttpMgr::Get_instance()->PostHttpReq(QUrl(gate_url_prefix+"/reset_pwd"),jsObj,
                                         Moudles::RESET_MOD,Reqids::ID_PassWord_Reset);
}

