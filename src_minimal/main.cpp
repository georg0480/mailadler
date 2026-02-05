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
#include <QListWidget>
#include <QTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QLabel>

class MailAdlerWindow : public QMainWindow
{
    Q_OBJECT

public:
    MailAdlerWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle(tr("mailadler - E-Mail Client"));
        resize(1200, 800);

        setupMenus();
        setupToolbar();
        setupDocks();
        setupCentralWidget();
        setupStatusBar();
    }

private:
    void setupMenus()
    {
        // Datei-Menü
        QMenu *fileMenu = menuBar()->addMenu(tr("&Datei"));
        fileMenu->addAction(tr("Neue Nachricht"), this, []() {}, QKeySequence::New);
        fileMenu->addAction(tr("E-Mails abrufen"), this, []() {}, QKeySequence(Qt::Key_F5));
        fileMenu->addSeparator();
        fileMenu->addAction(tr("Drucken..."), this, []() {}, QKeySequence::Print);
        fileMenu->addAction(tr("Als PDF speichern..."), this, []() {});
        fileMenu->addSeparator();
        fileMenu->addAction(tr("Beenden"), this, &QMainWindow::close, QKeySequence::Quit);

        // Bearbeiten-Menü
        QMenu *editMenu = menuBar()->addMenu(tr("&Bearbeiten"));
        editMenu->addAction(tr("Rückgängig"), this, []() {}, QKeySequence::Undo);
        editMenu->addAction(tr("Wiederherstellen"), this, []() {}, QKeySequence::Redo);
        editMenu->addSeparator();
        editMenu->addAction(tr("Ausschneiden"), this, []() {}, QKeySequence::Cut);
        editMenu->addAction(tr("Kopieren"), this, []() {}, QKeySequence::Copy);
        editMenu->addAction(tr("Einfügen"), this, []() {}, QKeySequence::Paste);
        editMenu->addSeparator();
        editMenu->addAction(tr("Suchen..."), this, []() {}, QKeySequence::Find);

        // Ansicht-Menü
        QMenu *viewMenu = menuBar()->addMenu(tr("&Ansicht"));
        viewMenu->addAction(tr("Ordner anzeigen"));
        viewMenu->addAction(tr("Vorschau anzeigen"));

        // Nachricht-Menü
        QMenu *msgMenu = menuBar()->addMenu(tr("&Nachricht"));
        msgMenu->addAction(tr("Antworten"), this, []() {}, QKeySequence(Qt::CTRL | Qt::Key_R));
        msgMenu->addAction(tr("Allen antworten"), this, []() {}, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
        msgMenu->addAction(tr("Weiterleiten"), this, []() {}, QKeySequence(Qt::CTRL | Qt::Key_F));
        msgMenu->addSeparator();
        msgMenu->addAction(tr("Als gelesen markieren"));
        msgMenu->addAction(tr("Als ungelesen markieren"));
        msgMenu->addSeparator();
        msgMenu->addAction(tr("Löschen"), this, []() {}, QKeySequence::Delete);

        // Hilfe-Menü
        QMenu *helpMenu = menuBar()->addMenu(tr("&Hilfe"));
        helpMenu->addAction(tr("Über mailadler..."), this, []() {});
    }

    void setupToolbar()
    {
        QToolBar *toolbar = addToolBar(tr("Hauptleiste"));
        toolbar->addAction(tr("Abrufen"));
        toolbar->addAction(tr("Neue Nachricht"));
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
        QTreeWidget *folderTree = new QTreeWidget();
        folderTree->setHeaderHidden(true);
        
        QTreeWidgetItem *inbox = new QTreeWidgetItem(folderTree, QStringList() << tr("Posteingang"));
        inbox->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
        
        QTreeWidgetItem *sent = new QTreeWidgetItem(folderTree, QStringList() << tr("Gesendet"));
        sent->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
        
        QTreeWidgetItem *drafts = new QTreeWidgetItem(folderTree, QStringList() << tr("Entwürfe"));
        drafts->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
        
        QTreeWidgetItem *trash = new QTreeWidgetItem(folderTree, QStringList() << tr("Papierkorb"));
        trash->setIcon(0, style()->standardIcon(QStyle::SP_TrashIcon));
        
        folderDock->setWidget(folderTree);
        addDockWidget(Qt::LeftDockWidgetArea, folderDock);
    }

    void setupCentralWidget()
    {
        QSplitter *splitter = new QSplitter(Qt::Vertical, this);

        // Mail-Liste (oben)
        QListWidget *mailList = new QListWidget();
        mailList->addItem(tr("📧 Max Mustermann - Wichtige Nachricht"));
        mailList->addItem(tr("📧 Anna Schmidt - Meeting morgen"));
        mailList->addItem(tr("📧 Support - Ihre Anfrage #12345"));
        splitter->addWidget(mailList);

        // Mail-Vorschau (unten)
        QTextEdit *preview = new QTextEdit();
        preview->setReadOnly(true);
        preview->setHtml(
            "<h2>Wichtige Nachricht</h2>"
            "<p><b>Von:</b> Max Mustermann &lt;max@example.com&gt;<br>"
            "<b>An:</b> Sie<br>"
            "<b>Datum:</b> 04.02.2026 18:30</p>"
            "<hr>"
            "<p>Hallo,</p>"
            "<p>Dies ist eine Test-Nachricht in mailadler.</p>"
            "<p>Mit freundlichen Grüßen,<br>Max</p>"
        );
        splitter->addWidget(preview);

        splitter->setSizes(QList<int>() << 300 << 500);
        setCentralWidget(splitter);
    }

    void setupStatusBar()
    {
        statusBar()->showMessage(tr("Bereit - 3 Nachrichten"));
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("mailadler");
    app.setOrganizationName("Georg Dahmen");
    app.setApplicationVersion("0.1.0");

    MailAdlerWindow window;
    window.show();

    return app.exec();
}

#include "main.moc"
