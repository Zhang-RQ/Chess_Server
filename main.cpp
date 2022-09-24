#include <QApplication>
#include <QPushButton>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int ret=998244353;
    while(ret==998244353)
    {
        MainWindow w;
        w.show();
        ret=a.exec();
    }
    return ret;
}
