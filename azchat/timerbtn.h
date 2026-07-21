#ifndef TIMERBTN_H
#define TIMERBTN_H
#include <QPushButton>

class TimerBtn :public QPushButton
{
public:
    TimerBtn(QWidget *parent = nullptr,int count = 180);


protected:
    void mouseReleaseEvent(QMouseEvent *e) override;

    QTimer* _timer;
    int _counter;
    //int _recovery;
};

#endif // TIMERBTN_H
