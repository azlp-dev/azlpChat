#ifndef ADDUSERITEM_H
#define ADDUSERITEM_H

#include <QWidget>
#include "itembase.h"

namespace Ui {
class AddUserItem;
}

class AddUserItem : public ItemBase
{
    Q_OBJECT

public:
    explicit AddUserItem(QWidget *parent = nullptr);
    ~AddUserItem();

private:
    Ui::AddUserItem *ui;
};

#endif // ADDUSERITEM_H
