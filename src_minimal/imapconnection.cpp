/*
 * mailadler - IMAP Connection
 * Copyright (c) 2026 Georg Dahmen
 * GPLv3 License
 */

#include "imapconnection.h"
#include <QDebug>
#include <QRegularExpression>

ImapConnection::ImapConnection(QObject *parent)
    : QObject(parent)
    , m_socket(new QSslSocket(this))
    , m_commandTag(0)
    , m_connected(false)
    , m_authenticated(false)
    , m_messageCount(0)
{
    connect(m_socket, &QSslSocket::encrypted, this, &ImapConnection::onConnected);
    connect(m_socket, &QSslSocket::readyRead, this, &ImapConnection::onReadyRead);
    connect(m_socket, &QSslSocket::errorOccurred, this, &ImapConnection::onError);
    connect(m_socket, &QSslSocket::sslErrors, this, &ImapConnection::onSslErrors);
}

ImapConnection::~ImapConnection()
{
    if (m_connected) {
        logout();
    }
}

void ImapConnection::connectToServer(const QString &host, int port)
{
    emit statusMessage(tr("Verbinde mit %1:%2...").arg(host).arg(port));
    m_socket->connectToHostEncrypted(host, port);
}

void ImapConnection::login(const QString &user, const QString &password)
{
    m_user = user;
    m_password = password;
    emit statusMessage(tr("Anmelden als %1...").arg(user));
    sendCommand(QString("LOGIN \"%1\" \"%2\"").arg(user, password));
    m_currentCommand = "LOGIN";
}

void ImapConnection::selectFolder(const QString &folder)
{
    m_currentFolder = folder;
    emit statusMessage(tr("Öffne Ordner %1...").arg(folder));
    sendCommand(QString("SELECT \"%1\"").arg(folder));
    m_currentCommand = "SELECT";
}

void ImapConnection::fetchHeaders(int count)
{
    if (m_messageCount == 0) {
        emit headersReceived(QList<EmailHeader>());
        return;
    }
    
    int start = qMax(1, m_messageCount - count + 1);
    emit statusMessage(tr("Lade Nachrichten %1-%2...").arg(start).arg(m_messageCount));
    sendCommand(QString("FETCH %1:%2 (UID FLAGS ENVELOPE)").arg(start).arg(m_messageCount));
    m_currentCommand = "FETCH";
    m_headers.clear();
}

void ImapConnection::logout()
{
    sendCommand("LOGOUT");
    m_currentCommand = "LOGOUT";
}

void ImapConnection::onConnected()
{
    m_connected = true;
    emit statusMessage(tr("Verbunden - warte auf Server..."));
    emit connected();
}

void ImapConnection::onReadyRead()
{
    m_buffer += QString::fromUtf8(m_socket->readAll());
    
    while (m_buffer.contains("\r\n")) {
        int idx = m_buffer.indexOf("\r\n");
        QString line = m_buffer.left(idx);
        m_buffer = m_buffer.mid(idx + 2);
        
        processResponse(line);
    }
}

void ImapConnection::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    emit this->error(tr("Verbindungsfehler: %1").arg(m_socket->errorString()));
}

void ImapConnection::onSslErrors(const QList<QSslError> &errors)
{
    for (const QSslError &e : errors) {
        qWarning() << "SSL:" << e.errorString();
    }
    // Für Entwicklung: Ignorieren (NICHT in Produktion!)
    m_socket->ignoreSslErrors();
}

void ImapConnection::sendCommand(const QString &command)
{
    m_commandTag++;
    QString fullCommand = QString("A%1 %2\r\n").arg(m_commandTag, 4, 10, QChar('0')).arg(command);
    
    // Passwort nicht loggen
    if (!command.startsWith("LOGIN")) {
        qDebug() << "IMAP >" << fullCommand.trimmed();
    } else {
        qDebug() << "IMAP > LOGIN ***";
    }
    
    m_socket->write(fullCommand.toUtf8());
}

void ImapConnection::processResponse(const QString &line)
{
    qDebug() << "IMAP <" << line.left(100);
    
    // Server-Greeting
    if (line.startsWith("* OK")) {
        emit statusMessage(tr("Server bereit"));
        return;
    }
    
    // EXISTS - Anzahl der Nachrichten
    static QRegularExpression existsRx(R"(\* (\d+) EXISTS)");
    auto existsMatch = existsRx.match(line);
    if (existsMatch.hasMatch()) {
        m_messageCount = existsMatch.captured(1).toInt();
        return;
    }
    
    // FETCH Response - Header parsen
    if (line.startsWith("* ") && line.contains("FETCH")) {
        parseHeaders(line);
        return;
    }
    
    // Tagged Response (Abschluss eines Befehls)
    QString tag = QString("A%1").arg(m_commandTag, 4, 10, QChar('0'));
    if (line.startsWith(tag)) {
        if (line.contains("OK")) {
            if (m_currentCommand == "LOGIN") {
                m_authenticated = true;
                emit statusMessage(tr("Anmeldung erfolgreich"));
                emit authenticated();
            } else if (m_currentCommand == "SELECT") {
                emit statusMessage(tr("Ordner %1: %2 Nachrichten").arg(m_currentFolder).arg(m_messageCount));
                emit folderSelected(m_currentFolder, m_messageCount);
            } else if (m_currentCommand == "FETCH") {
                emit statusMessage(tr("%1 Nachrichten geladen").arg(m_headers.size()));
                emit headersReceived(m_headers);
            }
        } else if (line.contains("NO") || line.contains("BAD")) {
            emit error(tr("Befehl fehlgeschlagen: %1").arg(line));
        }
    }
}

void ImapConnection::parseHeaders(const QString &response)
{
    EmailHeader header;
    
    // UID extrahieren
    static QRegularExpression uidRx(R"(UID (\d+))");
    auto uidMatch = uidRx.match(response);
    if (uidMatch.hasMatch()) {
        header.uid = uidMatch.captured(1);
    }
    
    // FLAGS prüfen (SEEN)
    header.seen = response.contains("\\Seen");
    
    // ENVELOPE parsen (vereinfacht)
    // Format: ENVELOPE ("date" "subject" ((from)) ((sender)) ((reply-to)) ((to)) ...)
    static QRegularExpression envRx(R"(ENVELOPE \(\"([^\"]*)\"\s*\"([^\"]*)\")");
    auto envMatch = envRx.match(response);
    if (envMatch.hasMatch()) {
        header.date = envMatch.captured(1);
        header.subject = envMatch.captured(2);
        
        // Dekodiere MIME-Encoded Subjects
        if (header.subject.contains("=?")) {
            // Einfache Dekodierung (vollständige Impl. später)
            header.subject = header.subject.replace("=?UTF-8?Q?", "").replace("?=", "");
            header.subject = header.subject.replace("=?utf-8?Q?", "").replace("?=", "");
            header.subject = header.subject.replace("_", " ");
        }
    }
    
    // From-Adresse extrahieren (vereinfacht)
    static QRegularExpression fromRx(R"(\(\(\"([^\"]*)\"\s*NIL\s*\"([^\"]*)\"\s*\"([^\"]*)\"\)\))");
    auto fromMatch = fromRx.match(response);
    if (fromMatch.hasMatch()) {
        QString name = fromMatch.captured(1);
        QString mailbox = fromMatch.captured(2);
        QString host = fromMatch.captured(3);
        if (name.isEmpty()) {
            header.from = QString("%1@%2").arg(mailbox, host);
        } else {
            header.from = QString("%1 <%2@%3>").arg(name, mailbox, host);
        }
    }
    
    if (!header.uid.isEmpty()) {
        m_headers.prepend(header); // Neueste zuerst
    }
}
