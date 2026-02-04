# Mail-Adler Erweiterte Features

## 1. Datenbank: SQLite vs. MariaDB

### Analyse für ~3000 Mails

| Kriterium | SQLite | MariaDB |
|-----------|--------|---------|
| **Größe** | 1 Datei | Server-basiert |
| **3000 Mails Größe** | ~500MB-1GB | ~100-200MB |
| **Backup** | ✅ Einfach (Datei kopieren) | ⚠️ MySQL-Dumps nötig |
| **Verschlüsselung** | ⚠️ SQLCipher (extra) | ✅ TLS ready |
| **Aufbewahrungsfristen** | ✅ Einfach (SQL Trigger) | ✅ Einfach (SQL Trigger) |
| **Performance** | ✅ Gut für lokal | ⚠️ Remote-Latenz |
| **Installation** | ✅ Qt-built-in | ⚠️ Server setup |
| **Deployment** | ✅ Mit App | ❌ Extern |

### EMPFEHLUNG: **SQLite + SQLCipher** (Phase B)
- Lokal = privat + schnell
- Einfaches Backup (Datei)
- Encryption eingebaut
- Keine Server-Abhängigkeit

**MariaDB später (Phase E+)** wenn Multi-User/Sync nötig

---

## 2. Gesetzliche Aufbewahrungsfristen + Auto-Löschung

### Implementierung

```python
# src/storage/retention_policy.py

class RetentionPolicy:
    def __init__(self):
        self.policies = {
            # Deutschland/EU (DSGVO)
            "GDPR": {
                "email": 7 * 365,  # 7 Jahre (falls geschäftsrelevant)
                "attachment": 7 * 365,
                "deleted_email": 30,  # Gelöschte 30 Tage
                "spam": 30,  # Spam 30 Tage
            },
            # Schweiz (StG)
            "CHE": {
                "email": 7 * 365,
                "attachment": 7 * 365,
                "deleted_email": 30,
                "spam": 30,
            },
            # USA (verschiedene Staaten)
            "USA": {
                "email": 5 * 365,  # 5 Jahre
                "attachment": 5 * 365,
                "deleted_email": 30,
                "spam": 30,
            },
        }
    
    def schedule_auto_delete(self):
        """
        Daily Job: Lösche alte Emails/Anhänge
        """
        scheduler.add_job(
            self.delete_old_emails,
            'cron',
            hour=3,  # 03:00 nachts
            minute=0
        )
    
    def delete_old_emails(self):
        """
        Lösche Emails älter als retention_days
        Speichere vorher Hash zum Audit
        """
        region = Settings.retentionRegion()  # GDPR, CHE, USA
        policy = self.policies[region]
        
        # Email löschen
        cutoff_date = datetime.now() - timedelta(days=policy["email"])
        old_emails = db.query(
            "SELECT id, subject, date FROM emails WHERE date < ? AND folder != 'trash'",
            cutoff_date
        )
        
        for email in old_emails:
            # Audit Log speichern (bevor löschen)
            audit_log.record({
                "email_id": email["id"],
                "subject": email["subject"],
                "deleted_at": datetime.now(),
                "reason": "retention_policy_auto_delete"
            })
            
            # Anhänge löschen
            db.delete_attachments(email["id"])
            
            # Email löschen
            db.delete_email(email["id"])
        
        log.info(f"Deleted {len(old_emails)} old emails")
        
        # Spam löschen (schneller)
        spam_cutoff = datetime.now() - timedelta(days=policy["spam"])
        db.delete_emails(f"folder = 'spam' AND date < ?", spam_cutoff)
```

### UI: Aufbewahrungsrichtlinie einstellen

```
Einstellungen → Datenschutz

Aufbewahrungsrichtlinie:
├─ Land/Region: [GDPR - Deutschland/EU] ▼
│  └─ Emails: 7 Jahre
│  └─ Gelöschte: 30 Tage
│  └─ Spam: 30 Tage
│
├─ Auto-Löschung:
│  ☑ Aktiviert (täglich 03:00)
│  ☑ Audit-Log speichern
│
└─ Info: "Vollständige Compliance mit DSGVO"
```

---

## 3. Anhänge: Lazy-Load (Nur bei Anklick herunterladen)

### Architektur

```cpp
// src/attachment/AttachmentManager.h/cpp
class AttachmentManager {
private:
    struct AttachmentMetadata {
        QString id;
        QString filename;
        QString mime_type;
        int size;  // Bytes
        bool downloaded;  // false = noch nicht heruntergeladen
        QString local_path;  // "" wenn nicht downloaded
    };
    
public:
    // Zeige Anhang-Preview (nur Metadaten)
    QVector<AttachmentMetadata> getAttachmentsMetadata(QString emailId) {
        // Keine Daten herunterladen, nur Info:
        // Größe, Name, Typ anzeigen
    }
    
    // Download on Click
    void downloadAttachment(QString attachmentId) {
        // Erst wenn User klickt:
        // 1. Download vom Server
        // 2. In ~/.local/share/mail-adler/attachments/cache/
        // 3. Beim Anklick öffnen
    }
    
    // Auto-Cleanup (nach Öffnen)
    void autoCleanupOldAttachments() {
        // Nach 7 Tagen gelöschte Attachments aus Cache löschen
        // Originale bleiben im Email-Archive
    }
};
```

### UI: Anhang-Anzeige

```
Email von alice@gmx.de

Subject: Dokumente für Projekt

Anhänge (3):
├─ 📄 Vertrag.pdf (2.3 MB) [⬇️ Download] [🔗 Öffnen]
├─ 📊 Budget.xlsx (1.2 MB) [⬇️ Download] [🔗 Öffnen]
└─ 🖼️ Logo.png (845 KB) [⬇️ Download] [🔗 Öffnen]

(Nur Name + Größe angezeigt, Download erst auf Klick)
```

---

## 4. UI: Ungelesene-Zähler + Navigation

### Ordner-Panel mit Zähler

```
📧 Eingang (23)  ← 23 ungelesene
├─ 📂 Arbeit (8)
├─ 📂 Privat (5)
├─ 📂 Spam (10)
└─ 🗑️ Papierkorb (2)

[Gesendet]
[Entwürfe]
[Archiv]
```

### Click auf "(23)" → Erste ungelesen

```cpp
// src/ui/FolderPanel.h/cpp
void FolderPanel::onUnreadCountClicked(QString folder) {
    // 1. Hole erste ungelesene Email
    Email firstUnread = db.query(
        "SELECT * FROM emails WHERE folder = ? AND unread = 1 ORDER BY date DESC LIMIT 1",
        folder
    );
    
    // 2. Springe zu dieser Email
    emit navigateToEmail(firstUnread.id);
    
    // 3. Markiere als gelesen
    email.markAsRead();
    
    // 4. (Optional) Nächste ungelesen
    // Wenn User Taste drückt (z.B. 'n' für next unread)
}

void MailListView::onKeyPressed(Qt::Key key) {
    if (key == Qt::Key_N) {  // 'n' = next unread
        Email next = getNextUnreadInFolder();
        if (next.id()) {
            navigateToEmail(next.id);
            next.markAsRead();
        }
    }
}
```

### Tastatur-Shortcuts für Ungelesene

```
n  → Nächste ungelesen
p  → Vorherige ungelesen
u  → Markiere als ungelesen
f  → Markiere als gelesen

Beispiel:
User klickt auf (23) → Erste ungelesen wird angezeigt
User drückt 'n' → Nächste ungelesen
User drückt 'n' → Nächste ungelesen
... etc
```

---

## 5. Ungelesene-Handling: Spam & Blockierte

### Spam-Check

```python
# src/email/UnreadHandler.py
class UnreadHandler:
    def categorizeUnread(self, email):
        """
        Prüfe: Ist ungelesene Email in Spam?
        Ist ungelesene Email blockiert?
        """
        
        # 1. Spam-Check
        if email.folder == "spam":
            return {
                "unread": True,
                "spam": True,
                "blocked": False,
                "action": "Nicht zählen in normalem Ungelesen"
            }
        
        # 2. Blockiert-Check
        sender = email.from_address
        if db.isBlocked(sender):
            return {
                "unread": True,
                "spam": False,
                "blocked": True,
                "action": "Nicht zählen in normalem Ungelesen"
            }
        
        # 3. Normal
        return {
            "unread": True,
            "spam": False,
            "blocked": False,
            "action": "Zähle in (23)"
        }
```

### UI: Separate Zähler

```
📧 Eingang (23 normal) [🚫 5 blocked] [🚫 8 spam]
   └─ 23 = nur legitim ungelesen
   └─ 5 = blockierte Absender
   └─ 8 = Spam

Wenn User auf (23) klickt:
  → Erste legitim ungelesen
  
Wenn User auf [🚫 5] klickt:
  → Erste blockierte (aber nicht vorschalten)
```

---

## 6. Serienbriefe: Massenmails mit Vorlagen

### Implementierung

```cpp
// src/mail/MailMerge.h/cpp
class MailMerge {
public:
    struct Template {
        QString id;
        QString name;
        QString subject;  // Mit {{var}} Platzhaltern
        QString body;     // Mit {{var}} Platzhaltern
        QStringList variables;  // ["name", "email", "company"]
    };
    
    void createSeriesEmail(Template tmpl, QVector<QMap<QString, QString>> data) {
        """
        Erstelle Massen-Email aus Vorlage + Daten
        
        data = [
            {"name": "Alice", "email": "alice@...", "company": "ABC Ltd"},
            {"name": "Bob", "email": "bob@...", "company": "XYZ Corp"},
        ]
        
        Beispiel Vorlage:
        Subject: Hallo {{name}}!
        Body: Lieber {{name}}, 
              {{company}} hat sich für unsere Services interessiert...
        """
        
        for (auto &row : data) {
            // 1. Ersetze {{var}} durch Wert
            QString subject = tmpl.subject;
            QString body = tmpl.body;
            
            for (auto &[var, value] : row) {
                subject.replace("{{" + var + "}}", value);
                body.replace("{{" + var + "}}", value);
            }
            
            // 2. Erstelle Email
            Email email;
            email.to = row["email"];
            email.subject = subject;
            email.body = body;
            email.delayed = true;  // Verzögerter Versand
            
            // 3. Speichern
            m_pendingEmails.push_back(email);
        }
    }
};
```

### UI: Serienbriefe Dialog

```
┌──────────────────────────────────┐
│ Serienbriefe                     │
├──────────────────────────────────┤
│ Vorlage: [Kundenangebot] ▼       │
│                                  │
│ Empfänger-Liste (CSV):           │
│ [Durchsuchen...]                 │
│ ✓ header row (name, email, co)   │
│                                  │
│ Preview:                         │
│ ┌────────────────────────────────┐
│ │Subject: Hallo Alice!           │
│ │                                │
│ │Lieber Alice, ABC Ltd hat...    │
│ └────────────────────────────────┘
│                                  │
│ Versand:                         │
│ ☑ Verzögerter Versand            │
│ └─ Nach Email: [1] Minute        │
│                                  │
│ [Preview] [Senden] [Abbrechen]   │
└──────────────────────────────────┘
```

---

## 7. Verzögertes Versenden (Scheduled Send)

### Implementierung

```cpp
// src/mail/DelayedSend.h/cpp
class DelayedSend {
public:
    struct ScheduledEmail {
        QString id;
        QString to;
        QString subject;
        QString body;
        QDateTime sendAt;  // Wann versenden
        QString status;  // "scheduled", "sending", "sent", "cancelled"
    };
    
    void scheduleEmail(Email email, QDateTime sendAt) {
        """
        Plane Email-Versand für später
        """
        ScheduledEmail scheduled;
        scheduled.id = generateId();
        scheduled.to = email.to;
        scheduled.subject = email.subject;
        scheduled.body = email.body;
        scheduled.sendAt = sendAt;
        scheduled.status = "scheduled";
        
        db.insert("scheduled_emails", scheduled);
        
        // Zeige Timer in Entwürfe
        emit scheduledEmailCreated(scheduled);
    }
    
    void checkAndSendScheduled() {
        """
        Alle 1 Minute prüfen: Welche Emails sind reif zum Versenden?
        """
        auto now = QDateTime::currentDateTime();
        
        auto ready = db.query(
            "SELECT * FROM scheduled_emails WHERE sendAt <= ? AND status = 'scheduled'",
            now
        );
        
        for (auto &email : ready) {
            sendEmail(email);
            db.update("scheduled_emails", email.id, {"status": "sent"});
        }
    }
};
```

### UI: Entwürfe mit Timer

```
Entwürfe (3)

[📝 Kundenangebot für Alice]
├─ Status: Versand geplant
├─ Versend um: 2025-02-05 14:30
├─ Countdown: 2h 15min
└─ [❌ Abbrechen] [✏️ Bearbeiten]

[📝 Besprechungsnotizen]
├─ Status: Normal (nicht geplant)

[📝 Test Email]
├─ Status: Fehler beim Versand
├─ Fehler: "SMTP Error 550"
└─ [🔄 Erneut versuchen] [Löschen]

Versand abbrechen:
┌──────────────────────────┐
│ Email wird versendet in:  │
│                          │
│ ⏱ [████░░░░░░░░░] 30s   │
│                          │
│ [❌ Jetzt abbrechen]      │
│ [▶️ Weiter mit 'E']      │
└──────────────────────────┘
```

---

## 8. AD/Microsoft Integration (Phase D+)

### LDAP + Outlook Sync

```python
# src/integration/MicrosoftAD.py

class MicrosoftADIntegration:
    def __init__(self):
        self.ldap_server = "ldap://ad.company.com"
        self.graph_api = "https://graph.microsoft.com/v1.0"
    
    def syncContacts(self):
        """
        Hole Kontakte aus AD LDAP
        """
        conn = ldap.initialize(self.ldap_server)
        results = conn.search_s(
            "OU=Users,DC=company,DC=com",
            ldap.SCOPE_SUBTREE,
            "(objectClass=person)",
            ['mail', 'displayName', 'telephoneNumber']
        )
        
        # Speichere in lokale Kontakt-Datenbank
        for dn, attrs in results:
            contact = {
                "name": attrs['displayName'][0],
                "email": attrs['mail'][0],
                "phone": attrs.get('telephoneNumber', [''])[0],
                "source": "AD"
            }
            db.save_contact(contact)
    
    def syncCalendar(self, user_email):
        """
        Hole Outlook-Kalender über MS Graph API
        """
        # Benötigt OAuth2 Token
        headers = {"Authorization": f"Bearer {self.get_token()}"}
        
        response = requests.get(
            f"{self.graph_api}/me/events",
            headers=headers
        )
        
        for event in response.json()['value']:
            cal_event = {
                "title": event['subject'],
                "start": event['start']['dateTime'],
                "end": event['end']['dateTime'],
                "organizer": event['organizer']['emailAddress']['address'],
                "source": "outlook"
            }
            db.save_calendar_event(cal_event)
    
    def showADAvailability(self, email):
        """
        Zeige AD-User Verfügbarkeit im Kalender
        """
        # Prüfe: Ist User frei/busy?
        # Zeige in Kalender-UI
```

### UI: AD-Integration

```
Kontakte

Quelle: [Alle] [Lokal] [AD] [Outlook]

Alice Schmidt (AD)
├─ Email: alice@company.de
├─ Phone: +49-30-12345678
├─ Verfügbar: 10:30-11:30 (aus Outlook)
└─ [Termin vereinbaren]

Bob Müller (AD)
├─ Email: bob@company.de
├─ Verfügbar: Ganztag frei
└─ [Termin vereinbaren]
```

---

## 9. Zusammenfassung: Erweiterte Features

| Feature | Phase | Priorität | Komplexität |
|---------|-------|-----------|-------------|
| SQLite → MariaDB | E+ | Niedrig | Hoch |
| Aufbewahrungsfristen | B+ | Hoch | Mittel |
| Lazy-Load Anhänge | B+ | Hoch | Mittel |
| Ungelesene-Zähler | B+ | Hoch | Niedrig |
| Serienbriefe | C | Mittel | Hoch |
| Verzögerter Versand | C | Mittel | Mittel |
| AD/Outlook Integration | D+ | Niedrig | Hoch |

### MVP (Must-Have Phase B+):
1. ✅ SQLite mit SQLCipher
2. ✅ Aufbewahrungsfristen (Auto-Löschung)
3. ✅ Lazy-Load Anhänge
4. ✅ Ungelesene-Zähler + Navigation

### Nice-to-Have (Phase C+):
5. ⏳ Serienbriefe
6. ⏳ Verzögerter Versand
7. ⏳ AD Integration
