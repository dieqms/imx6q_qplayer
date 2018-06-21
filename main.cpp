#include "mainwindow.h"
#include "player_core.h"
#include <QApplication>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 解析运行参数
    QStringList arguments = QCoreApplication::arguments();
    for (int i = 0; i < arguments.size(); i++) {
        printf("Arguments : %d %s\n", i, arguments[i].toStdString().c_str());
    }

    if (argc < 2) {
        printf("Usage: QPlayer file_path\n");
        return -1;
    }

    MainWindow w;
    w.setWindowFlags(Qt::FramelessWindowHint);

//    w.showMaximized();
//    w.showFullScreen();
    w.setFixedSize(1024, 600);
    w.show();

    return a.exec();
}
