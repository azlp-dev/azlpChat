#include "mainwindow.h"
#include "global.h"
#include <QApplication>
#include <QFile>

/******************************************************************************
 *
 * @file       main.cpp
 * @brief      主窗口 Function
 *
 * @author     AZLP
 * @date       2026/04/08
 * @history
 *****************************************************************************/

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        //qDebug("qss open failed!");
    }

    QString config_filename = "config.ini";
    QString app_path = QCoreApplication::applicationDirPath();
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + config_filename);

    QSettings settings(config_path,QSettings::IniFormat);
    QString gateserver = settings.value("GateServer/host").toString();
    QString gateport = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://"+gateserver+":"+gateport;

    MainWindow w;
    w.show();
    return a.exec();
}
