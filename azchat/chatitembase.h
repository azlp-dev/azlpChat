#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H
#include <QWidget>
#include "global.h"
#include <QLabel>

class ChatItemBase:public QWidget
{
public:
    ChatItemBase(chat_role role,QWidget* parent = nullptr);
    void setName(const QString& name);
    void setIcon(const QPixmap& pix);
    void setMsg( QWidget* w);

private:
    QLabel* _namelb;
    QLabel* _iconlb;
    QWidget* _bubble_wid;
};

#endif // CHATITEMBASE_H
