#include "mainwindow.h"
#include <qlogging.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>

#include "launcherdata.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LauncherData testData = LauncherData::loadFromFile(QString("launcher.dat"));

    qDebug(QDir::currentPath().toLatin1().data());
    qDebug(QString("gameSecret: %1").arg(testData.gameSecret).toStdString().c_str());
    qDebug(QString("patcherSecret: %1").arg(testData.patcherSecret).toStdString().c_str());

    MainWindow w;

    const QRect availableSize = QApplication::desktop()->availableGeometry(&w);
    w.move((availableSize.width() - w.width()) / 2, (availableSize.height() - w.height()) / 2);

    w.show();

    return a.exec();
}
