#include "grouptip_item.h"
#include "ui_grouptip_item.h"

Grouptip_Item::Grouptip_Item(QWidget *parent)
    : ItemBase(parent)
    , ui(new Ui::Grouptip_Item)
{
    ui->setupUi(this);
    this->SetType(ListItemType::GROUP_TIP_ITEM);

}

Grouptip_Item::~Grouptip_Item()
{
    delete ui;
}

void Grouptip_Item::setText(QString str)
{
    ui->label->setText(str);
}

QSize Grouptip_Item::sizeHint() const
{
    return QSize(250,25);
}
