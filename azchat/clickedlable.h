#ifndef CLICKEDLABLE_H
#define CLICKEDLABLE_H
#include "global.h"
#include <QLabel>

class ClickedLable:public QLabel
{
    Q_OBJECT
public:
    ClickedLable(QWidget *parent=nullptr);
    void mousePressEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    ClickedLbState getCur(){
        return _currentState;
    }

    bool setState(ClickedLbState state);
    void ResetNormalState();

signals:
    void clicked(QString,ClickedLbState);

private:
    ClickedLbState _currentState;
};

#endif // CLICKEDLABLE_H
