#ifndef FIND_FAILED_DLG_H
#define FIND_FAILED_DLG_H

#include <QDialog>

namespace Ui {
class find_failed_dlg;
}

class find_failed_dlg : public QDialog
{
    Q_OBJECT

public:
    explicit find_failed_dlg(QWidget *parent = nullptr);
    ~find_failed_dlg();

private slots:
    void on_ff_sure_pushButton_clicked();

private:
    Ui::find_failed_dlg *ui;
};

#endif // FIND_FAILED_DLG_H
