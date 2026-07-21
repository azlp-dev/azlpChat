#include "find_success_dlg.h"
#include "ui_find_success_dlg.h"
#include "addfriend_dlg.h"

Find_Success_Dlg::Find_Success_Dlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Find_Success_Dlg)
{
    ui->setupUi(this);
    setWindowTitle("添加");
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    // 获取当前应用程序的路径
    QPixmap head_pix(":/res/head_1.jpg");
    head_pix = head_pix.scaled(ui->fs_head_label->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->fs_head_label->setPixmap(head_pix);
    ui->pushButton->SetState("normal","hover","press");
    this->setModal(true);

}

Find_Success_Dlg::~Find_Success_Dlg()
{
    delete ui;
}

void Find_Success_Dlg::setSearchInfo(std::shared_ptr<SearchInfo> si)
{
    ui->fs_name_label->setText(si->name);
    if(si == nullptr){
        //qDebug()<<"error null searchinfo"<<Qt::endl;
    }
    _si = si;
}

void Find_Success_Dlg::on_pushButton_clicked()
{
    //qDebug()<<"find_dlg pushbutton"<<Qt::endl;
    AddFriend_Dlg* add = new AddFriend_Dlg(this);
    add->setModal(true);
    add->SetSearchInfo(_si);
    add->show();
    connect(add,&AddFriend_Dlg::sig_delete_findSuccess,[this](){
        this->hide();
        //this->deleteLater();
    });
}

