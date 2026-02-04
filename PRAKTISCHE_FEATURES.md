# Mail-Adler Praktische Features

## 1. Google Translate für Englisch → Andere Sprachen

### Workflow:
```
1. Englisch Strings exportieren
   python3 export_to_csv.py → glossary_en.csv

2. In Excel: Englisch-Spalte kopieren
   A1:A250 (alle English Strings)

3. Google Translate öffnen
   https://translate.google.com
   - Links: English Paste
   - Rechts: Wähle Zielsprache
   - Auto-Übersetzung

4. Ergebnis → Excel
   Gespiegelt in neue Spalte
   
5. CSV zurück → .ts
   python3 import_csv_to_ts.py
```

### Zeit sparen:
```
Deutsch → Englisch: Manuell sorgfältig (10 Min)
Englisch → Rest: Google Translate Auto (30 Sekunden pro Sprache)

Statt 30 Sprachen × 45 Min = 22.5 Std
Nur 30 Sprachen × 30 Sec = 15 Minuten!
```

**Das ist VIEL schneller!** ✅

---

## 2. Auto-Übersetzung für Email-Inhalte

### Feature für Phase C:

Wenn Email in anderer Sprache kommt → automatisch in deine Sprache übersetzen

```cpp
// src/translation/EmailTranslator.h/cpp
class EmailTranslator {
public:
    // Erkenne Sprache
    QString detectLanguage(const QString &emailBody);
    
    // Übersetze
    QString translateToUserLanguage(
        const QString &emailBody,
        const QString &detectedLanguage
    );
    
    // Nutze lokale LLM (Ollama)
    // Nicht Google (wegen Datenschutz!)
};
```

### Verwendung:
```cpp
// In MailViewWidget:
QString emailBody = "Bonjour, comment allez-vous?";

EmailTranslator translator;
QString detectedLang = translator.detectLanguage(emailBody);
// → "French"

if (detectedLang != userLanguage) {
    QString translated = translator.translateToUserLanguage(
        emailBody, 
        detectedLang
    );
    
    // Zeige Übersetzung
    ui->emailContent->setText(translated);
    
    // Zeige auch Original (kleiner)
    ui->originalContent->setText(emailBody);
}
```

### Optionen:

| Quelle | Datenschutz | Qualität | Geschwindigkeit |
|--------|-------------|----------|-----------------|
| **Google Translate API** | ❌ Schlecht | ✅✅ Sehr gut | ⚡ Schnell |
| **DeepL API** | ⚠️ EU | ✅✅ Sehr gut | ⚡ Schnell |
| **Ollama lokal** | ✅✅ Perfekt | ✅ Gut | ⚡⚡ Mittel |
| **LibreTranslate OSS** | ✅✅ Perfekt | ✅ Gut | ⚡⚡ Mittel |

**EMPFEHLUNG für Mail-Adler: Ollama lokal** (dezentralisiert!)

```python
# src/translation/ollama_translator.py
import requests

class OllamaEmailTranslator:
    def __init__(self, base_url="http://localhost:11434"):
        self.base_url = base_url
    
    def translate_email(self, text: str, target_lang: str) -> str:
        """Übersetze Email-Text mit lokalem Ollama"""
        
        prompt = f"""Übersetze folgende Email ins {target_lang}.
Halte Formatierung und Umlaute.
Antworte nur mit Übersetzung, keine Erklärung.

Text: {text}

Übersetzung:"""
        
        response = requests.post(
            f"{self.base_url}/api/generate",
            json={
                "model": "mistral:7b",
                "prompt": prompt,
                "stream": False,
            }
        )
        
        return response.json()["response"].strip()
```

---

## 3. Copy-Paste überall (Universell)

### Qt macht das automatisch:

```cpp
// In jedem QTextEdit/QLineEdit:
// Ctrl+C/Ctrl+V funktionieren IMMER
// Rechts-Klick → Copy/Paste funktioniert IMMER

// In QTableWidget/QTreeWidget:
// Auch Copy-Paste möglich (Zellen-Inhalte)

// Eigene Implementierung für Custom:
class CustomText : public QWidget {
    Q_OBJECT
private:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier) {
            QApplication::clipboard()->setText(selectedText());
        }
        if (event->key() == Qt::Key_V && event->modifiers() == Qt::ControlModifier) {
            pasteFromClipboard();
        }
    }
};
```

### In Mail-Adler:
- ✅ Email-Text: Copy-Paste überall
- ✅ Email-Header: Copy-Paste Absender, Betreff, etc.
- ✅ Anhang-Namen: Copy-Paste
- ✅ Links: Copy-Paste
- ✅ Metadaten: Alle markierbar & kopierbar

**Standard in Qt - keine spezielle Implementierung nötig!**

---

## 4. Tastatur-Shortcuts

### Mail-Adler Standard-Shortcuts

```
NAVIGATION:
├─ Tab              → Nächste Email / Feld
├─ Shift+Tab        → Vorherige Email / Feld
├─ Arrow Up/Down    → Navigation in Ordner/Liste
├─ Ctrl+Home        → Erste Email
├─ Ctrl+End         → Letzte Email
├─ Page Up/Down     → Seitenweise scrollen
└─ Escape           → Zurück zur Ordnerliste

LESEN:
├─ Space            → Page Down (Email lesen)
├─ Shift+Space      → Page Up
├─ Ctrl+F           → Im Text suchen
├─ Ctrl+P           → Email drucken
└─ Ctrl+Shift+V    → Plaintext-Modus

SCHREIBEN:
├─ Ctrl+N           → Neue Email
├─ Ctrl+Shift+D     → Aus Entwürfen fortfahren
├─ Tab              → Nächstes Feld (An → Cc → Betreff → Text)
├─ Ctrl+Enter       → Senden
└─ Ctrl+Shift+S     → Als Entwurf speichern

ORDNER:
├─ Ctrl+1           → Eingang
├─ Ctrl+2           → Gesendet
├─ Ctrl+3           → Entwürfe
├─ Ctrl+4           → Spam
├─ Ctrl+5           → Archiv
├─ Ctrl+6           → Custom Ordner
├─ Ctrl+Shift+N     → Neuer Ordner
└─ Delete           → Ordner löschen

AKTIONEN:
├─ Ctrl+R           → Antworten
├─ Ctrl+Shift+R     → Allen antworten
├─ Ctrl+Shift+F     → Weiterleiten
├─ Ctrl+M           → Als gelesen markieren
├─ Ctrl+*           → Als Markiert (*) togglen
├─ Delete           → Löschen → Papierkorb
├─ Ctrl+Delete      → Permanent löschen
├─ Ctrl+S           → Speichern / Synchronisieren
└─ F5               → Aktualisieren / Neu laden

ALLGEMEIN:
├─ Ctrl+Q           → Beenden
├─ Ctrl+,           → Einstellungen
├─ F1               → Hilfe
├─ Ctrl+H           → Verlauf
├─ Ctrl+L           → Adressleiste aktivieren
└─ Alt+Numbers     → Menu nutzen
```

### Implementierung in Qt:

```cpp
// src/ui/MainWindow.cpp

void MainWindow::setupKeyboardShortcuts() {
    // Neue Email
    new QShortcut(Qt::CTRL + Qt::Key_N, this, SLOT(on_actionNew_triggered()));
    
    // Antworten
    new QShortcut(Qt::CTRL + Qt::Key_R, this, SLOT(on_actionReply_triggered()));
    
    // Senden (in Compose)
    new QShortcut(Qt::CTRL + Qt::Key_Return, this, SLOT(on_actionSend_triggered()));
    
    // Navigation
    new QShortcut(Qt::CTRL + Qt::Key_1, this, [this]() {
        switchToFolder("Eingang");
    });
    
    new QShortcut(Qt::CTRL + Qt::Key_2, this, [this]() {
        switchToFolder("Gesendet");
    });
    
    // Löschen
    new QShortcut(Qt::Key_Delete, this, SLOT(on_actionDelete_triggered()));
    
    // Spam
    new QShortcut(Qt::CTRL + Qt::Key_Exlamation, this, SLOT(on_actionSpam_triggered()));
}
```

### Vim-Style Shortcuts (Optional für Phase D):

```cpp
// Für Power-User:
// :q → Beenden
// j/k → Down/Up
// d → Löschen
// a → Antworten
// r → Antworten
// w → Weiterleiten
// etc.

// Konfigurierbar in Einstellungen:
// [] Enable Vim Keybindings
```

---

## 5. Kombination: Englisch → Google Translate → Rest

### Praktischer Workflow (für dich):

```
Tag 1: ENGLISCH (Manuell - sorgfältig)
═══════════════════════════════════
python3 export_to_csv.py \
  --source translations/mail-adler_de.ts \
  --output glossary_all.csv \
  --languages "English"

→ Glossary mit Deutsch + leerer English-Spalte
→ LM Studio: 70 Wörter eingeben
   Abbrechen = Cancel
   Anmeldedaten = Credentials
   ... (30 Min)
→ Speichern

Day 2: GOOGLE TRANSLATE (Auto)
═══════════════════════════════════
1. Englisch-Spalte aus Excel kopieren (A1:A70)
2. Google Translate öffnen
3. Paste → Rechts: "Französisch" wählen
4. Auto-Übersetzung
5. Kopieren → Excel

→ Repeat für: Español, Português, Italiano, Niederländisch, Polnisch, ...
   (Pro Sprache: 2-3 Minuten)

Day 3: IMPORT & RELEASE
═══════════════════════
./batch_import_parallel.sh
   → 20-30 Sekunden
   
git push
   → GitHub Actions
   → Release

TOTAL: 2.5 Tage für 30 Sprachen statt 22 Tage!
```

### So sieht Excel aus:

```csv
Deutsch,English,Français,Español,Português,Italiano,Niederländisch,Polnisch,...
Abbrechen,Cancel,Annuler,Cancelar,Cancelar,Annulla,Annuleren,Anuluj,...
Anmeldedaten,Credentials,Identifiants,Credenciales,Credenciais,Credenziali,Inloggegevens,Poświadczenia,...
...
```

**Englisch = Manuell sorgfältig**
**Alles andere = Google Translate Auto**

---

## 6. Zusammenfassung: Praktische Mail-Adler Features

| Feature | Status | Nutzen |
|---------|--------|--------|
| **Auto-Translate Email-Inhalt** | Phase C | 🌍 User liest Emails in beliebiger Sprache |
| **Copy-Paste überall** | Phase B | ✅ Standard Qt |
| **Tastatur-Shortcuts** | Phase B | ⚡ Schnelles Arbeiten |
| **Englisch manuell** | Phase B | 👤 Sorgfaltig |
| **Englisch→Andere via Google** | Phase B | 🚀 Super schnell |
| **Vim-Keybindings** | Phase D | 🎮 Optional für Power-User |

---

## 7. Copy-Paste Implementierung (überall)

```cpp
// src/util/ClipboardHelper.h
class ClipboardHelper {
public:
    static QString getText() {
        return QApplication::clipboard()->text();
    }
    
    static void setText(const QString &text) {
        QApplication::clipboard()->setText(text);
    }
    
    static void copySelection(QAbstractItemView *view) {
        // Kopiere ausgewählte Zeilen/Zellen
        QModelIndexList indexes = view->selectionModel()->selectedIndexes();
        QString text;
        for (const auto &index : indexes) {
            text += index.data().toString() + "\t";
        }
        setText(text);
    }
};

// Nutzung überall:
// Rechts-Klick im Email-Text → Copy
// Rechts-Klick in Ordnerliste → Copy Ordnernamen
// etc. - alles Standard Qt!
```

---

## 8. Google Translate vs. Ollama für Email-Inhalt (Phase C)

```
Nutzer erhält Emails in verschiedenen Sprachen:

Option A: Google Translate (Phase C - später)
├─ Schnell ⚡
├─ Qualität sehr gut ✅✅
└─ Datenschutz ❌ (Daten zu Google)

Option B: Ollama lokal
├─ Schnell ⚡⚡ (lokal)
├─ Qualität gut ✅
└─ Datenschutz ✅✅ (alles lokal)

EMPFEHLUNG: Ollama (dezentralisiert!)
```

---

## Fazit:

✅ **Google Translate für deine Übersetzungsarbeit** (Englisch → Andere)
✅ **Ollama für User-Feature** (Email-Inhalt Auto-Übersetzen)
✅ **Copy-Paste überall** (Standard in Qt)
✅ **Tastatur-Shortcuts** (Schneller arbeiten)

**Alles machbar und praktisch!** 🎯
