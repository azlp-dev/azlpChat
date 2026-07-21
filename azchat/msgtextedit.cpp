#include "msgtextedit.h"
#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QFileIconProvider>
#include <QPainter>

MsgTextEdit::MsgTextEdit(QWidget *parent)
{
    this->setMaximumHeight(60);
}

MsgTextEdit::~MsgTextEdit()
{

}

QVector<msgInfo> MsgTextEdit::getMsgList()
{
    _getlist.clear();

    QString doc = this->document()->toPlainText();
    QString text = "";
    int indexUrl = 0;
    int count = _msglist.size();

    for(int i = 0;i<doc.size();i++){
        if(doc[i] == QChar::ObjectReplacementCharacter){
            if(!text.isEmpty()){
                QPixmap pix;
                insertMsgList(_getlist,"text",text,pix);
                text.clear();
            }
            while(indexUrl<count){
                msgInfo msg = _msglist[indexUrl];
                if(this->document()->toHtml().contains(msg.content,Qt::CaseSensitive)){
                    indexUrl++;
                    _getlist.append(msg);
                    break;
                }
                indexUrl++;
            }
        }
        else{
            text.append(doc[i]);
        }
    }
    if(!text.isEmpty()){
        QPixmap pix;
        insertMsgList(_getlist,"text",text,pix);
        text.clear();
    }
    this->clear();
    _msglist.clear();
    return _getlist;
}

void MsgTextEdit::insertFileFromUrl(const QStringList &urls)
{
    if(urls.isEmpty()){
        return;
    }
    foreach(auto url,urls){
        if(isImage(url)){
            insertImages(url);
        }else{
            insertTextFile(url);
        }
    }
    return;
}

void MsgTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->source() == this){
        event->ignore();
    }else{
        event->accept();
    }
}

void MsgTextEdit::dropEvent(QDropEvent *event)
{
    this->insertFromMimeData(event->mimeData());
    event->accept();
}

void MsgTextEdit::keyPressEvent(QKeyEvent *e)
{
    if((e->key() == Qt::Key_Enter||e->key() == Qt::Key_Return)&&(!e->modifiers()&Qt::ShiftModifier)){
        emit send();
        return;
    }
    QTextEdit::keyPressEvent(e);
}

void MsgTextEdit::insertImages(const QString &url)
{
    QImage img(url);
    if(img.height()>80||img.width()>120){
        img = img.scaled(120,80,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(img,url);

    insertMsgList(_msglist,"image",url,QPixmap::fromImage(img));
}

void MsgTextEdit::insertTextFile(const QString &url)
{
    QFileInfo f_info(url);
    if(f_info.isDir()){
        QMessageBox::information(this,"提示","只能传输单个文件");
        return;
    }
    if(f_info.size()>1024*1024*100){
        QMessageBox::information(this,"提示","文件大小不能超过100MB");
        return;
    }
    QPixmap pix = getFileIconPixmap(url);
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(pix.toImage(),url);
    insertMsgList(_msglist,"file",url,pix);
}

bool MsgTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

void MsgTextEdit::insertFromMimeData(const QMimeData *source)
{
    QStringList urls = getUrl(source->text());
    if(urls.isEmpty()){
        return;
    }
    foreach(auto url,urls){
        if(isImage(url)){
            insertImages(url);
        }else{
            insertTextFile(url);
        }
    }
}

bool MsgTextEdit::isImage(QString url)
{
    QString format = "bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp";
    QStringList list = format.split(",");
    QFileInfo f_info(url);
    QString suffix = f_info.suffix();
    if(list.contains(suffix,Qt::CaseSensitive)){
        return true;
    }else{
        return false;
    }
}

void MsgTextEdit::insertMsgList(QVector<msgInfo> &list, QString flag, QString text, QPixmap pix)
{
    msgInfo msg;
    msg.msgFlag = flag;
    msg.content = text;
    msg.pix = pix;
    list.append(msg);
}

QStringList MsgTextEdit::getUrl(QString text)
{
    QStringList urls;
    if(text.isEmpty()) return urls;

    QStringList list = text.split("\n");
    foreach (QString url, list) {
        if(!url.isEmpty()){
            QStringList str = url.split("///");
            if(str.size()>=2)
                urls.append(str.at(1));
        }
    }
    return urls;
}

QPixmap MsgTextEdit::getFileIconPixmap(const QString &url)
{
    QFileIconProvider provder;
    QFileInfo fileinfo(url);
    QIcon icon = provder.icon(fileinfo);

    QString strFileSize = getFileSize(fileinfo.size());
    //qDebug() << "FileSize=" << fileinfo.size();

    QFont font(QString("宋体"),10,QFont::Normal,false);
    QFontMetrics fontMetrics(font);
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());

    QSize FileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    int maxWidth = textSize.width() > FileSize.width() ? textSize.width() :FileSize.width();
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    QPainter painter;
    // painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.setFont(font);
    painter.begin(&pix);
    // 文件图标
    QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40,40));
    painter.setPen(Qt::black);
    // 文件名称
    QRect rectText(50+10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    // 文件大小
    QRect rectFile(50+10, textSize.height()+5, FileSize.width(), FileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}

QString MsgTextEdit::getFileSize(qint64 size)
{
    QString Unit;
    double num;
    if(size < 1024){
        num = size;
        Unit = "B";
    }
    else if(size < 1024 * 1224){
        num = size / 1024.0;
        Unit = "KB";
    }
    else if(size <  1024 * 1024 * 1024){
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else{
        num = size / 1024.0 / 1024.0/ 1024.0;
        Unit = "GB";
    }
    return QString::number(num,'f',2) + " " + Unit;
}

void MsgTextEdit::textEditChanged()
{
    //qDebug() << "text changed!" << Qt::endl;
}


