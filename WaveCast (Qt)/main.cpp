#include <QApplication>
#include "logindialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginDialog login;
    if (login.exec() != QDialog::Accepted) {
        return 0;
    }
    QString nickname = login.getNickname();

    MainWindow w(nickname);
    w.show();

    return a.exec();
}
