#ifndef FRIENDAPPLY_LIST_H
#define FRIENDAPPLY_LIST_H

#include <QListWidget>

class FriendApply_List: public QListWidget
{
    Q_OBJECT

public:
    FriendApply_List(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void sig_show_search(bool);

private:

};

#endif // FRIENDAPPLY_LIST_H
