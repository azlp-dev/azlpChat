#ifndef FIND_SUCCESS_DLG_H
#define FIND_SUCCESS_DLG_H

#include <QDialog>
#include "userinfo.h"
#include "global.h"
#include <memory>

namespace Ui {
class Find_Success_Dlg;
}

class Find_Success_Dlg : public QDialog
{
    Q_OBJECT

public:
    explicit Find_Success_Dlg(QWidget *parent = nullptr);
    ~Find_Success_Dlg();
    void setSearchInfo(std::shared_ptr<SearchInfo> si);

private slots:
    void on_pushButton_clicked();

private:
    Ui::Find_Success_Dlg *ui;
    QWidget* _parent;
    std::shared_ptr<SearchInfo> _si;
};

#endif // FIND_SUCCESS_DLG_H
