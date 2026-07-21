#include "statewidget.h"
#include <QVBoxLayout>
#include <QMouseEvent>

StateWidget::StateWidget(QWidget *parent)
{
    AddRedPoint();
    this->setAttribute(Qt::WA_StyledBackground);
}

void StateWidget::setState(QString normal, QString hover, QString press, QString selected, QString sel_hover, QString sel_press)
{
    _normal = normal;
    _hover = hover;
    _press = press;

    _selected = selected;
    _sel_hover = sel_hover;
    _sel_press = sel_press;

    setProperty("state",normal);
    polish(this);
}

void StateWidget::ClearState()
{
    _curstate = ClickedLbState::Normal;
    setProperty("state",_normal);
    polish(this);
    update();
}

void StateWidget::SetSelected(bool bselected)
{
    if(bselected){
        _curstate = ClickedLbState::Selected;
        setProperty("state",_selected);
        polish(this);
        update();
        return;
    }

    _curstate = ClickedLbState::Normal;
    setProperty("state",_normal);
    polish(this);
    update();
    return;
}

void StateWidget::AddRedPoint()
{
    _redpoint = new QLabel();
    _redpoint->setObjectName("red_point");
    QVBoxLayout* layout2 = new QVBoxLayout;
    _redpoint->setAlignment(Qt::AlignCenter);
    layout2->addWidget(_redpoint);
    layout2->setContentsMargins(0,0,0,0);
    this->setLayout(layout2);
    _redpoint->setVisible(false);
}

void StateWidget::ShowRedPoint(bool show)
{
    _redpoint->setVisible(show);
}

void StateWidget::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        if(_curstate == ClickedLbState::Selected){
            //qDebug()<<"PressEvent , already to selected press: "<< _sel_press;
            //emit clicked();
            // 调用基类的mousePressEvent以保证正常的事件处理
            QWidget::mousePressEvent(ev);
            return;
        }

        if(_curstate == ClickedLbState::Normal){
            //qDebug()<<"PressEvent , change to selected press: "<< _sel_press;
            _curstate = ClickedLbState::Selected;
            setProperty("state",_sel_press);
            polish(this);
            update();
        }

        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QWidget::mousePressEvent(ev);
}

void StateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickedLbState::Normal){
            //qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state",_hover);
            polish(this);
            update();

        }else{
            //qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state",_sel_hover);
            polish(this);
            update();
        }
        emit clicked();
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QWidget::mousePressEvent(event);
}

void StateWidget::enterEvent(QEnterEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
    if(_curstate == ClickedLbState::Normal){
        //qDebug()<<"enter , change to normal hover: "<< _normal_hover;
        setProperty("state",_hover);
        polish(this);
        update();

    }else{
        //qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",_sel_hover);
        polish(this);
        update();
    }

    QWidget::enterEvent(event);
}

void StateWidget::leaveEvent(QEvent *event)
{
    // 在这里处理鼠标悬停离开的逻辑
    if(_curstate == ClickedLbState::Normal){
        // qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",_normal);
        polish(this);
        update();

    }else{
        // qDebug()<<"leave , change to select normal : "<< _selected;
        setProperty("state",_selected);
        polish(this);
        update();
    }
    QWidget::leaveEvent(event);
}


