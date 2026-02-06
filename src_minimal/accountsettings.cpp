/*
 * mailadler - Account Settings Dialog
 * Copyright (c) 2026 Georg Dahmen
 * GPLv3 License
 */

#include "accountsettings.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>

AccountSettingsDialog::AccountSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Konto einrichten"));
    setMinimumWidth(400);

    auto *layout = new QVBoxLayout(this);

    // Provider-Auswahl
    auto *providerGroup = new QGroupBox(tr("E-Mail-Anbieter"));
    auto *providerLayout = new QVBoxLayout(providerGroup);
    
    auto *providerCombo = new QComboBox();
    providerCombo->addItem(tr("Anbieter auswählen..."));
    for (const auto &p : knownProviders()) {
        providerCombo->addItem(p.name);
    }
    providerLayout->addWidget(providerCombo);
    layout->addWidget(providerGroup);

    // Konto-Daten
    auto *accountGroup = new QGroupBox(tr("Anmeldedaten"));
    auto *accountLayout = new QFormLayout(accountGroup);
    
    m_email = new QLineEdit();
    m_email->setPlaceholderText("beispiel@gmx.de");
    accountLayout->addRow(tr("E-Mail-Adresse:"), m_email);
    
    m_password = new QLineEdit();
    m_password->setEchoMode(QLineEdit::Password);
    m_password->setPlaceholderText(tr("Passwort"));
    accountLayout->addRow(tr("Passwort:"), m_password);
    
    layout->addWidget(accountGroup);

    // Server-Einstellungen
    auto *serverGroup = new QGroupBox(tr("Server-Einstellungen"));
    auto *serverLayout = new QFormLayout(serverGroup);
    
    m_imapServer = new QLineEdit();
    m_imapServer->setPlaceholderText("imap.gmx.net");
    serverLayout->addRow(tr("IMAP-Server:"), m_imapServer);
    
    m_imapPort = new QSpinBox();
    m_imapPort->setRange(1, 65535);
    m_imapPort->setValue(993);
    serverLayout->addRow(tr("IMAP-Port:"), m_imapPort);
    
    layout->addWidget(serverGroup);

    // Provider-Auswahl verbinden
    connect(providerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, [this, providerCombo](int index) {
        if (index > 0) {
            auto providers = knownProviders();
            const auto &p = providers[index - 1];
            m_imapServer->setText(p.imapServer);
            m_imapPort->setValue(p.imapPort);
        }
    });

    // Buttons
    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        saveSettings();
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    loadSettings();
}

void AccountSettingsDialog::loadSettings()
{
    QSettings settings;
    settings.beginGroup("Account");
    m_email->setText(settings.value("email").toString());
    // Passwort laden (Base64-encoded für minimalen Schutz)
    QByteArray encoded = settings.value("password").toByteArray();
    if (!encoded.isEmpty()) {
        m_password->setText(QString::fromUtf8(QByteArray::fromBase64(encoded)));
    }
    m_imapServer->setText(settings.value("imapServer", "imap.gmx.net").toString());
    m_imapPort->setValue(settings.value("imapPort", 993).toInt());
    settings.endGroup();
}

void AccountSettingsDialog::saveSettings()
{
    QSettings settings;
    settings.beginGroup("Account");
    settings.setValue("email", m_email->text());
    // Passwort speichern (Base64-encoded)
    settings.setValue("password", m_password->text().toUtf8().toBase64());
    settings.setValue("imapServer", m_imapServer->text());
    settings.setValue("imapPort", m_imapPort->value());
    settings.endGroup();
}
