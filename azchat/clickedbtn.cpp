#include "clickedbtn.h"

ClickedBtn::ClickedBtn(QWidget* parent):QPushButton(parent) {
    this->setCursor(Qt::PointingHandCursor);
}

ClickedBtn::~ClickedBtn()
{

}

void ClickedBtn::SetState(QString normal, QString hover, QString press)
{
    _hover = hover;
    _normal = normal;
    _press = press;
    this->setProperty("state","normal");
    polish(this);
    this->update();
}

void ClickedBtn::mousePressEvent(QMouseEvent *event)
{
    this->setProperty("state","press");
    polish(this);
    this->update();
    QPushButton::mousePressEvent(event);
}

void ClickedBtn::enterEvent(QEnterEvent *event)
{
    this->setProperty("state","hover");
    polish(this);
    this->update();
}

void ClickedBtn::leaveEvent(QEvent *event)
{
    this->setProperty("state","normal");
    polish(this);
    this->update();
}


