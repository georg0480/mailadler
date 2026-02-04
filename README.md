# Mail-Adler 🦅

<div align="center">

<img src="icons/mailadler-logo-512.png" alt="Mail-Adler Logo" width="200"/>

**Ein moderner, datenschutzfreundlicher E-Mail-Client für Windows, Linux und macOS**

*Entwickelt in Deutschland – mit Fokus auf Privatsphäre, Einfachheit und deutsche E-Mail-Provider*

[Features](#-features) • [Warum Mail-Adler?](#-warum-mail-adler) • [Installation](#-installation) • [Build](#-build) • [Roadmap](#-roadmap) • [Mitwirken](#-mitwirken)

---

</div>

## 🎯 Was ist Mail-Adler?

**Mail-Adler** ist ein Open-Source E-Mail-Client, der speziell für Benutzer entwickelt wurde, die Wert auf **Datenschutz**, **Einfachheit** und **Kontrolle über ihre Daten** legen.

Im Gegensatz zu webbasierten E-Mail-Diensten oder großen kommerziellen Clients speichert Mail-Adler deine E-Mails **lokal auf deinem Computer** – verschlüsselt und unter deiner vollständigen Kontrolle.

### Das Problem mit bestehenden E-Mail-Clients

| Problem | Typische Clients | Mail-Adler |
|---------|------------------|------------|
| **Datenschutz** | Daten auf fremden Servern, Tracking | Lokale Speicherung, kein Tracking |
| **Deutsche Provider** | Oft schlechter Support für GMX, Web.de | Optimiert für deutsche Anbieter |
| **Komplexität** | Überladene Oberflächen, zu viele Funktionen | Fokussiert auf das Wesentliche |
| **Kosten** | Abo-Modelle, Premium-Funktionen | 100% kostenlos, Open Source |
| **Abhängigkeit** | Cloud-Zwang, Vendor Lock-in | Läuft komplett offline |

### Für wen ist Mail-Adler?

- 👨‍💼 **Privatanwender** die ihre E-Mails sicher verwalten möchten
- 🏢 **Kleine Unternehmen** die DSGVO-konform arbeiten müssen
- 🔒 **Datenschutz-bewusste Nutzer** die keine Cloud-Dienste wollen
- 🇩🇪 **Nutzer deutscher E-Mail-Provider** (GMX, Web.de, Telekom, Posteo)
- 💻 **Entwickler** die einen erweiterbaren, modernen Client suchen

---

## 🌟 Warum Mail-Adler?

### 1. Datenschutz steht an erster Stelle

```
┌─────────────────────────────────────────────────────────┐
│  Deine E-Mails                                          │
│  ├─ Gespeichert: Nur auf DEINEM Computer               │
│  ├─ Verschlüsselt: SQLite + SQLCipher (AES-256)        │
│  ├─ Backup: Eine Datei – du kontrollierst sie          │
│  └─ Telemetrie: Standardmäßig AUS (opt-in)             │
└─────────────────────────────────────────────────────────┘
```

**Keine Cloud, kein Tracking, keine Werbung.**

### 2. Optimiert für deutsche E-Mail-Provider

Die meisten E-Mail-Clients sind für Gmail und Outlook optimiert. Mail-Adler wurde von Anfang an für die **beliebtesten deutschen Anbieter** entwickelt:

| Provider | IMAP | SMTP | Kalender | Besonderheiten |
|----------|------|------|----------|----------------|
| **GMX** | ✅ | ✅ | ✅ iCal | Volle Integration |
| **Web.de** | ✅ | ✅ | ✅ iCal | Volle Integration |
| **Telekom** | ✅ | ✅ | ⏳ | T-Online Mail Support |
| **Posteo** | ✅ | ✅ | ✅ | Datenschutz-freundlich |
| **Gmail** | ⏳ | ⏳ | ⏳ | Später (Phase D) |

### 3. Läuft überall – auch auf dem Raspberry Pi

Mail-Adler ist ressourcenschonend und läuft auf:

| Plattform | Status |
|-----------|--------|
| **Windows** (10/11) | ✅ Unterstützt |
| **Linux** (Ubuntu, Debian) | ✅ Unterstützt |
| **macOS** | ✅ Unterstützt |
| **Raspberry Pi** (ARM64) | ✅ Unterstützt |

→ Ideal für einen **Mail-Server zu Hause** auf dem Pi!

### 4. Einfach und übersichtlich

Mail-Adler konzentriert sich auf das, was du wirklich brauchst:

```
┌─────────────────────────────────────────────────────────┐
│ 📧 Eingang (12)        │  Von: alice@gmx.de            │
│ ├─ 📂 Arbeit (5)       │  Betreff: Projektbesprechung  │
│ ├─ 📂 Privat (3)       │  ─────────────────────────────│
│ ├─ 📂 Newsletter       │                               │
│ └─ 🗑️ Papierkorb       │  Hallo!                       │
│                        │                               │
│ [Gesendet]             │  Anbei die Dokumente für      │
│ [Entwürfe]             │  unser Meeting morgen...      │
│ [Archiv]               │                               │
│                        │  📎 Dokumente.pdf (2.3 MB)    │
│ ──────────────         │  📎 Präsentation.pptx (5 MB)  │
│ 📅 Kalender            │                               │
│ 📋 Aufgaben            │  [Antworten] [Weiterleiten]   │
└─────────────────────────────────────────────────────────┘
```

---

## ✨ Features

### 📧 E-Mail-Verwaltung

| Feature | Beschreibung |
|---------|--------------|
| **Multi-Account** | Mehrere E-Mail-Konten in einer Oberfläche |
| **IMAP/SMTP** | Volle Unterstützung mit SSL/TLS-Verschlüsselung |
| **Ordner-Sync** | Automatische Synchronisation aller Ordner |
| **Ungelesen-Navigation** | Klick auf (12) → Springt zur ersten ungelesenen E-Mail |
| **Keyboard-Shortcuts** | `n` = Nächste ungelesen, `r` = Antworten, `d` = Löschen |
| **Verzögerter Versand** | E-Mails planen: "Sende morgen um 9:00" |
| **Serienbriefe** | CSV-Import für Massen-E-Mails mit Personalisierung |

### 🔐 Sicherheit & Datenschutz

| Feature | Beschreibung |
|---------|--------------|
| **Lokale Speicherung** | Alle Daten auf deinem Computer, nicht in der Cloud |
| **SQLite + SQLCipher** | Datenbank mit AES-256-Verschlüsselung |
| **Ende-zu-Ende-Verschlüsselung** | PSK-basierte E2EE für Gruppen-Kommunikation |
| **Verschlüsselte Anhänge** | Cloud-Upload mit Verschlüsselung (optional) |
| **DSGVO-konform** | Automatische Löschung nach Aufbewahrungsfristen |
| **Dezentrale Spam-Liste** | Keine zentrale Datensammlung |
| **Transparente Telemetrie** | Standardmäßig **aktiv** für bessere Fehleranalyse – jederzeit abschaltbar, offline = keine Daten |

### 📎 Anhänge intelligent verwalten

```
Anhänge (3):
├─ 📄 Vertrag.pdf (2.3 MB)     [⬇️ Download] [👁️ Vorschau]
├─ 📊 Budget.xlsx (1.2 MB)     [⬇️ Download] [👁️ Vorschau]
└─ 🖼️ Logo.png (845 KB)        [⬇️ Download] [👁️ Vorschau]

💡 Anhänge werden erst heruntergeladen, wenn du sie brauchst!
   → Spart Speicherplatz und Bandbreite
```

**Lazy-Load:** Anhänge werden nur bei Klick heruntergeladen – nicht automatisch. Das spart Speicherplatz und beschleunigt die Synchronisation.

### 🌍 Mehrsprachige E-Mails übersetzen

Erhältst du E-Mails in fremden Sprachen? Mit einem Klick übersetzen:

```
┌─────────────────────────────────────────────────────────┐
│ From: partner@company.fr                                │
│ Subject: Proposition commerciale                        │
│ ─────────────────────────────────────────────────────── │
│ Bonjour,                                                │
│ Nous vous proposons une collaboration pour...          │
│                                                         │
│ [🌍 Übersetzen → Deutsch]                               │
└─────────────────────────────────────────────────────────┘
         ↓ (1-2 Sekunden via DeepL)
┌─────────────────────────────────────────────────────────┐
│ 🌍 ÜBERSETZUNG (Französisch → Deutsch)                  │
│ ─────────────────────────────────────────────────────── │
│ Guten Tag,                                              │
│ Wir schlagen Ihnen eine Zusammenarbeit vor für...      │
│                                                         │
│ [Original anzeigen]                                     │
└─────────────────────────────────────────────────────────┘
```

- **On-Demand:** Nur wenn du klickst (keine automatische Übersetzung)
- **Caching:** Einmal übersetzt = gespeichert (spart API-Kosten)
- **DeepL-Integration:** Hochwertige Übersetzungen

### 📅 Kalender-Integration (Phase C)

Termine direkt in Mail-Adler verwalten – synchronisiert mit deinem GMX/Web.de Kalender:

```
┌─────────────────────────────────────────────────────────┐
│ Februar 2025                        [< Monat >] [Heute] │
│ ─────────────────────────────────────────────────────── │
│ Mo   Di   Mi   Do   Fr   Sa   So                        │
│                               1    2                    │
│  3    4    5    6    7    8    9                        │
│      ┌──────────────┐                                   │
│ 10   │11 Meeting    │   12   13   14   15   16          │
│      │ 14:00-15:00  │                                   │
│      └──────────────┘                                   │
│ 17   18   19   20   21   22   23                        │
│ 24   25   26   27   28                                  │
└─────────────────────────────────────────────────────────┘

🔍 Terminfindung: "Wann haben Alice, Bob und Charlie Zeit?"
   → System prüft Kalender aller Teilnehmer
   → Zeigt freie Slots an
   → Ein Klick zum Buchen + Einladungen senden
```

### 📋 Task-Management per E-Mail

**Kein kompliziertes Issue-System** – alles läuft über E-Mail!

#### So funktioniert es:

1. **Täglich** bekommst du eine Daily Mail mit allen offenen Aufgaben (im Laufe des Tages – nicht alle gleichzeitig)
2. **Antworten = Aktion:** Einfach auf die Mail antworten um Aufgaben zu erstellen, Prio zu ändern oder als erledigt zu markieren
3. **Experten-Ansicht:** In Mail-Adler siehst du alle Mails von/zu deinem Team gefiltert

```
┌─────────────────────────────────────────────────────────┐
│ 📧 DAILY MAIL – Mail-Adler Tasks – 04.02.2025           │
│ ─────────────────────────────────────────────────────── │
│                                                         │
│ 📤 EXTERNE AUFGABEN (nach Deadline sortiert):           │
│                                                         │
│ #501 [Prio 5⭐⭐⭐⭐⭐] IMAP Login 2FA                     │
│      Status: IN ARBEIT (Georg)                          │
│      Deadline: 05.02.2025 (MORGEN!) 🔴                  │
│                                                         │
│ #450 [Prio 4⭐⭐⭐⭐] Kalender Integration                │
│      Status: OFFEN                                      │
│      Deadline: 07.02.2025                               │
│                                                         │
│ ───────────────────────────────────────────────────────│
│ 📥 INTERNE AUFGABEN (nach Priorität sortiert):          │
│                                                         │
│ #512 [Prio 5⭐⭐⭐⭐⭐] Refactor IMAP Client               │
│      Status: OFFEN                                      │
│                                                         │
│ #445 [Prio 3⭐⭐⭐] Unit Tests schreiben                  │
│      Status: OFFEN                                      │
│                                                         │
│ ───────────────────────────────────────────────────────│
│ 📜 HISTORIE zu deinen Aufgaben:                         │
│ ├─ #501: GitHub Issue #234 "2FA Problem" (gelöst)       │
│ └─ #512: Commit a3f82d1 "Add IMAP auth"                │
│                                                         │
│ ───────────────────────────────────────────────────────│
│ 💬 ANTWORTEN AUF DIESE MAIL:                            │
│                                                         │
│ • Neue Aufgabe:  NEW: [Titel] Prio [1-5] [Beschreibung] │
│ • Prio ändern:   PRIO #501 -> 3                         │
│ • Erledigt:      DONE #501                              │
│ • Übernehmen:    TAKE #512                              │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

#### Mustervorlage: So schreibst du gute Aufgaben

Antworte auf die Daily Mail mit diesem Format:

```
NEW: IMAP Login mit 2FA implementieren Prio 5

WAS: 
Two-Factor Authentication für GMX und Telekom hinzufügen.

WARUM:
Benutzer mit 2FA können sich sonst nicht anmelden.

ANFORDERUNGEN:
- [ ] GMX Support
- [ ] Telekom Support  
- [ ] App-Passwort Eingabe
- [ ] Fehlermeldung bei falschen Daten

FERTIG WENN:
- [ ] User mit 2FA kann sich anmelden
- [ ] Tests bestanden
```

**Kurzform** (wenn es schnell gehen muss):
```
NEW: Button-Farbe ändern Prio 2 Der Speichern-Button soll blau statt grau sein.
```

#### Prioritäten-System (3 Stufen)

| Prio | Farbe | Bedeutung | Beispiele |
|------|-------|-----------|-----------|
| 🟢 **1** | Grün | **Feature** – Neue Funktion | Neues Feature, Verbesserung, UI-Änderung |
| 🟠 **2** | Orange | **Fehler** – Bug beheben | Fehler der auftritt, falsches Verhalten |
| 🔴 **3** | Rot | **KRITISCH** – Sofort beheben! | Datenverlust, Crash, Sicherheitslücke |

**Warum Bugs vor Features?**

Wir priorisieren **Fehlerbehebung vor neuen Features**. Um Missbrauch zu verhindern (Feature als "Bug" melden):

| Schutzmaßnahme | Wie es funktioniert |
|----------------|---------------------|
| **Fehler-ID vom Client** | Nur echte Crashes/Fehler generieren eine ID |
| **Automatische Klassifizierung** | System erkennt: Bug vs. Feature-Request |
| **Öffentliche Transparenz** | Jeder kann auf GitHub sehen ob es ein Bug oder Feature ist |
| **Community-Review** | Bei Unklarheit entscheidet die Community |

#### Automatische Fehlerberichte per E-Mail

Wenn Mail-Adler abstürzt oder ein schwerer Fehler auftritt, kann der Client automatisch einen Fehlerbericht per E-Mail senden:

```
┌─────────────────────────────────────────────────────────┐
│ ⚠️ Mail-Adler ist auf ein Problem gestoßen              │
│ ─────────────────────────────────────────────────────── │
│                                                         │
│ Was ist passiert?                                       │
│ Fehler beim Synchronisieren des Posteingangs            │
│                                                         │
│ Was hast du zuletzt gemacht?                            │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Ich habe auf "Alle synchronisieren" geklickt...     │ │
│ │                                                     │ │
│ └─────────────────────────────────────────────────────┘ │
│                                                         │
│ Technische Details (wird automatisch gesendet):         │
│ • Fehler-ID: ERR-2025-0204-A3F8 (eindeutig vom Client) │
│ • Version: 0.2.1                                        │
│ • System: Windows 10                                    │
│ • Zeitpunkt: 04.02.2025 14:32:15                       │
│                                                         │
│ [📧 Fehlerbericht senden]  [Nicht senden]               │
└─────────────────────────────────────────────────────────┘
```

**Warum per E-Mail statt HTTP?**

| Aspekt | HTTP-Reporting | E-Mail-Reporting |
|--------|----------------|------------------|
| **Funktioniert wenn** | Server erreichbar | Client kann noch E-Mail senden |
| **Datenschutz** | Daten an unsere Server | Geht über dein E-Mail-Konto |
| **Eindeutigkeit** | Server generiert ID | Client generiert Fehler-ID |
| **Duplikate** | Möglich | Fehler-ID verhindert doppelte Meldungen |
| **User-Kontext** | Oft vergessen | User wird direkt gefragt |
| **Transparenz** | Issue-ID versteckt | **Fehler-ID = GitHub Issue ID** (öffentlich nachverfolgbar) |

→ Wenn der Client noch eine E-Mail senden kann, funktioniert das Reporting!
→ Die Fehler-ID aus dem Client ist **dieselbe** wie im GitHub Issue – du kannst den Status öffentlich verfolgen!

#### Was macht das System besonders?

| Feature | Beschreibung |
|---------|--------------|
| **Kein Login nötig** | Alles per E-Mail – du brauchst keine Website |
| **Prio automatisch** | System ordnet Schweregrad zu (1-5) |
| **Jeder kann Prio ändern** | Ist dir etwas wichtig? → `PRIO #123 -> 5` |
| **Auto-Historie** | Das System zeigt automatisch, was es zum Thema schon gab |
| **Experten-Ansicht** | Filter in Mail-Adler: Zeige nur Team-Kommunikation |
| **Auto-Assign** | Task erledigt? → Nächste wird automatisch zugewiesen |
| **Fehlerberichte per Mail** | Client sendet Fehler direkt per E-Mail |
| **Duplikat-Warnung** | Fehler-ID + Ähnlichkeits-Check verhindern Duplikate |

---

## 📥 Installation

### Windows

```bash
# Coming soon: Installer
# mailadler-setup.exe
```

### Linux (Ubuntu/Debian)

```bash
# Coming soon
sudo apt install mailadler
```

### macOS

```bash
# Coming soon
brew install mailadler
```

**Aktuell:** Mail-Adler befindet sich in aktiver Entwicklung (Phase B). Für frühen Zugang: [Build from Source](#-build)

---

## 🔧 Build

### Voraussetzungen

| Komponente | Version | Hinweis |
|------------|---------|---------|
| **Qt** | 6.4+ | Core, Widgets, Network, Sql |
| **CMake** | 3.16+ | Build-System |
| **C++ Compiler** | C++17 | GCC 9+, Clang 10+, MSVC 2019+ |
| **OpenSSL** | 1.1+ | Für IMAP/SMTP SSL-Verschlüsselung |

### Build-Anleitung

```bash
# 1. Repository klonen
git clone https://github.com/georg0480/mailadler.git
cd mailadler

# 2. Build-Verzeichnis erstellen
mkdir build && cd build

# 3. CMake konfigurieren
cmake -DCMAKE_BUILD_TYPE=Release ..

# 4. Kompilieren (parallel für Geschwindigkeit)
cmake --build . --parallel

# 5. (Optional) Installieren
cmake --install .
```

### Mit Qt Creator (empfohlen für Entwickler)

1. Qt Creator öffnen
2. `File` → `Open File or Project`
3. `CMakeLists.txt` im Projektverzeichnis auswählen
4. Build-Kit auswählen (Qt 6.4+)
5. ▶️ Build & Run

---

## 🗺️ Roadmap

Mail-Adler wird in Phasen entwickelt:

| Phase | Status | Was wird gebaut? |
|-------|--------|------------------|
| **A** | ✅ Fertig | Grundgerüst, UI-Framework, Projektstruktur |
| **B** | 🔄 Aktuell | IMAP/SMTP, Sicherheit, Multi-Provider-Support |
| **C** | ⏳ Geplant | Kalender (iCal), E-Mail-Übersetzung (DeepL) |
| **D** | ⏳ Später | Google-Integration, OpenPGP, erweiterte Features |

### Phase B – Was passiert gerade?

```
✅ IMAP Sync (GMX, Web.de, Telekom)
✅ SMTP Versand
✅ Ende-zu-Ende-Verschlüsselung
✅ Dezentrale Spam-Liste
✅ Mehrsprachige UI (Deutsch, Englisch)
🔄 Multi-Account Support
🔄 Lazy-Load Anhänge
⏳ Aufbewahrungsfristen (Auto-Löschung)
```

Detaillierte Roadmap: [FINAL_ROADMAP.md](FINAL_ROADMAP.md)

---

## 🤝 Mitwirken

Wir freuen uns über jeden Beitrag – ob Code, Dokumentation, Übersetzung oder Feedback!

### Entwicklung

```bash
# 1. Fork erstellen (GitHub)

# 2. Lokal klonen
git clone https://github.com/DEIN-USERNAME/mailadler.git

# 3. Branch für dein Feature anlegen
git checkout -b feature/mein-feature

# 4. Änderungen machen und testen

# 5. Committen
git commit -m "Add: Beschreibung meines Features"

# 6. Push und Pull Request erstellen
git push origin feature/mein-feature
```

### Übersetzen

Übersetzungen werden mit einfachen CSV/TXT-Dateien verwaltet – kein kompliziertes System nötig!

```
translations/
├─ glossary_en.txt    # Englisch
├─ glossary_fr.txt    # Französisch  
├─ glossary_es.txt    # Spanisch
└─ ...
```

Format ist simpel:
```
Eingang = Inbox
Gesendet = Sent
Entwürfe = Drafts
```

Details: [EINFACHE_UEBERSETZUNG.md](EINFACHE_UEBERSETZUNG.md)

### Bugs melden & Features vorschlagen

- 🐛 **Bug gefunden?** → [Issue erstellen](https://github.com/georg0480/mailadler/issues/new)
- 💡 **Idee für Feature?** → [Discussion starten](https://github.com/georg0480/mailadler/discussions)

---

## 📄 Lizenz

Mail-Adler ist **100% Open Source** und lizenziert unter der **GNU General Public License v3.0**.

Das bedeutet:
- ✅ Kostenlos nutzen (privat und kommerziell)
- ✅ Code anschauen und ändern
- ✅ Weitergeben und verteilen
- ⚠️ Änderungen müssen auch Open Source sein (Copyleft)

Vollständiger Lizenztext: [COPYING](COPYING)

---

## 📚 Dokumentation

| Dokument | Beschreibung |
|----------|--------------|
| [FINAL_ROADMAP.md](FINAL_ROADMAP.md) | Detaillierte Entwicklungs-Roadmap |
| [ERWEITERTE_FEATURES.md](ERWEITERTE_FEATURES.md) | Geplante Features (Datenbank, Anhänge, etc.) |
| [PROJEKT_MANAGEMENT_SYSTEM.md](PROJEKT_MANAGEMENT_SYSTEM.md) | Task-Management Dokumentation |
| [EINFACHE_UEBERSETZUNG.md](EINFACHE_UEBERSETZUNG.md) | Übersetzungs-Workflow |
| [SICHERHEIT_VERSCHLUESSELUNG.md](SICHERHEIT_VERSCHLUESSELUNG.md) | Sicherheits-Konzepte |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Wie du beitragen kannst |

---

## 👥 Team

- **Georg** – Hauptentwickler & Projektleitung

---

## 📞 Kontakt & Community

- **GitHub Issues:** [Bug melden / Feature anfragen](https://github.com/georg0480/mailadler/issues)
- **GitHub Discussions:** [Fragen, Ideen, Community](https://github.com/georg0480/mailadler/discussions)

---

<div align="center">

**Made with ❤️ in Germany**

*Mail-Adler – Deine E-Mails, deine Daten, deine Kontrolle.*

</div>
