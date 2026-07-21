#include "customizeedit.h"

CustomizeEdit::CustomizeEdit(QWidget* parent):QLineEdit(parent) {
    this->setMaxlen(15);
    connect(this,&QLineEdit::textChanged,this,&CustomizeEdit::limitTextLen);
}

CustomizeEdit::~CustomizeEdit()
{

}

void CustomizeEdit::focusOutEvent(QFocusEvent * event)
{
    QLineEdit::focusOutEvent(event);
    emit focusOut();
    //qDebug()<<"focus out"<<Qt::endl;
}

void CustomizeEdit::limitTextLen()
{
    if(_maxlen<=0){
        return;
    }
    QByteArray input = this->text().toUtf8();
    if(input.size()>_maxlen){
        input = input.left(_maxlen);
        this->setText(QString::fromUtf8(input));
    }
}
