# Phase B - Mail-Adler Core-Architektur Planung

## Aktueller Status (Nach Phase 4+ Cleanup)
- ✅ Alle Video-abhängigen Abhängigkeiten aus CMakeLists.txt entfernt  
- ✅ Alle MLT/Video-Code in main.cpp, mainwindow.h, mainwindow.cpp deaktiviert
- ✅ Alle Player/Playlist/Timeline-Referenzen deaktiviert (23 Funktionen)
- ✅ Syntax-Validierung bestanden
- ⏳ Vollständiger Kompilierungstest ausstehend (CMake/Ninja-Setup)

## Erhaltene Infrastruktur (Phase A+4 Erfolg)
Diese Komponenten bleiben funktional und unterstützen Mail-Client-Operationen:

### Datenbankschicht
- **Datei:** `src/database.cpp`, `src/database.h`
- **Zweck:** SQLite3-Persistierung
- **Anwendungsfall:** Mail-Speicherung, Kontokonfigurationen, Ordnerhierarchie
- **Status:** Bereit zur Erweiterung mit Mail-Schema

### Job-Warteschlange
- **Datei:** `src/jobqueue.cpp`, `src/jobqueue.h`
- **Zweck:** Asynchrone Task-Warteschlange
- **Anwendungsfall:** Hintergrund-Mail-Sync, Ordner-Updates, Message-Download
- **Status:** Bereit für IMAP/SMTP-Operationen

### Einstellungssystem
- **Datei:** `src/settings.cpp`, `src/settings.h`
- **Zweck:** Anwendungspräferenzen-Persistierung
- **Anwendungsfall:** Kontoanmeldedaten, UI-Präferenzen, Sync-Intervalle
- **Status:** Erweiterbar für Mail-spezifische Einstellungen

### Protokollierung
- **Modul:** `CuteLogger/`
- **Zweck:** Debug- und Operationsprotokollierung
- **Status:** Verfügbar für Mail-Operationen

### UI-Framework
- **Basis:** Qt6 Widgets (bereits in Gebrauch)
- **Status:** Hauptfenster, Dialoge, Dock-Widgets funktional

## Phase B Leistungsergebnisse

### 1. Mail-Datenmodell & Schema
**Ziel:** Mail-Speicherstruktur definieren

**Komponenten:**
- `src/models/MailMessage.h/cpp` - E-Mail-Nachrichts-Entity
  - Von, An, Betreff, Text, Datum, Flags (Gelesen, Markiert, Spam)
  - UID, Ordner-ID, Konto-ID
  - Anhang-Metadaten
  
- `src/models/MailFolder.h/cpp` - IMAP-Ordner-Entity
  - Ordnername, Pfad, Flags
  - Gelesen/Ungelesen-Zählungen
  - Sync-Status-Verfolgung
  
- `src/models/MailAccount.h/cpp` - E-Mail-Konto-Entity
  - IMAP-Server-Einstellungen (Host, Port, Auth)
  - SMTP-Server-Einstellungen
  - Sync-Präferenzen (Intervall, Ordner)

- `src/database/MailSchema.h/cpp` - SQLite-Schema
  - Tabellen: accounts, folders, messages, attachments, sync_state, telemetry
  - Indizes für schnelle Abfragen
  - Migrationssystem

### 2. IMAP-Client-Modul
**Ziel:** E-Mail-Abruf und Ordnerverwaltung

**Komponenten:**
- `src/imap/ImapClient.h/cpp` - IMAP-Protokoll-Wrapper
  - Verbindungsverwaltung
  - Authentifizierung (LOGIN, PLAIN, OAuth2-Grundgerüst)
  - Ordner-Enumeration
  - Message-Abruf (Header + Text)
  - UID-Verfolgung für Sync
  
- `src/imap/ImapFolder.h/cpp` - Ordner-Operationen
  - Ordner-Status (EXISTS, RECENT, UNSEEN)
  - Select/Close-Operationen
  - Message-Suche (nach Datum, Absender, etc.)
  
- `src/imap/ImapSync.h/cpp` - Inkrementelles Sync-Engine
  - Letzten Sync-Status verfolgbar
  - Nur neue Nachrichten abrufen
  - Flag-Änderungen handhaben (gelesen/markiert-Status)
  - Konfliktauflösung

### 3. SMTP-Client-Modul
**Ziel:** E-Mail-Versandmöglichkeit

**Komponenten:**
- `src/smtp/SmtpClient.h/cpp` - SMTP-Protokoll-Wrapper
  - Verbindungsverwaltung
  - Authentifizierung
  - Message-Versand
  - Fehlerbehandlung
  
- `src/smtp/MessageComposer.h/cpp` - Kompositionsoperationen
  - MIME-Nachrichten erstellen
  - Anhänge handhaben
  - Zitieren/Antworten-Operationen

### 4. Konto-Manager
**Ziel:** Multi-Konto-Unterstützung

**Komponenten:**
- `src/account/AccountManager.h/cpp`
  - Konten hinzufügen/entfernen/bearbeiten
  - Anmeldedaten sicher speichern (verschlüsselt)
  - Kontowechsel
  
- `src/account/CredentialStorage.h/cpp`
  - Plattformspezifischer sicherer Speicher (Windows: DPAPI, Linux: keyring)
  - Anmeldedaten-Caching
  - Token-Aktualisierung (OAuth2)

### 5. Mail-Synchronisierungsdienst
**Ziel:** Automatisierte Hintergrund-Sync

**Komponenten:**
- `src/sync/SyncService.h/cpp`
  - Periodische Sync-Planung
  - JobQueue-Integration
  - Fehlerwiederherstellung
  - Konfliktauflösung
  
- `src/sync/SyncScheduler.h/cpp`
  - Intervallbasierter Sync
  - Manuelle Sync-Auslöser
  - Push-Benachrichtigungsunterstützung (IMAP IDLE)

### 6. UI-Schicht (Phase B Minimal)
**Ziel:** Basis-Mail-Client-Schnittstelle

**Komponenten:**
- `src/ui/MailListWidget.h/cpp` - Message-Listenansicht
  - TableView mit Absender, Betreff, Datum, Flags
  - Threading-Support-Grundgerüst
  
- `src/ui/MailViewWidget.h/cpp` - Message-Viewer
  - Gesamte Nachrichteninhalt anzeigen
  - HTML/Klartext handhaben
  - Anhang-Vorschau/Download
  
- `src/ui/ComposeDialog.h/cpp` - Message-Komposition
  - Text-Editor
  - Empfänger-Felder
  - Anhang-Verwaltung
  
- `src/ui/AccountSetupDialog.h/cpp` - Kontokonfiguration
  - Server-Details
  - Authentifizierung
  - Ordner-Auswahl

## Phase B Architektur-Diagramm

```
┌─────────────────────────────────────────────────────────┐
│              Mail-Adler Hauptfenster                     │
├─────────────────────────────────────────────────────────┤
│  Erhalten: Statusleiste, Menüs, Toolbars, Einstellungen│
│  Neu: Mail-Ansichten (Liste, Komposition, Konten)       │
└─────────────────────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────────────────────┐
│                UI-Komponenten (Phase B)                  │
├─────────────────────────────────────────────────────────┤
│  - MailListWidget                                        │
│  - MailViewWidget                                        │
│  - ComposeDialog                                         │
│  - AccountSetupDialog                                    │
└─────────────────────────────────────────────────────────┘
         ↓ (signals/slots)
┌─────────────────────────────────────────────────────────┐
│         Konto-Manager & Synchronisierungsdienst         │
├─────────────────────────────────────────────────────────┤
│  - AccountManager (hinzufügen/entfernen/wechseln)       │
│  - SyncService (Hintergrund-Sync)                       │
│  - CredentialStorage (sichere Auth)                     │
└─────────────────────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────────────────────┐
│         IMAP & SMTP Client-Module                       │
├─────────────────────────────────────────────────────────┤
│  - ImapClient (abrufen, sync, Ordner-Ops)               │
│  - SmtpClient (Versand)                                 │
│  - ImapSync (inkrementelles Sync-Engine)                │
└─────────────────────────────────────────────────────────┘
         ↓ (Datenbankoperationen)
┌─────────────────────────────────────────────────────────┐
│         Mail-Datenmodelle & Datenbank                    │
├─────────────────────────────────────────────────────────┤
│  - MailMessage, MailFolder, MailAccount                 │
│  - MailSchema (SQLite-Tabellen)                         │
│  - Database (von Phase A - Persistierungs-Schicht)      │
└─────────────────────────────────────────────────────────┘
         ↓
┌─────────────────────────────────────────────────────────┐
│             Infrastruktur-Schicht                        │
├─────────────────────────────────────────────────────────┤
│  - JobQueue (async Mail-Operationen)                    │
│  - Settings (App-Konfiguration)                         │
│  - Logging (Debug/Audit)                                │
│  - Telemetry (Fehlerberichterstattung)                  │
└─────────────────────────────────────────────────────────┘
```

## Phase B Implementierungs-Reihenfolge

### Meilenstein 1: Datenschicht
1. **MailMessage, MailFolder, MailAccount** Modelle
2. **MailSchema** - SQLite-Tabellen erstellen und Migrationen
3. **Datenbankschema-Validierung**

### Meilenstein 2: IMAP-Modul
1. **ImapClient** - Basis-Verbindung, Authentifizierung, Ordnerliste
2. **ImapFolder** - Ordner-Operationen (select, status)
3. **ImapSync** - UID-Verfolgung, inkrementeller Abruf
4. **Testen** - Mock-IMAP-Server (z.B. test.example.com)

### Meilenstein 3: SMTP-Modul
1. **SmtpClient** - Verbindung, Auth, grundlegender Versand
2. **MessageComposer** - MIME-Nachrichts-Erstellung
3. **Fehlerbehandlung & Wiederholungen**

### Meilenstein 4: Konto- & Sync-Services
1. **AccountManager** - Konten hinzufügen/entfernen/wechseln
2. **CredentialStorage** - Sichere Anmeldedaten-Verarbeitung
3. **SyncService** - JobQueue-Integration, periodischer Sync
4. **SyncScheduler** - Hintergrund-Sync-Automatisierung

### Meilenstein 5: Basis-UI
1. **MailListWidget** - Nachrichten anzeigen
2. **MailViewWidget** - Nachrichten lesen
3. **ComposeDialog** - Nachrichten versenden
4. **AccountSetupDialog** - Konten konfigurieren

## CMakeLists.txt Aktualisierungen erforderlich

### Neue Verzeichnisse hinzufügen
```cmake
add_subdirectory(src/models)
add_subdirectory(src/database)
add_subdirectory(src/imap)
add_subdirectory(src/smtp)
add_subdirectory(src/account)
add_subdirectory(src/sync)
add_subdirectory(src/ui)
```

### Externe Abhängigkeiten (bereits verfügbar)
- Qt6::Core
- Qt6::Sql
- Qt6::Network (für IMAP/SMTP-Verbindungen)
- Qt6::Widgets

### Potenzielle neue Abhängigkeiten
- **libsasl2** (SMTP-Authentifizierung)
- **openssl** (TLS/SSL für sichere Verbindungen)
- **libkeyring** (Linux-Anmeldedaten-Speicher)

## Erfolgskriterien

✅ **Phase B Abgeschlossen wenn:**
- Alle 6 Komponenten (Daten, IMAP, SMTP, Konto, Sync, UI) implementiert
- Einzel-Konto IMAP-Sync funktional
- E-Mail-Versand über SMTP funktional
- Nachrichten bleiben in SQLite-Datenbank erhalten
- Hintergrund-Sync läuft ohne UI-Blockierung
- Einstellungen bleiben über Sitzungen erhalten

## Nächste Schritte nach Phase B
- Phase C: Multi-Konto-Unterstützungs-Verbesserung
- Phase D: IMAP IDLE Push-Benachrichtigungen
- Phase E: Erweiterte Suche, Filter, Threading
- Phase F: Anhang-Download/Anzeige
- Phase G: Verschlüsselung (PGP/S-MIME) Unterstützung
