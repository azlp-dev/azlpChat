#ifndef SEARCHLIST_H
#define SEARCHLIST_H
#include <QWidget>
#include "loading_dialog.h"
#include <QListWidget>
#include <QListWidgetItem>
#include "userinfo.h"

class SearchList:public QListWidget
{
    Q_OBJECT
public:
    SearchList(QWidget* parent = nullptr);
    void closeFindDlg();
    void setSeacrhEdit(QWidget* w);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void waitPending(bool b);
    void test_addItem();

signals:
    void sig_jump_chat(std::shared_ptr<UserInfo>);

public slots:
    void slot_item_clicked(QListWidgetItem* item);
    void slot_search_recved(std::shared_ptr<SearchInfo> info);

private:
    bool _b_pendiong;
    std::shared_ptr<QDialog> _find_dlg;
    QWidget* _search_edit;
    Loading_Dialog* load_dlg;
};

#endif // SEARCHLIST_H
