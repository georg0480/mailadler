/*
 * mailadler - IMAP Connection
 * Copyright (c) 2026 Georg Dahmen
 * GPLv3 License
 */

#ifndef IMAPCONNECTION_H
#define IMAPCONNECTION_H

#include <QObject>
#include <QSslSocket>
#include <QQueue>
#include <QTimer>

struct EmailHeader {
    QString uid;
    QString from;
    QString to;
    QString subject;
    QString date;
    bool seen;
};

class ImapConnection : public QObject
{
    Q_OBJECT

public:
    explicit ImapConnection(QObject *parent = nullptr);
    ~ImapConnection();

    void connectToServer(const QString &host, int port = 993);
    void login(const QString &user, const QString &password);
    void selectFolder(const QString &folder);
    void fetchHeaders(int count = 50);
    void logout();

    bool isConnected() const { return m_connected; }
    bool isAuthenticated() const { return m_authenticated; }

signals:
    void connected();
    void authenticated();
    void folderSelected(const QString &folder, int messageCount);
    void headersReceived(const QList<EmailHeader> &headers);
    void error(const QString &message);
    void statusMessage(const QString &message);

private slots:
    void onConnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError error);
    void onSslErrors(const QList<QSslError> &errors);

private:
    void sendCommand(const QString &command);
    void processResponse(const QString &line);
    void parseHeaders(const QString &response);

    QSslSocket *m_socket;
    QString m_buffer;
    QString m_currentCommand;
    int m_commandTag;
    bool m_connected;
    bool m_authenticated;
    
    QString m_user;
    QString m_password;
    QString m_currentFolder;
    int m_messageCount;
    
    QList<EmailHeader> m_headers;
};

#endif // IMAPCONNECTION_H
