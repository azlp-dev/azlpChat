#ifndef ADDFRIEND_DLG_H
#define ADDFRIEND_DLG_H

#include <QDialog>
#include "userinfo.h"
#include "clickedlable.h"

class FriendLb;
namespace Ui {
class AddFriend_Dlg;
}

class AddFriend_Dlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddFriend_Dlg(QWidget *parent = nullptr);
    ~AddFriend_Dlg();
    void InitTipLbs();
    void AddTipLbs(ClickedLable*, QPoint cur_point, QPoint &next_point, int text_width, int text_height);
    bool eventFilter(QObject *obj, QEvent *event);
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);

signals:
    void sig_delete_findSuccess();

public slots:
    //显示更多label标签
    void ShowMoreLabel();
    //输入label按下回车触发将标签加入展示栏
    void SlotLabelEnter();
    //点击关闭，移除展示栏好友便签
    void SlotRemoveFriendLabel(QString);
    //通过点击tip实现增加和减少好友便签
    void SlotChangeFriendLabelByTip(QString, ClickedLbState);
    //输入框文本变化显示不同提示
    void SlotLabelTextChange(const QString& text);
    //输入框输入完成
    void SlotLabelEditFinished();
    //输入标签显示提示框，点击提示框内容后添加好友便签
    void SlotAddFirendLabelByClickTip(QString text);
    //处理确认回调
    void SlotApplySure();
    //处理取消回调
    void SlotApplyCancel();

private:
    Ui::AddFriend_Dlg *ui;
    void resetLabels();

    //已经创建好的标签
    QMap<QString, ClickedLable*> _add_labels;
    std::vector<QString> _add_label_keys;
    QPoint _label_point;
    //用来在输入框显示添加新好友的标签
    QMap<QString, FriendLb*> _friend_labels;
    std::vector<QString> _friend_label_keys;
    void addLabel(QString name);
    std::vector<QString> _tip_data;
    QPoint _tip_cur_point;
    std::shared_ptr<SearchInfo> _si;
};

#endif // ADDFRIEND_DLG_H
