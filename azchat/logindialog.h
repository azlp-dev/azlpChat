#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

signals:
    void switchRegister();
    void switchReset();
    void sig_con_server(ServerInfo);

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();


private slots:
    void on_reset_pushButton_clicked();
    void slot_Login_finished(Reqids reqid,QString result,Errids errid);
    void slot_tcpcon_success(bool);
    void slot_login_failed(Errids);

    void on_loginButton_clicked();

private:
    void initHead();
    void showTip(QString str,bool is_ok);
    void addErrTip(ErrTips eid,const QString& tip);
    void DelErrtip(ErrTips eid);
    void checkUserEdit();
    //void checkEmailEdit();
    void checkPassEdit();
    void connectCheck();
    void initHandler();


    int _uid;
    QString _token;
    QMap<ErrTips,QString> _tips_map;
    Ui::LoginDialog *ui;
    QMap<Reqids,std::function<void(const QJsonObject&)>> _handlers;
};

#endif // LOGINDIALOG_H
