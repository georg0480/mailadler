# Mail-Adler Final Roadmap

## Phase B - Mail-Core (AKTUELL)

### B1: IMAP/SMTP Grundlagen
- ✅ Englisch Strings manuell
- ✅ DeepL für andere Sprachen (CSV)
- ✅ Import & Compile automatisch
- ✅ GMX, Web.de, Telekom Support

### B2: Sicherheit & Datenschutz
- ✅ PSK-basierte E2EE Gruppen
- ✅ Cloud-Anhänge (verschlüsselt)
- ✅ Spam-Liste (dezentralisiert)
- ✅ Telemetrie optional

---

## Phase C - Email-Features + Kalender

### C1: Email-Übersetzung (ON-DEMAND ONLY)

**Strategie:**
```cpp
// Nur wenn User klickt!
void MailViewWidget::on_translateButton_clicked() {
    // 1. Check Cache (0.05s)
    QString cached = cache.get(emailId, "Deutsch");
    if (!cached.isEmpty()) {
        showTranslation(cached);
        return;  // SOFORT
    }
    
    // 2. DeepL API (1-2s)
    QtConcurrent::run([this]() {
        QString translated = deepl.translate(
            m_email.body(),
            "DE"
        );
        
        // Cache speichern
        cache.save(m_email.id(), "Deutsch", translated);
        
        // UI Update
        showTranslation(translated);
    });
    
    // UI zeigt: "⟳ Übersetzung läuft..."
    // Bleibt responsive
}
```

**Kosten:**
- DeepL Free: 12.500 Zeichen/Monat
- Nur wenn User klickt = minimale Nutzung
- Cache spart 95% der API-Calls

**OLLAMA RAUSNEHMEN** ✅
- Zu langsam (2-5s)
- Lokal = mehr Ressourcen
- DeepL ist besser + schneller

---

### C2: iCal-Kalender (GMX)

**Feature-Set:**

```
📅 KALENDER-VIEWS
├─ Monat (Standard)
├─ Woche (4 Wochen nebeneinander)
├─ Tag (Stunden-Übersicht)
└─ Agenda (Liste kommender Termine)

✏️ BEARBEITUNG
├─ Neuer Termin: [+ Neuer Termin] Button
├─ Termin bearbeiten: Doppel-Click
├─ Termin löschen: Right-Click → Löschen
├─ Automatisches Speichern zu GMX (iCal PUSH)
└─ Konflikt-Detection (Überschneidungen warnen)

🔍 TERMINFINDUNG (Meeting Scheduler)
├─ "Mit wem?" → E-Mail Adressen eingeben
├─ Laden: Verfügbarkeit von allen prüfen
├─ Zeige: Gemeinsame freie Slots
├─ Auto-Buchen: Erste freie Zeit → Termin erstellen
├─ Sende: Einladungen an alle
└─ Synchronisiere: Mit allen GMX-Kalendern
```

### iCal-Integration (RFC 5545)

```cpp
// src/calendar/CalendarManager.h/cpp
class CalendarManager {
private:
    QString m_gmxCalendarPath;  // iCal File Path
    Database *m_db;  // Local cache
    
public:
    // iCal Datei laden
    bool loadFromGMX(const QString &imapPath);
    
    // Event hinzufügen
    void addEvent(const CalendarEvent &event);
    
    // Event bearbeiten
    void updateEvent(const QString &eventId, const CalendarEvent &event);
    
    // Event löschen
    void deleteEvent(const QString &eventId);
    
    // Zu GMX speichern (IMAP APPEND)
    bool syncToGMX();
    
    // Termine in Bereich
    QVector<CalendarEvent> getEventsInRange(QDate start, QDate end);
};

struct CalendarEvent {
    QString id;  // UID
    QString title;
    QString description;
    QDateTime start;
    QDateTime end;
    QString location;
    QStringList attendees;  // Email-Adressen
    bool allDay;
    QStringList alarms;  // Vor 15min, 1h, 1d, etc.
};
```

### Terminfindung-Algorithmus

```cpp
// src/calendar/MeetingScheduler.h/cpp
class MeetingScheduler {
public:
    struct FreeSlot {
        QDateTime start;
        QDateTime end;
        int numberOfParticipantsAvailable;  // Alle verfügbar?
    };
    
    // Finde gemeinsame freie Zeiten
    QVector<FreeSlot> findFreeSlots(
        const QStringList &emailAddresses,  // ["alice@gmx.de", "bob@web.de", "charlie@gmail.com"]
        QDate start,
        QDate end,
        int durationMinutes = 60
    ) {
        // 1. Lade Kalender von allen
        QMap<QString, QVector<CalendarEvent>> allCalendars;
        for (const auto &email : emailAddresses) {
            allCalendars[email] = loadCalendarFromIMAP(email);
        }
        
        // 2. Finde Überschneidungen (freie Zeit wenn ALL verfügbar)
        QVector<FreeSlot> freeSlots;
        
        for (QDate date = start; date <= end; date = date.addDays(1)) {
            for (int hour = 8; hour <= 18; hour++) {
                QDateTime slotStart(date, QTime(hour, 0));
                QDateTime slotEnd = slotStart.addSecs(durationMinutes * 60);
                
                bool allFree = true;
                for (const auto &email : emailAddresses) {
                    if (hasConflict(allCalendars[email], slotStart, slotEnd)) {
                        allFree = false;
                        break;
                    }
                }
                
                if (allFree) {
                    freeSlots.push_back({slotStart, slotEnd, emailAddresses.size()});
                }
            }
        }
        
        return freeSlots;
    }
    
    // Buche erste freie Zeit automatisch
    void autoBookFirstAvailable(
        const QStringList &emailAddresses,
        const QString &title,
        const QString &description
    ) {
        auto slots = findFreeSlots(emailAddresses, QDate::currentDate(), QDate::currentDate().addDays(30), 60);
        
        if (!slots.isEmpty()) {
            // Buche ersten Slot
            auto firstSlot = slots.first();
            
            CalendarEvent event;
            event.title = title;
            event.description = description;
            event.start = firstSlot.start;
            event.end = firstSlot.end;
            event.attendees = emailAddresses;
            
            // 1. Erstelle Event
            calendar.addEvent(event);
            calendar.syncToGMX();
            
            // 2. Sende Einladungen
            for (const auto &email : emailAddresses) {
                sendMeetingInvitation(email, event);
            }
        }
    }
};
```

### UI: Terminfindung Dialog

```
┌────────────────────────────────────┐
│ Terminfindung                      │
├────────────────────────────────────┤
│                                    │
│ Mit wem?                           │
│ [alice@gmx.de]  [Entfernen]       │
│ [bob@web.de]    [Entfernen]       │
│ [charlie@gmail.com] [Entfernen]  │
│ [+ Weitere Person]                │
│                                    │
│ Dauer: [60 Minuten]                │
│ Suchbereich: [1 Woche] ab [heute] │
│                                    │
│ [Verfügbarkeiten laden...]         │
│ (Laden: Kalender von 3 Personen)   │
│                                    │
│ Freie Termine:                     │
│ ☑ Morgen 10:00-11:00 (Alle frei)  │
│   [Buchen]                         │
│ ☑ Morgen 14:00-15:00 (Alle frei)  │
│   [Buchen]                         │
│ ☑ Übermorgen 09:00-10:00 (Alle)   │
│   [Buchen]                         │
│                                    │
│ [Automatisch buchen] [Abbrechen]   │
└────────────────────────────────────┘
```

---

## Phase D - Google + Erweit.

### D1: Google (später - zu kompliziert jetzt)
```
Problem: 2-Factor Authentication kompliziert
Lösung: Phase D (wenn Zeit)

Features später:
- Google Calendar (iCal Export)
- Google Drive (Cloud-Attachment Integration)
- Gmail (über Google OAuth2)

Kosten: Kostenlos (aber kompliziert)
```

### D2: Weitere Features
- OpenPGP/PGP Integration
- S/MIME Zertifikate
- IMAP IDLE (Push-Notifications)
- Advanced Search
- Rules/Filters

---

## Implementation Roadmap

### Phase B Timeline (Nächste 2-3 Wochen)

```
Woche 1:
├─ IMAP Sync (GMX, Web.de)
├─ SMTP Send
├─ Database Schema
└─ Settings

Woche 2:
├─ Multi-Folder Support
├─ Spam-Liste Integration
├─ DeepL String-Übersetzung
└─ Testings (GMX/Web.de/Telekom)

Woche 3:
├─ Polish & Bugs
├─ Release v0.1.0
└─ Vorbereitung Phase C
```

### Phase C Timeline (3-4 Wochen danach)

```
Woche 1:
├─ iCal Parser
├─ Kalender-UI (Monat-View)
└─ IMAP iCal Support

Woche 2:
├─ Woche/Tag-Ansicht
├─ Bearbeitungs-Dialog
└─ Zu GMX speichern

Woche 3:
├─ Terminfindung-Algorithmus
├─ Meeting Scheduler UI
└─ Auto-Booking

Woche 4:
├─ Email-Übersetzung (On-Demand DeepL)
├─ Cache-System
└─ Testing
```

---

## Summary: Deine Anforderungen

### ✅ Email-Übersetzung
- **Nur On-Demand** (User klickt Button)
- **Ollama raus** (zu langsam)
- **DeepL nur wenn nötig** (Kosten minimal)
- **Cache** (niemals doppelt übersetzen)
- **Performance:** Cache-Hit 0.05s, DeepL 1-2s

### ✅ iCal-Kalender (GMX)
- **RFC 5545 Standard** (iCal)
- **Monat/Woche/Tag View**
- **Bearbeitung + Speicherung** (IMAP)
- **Terminfindung:** 
  - Eingabe: 3+ Email-Adressen
  - Laden: Verfügbarkeit prüfen
  - Zeigen: Freie Slots
  - Auto-Book: Erste freie Zeit buchen
  - Einladungen senden

### ✅ Google später (Phase D)
- Zu kompliziert jetzt (2FA)
- Nach Mail-Core stabil ist

---

## Kosten & Performance

```
Phase B (Mail-Core):
├─ DeepL: €0 (12.5K chars/Monat kostenlos)
├─ LanguageTool: €0
└─ Hosting: 1GB RAM, CPU niedrig

Phase C (Kalender):
├─ iCal: €0 (Standard Protocol)
├─ GMX IMAP: €0 (kostenlos)
└─ Hosting: +500MB RAM für Calendar DB

Phase D (Google):
├─ Google OAuth: €0 (aber kompliziert)
└─ Später entscheiden
```

---

## Nächste Konkrete Schritte

```
1. ✅ Phase B Strings übersetzen
   → DeepL CSV System verwenden
   
2. ✅ Phase B kompilieren & testen
   → GMX/Web.de/Telekom
   
3. ✅ Phase C Kalender entwickeln
   → iCal Parser
   → UI (Monat View)
   → Terminfindung
   
4. ⏳ Phase C Email-Übersetzung
   → DeepL On-Demand
   → Cache System
   
5. ⏳ Phase D Google (später)
```

**Fertig dokumentiert!** 🎯
