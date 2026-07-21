#include "friendlb.h"
#include "ui_friendlb.h"
#include "clickedlable.h"

FriendLb::FriendLb(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FriendLb)
{
    ui->setupUi(this);
    connect(ui->close_lb, &ClickedLable::clicked, this, &FriendLb::slot_close);
}

FriendLb::~FriendLb()
{
    delete ui;
}

void FriendLb::SetText(QString text)
{
    _text = text;
    ui->tip_lb->setText(_text);
    ui->tip_lb->adjustSize();

    QFontMetrics fontMetrics(ui->tip_lb->font()); // 获取QLabel控件的字体信息
    auto textWidth = fontMetrics.horizontalAdvance(ui->tip_lb->text()); // 获取文本的宽度
    auto textHeight = fontMetrics.height(); // 获取文本的高度

    // qDebug()<< " ui->tip_lb.width() is " << ui->tip_lb->width();
    // qDebug()<< " ui->close_lb->width() is " << ui->close_lb->width();
    // qDebug()<< " textWidth is " << textWidth;
    this->setFixedWidth(ui->tip_lb->width()+ui->close_lb->width()+5);
    this->setFixedHeight(textHeight+2);
    //qDebug()<< "  this->setFixedHeight " << this->height();
    _width = this->width();
    _height = this->height();
}

int FriendLb::Width()
{
    return _width;
}

int FriendLb::Height()
{
    return _height;
}

QString FriendLb::Text()
{
    return _text;
}

void FriendLb::slot_close()
{
    emit sig_close(_text);
}