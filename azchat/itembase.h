#ifndef ITEMBASE_H
#define ITEMBASE_H
#include <QWidget>
#include <global.h>

class ItemBase:public QWidget
{
    Q_OBJECT
public:
    ItemBase(QWidget* parent = nullptr);
    void SetType(ListItemType type){
        _type = type;
    }

    ListItemType GetType(){
        return _type;
    }

private:
    ListItemType _type;
};

#endif // ITEMBASE_H
