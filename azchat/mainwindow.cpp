#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tcpmgr.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _login_dlg = new LoginDialog();
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);
    connect(_login_dlg,&LoginDialog::switchRegister,this,&MainWindow::slotSwitchRegister);
    connect(_login_dlg,&LoginDialog::switchReset,this,&MainWindow::slotSwitchReset);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::sig_switch_chatdlg,this,&MainWindow::slotSwitchChatDlg);
    connect(TcpMgr::Get_instance().get(),&TcpMgr::switchChatDlg,this,&MainWindow::slotSwitchChatDlg);
    //slotSwitchChatDlg();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotSwitchRegister(){
    _reg_dlg = new Register_Dialog(this);
    _reg_dlg->hide();
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    connect(_reg_dlg,&Register_Dialog::switchLogin,this,&MainWindow::slotSwitchLogin);
    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();
}

void MainWindow::slotSwitchLogin(){
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);
    _reg_dlg->hide();
    _login_dlg->show();
    //连接登录界面注册信号
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::slotSwitchRegister);
    //连接登录界面忘记密码信号
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::slotSwitchReset);
}

void MainWindow::slotSwitchReset(){
    _reset_dlg = new ResetDialog(this);
    _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_reset_dlg);
    _login_dlg->hide();
    _reset_dlg->show();
    connect(_reset_dlg,&ResetDialog::switchLogin,this,&MainWindow::slotSwitchLogin2);
}

void MainWindow::slotSwitchLogin2(){
    _login_dlg = new LoginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);
    _reset_dlg->hide();
    _login_dlg->show();
    //连接登录界面注册信号
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::slotSwitchRegister);
    //连接登录界面忘记密码信号
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::slotSwitchReset);

}

void MainWindow::slotSwitchChatDlg()
{
    _chat_dlg = new ChatDialog();
    _chat_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_chat_dlg);
    _chat_dlg->show();
    _login_dlg->hide();
    this->setMinimumSize(QSize(1050,900));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}



