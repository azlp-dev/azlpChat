#ifndef GROUPTIP_ITEM_H
#define GROUPTIP_ITEM_H

#include <QWidget>
#include "itembase.h"

namespace Ui {
class Grouptip_Item;
}

class Grouptip_Item : public ItemBase
{
    Q_OBJECT

public:
    explicit Grouptip_Item(QWidget *parent = nullptr);
    ~Grouptip_Item();
    void setText(QString str);
    virtual QSize sizeHint() const override;


private:
    Ui::Grouptip_Item *ui;
};

#endif // GROUPTIP_ITEM_H
