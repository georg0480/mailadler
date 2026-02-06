/*
 * mailadler - Account Settings Dialog
 * Copyright (c) 2026 Georg Dahmen
 * GPLv3 License
 */

#ifndef ACCOUNTSETTINGS_H
#define ACCOUNTSETTINGS_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QSettings>

class AccountSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountSettingsDialog(QWidget *parent = nullptr);

    QString imapServer() const { return m_imapServer->text(); }
    int imapPort() const { return m_imapPort->value(); }
    QString email() const { return m_email->text(); }
    QString password() const { return m_password->text(); }

    void loadSettings();
    void saveSettings();

private:
    QLineEdit *m_email;
    QLineEdit *m_password;
    QLineEdit *m_imapServer;
    QSpinBox *m_imapPort;
};

// Bekannte Provider
struct MailProvider {
    QString name;
    QString imapServer;
    int imapPort;
    QString smtpServer;
    int smtpPort;
};

inline QList<MailProvider> knownProviders() {
    return {
        {"GMX", "imap.gmx.net", 993, "mail.gmx.net", 587},
        {"Web.de", "imap.web.de", 993, "smtp.web.de", 587},
        {"Gmail", "imap.gmail.com", 993, "smtp.gmail.com", 587},
        {"Outlook", "outlook.office365.com", 993, "smtp.office365.com", 587},
        {"Yahoo", "imap.mail.yahoo.com", 993, "smtp.mail.yahoo.com", 587},
        {"T-Online", "secureimap.t-online.de", 993, "securesmtp.t-online.de", 587},
        {"iCloud", "imap.mail.me.com", 993, "smtp.mail.me.com", 587},
    };
}

#endif // ACCOUNTSETTINGS_H
