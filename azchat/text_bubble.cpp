#include "text_bubble.h"
#include <QFontMetrics>
#include <QTextBlock>

Text_Bubble::Text_Bubble(chat_role role, const QString& str, QWidget *parent)
    :Bubble_Frame(role,parent)
{
    _text = new QTextEdit();
    _text->setReadOnly(true);
    _text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QFont font("Microsoft YaHei");
    font.setPointSize(12);
    _text->setFont(font);

    _text->installEventFilter(this);
    this->setText(str);
    this->setWidget(_text);
    initstyle();
}

bool Text_Bubble::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == _text&& event->type() == QEvent::Paint){
        adujstTextHeight();
    }
    return QFrame::eventFilter(watched,event);
}

void Text_Bubble::adujstTextHeight()
{
    qreal doc_margin = _text->document()->documentMargin();    //字体到边框的距离默认为4
    QTextDocument *doc = _text->document();
    qreal text_height = 0;
    //把每一段的高度相加=文本高
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
    {
        QTextLayout *pLayout = it.layout();
        QRectF text_rect = pLayout->boundingRect();                             //这段的rect
        text_height += text_rect.height();
    }
    int vMargin = this->layout()->contentsMargins().top();
    //设置这个气泡需要的高度 文本高+文本边距+TextEdit边框到气泡边框的距离
    setFixedHeight(text_height + doc_margin *2 + vMargin*2 );
}

void Text_Bubble::setText(const QString &str)
{
    _text->setText(str);

    int docmargin = _text->document()->documentMargin();
    int marginL = this->layout()->contentsMargins().left();
    int marginR = this->layout()->contentsMargins().right();

    QFontMetrics fm(_text->font());
    QTextDocument* doc = _text->document();
    int maxW = 0;
    for(QTextBlock it = doc->begin();it != doc->end();it = it.next()){
        int txtW = int(fm.horizontalAdvance(it.text()));
        maxW = maxW<txtW?txtW:maxW;
    }
    this->setMaximumWidth(maxW+docmargin*2+marginL+marginR+4);
}

void Text_Bubble::initstyle()
{
    _text->setStyleSheet("QTextEdit{background:transparent;border:none}");
}
















