#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

    void showTip(QString,bool);
    void checkUserEdit();
    void checkEmailEdit();
    void checkCodeEdit();
    void checkNewPassEdit();
    void addErrTip(ErrTips eid,const QString& tip);
    void DelErrtip(ErrTips eid);
    void Checkconnect(); //绑定输入检查

    void initHandler();
signals:
    void switchLogin(void);

private slots:
    //void on_pushButton_2_clicked();

    void on_code_pushButton_clicked();
    void slot_reset_finished(Reqids id, QString result, Errids errid);

    void on_cancel_pushButton_clicked();

    void on_yes_pushButton_clicked();

private:
    Ui::ResetDialog *ui;
    QMap<Reqids,std::function<void(const QJsonObject&)>> _handlers;
    QMap<ErrTips,QString> _tips_map;
};

#endif // RESETDIALOG_H
