#ifndef FRIENDLB_H
#define FRIENDLB_H

#include <QWidget>

namespace Ui {
class FriendLb;
}

class FriendLb : public QWidget
{
    Q_OBJECT

public:
    explicit FriendLb(QWidget *parent = nullptr);
    ~FriendLb();
    void SetText(QString text);
    int Width();
    int Height();
    QString Text();

private:
    Ui::FriendLb *ui;
    QString _text;
    int _width;
    int _height;
public slots:
    void slot_close();
signals:
    void sig_close(QString);
};

#endif // FRIENDLB_H
