# Mail-Adler Testing Plan

## 1. Lokalisierung - Deutsch

### 1.1 Ordner-Namen (Standard IMAP)

Mail-Adler zeigt deutsche Namen für Standard-Ordner:

| IMAP-Name | Mail-Adler Anzeige (Deutsch) |
|-----------|-------------------------------|
| `INBOX` | **Eingang** |
| `Sent` / `[Gmail]/Sent Mail` | **Gesendet** |
| `Drafts` / `[Gmail]/Drafts` | **Entwürfe** |
| `Trash` / `[Gmail]/Bin` | **Papierkorb** |
| `Spam` / `[Gmail]/Spam` | **Spam** |
| `Archive` / `[Gmail]/All Mail` | **Archiv** |
| `Flagged` | **Markiert** |

```cpp
// src/models/MailFolder.h/cpp
class MailFolder {
private:
    QString getLocalizedName(const QString &imapName) const {
        static QMap<QString, QString> localization = {
            {"INBOX", "Eingang"},
            {"Sent", "Gesendet"},
            {"Drafts", "Entwürfe"},
            {"Trash", "Papierkorb"},
            {"Spam", "Spam"},
            {"Archive", "Archiv"},
            {"Flagged", "Markiert"}
        };
        return localization.value(imapName, imapName);
    }
};
```

### 1.2 UI-Lokalisierung (Qt Translations)

```cpp
// src/ui/MainWindow.cpp
tr("Eingang")      // INBOX
tr("Gesendet")     // Sent
tr("Entwürfe")     // Drafts
tr("Papierkorb")   // Trash
tr("Spam")         // Spam
tr("Archiv")       // Archive
tr("Markiert")     // Flagged
tr("Synchronisieren")  // Sync
tr("Neue Nachricht")   // New Message
tr("Antworten")        // Reply
```

---

## 2. Test-Plan: Deutsche Mail-Anbieter

### 2.1 Test-Konten Vorbereitung

**Verfügbare Test-Konten:**

| Provider | Email | Status |
|----------|-------|--------|
| GMX | `georg.dahmen@gmx.de` | ✅ Verfügbar |
| Web.de | `georg.dahmen.test@web.de` | ✅ Verfügbar |
| Telekom | `georg.dahmen.gd@googlemail.com` | ✅ Verfügbar |

### 2.2 Test-Matrix

```
┌─────────────────┬─────────────────┬──────────────┬──────────────┐
│ Funktion        │ GMX             │ Web.de       │ Telekom/GM   │
├─────────────────┼─────────────────┼──────────────┼──────────────┤
│ Verbindung      │ imap.gmx.net:993│ imap.web.de  │ imap.gmail.c │
│ IMAP            │ ✅ IMAP4rev1    │ ✅ IMAP4rev1 │ ✅ IMAP4rev1 │
│ SMTP            │ mail.gmx.net:587│ mail.web.de  │ smtp.gmail.c │
│ TLS/SSL         │ ✅ 1.3          │ ✅ 1.3       │ ✅ 1.3       │
│ OAuth2          │ ❌ Nicht        │ ❌ Nicht     │ ✅ Google    │
│ STARTTLS        │ ✅ 587          │ ✅ 587       │ ✅ 587       │
│ 2FA Support     │ ✅              │ ✅           │ ✅           │
└─────────────────┴─────────────────┴──────────────┴──────────────┘
```

### 2.3 Phase B Test-Szenarien

#### Test 1: Verbindung & Authentifizierung

**GMX:**
```
1. Öffne Account-Setup Dialog
2. Email: georg.dahmen@gmx.de
3. Passwort: [Test-Passwort]
4. Server-Auto-Detect: imap.gmx.net:993
5. Ergebnis: ✅ Verbindung erfolgreich
```

**Web.de:**
```
1. Email: georg.dahmen.test@web.de
2. Passwort: [f6r8Z9uZAq83IMztmiyc]
3. Server-Auto-Detect: imap.web.de:993
4. Ergebnis: ✅ Verbindung erfolgreich
```

**Telekom/Google:**
```
1. Email: georg.dahmen.gd@googlemail.com
2. Passwort: [b*yZXxjd6CdwQAb6]
3. Oder OAuth2: https://accounts.login.idm.telekom.com/oauth2/auth
4. Ergebnis: ✅ Verbindung erfolgreich
```

#### Test 2: Ordner-Abruf

```
Erwartete Ordner (GMX):
✅ Eingang (INBOX)
✅ Gesendet (Sent)
✅ Entwürfe (Drafts)
✅ Papierkorb (Trash)
✅ Spam
✅ Archiv (Archive)
✅ Verschiedenes (Misc)

(Web.de & Telekom ähnlich)
```

#### Test 3: Email-Sync

```
1. Öffne Eingang
2. Klick "Synchronisieren"
3. Ergebnis: ✅ Alle Nachrichten abgerufen
   - Header angezeigt
   - Absender korrekt
   - Betreffzeilen angezeigt
   - Datum angezeigt
```

#### Test 4: Email lesen

```
1. Klick auf erste Email
2. Ergebnis: ✅ Vollständiger Text angezeigt
   - Formatierung korrekt
   - HTML-Mails korrekt (Falls vorhanden)
   - Anhänge angezeigt
```

#### Test 5: Email versenden

```
GMX-Konto test:
1. Neue Nachricht
2. An: [Ihre andere Email]
3. Betreff: Test Mail-Adler
4. Text: "Dies ist eine Test-Email von Mail-Adler"
5. Senden
6. Ergebnis: ✅ Email in "Gesendet" Ordner

Verification:
- Email im Gesendet-Ordner sichtbar
- Zeitstempel korrekt
- Text korrekt empfangen
```

#### Test 6: Email löschen

```
1. Öffne Email
2. Klick Löschen
3. Ergebnis: ✅ Email im Papierkorb
4. Verifizierung: 
   - Papierkorb Ordner zeigt Email
   - Eingang zeigt Email nicht mehr
```

#### Test 7: Email wiederherstellen

```
1. Öffne Papierkorb
2. Rechts-Klick auf gelöschte Email
3. "Wiederherstellen"
4. Ergebnis: ✅ Email zurück in Eingang
```

#### Test 8: Spam-Markierung

```
1. Öffne Email
2. Klick "Als Spam markieren"
3. Ergebnis: ✅ Email im Spam-Ordner
4. Lokal: Email in lokale Spam-Datenbank eingetragen
5. Täglich 9:00 Uhr: Zu zentralem Service hochgeladen
```

---

## 3. Cross-Platform Testing

### 3.1 Windows 11 Testing

**Getestet von:** Georg Dahmen (Dein System)

```
OS: Windows 11 (Build 22621)
Architektur: x86_64
RAM: 16 GB
Qt: 6.4.2

Test-Szenarien:
✅ App-Start
✅ Account-Setup
✅ IMAP-Sync (3 Provider)
✅ Email-Versand
✅ Lokale Persistierung
```

### 3.2 Linux Testing (geplant)

```
Distribution: Ubuntu 22.04 LTS (oder Debian)
Architektur: x86_64
Qt: 6.4.2+

Test-Szenarien:
- [ ] App-Start
- [ ] Account-Setup (GMX, Web.de)
- [ ] Keyring-Integration (Secret Service)
- [ ] Desktop-Integration
```

### 3.3 macOS Testing (geplant)

```
OS: macOS 13+
Architektur: x86_64 + ARM64
Qt: 6.4.2+

Test-Szenarien:
- [ ] App-Start
- [ ] Account-Setup
- [ ] Keychain-Integration
- [ ] Notarization & Signing
```

---

## 4. ARM64 Support - Raspberry Pi 5

### 4.1 Raspberry Pi 5 Architektur

**Spezifikationen:**
```
Prozessor: ARM Cortex-A76 (64-bit)
Architektur: ARMv8 / ARM64
RAM: 4GB / 8GB
CPU-Kerne: 4 @ 2.4 GHz
GPU: Broadcom VideoCore VII

Kompatibilität: ✅ Vollständig kompatibel mit Mail-Adler
Vergleich zu macOS ARM: Ähnlich, aber weniger Power
```

### 4.2 Build-Prozess für ARM64

```bash
# CMake-Konfiguration für ARM64
cd build-arm64
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=../cmake/arm64-toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -GNinja

# Für Raspberry Pi:
cmake .. \
  -DCMAKE_SYSTEM_NAME=Linux \
  -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
  -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
  -GNinja

ninja
ninja install
```

### 4.3 Raspberry Pi OS Installation

**Vorbereitung:**
```bash
# Raspberry Pi OS Lite 64-bit
# Download: https://www.raspberrypi.com/software/

# SSH aktivieren
touch /boot/ssh

# Qt6 & Dependencies installieren
sudo apt update
sudo apt install -y \
  qt6-base-dev \
  qt6-sql-plugins \
  libsqlite3-dev \
  libssl-dev \
  libsasl2-dev \
  cmake \
  ninja-build \
  build-essential

# Mail-Adler compilieren & installieren
git clone https://github.com/georg0480/mail-adler.git
cd mail-adler
mkdir build && cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
ninja
sudo ninja install
```

### 4.4 Performance-Erwartungen

| Operation | Ergebnis |
|-----------|----------|
| App-Start | ~3-5 Sekunden |
| Account-Setup | ~2-3 Sekunden |
| IMAP-Sync (10 Emails) | ~2-4 Sekunden |
| Email-Render (HTML) | ~1 Sekunde |
| Gesamtspeicher | ~80-150 MB (mit Qt6) |

### 4.5 Testing auf Pi 5

**Minimales Test-Setup:**

```
Hardware:
- Raspberry Pi 5 (8GB)
- SD-Karte (64GB+)
- Ethernet-Kabel (oder WiFi)

Software:
- Raspberry Pi OS 64-bit Lite
- Qt6
- Mail-Adler Build

Tests:
1. [ ] App startet
2. [ ] GMX-Account konfigurierbar
3. [ ] Sync funktioniert
4. [ ] Email lesbar
5. [ ] Email versendbar
6. [ ] CPU-Last akzeptabel
7. [ ] RAM-Nutzung ok
```

---

## 5. macOS ARM64 Support

### 5.1 Apple Silicon Kompatibilität

**Modelle:**
- ✅ M1 / M1 Pro / M1 Max
- ✅ M2 / M2 Pro / M2 Max
- ✅ M3 / M3 Pro / M3 Max
- ✅ M4 (zukünftig)

**Vergleich:**
| Kriterium | Pi 5 | macOS M1 |
|-----------|------|----------|
| CPU Kerne | 4 @ 2.4 GHz | 8 (4P+4E) @ 3.2 GHz |
| RAM | 4-8 GB | 8-32 GB |
| Performance | Niedrig | Sehr hoch |
| Ideal für | Heimserver | Desktop/Laptop |

### 5.2 ARM64 Build für macOS

```bash
# M1/M2/M3 Build
arch -arm64 brew install qt6
mkdir build-arm64-mac && cd build-arm64-mac

cmake .. \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -GNinja \
  -DCMAKE_BUILD_TYPE=Release

ninja
# Codesign & Notarize für App Store
codesign -s - build/Mail-Adler.app
```

---

## 6. Windows ARM64 Support (Zukunft)

Microsoft bietet auch Windows on ARM an (z.B. Copilot+ PCs).

```
Zukunft: Phase D+
- [ ] Windows ARM64 Build
- [ ] Testing auf ARM64 Windows
- [ ] Offizieller Release
```

---

## 7. Test-Automation

### 7.1 Unit Tests (Qt Test Framework)

```cpp
// tests/imap_client_test.h
#include <QtTest>

class ImapClientTest : public QObject {
    Q_OBJECT
    
private slots:
    void testGMXConnection();
    void testWebDeConnection();
    void testTelekomConnection();
    void testEmailSync();
    void testEmailSend();
};

// Beispiel:
void ImapClientTest::testGMXConnection() {
    ImapClient client;
    bool connected = client.connect(
        "imap.gmx.net", 993,
        "georg.dahmen@gmx.de",
        "password"
    );
    QVERIFY(connected);
}
```

### 7.2 Integration Tests

```cpp
// tests/e2e_tests.h
class E2ETest : public QObject {
    Q_OBJECT
    
private slots:
    void testFullWorkflow_GMX();
    void testFullWorkflow_WebDe();
    void testFullWorkflow_Telekom();
};

void E2ETest::testFullWorkflow_GMX() {
    // 1. App starten
    // 2. Account hinzufügen
    // 3. Sync
    // 4. Email lesen
    // 5. Email versenden
    // 6. Verifikation
}
```

---

## 8. Test-Checkliste Phase B

### Phase B Completion Checklist

```
Lokalisierung:
- [x] Ordner-Namen Deutsch
- [x] UI-Text Deutsch
- [ ] Error-Messages Deutsch
- [ ] Tooltips Deutsch

GMX Testing (Windows 11):
- [ ] Verbindung erfolgreich
- [ ] Ordner abrufbar
- [ ] Emails synchronisierbar
- [ ] Emails lesbar
- [ ] Emails versendbar
- [ ] Löschen funktioniert
- [ ] Spam-Markierung funktioniert

Web.de Testing (Windows 11):
- [ ] Verbindung erfolgreich
- [ ] Ordner abrufbar
- [ ] Emails synchronisierbar
- [ ] Emails lesbar
- [ ] Emails versendbar
- [ ] 2FA funktioniert

Telekom/Google Testing (Windows 11):
- [ ] Verbindung erfolgreich
- [ ] OAuth2 funktioniert
- [ ] Ordner abrufbar
- [ ] Emails synchronisierbar
- [ ] Emails lesbar
- [ ] Emails versendbar

Linux Testing (geplant):
- [ ] Compilation erfolgreich
- [ ] App startet
- [ ] Account-Setup funktioniert
- [ ] Keyring-Integration ok

macOS Testing (geplant):
- [ ] Compilation erfolgreich (x86_64 + ARM64)
- [ ] App startet
- [ ] Keychain-Integration ok

ARM64 Testing (Zukunft):
- [ ] Pi 5 Build funktioniert
- [ ] App startet auf Pi 5
- [ ] Basis-Funktionalität ok
- [ ] Performance akzeptabel

Sicherheit:
- [ ] Passwörter verschlüsselt gespeichert
- [ ] TLS 1.3 verwendet
- [ ] Keine Passwörter in Logs
- [ ] Telemetrie optional
```

---

## 9. Release-Roadmap

### Phase B (aktuell) - März 2025
- Single-Account IMAP/SMTP
- Deutsch-lokalisiert
- Windows 11 Testing
- GMX, Web.de, Telekom Support

### Phase B+ - April 2025
- Multi-Account Support
- Linux Build verfügbar
- macOS Build verfügbar

### Phase C - Mai 2025
- OpenPGP/E2EE Support
- ARM64 Testing (Pi 5, M1/M2)
- Beta-Release

### Phase D - Juni 2025
- S/MIME Support
- Stable Release v1.0

---

## 10. Feedback & Bug-Reporting

**Für Testing-Ergebnisse:**
- Email: georg.dahmen@proton.me
- Format: Betriebssystem, Reproduktionsschritte, Fehler-Details
- Screenshot/Log anhängen wenn möglich

**Test-Daten speichern:**
```
~/.config/mail-adler/test-logs/
├─ gmx_sync_20250203.log
├─ web_send_20250203.log
└─ telekom_oauth_20250203.log
```
