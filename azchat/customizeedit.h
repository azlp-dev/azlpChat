#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H
#include <QLineEdit>

class CustomizeEdit:public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget* parent = nullptr);
    ~CustomizeEdit();


    void focusOutEvent(QFocusEvent *) override;
    void setMaxlen(int len){
        _maxlen = len;
    }
    void limitTextLen();

signals:
    void focusOut();

private:
    int _maxlen{0};
};

#endif // CUSTOMIZEEDIT_H
