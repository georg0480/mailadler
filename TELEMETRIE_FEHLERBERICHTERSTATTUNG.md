# Telemetrie & Fehlerberichterstattung System

## Übersicht
Mail-Adler wird ein optionales Fehlerberichterstattungs- und Telemetrie-System mit vollständiger Benutzerkontrolle implementieren.

## 1. Datenschutz & Benutzer-Einwilligung

### Installation & Onboarding
Bei der **ersten Anwendung** wird der Benutzer gefragt:

```
┌─────────────────────────────────────────────────┐
│  Mail-Adler Willkommen                          │
├─────────────────────────────────────────────────┤
│                                                 │
│  Fehlerberichterstattung helfen, Mail-Adler    │
│  zu verbessern. Ihre Privatsphäre ist          │
│  wichtig: Keine persönlichen Daten werden      │
│  ohne Ihre Zustimmung übertragen.              │
│                                                 │
│  ☐ Automatische Fehlerberichterstattung        │
│    (Betriebssystem, Fehlertyp, Stack-Trace)    │
│                                                 │
│  ☐ Anonyme Nutzungsstatistiken                 │
│    (Feature-Nutzung, Sync-Erfolgsrate)         │
│                                                 │
│  [Ja, Standard aktiviert]  [Nein, später]      │
└─────────────────────────────────────────────────┘
```

**Standardverhalten:** Aktiviert (Benutzer wird informiert)

**Speicherort:** `~/.config/mail-adler/telemetry_consent.json`
```json
{
  "version": 1,
  "timestamp": "2025-02-03T12:00:00Z",
  "error_reporting": true,
  "usage_statistics": false,
  "last_asked": "2025-02-03T12:00:00Z"
}
```

### Wiederholung
Alle 90 Tage wird der Benutzer erneut gefragt, ob die Einwilligung noch aktuell ist.

---

## 2. Fehlerberichterstattungs-Architektur

### 2.1 Fehlererfassung

#### Automatische Erfassung
```cpp
// src/telemetry/ErrorReporter.h/cpp
class ErrorReporter {
public:
    static void reportError(
        const QString &errorType,      // z.B. "ImapConnectionFailed"
        const QString &message,         // Fehlermeldung
        const QString &stackTrace,      // Stack-Trace
        const QMap<QString, QString> &context  // Zusätzlicher Kontext
    );
    
    static void reportException(const std::exception &e);
    static void reportWarning(const QString &warning);
};
```

#### Fehlertypen
| Fehler-ID | Beschreibung | Beispiel |
|-----------|-------------|---------|
| `IMAP_CONNECTION_FAILED` | IMAP-Verbindungsfehler | Timeout, SSL-Fehler |
| `SMTP_SEND_FAILED` | SMTP-Versandfehler | Auth-Fehler, Relay-Fehler |
| `DATABASE_ERROR` | SQLite-Fehler | Schema-Migration, Locking |
| `SYNC_FAILED` | Sync-Fehler | Netzwerkfehler, Konflikt |
| `CRASH` | Anwendungs-Crash | Segfault, Assertion |
| `UI_ERROR` | UI-Rendering-Fehler | Widget-Initialisierung |

### 2.2 Fehler-Kontext

Jeder Fehler enthält:

```json
{
  "error_id": "unique-uuid-v4",
  "error_type": "IMAP_CONNECTION_FAILED",
  "message": "Connection timeout after 30s",
  "timestamp": "2025-02-03T12:34:56.789Z",
  "severity": "ERROR",
  
  "system": {
    "os": "Windows 11 (Build 22621)",
    "os_version": "11.0.22621",
    "architecture": "x86_64",
    "cpu_cores": 8,
    "memory_mb": 16384,
    "qt_version": "6.4.2"
  },
  
  "application": {
    "version": "0.1.0-beta",
    "build_hash": "abc1234567",
    "uptime_seconds": 3600,
    "session_duration_seconds": 1800
  },
  
  "account_info": {
    "account_id": "hash(account-uuid)",  // Anonymisiert
    "provider": "gmail",  // "gmail", "gmx", "web.de", "telekom", etc.
    "last_sync_minutes_ago": 15
  },
  
  "stack_trace": "...",
  "additional_context": {
    "operation": "ImapSync.incrementalFetch",
    "retry_count": 2,
    "network_available": true
  },
  
  "hash": "sha256(message+stacktrace)"  // Zur Deduplizierung
}
```

### 2.3 Fehler-Deduplizierung

Wenn derselbe Fehler erneut auftritt:

```cpp
// Fehler wird NICHT erneut gesendet, sondern lokal gezählt
// Nach dem 1. Bericht: Zähler++ (lokal gespeichert)
// Nach 10 Vorkommen: Bericht mit occurrence_count: 10

// Speicherung in: ~/.config/mail-adler/error_cache.json
{
  "sha256_hash_of_error": {
    "first_occurrence": "2025-02-03T12:00:00Z",
    "occurrence_count": 5,
    "last_reported": "2025-02-03T13:00:00Z"
  }
}
```

### 2.4 Fehler-Upload

**Trigger-Punkte:**
1. Unmittelbar nach kritischem Fehler (mit User-Bestätigung)
2. Täglich um 8:00 Uhr (gesammelt)
3. Beim Beenden der Anwendung
4. Nach WiFi/Netzwerk-Wiederherstellung

**Upload-Ziel:**
```
POST https://mail-adler-telemetry.example.com/api/v1/errors
Authorization: Bearer <api-key>
Content-Type: application/json

[
  { error_json_1 },
  { error_json_2 },
  ...
]
```

**Fehlerbehandlung beim Upload:**
- Lokal gecacht, wenn Netzwerk nicht verfügbar
- Max. 1000 Fehler gecacht (älteste werden verworfen)
- Keine Blockierung der UI während Upload

---

## 3. Nutzungsstatistiken

### 3.1 Sammlung (nur wenn aktiviert)

```json
{
  "session_id": "uuid-v4",
  "timestamp": "2025-02-03T12:00:00Z",
  "application": {
    "version": "0.1.0-beta"
  },
  
  "usage": {
    "feature_usage": {
      "imap_sync_count": 5,
      "smtp_send_count": 2,
      "read_messages_count": 50,
      "compose_messages_count": 3
    },
    
    "sync_statistics": {
      "successful_syncs": 98,
      "failed_syncs": 2,
      "average_sync_duration_seconds": 12.5,
      "total_messages_synced": 1250
    },
    
    "ui_interactions": {
      "features_used": ["MailList", "MailView", "Compose"],
      "session_duration_seconds": 3600
    }
  },
  
  "system": {
    "os": "Windows 11",
    "architecture": "x86_64"
  }
}
```

**Upload:** Täglich um 8:00 Uhr (zusammen mit Fehlern)

---

## 4. Expert-Modus

### 4.1 Aktivierung

**Menü:** `Einstellungen → Expertenoptionen → Expert-Modus aktivieren`

```cpp
// src/telemetry/ExpertMode.h/cpp
class ExpertMode {
public:
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Zeige Telemetrie-Daten an
    void showSentErrorReports();      // Fehler, die zu uns gesendet wurden
    void showReceivedUpdates();       // Updates, die von uns kamen
    void showTelemetryLog();          // Vollständiges Telemetrie-Log
};
```

### 4.2 Expert-Modus UI

#### Fenster: "Telemetrie-Inspektor"

```
┌──────────────────────────────────────────────────────┐
│  Telemetrie-Inspektor                          [X]    │
├──────────────────────────────────────────────────────┤
│                                                      │
│  📤 Zu uns gesendete Fehler                         │
│  ├─ 2025-02-03 12:34 | IMAP_CONNECTION_FAILED      │
│  ├─ 2025-02-03 11:20 | SYNC_FAILED (5x)            │
│  └─ 2025-02-02 15:45 | SMTP_SEND_FAILED            │
│                                                      │
│  📥 Von uns empfangene Meldungen                    │
│  ├─ 2025-02-03 08:00 | Spam-Liste aktualisiert     │
│  ├─ 2025-02-02 08:00 | Feature-Ankündigung         │
│  └─ 2025-02-01 10:30 | Sicherheitsupdate           │
│                                                      │
│  [Details ansehen] [Export als JSON]               │
└──────────────────────────────────────────────────────┘
```

#### Details-Ansicht (Fehler)
```json
{
  "error_id": "uuid-1234",
  "type": "IMAP_CONNECTION_FAILED",
  "timestamp": "2025-02-03T12:34:56Z",
  "status": "SENT",
  "sent_at": "2025-02-03T13:00:00Z",
  "message": "Connection timeout after 30s",
  "stack_trace": "..."
}
```

---

## 5. Fehlerbehandlung mit Benutzer-Dialog

### 5.1 Kritischer Fehler

Wenn ein kritischer Fehler auftritt:

```
┌─────────────────────────────────────────────────┐
│  Ein Fehler ist aufgetreten                     │
├─────────────────────────────────────────────────┤
│                                                 │
│  Ein unerwarteter Fehler hat die Anwendung     │
│  beeinträchtigt. Helfen Sie, Mail-Adler zu     │
│  verbessern, indem Sie diesen Fehler           │
│  berichten.                                     │
│                                                 │
│  Fehlernummer: ERR-20250203-001                │
│                                                 │
│  Was haben Sie zuvor getan?                    │
│  [______________________________]               │
│                                                 │
│  ☑ Fehler automatisch berichten                │
│                                                 │
│  [Bericht senden]  [Verwerfen]                 │
└─────────────────────────────────────────────────┘
```

### 5.2 Fehler-Tracking

Nach Fehler wird gespeichert:
```json
{
  "error_number": "ERR-20250203-001",
  "type": "IMAP_CONNECTION_FAILED",
  "timestamp": "2025-02-03T12:34:56Z",
  "user_description": "Ich habe auf 'Synchronisieren' geklickt",
  "was_reported": true,
  "reported_at": "2025-02-03T12:35:00Z"
}
```

Wenn **derselbe Fehler erneut auftritt**:
- Lokale Fehler-ID erhöhen (ERR-20250203-002)
- Benutzer wird erneut gefragt
- Bericht mit Details über vorherige Fehlerberichte senden

---

## 6. Spam-Liste Service

### 6.1 Zentrale Spam-Datenbank

Täglich um 9:00 Uhr:
- Alle Benutzer melden lokale Spam-Markierungen
- Zentrale Datenbank aggregiert und dedupliziert
- Um 10:00 Uhr: Clients fragen aktualisierte Spam-Liste ab

### 6.2 Spam-Liste Schema

```json
{
  "timestamp": "2025-02-03T09:00:00Z",
  "version": "20250203-0900",
  "entries": [
    {
      "email_hash": "sha256(spam@example.com)",
      "type": "PHISHING",
      "severity": 1.0,
      "reported_count": 157,
      "last_seen": "2025-02-03T08:45:00Z"
    },
    {
      "email_hash": "sha256(spam2@example.com)",
      "type": "BULK",
      "severity": 0.7,
      "reported_count": 43,
      "last_seen": "2025-02-03T07:30:00Z"
    }
  ]
}
```

### 6.3 Spam-Klassifikation

| Typ | Beschreibung | Aktion |
|-----|-------------|--------|
| `PHISHING` | Phishing/Social Engineering | Block alle |
| `MALWARE` | Malware-Quellen | Block alle |
| `SPAM_BULK` | Massenmails | Block für alle |
| `BLOCKED_BY_USER` | Einzelne Personen blockiert | Nur eigene Blockierung |

---

## 7. Tägliche Fehler-Zusammenfassung

### 7.1 Fehler-Zusammenfassung Email

**Täglich um 9:00 Uhr** sendet zentraler Service eine Email an `georg.dahmen@proton.me`:

```
Betreff: Mail-Adler Fehler-Zusammenfassung - 2025-02-03

─────────────────────────────────────────────────────

Gestrige Fehler (2025-02-02):

Kritische Fehler (2):
  • IMAP_CONNECTION_FAILED (5 Benutzer, 12 Vorkommen)
    → Mögliche Ursache: TLS-Upgrade auf Gmail-Konten
    → Link: https://mail-adler-telemetry.example.com/errors/IMAP_CONNECTION_FAILED
  
  • DATABASE_ERROR (1 Benutzer, 3 Vorkommen)
    → Mögliche Ursache: Schema-Migration fehlgeschlagen
    → Link: https://mail-adler-telemetry.example.com/errors/DATABASE_ERROR

Warnungen (8):
  • SYNC_FAILED (18 Benutzer)
  • UI_ERROR (4 Benutzer)
  ...

Spam-Report:
  • 242 neue Spam-Quellen gemeldet
  • Top 5: phishing@attacker1.com, spam@bulkmail2.ru, ...

Nutzungsstatistiken:
  • Aktive Benutzer: 1,250
  • Durchschn. Sync-Erfolgsrate: 98.3%
  • Durchschn. Session-Dauer: 35 Minuten

─────────────────────────────────────────────────────
Vollständiger Bericht: https://mail-adler-telemetry.example.com/reports/2025-02-02
```

---

## 8. Client-Update Mechanismus

### 8.1 Update-Check

**Täglich um 8:00 Uhr** fragt Client ab:
```
GET /api/v1/updates?version=0.1.0-beta&os=windows&arch=x86_64
```

**Antwort:**
```json
{
  "current_version": "0.1.0-beta",
  "latest_version": "0.2.0",
  "update_available": true,
  "critical": false,
  "download_url": "https://...",
  "release_notes": "...",
  "checksum": "sha256=..."
}
```

### 8.2 Update-Dialog

Wenn Update verfügbar:
```
┌─────────────────────────────────────────────────┐
│  Update verfügbar: Mail-Adler 0.2.0             │
├─────────────────────────────────────────────────┤
│                                                 │
│  Neue Features:                                 │
│  • Multi-Account Unterstützung                  │
│  • Verbesserte Verschlüsselung                  │
│  • 5 Bugfixes                                   │
│                                                 │
│  Größe: 45 MB                                  │
│                                                 │
│  [Jetzt aktualisieren]  [Später]               │
└─────────────────────────────────────────────────┘
```

---

## 9. Datenspeicherung (Server-Seite)

### 9.1 Fehler-Datenbank

```sql
CREATE TABLE telemetry_errors (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  error_id TEXT UNIQUE NOT NULL,
  error_type TEXT NOT NULL,
  message TEXT,
  severity TEXT,
  
  system_os TEXT,
  system_arch TEXT,
  app_version TEXT,
  
  account_provider TEXT,  -- "gmail", "gmx", "web.de", etc.
  
  stack_trace TEXT,
  hash TEXT UNIQUE,  -- Für Deduplizierung
  
  timestamp DATETIME,
  occurrence_count INTEGER DEFAULT 1,
  
  INDEX idx_error_type (error_type),
  INDEX idx_timestamp (timestamp),
  INDEX idx_hash (hash)
);
```

### 9.2 Aufbewahrungsrichtlinie

- **Detaillierte Fehler:** 90 Tage
- **Aggregierte Statistiken:** 1 Jahr
- **Spam-Liste:** Permanent (mit Deduplizierung)

---

## 10. Sicherheit & Datenschutz

### 10.1 Verschlüsselung

- Alle Übertragungen: **HTTPS/TLS 1.3**
- Passwörter/Tokens: **Keine** im Telemetrie-Daten
- Stack-Traces: Redaktioniert, um Dateipfade zu verbergen

### 10.2 Anonymisierung

- Account-ID: **gehashed** (SHA256)
- Benutzernamen: **nicht erfasst**
- IP-Adressen: **nicht gespeichert**
- Email-Adressen: Nur gehashed (für Spam-Liste)

### 10.3 Benutzerkontrolle

```cpp
// src/telemetry/TelemetrySettings.h
class TelemetrySettings {
public:
    bool errorReportingEnabled() const;
    void setErrorReportingEnabled(bool enabled);
    
    bool usageStatisticsEnabled() const;
    void setUsageStatisticsEnabled(bool enabled);
    
    // Daten exportieren
    void exportAllTelemetryData(const QString &filePath);
    
    // Daten löschen
    void deleteAllLocalTelemetryData();
    void deleteOldTelemetryData(int daysOld);
};
```

---

## 11. Implementierungs-Roadmap

### Phase 1: Fehlerberichterstattungs-Kern
- [ ] ErrorReporter Klasse
- [ ] Error-JSON Schema
- [ ] Fehler-Deduplication
- [ ] Lokale Caching-Logik

### Phase 2: Server-Infrastruktur
- [ ] Telemetry-API Server aufsetzen
- [ ] Fehler-Datenbank
- [ ] Spam-Liste Service
- [ ] Update-Check Endpoint

### Phase 3: Client-Integration
- [ ] Fehler-Dialog UI
- [ ] Expert-Modus
- [ ] Onboarding-Consent Dialog
- [ ] Tägliche Synchronisierung

### Phase 4: Monitoring & Analyse
- [ ] Dashboard für Entwickler
- [ ] Fehler-Trends Analyse
- [ ] Spam-Statistiken
- [ ] Tägliche Summary-Emails

---

## 12. Kontakt & Support

**Fehlerberichterstattung:** georg.dahmen@proton.me
**Sicherheitsbedenken:** security@mail-adler.dev
**Datenschutz:** privacy@mail-adler.dev

---

## 13. FAQ

**F: Werden meine Passwörter übertragen?**
A: Nein. Passwörter und API-Tokens werden niemals in Fehlerberichten erfasst.

**F: Kann ich Telemetrie deaktivieren?**
A: Ja. Einstellungen → Datenschutz → Telemetrie-Optionen.

**F: Wie lange werden Fehler gespeichert?**
A: 90 Tage detaillierte Fehler, dann aggregierte Statistiken für 1 Jahr.

**F: Sind meine Daten exportierbar?**
A: Ja. Einstellungen → Datenschutz → "Alle Telemetrie-Daten exportieren".

**F: Kann ich einen Fehler manuell löschen?**
A: Ja. Expert-Modus → Telemetrie-Inspektor → Fehler auswählen → Löschen.
