# Sicherheit & Verschlüsselung - Mail-Adler

## 1. End-to-End Verschlüsselung (E2EE)

### 1.1 Unterstützte Standards

Mail-Adler wird folgende E2EE-Standards unterstützen:

| Standard | Beschreibung | Unterstützung | Status |
|----------|-------------|---------------|--------|
| **OpenPGP (RFC 4880)** | Public-Key Verschlüsselung | ✅ Voll | Phase C |
| **S/MIME (RFC 5751)** | Certificate-based | ✅ Geplant | Phase D |
| **Pre-shared Key (PSK)** | Manuelle Schlüsselverwaltung | ✅ Phase B | Beta |

### 1.2 Pre-Shared Key (PSK) - Phase B

Für Phase B wird ein einfaches PSK-System implementiert:

#### Szenario: Gruppe mit gemeinsamer Verschlüsselung

**Beteiligung:** Alice, Bob, Charlie

**Workflow:**

1. **Schlüssel-Generierung**
```cpp
// src/encryption/KeyGenerator.h/cpp
class KeyGenerator {
public:
    // Generiert sicheren zufälligen Schlüssel
    static QString generateGroupKey(int lengthBytes = 32);  // 256-bit
    
    // Beispiel: "K9mX2pL7vQ4bJ8fN3gW5hR1sT6cD9jE2uP8vM4nO7qA"
};
```

**Schlüssel-Format:** Base64, 44 Zeichen (256-bit)

2. **Schlüssel-Verteilung**
   - Nicht per Email! Nur out-of-band (Telefon, Signal, Persönlich)
   - In lokaler Datei speichern: `~/.config/mail-adler/group_keys.json`

```json
{
  "group_keys": [
    {
      "group_id": "uuid-1234",
      "group_name": "Firmenteam A",
      "members": ["alice@gmx.de", "bob@web.de", "charlie@gmail.com"],
      "key": "K9mX2pL7vQ4bJ8fN3gW5hR1sT6cD9jE2uP8vM4nO7qA",
      "key_expiry": "2026-02-03T00:00:00Z",
      "created_at": "2025-02-03T12:00:00Z"
    }
  ]
}
```

3. **Verschlüsselte Email versenden**

```
Empfänger: bob@web.de, charlie@gmail.com
Betreff: [ENCRYPTED] Vertrauliche Mitteilung

────────────────────────────────────────
BEGIN ENCRYPTED MESSAGE
────────────────────────────────────────
AES-256-GCM ENCRYPTED CONTENT
ENC_DATA_LENGTH: 2048
NONCE: 16 bytes
AUTHENTICATION_TAG: 16 bytes
────────────────────────────────────────
END ENCRYPTED MESSAGE
────────────────────────────────────────
```

4. **Verschlüsselung & Entschlüsselung**

```cpp
// src/encryption/E2EEncryption.h/cpp
class E2EEncryption {
public:
    // Verschlüsseln
    static QByteArray encrypt(
        const QString &plaintext,
        const QString &groupKey
    );  // Returns: Encrypted data with nonce + tag
    
    // Entschlüsseln
    static QString decrypt(
        const QByteArray &ciphertext,
        const QString &groupKey
    );  // Returns: Plaintext
    
    // Algorithmus: AES-256-GCM (AUTHENTICATED ENCRYPTION)
    // - Confidentiality: AES-256
    // - Integrity: Galois/Counter Mode (GCM)
};
```

**Algorithmus-Details:**
- **Verschlüsselung:** AES-256 in GCM-Modus
- **Key Derivation:** PBKDF2-SHA256 (optional, für Passwort-basierte Keys)
- **Nonce:** Zufällig, 12 Bytes (GCM-Standard)
- **Authentication Tag:** 16 Bytes

### 1.3 Voraussetzungen für Gruppe

**Alle Gruppenmitglieder MÜSSEN** den PSK haben.

Wenn ein Mitglied keinen Schlüssel hat:
```
⚠️ Verschlüsselung nicht möglich

charlie@gmail.com hat keinen Schlüssel für
"Firmenteam A".

Optionen:
[Nur an bob@web.de senden]
[Zu verschlüsselter Gruppe hinzufügen]
[Unverschlüsselt senden]
```

### 1.4 Cloud-Anhänge mit Verschlüsselung

Statt große Dateien zu verschlüsseln und zu versenden:

#### Workflow:
1. **Lokal hochladen & Verschlüsseln**
   - User klickt "Anhang hinzufügen" (10MB-Datei)
   - Datei wird mit Gruppen-PSK verschlüsselt
   - Zu Cloud-Storage hochgeladen (z.B. lokaler Server)

2. **Sichere Passwort-Generierung**
```cpp
// src/encryption/PasswordGenerator.h/cpp
class PasswordGenerator {
public:
    // Generiert sicheres Passwort für Datei-Download
    static QString generateDownloadPassword(int length = 15);
    // Beispiel: "K9mX2pL7vQ4bJ8f"
    
    // Zeichen-Set: Groß- + Kleinbuchstaben + Zahlen (kein Sonderzeichen)
    // Warum? Um Copy-Paste zu vereinfachen, keine Shell-Escape-Probleme
};
```

3. **Email versenden**
```
Betreff: Dokument: Vertrag.pdf (verschlüsselt)

Hallo Bob,

anbei das angeforderte Dokument. Es wurde
mit unserem Gruppen-Schlüssel verschlüsselt
und auf dem Server hochgeladen.

Datei-Link: https://files.mail-adler.local/d/abc123def456
Größe: 10.2 MB
Download-Passwort: K9mX2pL7vQ4bJ8f

⚠️ WICHTIG: Passwort nicht weitergeben!
Speichern Sie es sicher!

Datei verfällt in: 30 Tagen

[Link anklicken zum Herunterladen]
```

4. **Download & Automatische Entschlüsselung**
   - Click auf Link → Browser öffnet Download-Dialog
   - Client verlangt Passwort → Verifiziert auf Server
   - Datei wird heruntergeladen & mit PSK entschlüsselt
   - Lokal gespeichert unter `~/Downloads/Vertrag.pdf`

---

## 2. Gmail/Outlook Spezialbehandlung

### 2.1 Google Mail - Keine native E2EE

**Problem:** Google unterstützt **kein OpenPGP/S-MIME nativ** über IMAP.

**Lösung:** Kontakt-Austausch Workflow

```
Benutzer: Alice (alice@gmail.com)
Gruppe: Firmenteam A (mit PSK)
Ziel: Mit Google-Konten verschlüsselt kommunizieren

Workflow:
1. Compose → Gruppe: "Firmenteam A + Google-Nutzer"
2. System erkennt: google@gmail.com hat keinen PSK
3. Dialog erscheint:
   
   ┌──────────────────────────────────────┐
   │  Google-Konto erkannt                │
   ├──────────────────────────────────────┤
   │                                      │
   │  google@gmail.com hat keinen Zugang  │
   │  zu verschlüsseltem Gruppen-Content. │
   │                                      │
   │  Alternativen:                       │
   │  ☐ Kontaktdaten abfragen             │
   │    → Email senden: "Bitte antwort    │
   │      mit alternativer Email wenn     │
   │      Sie Verschlüsselung möchten"    │
   │                                      │
   │  ☐ Unverschlüsselt senden            │
   │                                      │
   │  [Kontakt anfordern] [Senden]       │
   └──────────────────────────────────────┘

4. Wenn "Kontakt anfordern" → Automatische Email:
   
   An: google@gmail.com
   Betreff: Verschlüsselte Kommunikation

   Hallo,
   
   die Gruppe "Firmenteam A" verwendet
   verschlüsselte Email-Kommunikation mit
   AES-256 Verschlüsselung.
   
   Falls Sie teilnehmen möchten, antworten Sie
   bitte mit einer alternativen Email-Adresse
   (z.B. ProtonMail, Posteo) die E2EE
   unterstützt.
   
   Alternativ können wir auch mit Ihrer Google-
   Adresse kommunizieren (unverschlüsselt).
   
   Viele Grüße,
   Alice (via Mail-Adler)

5. Google-Nutzer antwortet:
   "Ja, verwenden Sie: charlie@protonmail.com"
   
6. System aktualisiert Gruppe:
   └─ charlie@gmail.com → charlie@protonmail.com (für verschlüsselte Mails)
```

### 2.2 Outlook/Hotmail - S/MIME Support

Microsoft Outlook unterstützt S/MIME nativ über IMAP.

**Phase D:** S/MIME-Integration

```cpp
// src/encryption/SMIMEHandler.h/cpp
class SMIMEHandler {
public:
    // S/MIME Zertifikat verwalten
    void importCertificate(const QString &certPath);
    void exportCertificate(const QString &destPath);
    
    // Signieren & Verschlüsseln
    QByteArray signAndEncrypt(
        const QString &message,
        const QStringList &recipientCerts
    );
};
```

---

## 3. Spam-Schutz mit Verschlüsselung

### 3.1 Problem: SPF/DKIM/DMARC funktioniert nicht mit E2EE

**Unverschlüsselte Email:** ISP/Mail-Provider prüft automatisch:
- **SPF:** Absender-IP autorisiert?
- **DKIM:** Digitale Signatur korrekt?
- **DMARC:** SPF/DKIM Policy erfüllt?

**Verschlüsselte Email:** Header sind verschlüsselt → Spam-Filter können nicht prüfen.

### 3.2 Lösung: Client-seitige Validierung

Mail-Adler implementiert zusätzliche Checks:

```cpp
// src/security/SpamDetector.h/cpp
class SpamDetector {
public:
    enum SpamLevel {
        NOT_SPAM = 0,
        SUSPICIOUS = 1,
        LIKELY_SPAM = 2,
        DEFINITE_SPAM = 3
    };
    
    SpamLevel analyzeEmail(
        const MailMessage &msg,
        const MailAccount &account
    ) const;
};
```

**Prüfregeln:**

| Regel | Beschreibung | Aktion |
|-------|-------------|--------|
| **SMTP-Match** | SMTP From ≠ Message From | ⚠️ Warnung |
| **SPF-Fail** | SPF-Record nicht erfüllt | ⚠️ Warnung |
| **DKIM-Fail** | DKIM-Signatur ungültig | ⚠️ Warnung |
| **Spam-Liste** | In tägl. Spam-Liste | 🚫 Blockieren |
| **User-Blocked** | Nutzer hat blockiert | 🚫 Blockieren |
| **Known-Phishing** | Bekannte Phishing-Domain | 🚫 Blockieren |

### 3.3 Spam-Einstufung

```
E-Mail von: spammer@evil.com
SMTP-From: evil-server@attacker.net

┌─────────────────────────────┐
│ 🚨 VERDÄCHTIG               │
├─────────────────────────────┤
│                             │
│ ⚠️ SPF-Check fehlgeschlagen │
│    Domain: evil.com         │
│                             │
│ ⚠️ DKIM-Signatur ungültig   │
│                             │
│ ⚠️ SMTP-From ≠ From-Header  │
│    evil-server@attacker.net │
│    ≠ spammer@evil.com       │
│                             │
│ [Als Spam markieren]        │
│ [Spam-Filter anpassen]      │
└─────────────────────────────┘
```

### 3.4 Täglich Spam-List Upload

**Jeden Tag um 9:00 Uhr:**

```cpp
// src/sync/SpamListService.h/cpp
class SpamListService {
public:
    // Sammle lokale Spam-Markierungen
    void uploadLocalSpamList();
    
    // 10:00 Uhr: Download aktualisierte Liste
    void downloadSpamListUpdate();
};
```

**Upload Schema:**

```json
{
  "user_id_hash": "sha256(user-uuid)",
  "timestamp": "2025-02-03T09:00:00Z",
  "entries": [
    {
      "email_hash": "sha256(spammer@evil.com)",
      "domain_hash": "sha256(evil.com)",
      "type": "PHISHING",
      "marked_at": "2025-02-02T14:30:00Z"
    },
    {
      "email_hash": "sha256(bulk@spam.ru)",
      "type": "BULK_MAIL",
      "marked_at": "2025-02-02T10:15:00Z"
    }
  ]
}
```

---

## 4. Sichere Speicherung von Anmeldedaten

### 4.1 Betriebssystem-spezifische Speicher

#### Windows
```cpp
// src/account/CredentialStorage.h/cpp (Windows)
class WindowsCredentialStorage {
private:
    // Nutzt Windows Credential Manager mit DPAPI
    // Verschlüsselung: Automatisch mit Windows-Benutzer-Key
    
public:
    void storePassword(const QString &account, const QString &password);
    QString retrievePassword(const QString &account);
};

// Speicherort: Windows Credential Manager
// Sicherheit: Systemweit verschlüsselt
// Zugriff: Nur über autorisierten Prozess
```

#### Linux
```cpp
// src/account/CredentialStorage.h/cpp (Linux)
class LinuxCredentialStorage {
private:
    // Nutzt freedesktop.org Secret Service (DBus)
    // Fallback: Encrypted file (~/.config/mail-adler/secrets.enc)
    
public:
    void storePassword(const QString &account, const QString &password);
    QString retrievePassword(const QString &account);
};

// Speicherort: Secret Service / ~/.config/mail-adler/secrets.enc
// Verschlüsselung: AES-256 mit Master-Key
// Master-Key: Abgeleitet von System-UUID + User-UID (PBKDF2)
```

#### macOS
```cpp
// src/account/CredentialStorage.h/cpp (macOS)
class MacOSCredentialStorage {
private:
    // Nutzt Keychain
    
public:
    void storePassword(const QString &account, const QString &password);
    QString retrievePassword(const QString &account);
};

// Speicherort: macOS Keychain
// Sicherheit: Systemweit verschlüsselt
// Zugriff: Benutzer muss genehmigen (beim Abruf)
```

### 4.2 OAuth2 Token Management

```cpp
class OAuth2Manager {
public:
    // Tokens sicher speichern
    void storeAccessToken(
        const QString &account,
        const QString &accessToken,
        const QString &refreshToken,
        qint64 expiresInSeconds
    );
    
    // Automatische Erneuerung
    bool refreshAccessTokenIfNeeded(const QString &account);
};
```

---

## 5. Transport Security

### 5.1 TLS/SSL Anforderungen

**IMAP:**
- Minimum: **TLS 1.2**
- Bevorzugt: **TLS 1.3**
- STARTTLS oder SSL/TLS auf Port 993

**SMTP:**
- Minimum: **TLS 1.2**
- Bevorzugt: **TLS 1.3**
- Submission Port: 587 (mit STARTTLS)
- Secure Port: 465 (Implicit TLS)

### 5.2 Certificate Validation

```cpp
// src/network/SSLValidator.h/cpp
class SSLValidator {
public:
    bool validateServerCertificate(
        const QSslCertificate &serverCert,
        const QString &hostname
    );
    
private:
    // Prüfe:
    // 1. CN/SAN matches hostname
    // 2. Cert gültig (nicht abgelaufen)
    // 3. Signiert von bekannter CA
    // 4. Certificate Pinning (optional)
};
```

### 5.3 Certificate Pinning (Optional)

Für unternehmenseigene Server:

```json
{
  "pinned_certificates": [
    {
      "hostname": "imap.company.com",
      "pin": "sha256/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=",
      "backup_pin": "sha256/BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB="
    }
  ]
}
```

---

## 6. Datenspeicherung-Sicherheit

### 6.1 SQLite Datenbank Verschlüsselung

**Phase B:** Verschlüsselte Datenbank mit SQLCipher

```cpp
// src/database/Database.h/cpp
class Database {
private:
    sqlite3 *db;
    QString masterKey;
    
public:
    bool openEncrypted(const QString &path, const QString &password);
    // Nutzt: SQLCipher mit AES-256
    
    // Master-Key wird abgeleitet von:
    // PBKDF2-SHA256(password, salt=app_id, iterations=4096)
};
```

### 6.2 Temp-Datei Sicherheit

```cpp
// src/util/SecureFile.h/cpp
class SecureFile {
public:
    // Erstelle Temp-Datei mit sicheren Rechten
    static QString createSecureTempFile(
        const QString &prefix,  // z.B. "mail-adler-"
        const QString &suffix   // z.B. ".eml"
    );
    // Datei-Permissions: 0600 (Owner read/write only)
    
    // Sichere Löschung (mit Überschreibung)
    static void secureDelete(const QString &filePath);
    // Überschreibe mit Zufallsdaten vor Löschen
};
```

---

## 7. Phase-Übersicht

### Phase B (Aktuell)
- ✅ PSK-basierte Verschlüsselung
- ✅ AES-256-GCM
- ✅ Cloud-Anhänge mit Passwort
- ✅ Spam-Detektion
- ✅ Sichere Passwort-Speicherung

### Phase C (Nächste)
- ⏳ OpenPGP/GPG Integration
- ⏳ Public-Key Exchange
- ⏳ Key-Revocation

### Phase D
- ⏳ S/MIME Support
- ⏳ X.509 Certificate Management
- ⏳ Outlook Integration

### Phase E+
- ⏳ Forward Secrecy
- ⏳ Perfect Forward Secrecy (PFS)
- ⏳ Decentralized Key Server

---

## 8. Best Practices für Benutzer

### 8.1 Sichere Grup pen-Verwaltung

1. **Schlüssel NICHT per Email versenden**
   - Nur out-of-band (Telefon, Signal, persönlich)

2. **Regelmäßig Schlüssel rotieren**
   - Alle 6-12 Monate neuen Schlüssel generieren
   - Alte Schlüssel archivieren

3. **Sicherung des Master-Keys**
   - Exportieren & offline sichern
   - Passwort-geschützt speichern

### 8.2 Passwort-Sicherheit

1. **Starke Passwörter für Cloud-Dateien**
   - Auto-generierte Passwörter verwenden (15+ Zeichen)
   - Nicht speichern oder weitergeben

2. **Zwei-Faktor-Authentifizierung**
   - Falls möglich, aktivieren (Gmail, Outlook, etc.)

### 8.3 Spam-Reporting

1. **Konsistent markieren**
   - Wenn Phishing → IMMER markieren
   - Hilft anderen Nutzern

2. **Verdächtige Emails prüfen**
   - Expert-Modus: Spam-Details ansehen
   - SMTP-Mismatch = großes Warnsignal

---

## 9. Häufig gestellte Fragen

**F: Was ist PSK?**
A: Pre-Shared Key - Ein gemeinsamer geheimer Schlüssel, den alle Gruppenmitglieder haben.

**F: Ist AES-256 sicher?**
A: Ja. AES-256 ist von US-Regierung für TOP SECRET klassifiziert.

**F: Kann ich OpenPGP nutzen?**
A: Phase C wird OpenPGP unterstützen. Phase B nutzt PSK.

**F: Was ist mit Google-Mails?**
A: Google unterstützt kein E2EE über IMAP. Wir fragen nach alternativer Email.

**F: Ist Datei-Passwort sicher?**
A: Ja. Passwort wird auf Client generiert, Server speichert nur gehashed.

**F: Wer hat Zugriff auf meine Schlüssel?**
A: Niemand. Schlüssel werden lokal mit Betriebssystem-Verschlüsselung gespeichert.

**F: Was wenn ich den PSK vergesse?**
A: Schlüssel muss erneut verteilt werden. Alte Nachrichten können nicht entschlüsselt werden.
