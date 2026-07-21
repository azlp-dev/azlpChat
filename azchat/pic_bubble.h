#ifndef PIC_BUBBLE_H
#define PIC_BUBBLE_H

#include "bubble_frame.h"

class Pic_Bubble : public Bubble_Frame
{
public:
    Pic_Bubble(const QPixmap& picture,chat_role role,QWidget* parent = nullptr);


};

#endif // PIC_BUBBLE_H
