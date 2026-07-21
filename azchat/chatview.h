#ifndef CHATVIEW_H
#define CHATVIEW_H
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>

class ChatView:public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget* parent = nullptr);
    void appendItem(QWidget* item);
    void prependItem(QWidget* item);
    void insertItem(QWidget* front,QWidget* item);
    void RemoveAllItem();

protected:
    //virtual void paintEvent(QPaintEvent *event) override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onScrollBarMoved();

private:
    void initStyle();

private:
    QVBoxLayout* _mainlayout;
    QScrollArea* _scrollArea;
    bool _is_appended{false};
};

#endif // CHATVIEW_H
