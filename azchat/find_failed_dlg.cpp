#include "find_failed_dlg.h"
#include "ui_find_failed_dlg.h"

find_failed_dlg::find_failed_dlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::find_failed_dlg)
{
    ui->setupUi(this);
    this->setObjectName("Find_Failed_Dlg");
    this->setWindowTitle("查找失败");
    this->setWindowFlag(Qt::FramelessWindowHint);
    ui->ff_sure_pushButton->SetState("normal","hover","press");
    this->setModal(true);
}

find_failed_dlg::~find_failed_dlg()
{
    delete ui;
}

void find_failed_dlg::on_ff_sure_pushButton_clicked()
{
    this->hide();
}

