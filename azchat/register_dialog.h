#ifndef REGISTER_DIALOG_H
#define REGISTER_DIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class Register_Dialog;
}

class Register_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Register_Dialog(QWidget *parent = nullptr);
    void showTip(QString,bool);
    ~Register_Dialog();

private slots:
    void code_pushButton_clicked();     //获取验证码按钮槽函数
    void slot_reg_finish(Reqids id,QString result,Errids errid); //处理注册请求的回包

    void on_reg_pushButton_clicked();      //确认按钮槽函数
    void ConnectErrTips();

    void addErrTip(ErrTips eid,const QString& tip);
    void DelErrtip(ErrTips eid);
    void checkUserEdit();
    void checkEmailEdit();
    void checkPassEdit();
    void checkConfirmEdit();

    void regsuccess_page();    //注册成功返回登录页

    void on_pushButton_clicked();        //点击提前返回登录页

    void on_cancle_pushButton_clicked();     //取消注册按钮槽函数

signals:
    void switchLogin(void);

private:
    void InitHandlers();                //在这里完善回包处理逻辑

    Ui::Register_Dialog *ui;
    QMap<Reqids,std::function<void(const QJsonObject&)>> _handlers;
    QMap<ErrTips,QString> _tips_map;
    QTimer* _timer;
    int _countdown;
};

#endif // REGISTER_DIALOG_H
