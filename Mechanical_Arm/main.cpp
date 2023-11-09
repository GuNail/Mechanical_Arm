
#include "mainwindow.h"
#include "register.h"
#include <QApplication>
#include <QSettings>

QSettings *settings = nullptr;

int main(int argc, char *argv[])
{
    RegisterAll();

    QApplication a(argc, argv);
    MainWindow w;

    settings = new QSettings("zhwx", "MechanicalArm");

    w.setWindowTitle(QString::fromUtf8("机械臂控制助手"));
    w.setWindowIcon(QIcon(":/icon/resources/icon.png"));
    w.resize(800, 480);
    w.show();
    return a.exec();
}
