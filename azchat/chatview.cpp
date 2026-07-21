#include "chatview.h"
#include <QScrollBar>
#include <QEvent>
#include <QTimer>

ChatView::ChatView(QWidget *parent):QWidget(parent)
{
    _mainlayout = new QVBoxLayout(this);
    _mainlayout->setContentsMargins(0,0,0,0);

    _scrollArea = new QScrollArea();
    _scrollArea->setObjectName("scroll_area");
    _mainlayout->addWidget(_scrollArea);

    QWidget* w = new QWidget();
    w->setObjectName("chatBG_wid");
    w->setAutoFillBackground(true);
    _scrollArea->setWidget(w);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(new QWidget(),100000);
    w->setLayout(v1);

    _scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollBar* temp = _scrollArea->verticalScrollBar();
    connect(temp,&QScrollBar::rangeChanged,this,&ChatView::onScrollBarMoved);

    _scrollArea->setWidgetResizable(true);
    _scrollArea->installEventFilter(this);
    initStyle();
    this->setAttribute(Qt::WA_StyledBackground);
}

void ChatView::appendItem(QWidget *item)
{
    QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(_scrollArea->widget()->layout());
    vl->insertWidget(vl->count()-1, item);
    _is_appended = true;
}

void ChatView::prependItem(QWidget *item)
{
    QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(_scrollArea->widget()->layout());
    vl->insertWidget(0, item);
    _is_appended = true;
}

void ChatView::insertItem(QWidget *front, QWidget *item)
{
    if (!item) {
        return;
    }

    QWidget *contentWidget = _scrollArea->widget();
    if (!contentWidget) {
        return;
    }

    QVBoxLayout *vl = qobject_cast<QVBoxLayout *>(contentWidget->layout());
    if (!vl) {
        return;
    }

    // 如果 front 为空，或者布局里找不到 front，就默认插入到最后的占位控件前面
    int insertIndex = vl->count() - 1;

    if (front) {
        int frontIndex = vl->indexOf(front);

        if (frontIndex != -1) {
            insertIndex = frontIndex + 1;
        }
    }

    // 防止插入位置越界
    if (insertIndex < 0) {
        insertIndex = 0;
    }

    if (insertIndex > vl->count()) {
        insertIndex = vl->count();
    }

    vl->insertWidget(insertIndex, item);
    _is_appended = true;
}

void ChatView::RemoveAllItem()
{
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(_scrollArea->widget()->layout());
    int count = layout->count();
    for(int i =0;i<count-1;i++){
        QLayoutItem* item = layout->takeAt(0);
        if(item){
            if(QWidget* w = item->widget()){
                delete w;
            }
        }
    }
}

bool ChatView::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == _scrollArea&& event->type() == QEvent::Enter){
        _scrollArea->verticalScrollBar()->setHidden(_scrollArea->verticalScrollBar()->maximum()==0);
    }
    else if(watched == _scrollArea&& event->type() == QEvent::Leave){
        _scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QWidget::eventFilter(watched, event);
}

void ChatView::onScrollBarMoved()
{
    if(_is_appended) //添加item可能调用多次
    {
        QScrollBar *pVScrollBar = _scrollArea->verticalScrollBar();
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());

        //500毫秒内可能调用多次
        QTimer::singleShot(500, [this]()
                           {
                               _is_appended = false;
                           });
    }
}

void ChatView::initStyle()
{

}




