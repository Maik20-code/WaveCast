#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "udpchat.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &nickname, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSendButtonClicked();
    void onNewPeer(const QString &nick);
    void onPeerLeft(const QString &nick);
    void onMessageReceived(const QString &sender, const QString &text);
    void onError(const QString &error);

private:
    void appendMessage(const QString &sender, const QString &text, bool isMe);
    void updateUserList();

    Ui::MainWindow *ui;
    UdpChat *m_udpChat;
    QString m_myNickname;
};

#endif // MAINWINDOW_H
