#include "chatitembase.h"
#include <QGridLayout>
#include <QSpacerItem>

ChatItemBase::ChatItemBase(chat_role role,QWidget *parent)
{
    _namelb = new QLabel();
    _namelb->setObjectName("name_lb");
    QFont font("Microsoft YaHei");
    font.setPointSize(9);
    _namelb->setFont(font);
    _namelb->setFixedHeight(20);

    _iconlb = new QLabel();
    _iconlb->setObjectName("icon_lb");
    _iconlb->setScaledContents(true);
    _iconlb->setFixedSize(42,42);

    _bubble_wid = new QWidget();

    QGridLayout* gl = new QGridLayout();
    gl->setVerticalSpacing(3);
    gl->setHorizontalSpacing(3);
    gl->setContentsMargins(3,3,3,3);

    QSpacerItem* spacer_1 = new QSpacerItem(40,20,QSizePolicy::Expanding,QSizePolicy::Minimum);
    if(role == chat_role::self){
        _namelb->setContentsMargins(0,0,8,0);
        _namelb->setAlignment(Qt::AlignRight);

        gl->addWidget(_namelb,0,1,1,1);
        gl->addWidget(_iconlb,0,2,2,1,Qt::AlignTop);

        gl->addItem(spacer_1,1,0,1,1);
        gl->addWidget(_bubble_wid,1,1,1,1);
        gl->setColumnStretch(0,2);
        gl->setColumnStretch(1,3);
    }else{
        _namelb->setContentsMargins(8,0,0,0);
        _namelb->setAlignment(Qt::AlignLeft);

        gl->addWidget(_namelb,0,1,1,1);
        gl->addWidget(_iconlb,0,0,2,1,Qt::AlignTop);
        gl->addItem(spacer_1,1,2,1,1);
        gl->addWidget(_bubble_wid,1,1,1,1);
        gl->setColumnStretch(1,3);
        gl->setColumnStretch(2,2);

    }
    this->setLayout(gl);
}

void ChatItemBase::setName(const QString &name)
{
    this->_namelb->setText(name);
}

void ChatItemBase::setIcon(const QPixmap &pix)
{
    this->_iconlb->setPixmap(pix);
}

void ChatItemBase::setMsg( QWidget *w)
{
    QGridLayout* temp =qobject_cast<QGridLayout*>(this->layout());
    temp->replaceWidget(_bubble_wid,w);
    delete _bubble_wid;
    _bubble_wid = w;
}
