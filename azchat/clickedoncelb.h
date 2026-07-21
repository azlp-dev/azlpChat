#ifndef CLICKEDONCELB_H
#define CLICKEDONCELB_H

#include <QLabel>

class ClickedOnceLB : public QLabel
{
    Q_OBJECT
public:
    ClickedOnceLB(QWidget* parent = nullptr);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
signals:
    void clicked(QString);
};

#endif // CLICKEDONCELB_H
