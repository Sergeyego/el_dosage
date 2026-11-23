#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include "rest/restlogin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/ico.ico"));
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || defined(Q_OS_MAC)
    QApplication::setStyle(QStyleFactory::create("Fusion"));
#endif
    RestLogin d(QObject::tr("Дозирование компонентов"));
    d.setHost("https://192.168.1.134");
    if (d.exec()!=QDialog::Accepted) {
        exit(1);
    }
    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
