#ifndef TEXT_BUBBLE_H
#define TEXT_BUBBLE_H
#include "bubble_frame.h"
#include <QTextEdit>
#include "global.h"

class Text_Bubble: public Bubble_Frame
{
    Q_OBJECT
public:

    Text_Bubble(chat_role role,const QString& str,QWidget* parent = nullptr);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    void adujstTextHeight();
    void setText(const QString& str);
    void initstyle();

private:
    QTextEdit* _text;
};

#endif // TEXT_BUBBLE_H
