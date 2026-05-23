#include "udpchat.h"
#include <QDateTime>
#include <QDebug>

#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

UdpChat::UdpChat(const QString &myNickname, QObject *parent)
    : QObject(parent), m_myNickname(myNickname)
{
    m_socket = new QUdpSocket(this);
    // Разрешить broadcast через низкоуровневый сокет
    int broadcast = 1;
    setsockopt(m_socket->socketDescriptor(), SOL_SOCKET, SO_BROADCAST,
               (char*)&broadcast, sizeof(broadcast));

    // Привязать сокет к фиксированному порту с флагом повторного использования
    m_socket->bind(DEFAULT_PORT, QAbstractSocket::ReuseAddressHint);
    m_port = m_socket->localPort();

    connect(m_socket, &QUdpSocket::readyRead, this, &UdpChat::onReadyRead);

    m_heartbeatTimer = new QTimer(this);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &UdpChat::onHeartbeat);
    m_heartbeatTimer->start(HEARTBEAT_INTERVAL);
    sendPresence();
}
UdpChat::~UdpChat()
{
    m_socket->close();
}

void UdpChat::sendPresence()
{
    QByteArray datagram = QString("PRESENCE:%1").arg(m_myNickname).toUtf8();
    m_socket->writeDatagram(datagram, QHostAddress::Broadcast, DEFAULT_PORT);
    m_socket->writeDatagram(datagram, QHostAddress::LocalHost, DEFAULT_PORT);
}

void UdpChat::onHeartbeat()
{
    sendPresence();

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    QStringList toRemove;
    for (auto it = m_peers.begin(); it != m_peers.end(); ++it) {
        if (now - it.value().lastSeen > PEER_TIMEOUT) {
            toRemove.append(it.key());
        }
    }
    for (const QString &nick : toRemove) {
        m_peers.remove(nick);
        emit peerLeft(nick);
    }
}

void UdpChat::onReadyRead()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        QHostAddress senderAddr;
        quint16 senderPort;
        m_socket->readDatagram(datagram.data(), datagram.size(), &senderAddr, &senderPort);
        processDatagram(datagram, senderAddr, senderPort);
    }
}

void UdpChat::processDatagram(const QByteArray &data, const QHostAddress &/*senderAddr*/, quint16 /*senderPort*/)
{
    QString message = QString::fromUtf8(data);
    if (message.startsWith("PRESENCE:")) {
        QString nick = message.mid(9);
        if (nick == m_myNickname) return;

        qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (!m_peers.contains(nick)) {
            PeerInfo info;
            info.address = QHostAddress::LocalHost;
            info.port = DEFAULT_PORT;
            info.lastSeen = now;
            m_peers[nick] = info;
            emit newPeer(nick);
        } else {
            m_peers[nick].lastSeen = now;
        }
    }
    else if (message.startsWith("MSG:")) {
        int firstColon = message.indexOf(':', 4);
        if (firstColon != -1) {
            QString senderNick = message.mid(4, firstColon - 4);
            if (senderNick == m_myNickname) return;
            QString text = message.mid(firstColon + 1);
            // Защита от дублей (500 мс)
            qint64 now = QDateTime::currentMSecsSinceEpoch();
            if (m_lastMsgTime.contains(senderNick) && (now - m_lastMsgTime[senderNick] < 500)) {
                return;
            }
            m_lastMsgTime[senderNick] = now;
            // Добавляем отправителя, если его ещё нет
            if (!m_peers.contains(senderNick)) {
                PeerInfo info;
                info.address = QHostAddress::LocalHost;
                info.port = DEFAULT_PORT;
                info.lastSeen = now;
                m_peers[senderNick] = info;
                emit newPeer(senderNick);
            }
            emit messageReceived(senderNick, text);
        }
    }
}

void UdpChat::sendToAll(const QString &text)
{
    QString fullMsg = QString("MSG:%1:%2").arg(m_myNickname, text);
    QByteArray datagram = fullMsg.toUtf8();
    m_socket->writeDatagram(datagram, QHostAddress::LocalHost, DEFAULT_PORT);
    m_socket->writeDatagram(datagram, QHostAddress::Broadcast, DEFAULT_PORT);
}

void UdpChat::sendPrivate(const QString &targetNick, const QString &text)
{
    auto it = m_peers.find(targetNick);
    if (it != m_peers.end()) {
        QString fullMsg = QString("MSG:%1:%2").arg(m_myNickname, text);
        QByteArray datagram = fullMsg.toUtf8();
        m_socket->writeDatagram(datagram, it.value().address, DEFAULT_PORT);
    }
}

QStringList UdpChat::getPeerNicks() const
{
    return m_peers.keys();
}
