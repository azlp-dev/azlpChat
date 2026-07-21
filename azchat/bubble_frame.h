#ifndef BUBBLE_FRAME_H
#define BUBBLE_FRAME_H

#include <QFrame>
#include "global.h"
#include <QHBoxLayout>

class Bubble_Frame : public QFrame
{
    Q_OBJECT
public:
    Bubble_Frame(chat_role role,QWidget* parent = nullptr);
    void setMargin(int margin);
    void setWidget(QWidget* w);
    void paintEvent(QPaintEvent * p) override;


private:
    QHBoxLayout* _hl;
    int _margin;
    chat_role m_role;
};

#endif // BUBBLE_FRAME_H
