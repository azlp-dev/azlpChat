#ifndef STATEWIDGET_H
#define STATEWIDGET_H
#include <QWidget>
#include "global.h"
#include <QLabel>

class StateWidget: public QWidget
{
    Q_OBJECT
public:
    StateWidget(QWidget* parent = nullptr);
    void setState(QString normal,QString hover,QString press,QString selected,QString sel_hover,QString sel_press);

    ClickedLbState GetCurState(){
        return _curstate;
    }

    void ClearState();

    void SetSelected(bool bselected);
    void AddRedPoint();
    void ShowRedPoint(bool show=true);

public slots:


protected:
    // void paintEvent(QPaintEvent* event) override;

    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent* event) override;

signals:
    void clicked();

private:
    QString _normal;
    QString _hover;
    QString _press;
    QString _selected;
    QString _sel_hover;
    QString _sel_press;

    ClickedLbState _curstate;
    QLabel* _redpoint;

};

#endif // STATEWIDGET_H

