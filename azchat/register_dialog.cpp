#include "register_dialog.h"
#include "ui_register_dialog.h"
#include <QRegularExpression>
#include "httpmgr.h"
#include <QTimer>

Register_Dialog::Register_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Register_Dialog)
    ,_countdown(5)
{
    // qDebug()<<"register dialog create"<<this;
    ui->setupUi(this);
    ui->tips_label->setProperty("state","normal");
    polish(ui->widget);
    ConnectErrTips();
    connect(ui->pass_visiable_label,&ClickedLable::clicked,[this](){
        if(ui->pass_visiable_label->getCur() == ClickedLbState::Normal){
            ui->pass_lineEdit->setEchoMode(QLineEdit::Password);
        }else{
            ui->pass_lineEdit->setEchoMode(QLineEdit::Normal);
        }
    });
    connect(ui->confirm_visiable_label,&ClickedLable::clicked,[this](){
        if(ui->confirm_visiable_label->getCur() == ClickedLbState::Normal){
            ui->confirm_lineEdit->setEchoMode(QLineEdit::Password);
        }else{
            ui->confirm_lineEdit->setEchoMode(QLineEdit::Normal);
        }
    });
    connect(HttpMgr::Get_instance().get(),&HttpMgr::sig_reg_finish
            ,this,&Register_Dialog::slot_reg_finish);
    _timer = new QTimer(this);
    InitHandlers();

    connect(_timer,&QTimer::timeout,[this](){
        _countdown--;

        auto str =
            QString("将在%1秒后返回登录页，点击下方按钮直接返回")
                .arg(_countdown);

        if(_countdown == 0){
            _timer->stop();
            emit switchLogin();
            return;
        }
        ui->label_2->setText(str);;
    });
    //connect(ui->pushButton,&QPushButton::clicked,this,&Register_Dialog::on_pushButton_clicked);
    connect(ui->code_pushButton,&QPushButton::clicked,this,&Register_Dialog::code_pushButton_clicked);
}

Register_Dialog::~Register_Dialog()
{
    delete ui;
}

void Register_Dialog::code_pushButton_clicked()
{
    // static int send = 1;
    // qDebug()<<QString("这是第%1次触发获取按钮").arg(send++)<<Qt::endl;
    // qDebug()
    //     << "sender="
    //     << sender()
    //     << "objectName="
    //     << sender()->objectName();
    auto email = ui->email_lineEdit->text();
    QRegularExpression regex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");

    bool match = regex.match(email).hasMatch();
    if(match){
        QJsonObject obj;
        obj["email"] = email;
        //qDebug()<<email<<Qt::endl;
        HttpMgr::Get_instance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode")
                                             ,obj,Moudles::REGISTER_MOD,Reqids::ID_GET_VARIFY_CODE);

    }else{
        showTip(tr("邮箱格式不正确 请重新输入"),false);
    }
}

void Register_Dialog::slot_reg_finish(Reqids id, QString result, Errids errid)
{
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

void Register_Dialog::regsuccess_page(){
    _timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);

    _timer->start(1000);
}

void Register_Dialog::InitHandlers()
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
    _handlers.insert(Reqids::ID_REQUEST_REG_USER,[this](const QJsonObject& json_obj){
        int errid = json_obj["error"].toInt();
        if(errid!=Errids::SUCCESS){
            showTip("用户注册参数错误",false);
            return;
        }
        auto email = json_obj["email"].toString();
        //qDebug()<<email<<"用户注册成功"<<Qt::endl;
        //qDebug()<<json_obj["uid"].toString()<<Qt::endl;
        regsuccess_page();
        return;
    });
}

void Register_Dialog::showTip(QString str,bool is_ok){
    if(is_ok){
        ui->tips_label->setProperty("state","normal");
    }else{
        ui->tips_label->setProperty("state","err");

    }

    ui->tips_label->setText(str);
    polish(ui->tips_label);
}

void Register_Dialog::addErrTip(ErrTips eid,const QString& tip){
    _tips_map[eid] = tip;
    showTip(tip,false);
}

void Register_Dialog::DelErrtip(ErrTips eid){
    _tips_map.remove(eid);
    if(_tips_map.empty()){
        showTip("No error",true);
        return;
    }
    showTip(_tips_map.first(),false);
    return;
}

void Register_Dialog::checkUserEdit(){
    if(ui->user_lineEdit->text() == ""){
        addErrTip(ErrTips::UserNameErr,"用户名不可为空");
        return;
    }
    DelErrtip(ErrTips::UserNameErr);
}

void Register_Dialog::checkEmailEdit(){
    auto email = ui->email_lineEdit->text();
    QRegularExpression regex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");

    bool match = regex.match(email).hasMatch();
    if(!match){
        addErrTip(ErrTips::EmailErr,"请输入正确的邮箱");
        return;
    }
    DelErrtip(ErrTips::EmailErr);
}

void Register_Dialog::checkPassEdit(){
    auto pass = ui->pass_lineEdit->text();
    QRegularExpression regx(R"(^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)[A-Za-z\d]{8,20}$)");
    bool match = regx.match(pass).hasMatch();
    if(!match){
        addErrTip(ErrTips::PassErr,"请输入格式为8-20位包含大小写字母和数字的密码");
        return;
    }
    DelErrtip(ErrTips::PassErr);
}

void Register_Dialog::checkConfirmEdit(){
    if(ui->pass_lineEdit->text() != ui->confirm_lineEdit->text()){
        addErrTip(ErrTips::ConfirmErr,"密码不一致，请再次确认");
        return;
    }
    DelErrtip(ErrTips::ConfirmErr);
}

void Register_Dialog::ConnectErrTips(){
    connect(ui->user_lineEdit,&QLineEdit::editingFinished,[this](){
        checkUserEdit();
    });
    connect(ui->email_lineEdit,&QLineEdit::editingFinished,[this](){
        checkEmailEdit();
    });
    connect(ui->pass_lineEdit,&QLineEdit::editingFinished,[this](){
        checkPassEdit();
    });
    connect(ui->confirm_lineEdit,&QLineEdit::editingFinished,[this](){
        checkConfirmEdit();
    });


}

void Register_Dialog::on_reg_pushButton_clicked()
{
    if(ui->user_lineEdit->text() == ""){
        showTip("请输入正确的用户名",false);
        return;
    }
    auto email = ui->email_lineEdit->text();
    QRegularExpression regex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");

    bool match = regex.match(email).hasMatch();
    if(match == false){
        showTip("请输入正确的邮箱",false);
        return;
    }
    if(ui->pass_lineEdit->text() == ""){
        showTip("请设置密码",false);
        return;
    }
    if(ui->confirm_lineEdit->text() == ""){
        showTip("请确认密码",false);
        return;
    }
    if(ui->pass_lineEdit->text()!=ui->confirm_lineEdit->text()){
        showTip("密码与确认密码不一致",false);
        return;
    }
    if(ui->code_lineEdit->text()==""){
        showTip("请输入验证码",false);
        return;
    }
    if(!_tips_map.empty()){
        showTip(_tips_map.first(),false);
        return;
    }
    //showTip("注册成功",true);
    QJsonObject reginfo_json;
    reginfo_json["user"] = ui->user_lineEdit->text();
    reginfo_json["email"] = ui->email_lineEdit->text();
    reginfo_json["pass"] =  Enc_sha256(ui->pass_lineEdit->text());
    reginfo_json["confirm"] = Enc_sha256(ui->confirm_lineEdit->text());
    reginfo_json["varify_code"] = ui->code_lineEdit->text();
    HttpMgr::Get_instance()->PostHttpReq(QUrl(gate_url_prefix + "/user_register")
                                         ,reginfo_json,Moudles::REGISTER_MOD,Reqids::ID_REQUEST_REG_USER);
}


void Register_Dialog::on_pushButton_clicked()
{
    _timer->stop();
    emit switchLogin();
    return;
}


void Register_Dialog::on_cancle_pushButton_clicked()
{
    _timer->stop();
    emit switchLogin();
    return;
}

