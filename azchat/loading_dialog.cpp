#include "loading_dialog.h"
#include "ui_loading_dialog.h"
#include <QMovie>

Loading_Dialog::Loading_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Loading_Dialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::SubWindow|Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground);
    int max = parent->size().height()>parent->size().width()?parent->size().height():parent->size().width();
    this->setFixedSize(QSize(max,max)/5);

    QMovie* mov = new QMovie(":/res/loading.gif");
    ui->loading_label->setFixedSize(this->size());

    // 让 gif 缩放到 QLabel 的尺寸
    mov->setScaledSize(ui->loading_label->size());
    ui->loading_label->setAlignment(Qt::AlignCenter);
    ui->loading_label->setMovie(mov);
    mov->start();
}

Loading_Dialog::Loading_Dialog(QWidget *parent, QSize psize)
    : QDialog(parent)
    , ui(new Ui::Loading_Dialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_TranslucentBackground);
    int max = psize.height()>psize.width()?psize.height():psize.width();
    this->setFixedSize(QSize(max,max)/5);


    QMovie* mov = new QMovie(":/res/loading.gif");
    ui->loading_label->setFixedSize(this->size());

    // 让 gif 缩放到 QLabel 的尺寸
    mov->setScaledSize(ui->loading_label->size());
    ui->loading_label->setAlignment(Qt::AlignCenter);
    ui->loading_label->setMovie(mov);
    mov->start();
}

Loading_Dialog::~Loading_Dialog()
{
    delete ui;
}
