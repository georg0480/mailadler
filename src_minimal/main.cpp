/*
 * mailadler - E-Mail Client
 * Copyright (c) 2026 Georg Dahmen
 * GPLv3 License
 */

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QSettings>

#include <QInputDialog>

#include "imapconnection.h"
#include "accountsettings.h"

class MailAdlerWindow : public QMainWindow
{
    Q_OBJECT

public:
    MailAdlerWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle(tr("mailadler - E-Mail Client"));
        resize(1200, 800);

        m_imap = new ImapConnection(this);
        connectImapSignals();

        setupMenus();
        setupToolbar();
        setupDocks();
        setupCentralWidget();
        setupStatusBar();

        // Automatisch verbinden wenn Einstellungen vorhanden
        QSettings settings;
        if (!settings.value("Account/email").toString().isEmpty()) {
            QTimer::singleShot(500, this, &MailAdlerWindow::onFetchMails);
        }
    }

private slots:
    void onNewMail()
    {
        QMessageBox::information(this, tr("Neue Nachricht"), 
            tr("Compose-Dialog wird in Phase C implementiert."));
    }

    void onFetchMails()
    {
        QSettings settings;
        QString email = settings.value("Account/email").toString();
        QString server = settings.value("Account/imapServer", "imap.gmx.net").toString();
        int port = settings.value("Account/imapPort", 993).toInt();
        
        // Gespeichertes Passwort laden
        QByteArray encoded = settings.value("Account/password").toByteArray();
        QString password = QString::fromUtf8(QByteArray::fromBase64(encoded));

        if (email.isEmpty() || password.isEmpty()) {
            onAccountSettings();
            return;
        }

        m_imap->connectToServer(server, port);
        m_pendingPassword = password;
    }

    void onAccountSettings()
    {
        AccountSettingsDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            statusBar()->showMessage(tr("Konto gespeichert: %1").arg(dialog.email()));
        }
    }

    void onImapConnected()
    {
        QSettings settings;
        QString email = settings.value("Account/email").toString();
        m_imap->login(email, m_pendingPassword);
        m_pendingPassword.clear();
    }

    void onImapAuthenticated()
    {
        QString folder = m_pendingFolder.isEmpty() ? "INBOX" : m_pendingFolder;
        m_pendingFolder.clear();
        m_imap->selectFolder(folder);
    }

    void onFolderSelected(const QString &folder, int count)
    {
        Q_UNUSED(folder)
        Q_UNUSED(count)
        m_imap->fetchHeaders(50);
    }

    void onHeadersReceived(const QList<EmailHeader> &headers)
    {
        m_mailTable->setRowCount(0);
        
        for (const auto &h : headers) {
            int row = m_mailTable->rowCount();
            m_mailTable->insertRow(row);
            
            // Ungelesen-Markierung
            QString prefix = h.seen ? "" : "📧 ";
            
            auto *fromItem = new QTableWidgetItem(prefix + h.from);
            if (!h.seen) {
                QFont font = fromItem->font();
                font.setBold(true);
                fromItem->setFont(font);
            }
            m_mailTable->setItem(row, 0, fromItem);
            
            auto *subjectItem = new QTableWidgetItem(h.subject);
            if (!h.seen) {
                QFont font = subjectItem->font();
                font.setBold(true);
                subjectItem->setFont(font);
            }
            m_mailTable->setItem(row, 1, subjectItem);
            
            m_mailTable->setItem(row, 2, new QTableWidgetItem(h.date));
            
            // UID speichern
            m_mailTable->item(row, 0)->setData(Qt::UserRole, h.uid);
        }
        
        statusBar()->showMessage(tr("%1 Nachrichten").arg(headers.size()));
    }

    void onImapError(const QString &msg)
    {
        QMessageBox::warning(this, tr("Verbindungsfehler"), msg);
    }

    void onImapStatus(const QString &msg)
    {
        statusBar()->showMessage(msg);
    }

    void onMailSelected(int row, int column)
    {
        Q_UNUSED(column)
        if (row < 0) return;
        
        QString from = m_mailTable->item(row, 0)->text();
        QString subject = m_mailTable->item(row, 1)->text();
        QString date = m_mailTable->item(row, 2)->text();
        
        m_preview->setHtml(QString(
            "<h2>%1</h2>"
            "<p><b>Von:</b> %2<br>"
            "<b>Datum:</b> %3</p>"
            "<hr>"
            "<p><i>Nachrichteninhalt wird in Phase C geladen...</i></p>"
        ).arg(subject.toHtmlEscaped(), from.toHtmlEscaped(), date.toHtmlEscaped()));
    }

private:
    void connectImapSignals()
    {
        connect(m_imap, &ImapConnection::connected, 
                this, &MailAdlerWindow::onImapConnected);
        connect(m_imap, &ImapConnection::authenticated, 
                this, &MailAdlerWindow::onImapAuthenticated);
        connect(m_imap, &ImapConnection::folderSelected, 
                this, &MailAdlerWindow::onFolderSelected);
        connect(m_imap, &ImapConnection::headersReceived, 
                this, &MailAdlerWindow::onHeadersReceived);
        connect(m_imap, &ImapConnection::error, 
                this, &MailAdlerWindow::onImapError);
        connect(m_imap, &ImapConnection::statusMessage, 
                this, &MailAdlerWindow::onImapStatus);
    }

    void setupMenus()
    {
        // Datei-Menü
        QMenu *fileMenu = menuBar()->addMenu(tr("&Datei"));
        fileMenu->addAction(tr("Neue Nachricht"), QKeySequence::New, this, &MailAdlerWindow::onNewMail);
        fileMenu->addAction(tr("E-Mails abrufen"), QKeySequence(Qt::Key_F5), this, &MailAdlerWindow::onFetchMails);
        fileMenu->addSeparator();
        fileMenu->addAction(tr("Konto einrichten..."), this, &MailAdlerWindow::onAccountSettings);
        fileMenu->addSeparator();
        fileMenu->addAction(tr("Beenden"), QKeySequence::Quit, this, &QMainWindow::close);

        // Bearbeiten-Menü
        QMenu *editMenu = menuBar()->addMenu(tr("&Bearbeiten"));
        editMenu->addAction(tr("Suchen..."), QKeySequence::Find, this, []() {});

        // Ansicht-Menü
        QMenu *viewMenu = menuBar()->addMenu(tr("&Ansicht"));
        viewMenu->addAction(tr("Ordner anzeigen"));
        viewMenu->addAction(tr("Vorschau anzeigen"));

        // Nachricht-Menü
        QMenu *msgMenu = menuBar()->addMenu(tr("&Nachricht"));
        msgMenu->addAction(tr("Antworten"), QKeySequence(Qt::CTRL | Qt::Key_R), this, []() {});
        msgMenu->addAction(tr("Weiterleiten"), QKeySequence(Qt::CTRL | Qt::Key_F), this, []() {});
        msgMenu->addSeparator();
        msgMenu->addAction(tr("Löschen"), QKeySequence::Delete, this, []() {});

        // Hilfe-Menü
        QMenu *helpMenu = menuBar()->addMenu(tr("&Hilfe"));
        helpMenu->addAction(tr("Über mailadler..."), this, [this]() {
            QMessageBox::about(this, tr("Über mailadler"),
                tr("<h2>mailadler 0.1.0</h2>"
                   "<p>E-Mail Client für Windows, Linux und macOS</p>"
                   "<p>Copyright © 2026 Georg Dahmen</p>"
                   "<p>Lizenz: GPLv3</p>"));
        });
    }

    void setupToolbar()
    {
        QToolBar *toolbar = addToolBar(tr("Hauptleiste"));
        toolbar->setIconSize(QSize(24, 24));
        
        auto *fetchAction = toolbar->addAction(tr("Abrufen"));
        connect(fetchAction, &QAction::triggered, this, &MailAdlerWindow::onFetchMails);
        
        auto *newAction = toolbar->addAction(tr("Neue Nachricht"));
        connect(newAction, &QAction::triggered, this, &MailAdlerWindow::onNewMail);
        
        toolbar->addSeparator();
        toolbar->addAction(tr("Antworten"));
        toolbar->addAction(tr("Weiterleiten"));
        toolbar->addSeparator();
        toolbar->addAction(tr("Löschen"));
    }

    void setupDocks()
    {
        // Ordner-Dock (links)
        QDockWidget *folderDock = new QDockWidget(tr("Ordner"), this);
        m_folderTree = new QTreeWidget();
        m_folderTree->setHeaderHidden(true);
        
        // IMAP-Ordnernamen als Data speichern
        QTreeWidgetItem *inbox = new QTreeWidgetItem(m_folderTree, QStringList() << tr("Posteingang"));
        inbox->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
        inbox->setData(0, Qt::UserRole, "INBOX");
        
        QTreeWidgetItem *sent = new QTreeWidgetItem(m_folderTree, QStringList() << tr("Gesendet"));
        sent->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
        sent->setData(0, Qt::UserRole, "Sent");  // GMX: "Gesendet" oder "Sent"
        
        QTreeWidgetItem *drafts = new QTreeWidgetItem(m_folderTree, QStringList() << tr("Entwürfe"));
        drafts->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
        drafts->setData(0, Qt::UserRole, "Drafts");
        
        QTreeWidgetItem *spam = new QTreeWidgetItem(m_folderTree, QStringList() << tr("Spam"));
        spam->setIcon(0, style()->standardIcon(QStyle::SP_DialogNoButton));
        spam->setData(0, Qt::UserRole, "Spam");
        
        QTreeWidgetItem *trash = new QTreeWidgetItem(m_folderTree, QStringList() << tr("Papierkorb"));
        trash->setIcon(0, style()->standardIcon(QStyle::SP_TrashIcon));
        trash->setData(0, Qt::UserRole, "Trash");
        
        connect(m_folderTree, &QTreeWidget::itemClicked, this, &MailAdlerWindow::onFolderClicked);
        
        folderDock->setWidget(m_folderTree);
        addDockWidget(Qt::LeftDockWidgetArea, folderDock);
    }
    
    void onFolderClicked(QTreeWidgetItem *item, int column)
    {
        Q_UNUSED(column)
        QString folder = item->data(0, Qt::UserRole).toString();
        if (folder.isEmpty()) return;
        
        if (!m_imap->isAuthenticated()) {
            onFetchMails();  // Erst verbinden
            m_pendingFolder = folder;
        } else {
            m_imap->selectFolder(folder);
        }
    }

    void setupCentralWidget()
    {
        QSplitter *splitter = new QSplitter(Qt::Vertical, this);

        // Mail-Tabelle (oben)
        m_mailTable = new QTableWidget();
        m_mailTable->setColumnCount(3);
        m_mailTable->setHorizontalHeaderLabels({tr("Von"), tr("Betreff"), tr("Datum")});
        m_mailTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_mailTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        m_mailTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        m_mailTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_mailTable->setSelectionMode(QAbstractItemView::SingleSelection);
        m_mailTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_mailTable->verticalHeader()->hide();
        
        connect(m_mailTable, &QTableWidget::cellClicked, 
                this, &MailAdlerWindow::onMailSelected);
        
        splitter->addWidget(m_mailTable);

        // Mail-Vorschau (unten)
        m_preview = new QTextEdit();
        m_preview->setReadOnly(true);
        m_preview->setHtml(
            "<h2>Willkommen bei mailadler</h2>"
            "<p>Drücke <b>F5</b> oder klicke <b>Abrufen</b> um E-Mails zu laden.</p>"
            "<p>Richte zuerst dein Konto ein unter <b>Datei → Konto einrichten</b>.</p>"
        );
        splitter->addWidget(m_preview);

        splitter->setSizes(QList<int>() << 300 << 500);
        setCentralWidget(splitter);
    }

    void setupStatusBar()
    {
        statusBar()->showMessage(tr("Bereit"));
    }

    ImapConnection *m_imap;
    QTableWidget *m_mailTable;
    QTextEdit *m_preview;
    QTreeWidget *m_folderTree;
    QString m_pendingPassword;
    QString m_pendingFolder;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("mailadler");
    app.setOrganizationName("Georg Dahmen");
    app.setOrganizationDomain("mailadler.de");
    app.setApplicationVersion("0.1.0");

    MailAdlerWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"
