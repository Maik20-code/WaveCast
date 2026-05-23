#ifndef UDPCHAT_H
#define UDPCHAT_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QMap>
#include <QHostAddress>

class UdpChat : public QObject
{
    Q_OBJECT
public:
    explicit UdpChat(const QString &myNickname, QObject *parent = nullptr);
    ~UdpChat();

    // Отправить сообщение всем известным пирам
    void sendToAll(const QString &message);
    // Отправить личное сообщение указанному пиру (по нику)
    void sendPrivate(const QString &targetNick, const QString &message);
    // Получить список ников всех активных пиров
    QStringList getPeerNicks() const;

signals:
    void newPeer(const QString &nick);          // появился новый участник
    void peerLeft(const QString &nick);         // участник покинул сеть
    void messageReceived(const QString &senderNick, const QString &text); // получено сообщение
    void error(const QString &errorText);       // критическая ошибка

private slots:
    void onReadyRead();      // пришли данные на сокет
    void onHeartbeat();      // таймер для отправки присутствия и проверки таймаутов

private:
    QMap<QString, qint64> m_lastMsgTime;
    static constexpr quint16 DEFAULT_PORT = 42123;
    void sendPresence();     // отправить широковещательный пакет "я здесь"
    void processDatagram(const QByteArray &data, const QHostAddress &senderAddr, quint16 senderPort);

    QUdpSocket *m_socket;
    QTimer *m_heartbeatTimer;
    QString m_myNickname;
    quint16 m_port;          // порт, на котором слушаем

    struct PeerInfo {
        QHostAddress address;
        quint16 port;
        qint64 lastSeen;     // время последнего получения PRESENCE (мс)
    };
    QMap<QString, PeerInfo> m_peers;

    static constexpr qint64 HEARTBEAT_INTERVAL = 5000;   // 5 секунд
    static constexpr qint64 PEER_TIMEOUT = 15000;        // 15 секунд без вестей -> пир считается ушедшим
};

#endif // UDPCHAT_H
