#include "clickedoncelb.h"
#include <QMouseEvent>


ClickedOnceLB::ClickedOnceLB(QWidget *parent)
    :QLabel(parent)
{
    this->setCursor(Qt::PointingHandCursor);
}

void ClickedOnceLB::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        emit clicked(this->text());
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(ev);
}
