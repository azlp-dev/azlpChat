#ifndef LOADING_DIALOG_H
#define LOADING_DIALOG_H

#include <QDialog>

namespace Ui {
class Loading_Dialog;
}

class Loading_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Loading_Dialog(QWidget *parent = nullptr);
    Loading_Dialog(QWidget *parent,QSize size);
    ~Loading_Dialog();

private:
    Ui::Loading_Dialog *ui;
};

#endif // LOADING_DIALOG_H
