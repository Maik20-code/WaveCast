#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListWidgetItem>
#include <QDebug>

MainWindow::MainWindow(const QString &nickname, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_myNickname(nickname)
{
    ui->setupUi(this);

    setWindowTitle(QString("WaveCast - %1").arg(m_myNickname));

    ui->sendButton->setEnabled(false);
    ui->messageEdit->setEnabled(false);

    // Инициализация UDP чата
    m_udpChat = new UdpChat(m_myNickname, this);
    connect(m_udpChat, &UdpChat::newPeer, this, &MainWindow::onNewPeer);
    connect(m_udpChat, &UdpChat::peerLeft, this, &MainWindow::onPeerLeft);
    connect(m_udpChat, &UdpChat::messageReceived, this, &MainWindow::onMessageReceived);
    connect(m_udpChat, &UdpChat::error, this, &MainWindow::onError);

    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(ui->messageEdit, &QLineEdit::returnPressed, this, &MainWindow::onSendButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendMessage(const QString &sender, const QString &text, bool isMe)
{
    QListWidgetItem *item = new QListWidgetItem;
    QString displayText = isMe ? text : QString("%1: %2").arg(sender, text);
    item->setText(displayText);
    if (isMe) {
        item->setTextAlignment(Qt::AlignRight);
        item->setBackground(QColor(0xDCF8C6));  // светло-зелёный
    } else {
        item->setTextAlignment(Qt::AlignLeft);
        item->setBackground(QColor(0xF0F0F0));  // светло-серый
    }
    ui->chatListWidget->addItem(item);
    ui->chatListWidget->scrollToBottom();
}

void MainWindow::updateUserList()
{
    QStringList peers = m_udpChat->getPeerNicks();
    ui->userListWidget->clear();
    ui->userListWidget->addItems(peers);
    // Включаем отправку, если есть хотя бы один собеседник
    bool hasPeers = !peers.isEmpty();
    ui->sendButton->setEnabled(hasPeers);
    ui->messageEdit->setEnabled(hasPeers);
    if (hasPeers)
        ui->messageEdit->setFocus();
}

void MainWindow::onSendButtonClicked()
{
    QString text = ui->messageEdit->text().trimmed();
    if (text.isEmpty())
        return;
    m_udpChat->sendToAll(text);
    appendMessage(m_myNickname, text, true);
    ui->messageEdit->clear();
}

void MainWindow::onNewPeer(const QString &nick)
{
    updateUserList();
}

void MainWindow::onPeerLeft(const QString &nick)
{
    updateUserList();
}

void MainWindow::onMessageReceived(const QString &sender, const QString &text)
{
    appendMessage(sender, text, false);
}

void MainWindow::onError(const QString &error)
{
    // Выводим ошибку, например, в строку состояния
    statusBar()->showMessage("Ошибка: " + error, 3000);
}
