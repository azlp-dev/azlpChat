#include "global.h"
#include <QWidget>
#include <QStyle>

std::function<void(QWidget*)> polish = [](QWidget* w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};

QString gate_url_prefix;

std::function<QString(QString)> Enc_sha256 = [](QString str){
    QByteArray hash = QCryptographicHash::hash(str.toUtf8(),QCryptographicHash::Sha256);
    QString str_hash = hash.toHex();
    return str_hash;
};