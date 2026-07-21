#include "clickedlable.h"
#include <QMouseEvent>
#include "global.h"

ClickedLable::ClickedLable(QWidget* parent): QLabel(parent),_currentState(ClickedLbState::Normal)
{
    this->setProperty("state","Normal");
    polish(this);
    this->update();
}

void ClickedLable::mousePressEvent(QMouseEvent *ev) {
    if(ev->button() == Qt::LeftButton){
        if(_currentState == ClickedLbState::Normal){
            //qDebug()<<"change to selected"<<Qt::endl;
            _currentState = ClickedLbState::Selected;
            this->setProperty("state","Selected");
            polish(this);
            this->update();
        }else{
            //qDebug()<<"change to Normal"<<Qt::endl;
            _currentState = ClickedLbState::Normal;
            this->setProperty("state","Normal");
            polish(this);
            this->update();
        }
        emit  clicked("",ClickedLbState::Normal);
    }
    QLabel::mousePressEvent(ev);
    return;
}

void ClickedLable::enterEvent(QEnterEvent *event){
    if(_currentState == ClickedLbState::Normal){
        this->setProperty("state","Normal_Hover");
        polish(this);
        this->update();
    }else{
        this->setProperty("state","Selected_Hover");
        polish(this);
        this->update();
    }
    QLabel::enterEvent(event);
    return;
}

void ClickedLable::leaveEvent(QEvent *event){
    if(_currentState == ClickedLbState::Normal){
        this->setProperty("state","Normal");
        polish(this);
        this->update();
    }else{
        this->setProperty("state","Selected");
        polish(this);
        this->update();
    }
    QLabel::leaveEvent(event);
    return;
}

void ClickedLable::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_currentState == ClickedLbState::Normal){
            // qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state","normal_hover");
            polish(this);
            update();
            emit clicked(this->text(),ClickedLbState::Normal);

        }else{
            //  qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state","selected_hover");
            polish(this);
            update();
            emit clicked(this->text(), ClickedLbState::Selected);
        }

        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

bool ClickedLable::setState(ClickedLbState state)
{
    _currentState = state;
    if (_currentState == ClickedLbState::Normal) {
        setProperty("state", "normal");
        polish(this);
    }
    else if (_currentState == ClickedLbState::Selected) {
        setProperty("state", "selected");
        polish(this);
    }

    return true;
}

void ClickedLable::ResetNormalState()
{
    _currentState = ClickedLbState::Normal;
    setProperty("state", "normal");
    polish(this);
}