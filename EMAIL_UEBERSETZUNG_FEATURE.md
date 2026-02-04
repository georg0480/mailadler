# Email-Übersetzungs-Feature (Phase C)

## 1. Ollama besser ansteuern (weniger kreativ)

### Problem:
```
>>> übersetze ins englische: Eingang
Mistral antwortet: "Ihr Schreiben enthält eine Mischung..."
❌ Zu viel Text!
```

### Lösung: Strikter Prompt

```bash
ollama run mistral:7b

# Vor jeder Frage eingeben (einmalig):
>>> Du bist ein Übersetzer.
>>> Antworte NUR mit dem Übersetzungs-Wort.
>>> KEINE Erklärung.
>>> EINE Zeile.

# Dann:
>>> Englisch: Eingang
Inbox

>>> Englisch: Synchronisieren
Synchronize
```

**Oder: Simpler Prompt in Python:**

```python
prompt = """Du bist Übersetzer. Antworte NUR mit dem Wort.
Englisch: Eingang"""

# Mistral antwortet: "Inbox"
```

---

## 2. Email-Übersetzung als Feature (Phase C)

### Architektur

```cpp
// src/translation/EmailTranslator.h/cpp
class EmailTranslator {
public:
    // On-Demand Übersetzung
    QString translateEmail(
        const MailMessage &email,
        const QString &targetLanguage  // "Deutsch", "Französisch", etc.
    );
    
    // Speichere Übersetzung in DB
    void cacheTranslation(
        const QString &emailId,
        const QString &language,
        const QString &translatedText
    );
    
    // Prüfe ob schon übersetzt
    QString getCachedTranslation(
        const QString &emailId,
        const QString &language
    );
    
    // Zeichenlimit prüfen
    int getRemainingCharacters(const QString &service);  // "deepl"
};
```

### UI: Übersetzungs-Button

```
Email angezeigt:
┌──────────────────────────────────┐
│ Von: alice@gmail.com             │
│ Betreff: Bonjour                 │
├──────────────────────────────────┤
│ Original:                         │
│ Bonjour, comment allez-vous?    │
│                                  │
│ [🌍 Übersetzen zu Deutsch]       │  ← Button
│                                  │
│ Übersetzung (gecacht):           │
│ Hallo, wie geht es dir?         │
└──────────────────────────────────┘
```

---

## 3. Character-Budgeting für DeepL

### DeepL Free: 12.500 Zeichen/Monat

**Umrechnung:**

```
Durchschnittliche Email:
- Header (Von, An, Betreff): ~100 Zeichen
- Body: 500-2000 Zeichen
- Total: ~600 Zeichen pro Email

12.500 Zeichen / 600 = ~20 Emails/Monat kostenlos

ODER:
Wenn du viele Emails übersetzt:
12.500 / 30 Tage = 416 Zeichen/Tag
= ~1 lange Email pro Tag kostenlos
```

### Character-Tracking implementieren

```python
#!/usr/bin/env python3
# src/translation/deepl_budget.py

import json
from datetime import datetime, timedelta
from pathlib import Path

class DeepLBudget:
    def __init__(self, api_key: str):
        self.api_key = api_key
        self.budget_file = "~/.config/mail-adler/deepl_budget.json"
        self.monthly_limit = 12500
        self.budget = self.load_budget()
    
    def load_budget(self):
        """Lade Budget-Tracking"""
        if Path(self.budget_file).exists():
            with open(self.budget_file, 'r') as f:
                return json.load(f)
        
        return {
            "month": datetime.now().strftime("%Y-%m"),
            "used": 0,
            "remaining": 12500,
            "history": []
        }
    
    def save_budget(self):
        """Speichere Budget"""
        with open(self.budget_file, 'w') as f:
            json.dump(self.budget, f, indent=2)
    
    def translate_email(self, email_text: str, target_lang: str) -> dict:
        """Übersetze mit Budgetprüfung"""
        
        # Prüfe Limit
        estimated_chars = len(email_text) + 100  # +100 für API-Overhead
        
        if estimated_chars > self.budget["remaining"]:
            return {
                "success": False,
                "error": f"Budget überschritten! Nur {self.budget['remaining']} Zeichen übrig.",
                "remaining": self.budget["remaining"],
                "limit": self.monthly_limit
            }
        
        # Übersetze
        import deepl
        translator = deepl.Translator(self.api_key)
        
        result = translator.translate_text(email_text, target_lang=target_lang)
        
        # Update Budget
        self.budget["used"] += len(email_text)
        self.budget["remaining"] = self.monthly_limit - self.budget["used"]
        self.budget["history"].append({
            "timestamp": datetime.now().isoformat(),
            "language": target_lang,
            "characters": len(email_text)
        })
        self.save_budget()
        
        return {
            "success": True,
            "translation": result.text,
            "used": self.budget["used"],
            "remaining": self.budget["remaining"]
        }
    
    def reset_if_new_month(self):
        """Reset Budget am 1. des Monats"""
        current_month = datetime.now().strftime("%Y-%m")
        
        if self.budget["month"] != current_month:
            self.budget["month"] = current_month
            self.budget["used"] = 0
            self.budget["remaining"] = self.monthly_limit
            self.budget["history"] = []
            self.save_budget()

if __name__ == '__main__':
    budget = DeepLBudget("your-api-key")
    
    # Check Budget
    print(f"Genutzet: {budget.budget['used']} Zeichen")
    print(f"Übrig: {budget.budget['remaining']} Zeichen")
    
    # Übersetze
    result = budget.translate_email(
        "Bonjour, comment allez-vous?",
        "DE"
    )
    
    print(result)
```

### In C++:

```cpp
// src/translation/DeepLBudget.h/cpp
class DeepLBudget {
private:
    int monthly_limit = 12500;
    int used = 0;
    QString budget_file;
    
public:
    bool canTranslate(int estimated_chars) {
        return (used + estimated_chars) <= monthly_limit;
    }
    
    int getRemainingCharacters() {
        return monthly_limit - used;
    }
    
    void updateUsage(int chars) {
        used += chars;
        saveBudget();
    }
    
    void resetIfNewMonth() {
        // Check Datum, reset wenn neuer Monat
    }
};
```

---

## 4. Übersetzungs-Caching (nie doppelt übersetzen)

### Strategie: Lokale Datenbank

```sql
CREATE TABLE email_translations (
  id INTEGER PRIMARY KEY,
  email_id TEXT UNIQUE,
  source_language TEXT,
  target_language TEXT,
  original_text TEXT,
  translated_text TEXT,
  timestamp DATETIME,
  character_count INTEGER
);

-- Beispiel:
INSERT INTO email_translations VALUES (
  1,
  "gmail_abc123",
  "Französisch",
  "Deutsch",
  "Bonjour, comment allez-vous?",
  "Hallo, wie geht es dir?",
  "2025-02-03 14:30:00",
  35
);
```

### In C++:

```cpp
// src/translation/TranslationCache.h/cpp
class TranslationCache {
private:
    Database *m_db;
    
public:
    // Cache prüfen
    QString getCachedTranslation(
        const QString &emailId,
        const QString &language
    ) {
        // SELECT translated_text WHERE email_id = ?
        // RETURN cached version
    }
    
    // Cachen speichern
    void cacheTranslation(
        const QString &emailId,
        const QString &language,
        const QString &translatedText,
        int characterCount
    ) {
        // INSERT INTO email_translations
    }
    
    // Statistik
    int getTotalCharactersTranslated() {
        // SELECT SUM(character_count)
    }
};

// Nutzung:
EmailTranslator translator;
TranslationCache cache;

// 1. Check Cache
QString cached = cache.getCachedTranslation("email123", "Deutsch");
if (!cached.isEmpty()) {
    // Zeige cached Übersetzung
    ui->translationLabel->setText(cached);
    return;  // Keine API-Anfrage nötig!
}

// 2. Neu übersetzen
QString translated = translator.translateEmail(email, "Deutsch");

// 3. Cache speichern
cache.cacheTranslation("email123", "Deutsch", translated, translated.length());
```

---

## 5. On-Demand Übersetzung (Klick-Button oder Shortcut)

### Workflow:

```
Email öffnen:
┌──────────────────────────────────┐
│ Von: alice@gmail.com             │
│ Betreff: Bonjour                 │
├──────────────────────────────────┤
│ Bonjour, comment allez-vous?    │
│ Je suis heureux de vous écrire. │
│                                  │
│ [🌍 Zu Deutsch übersetzen]       │  ← Click hier
│   [Zu Englisch übersetzen]       │
│   [Zu Spanisch übersetzen]       │
│                                  │
│ ⟳ (Übersetzung läuft...)         │  ← Loading
│                                  │
│ Deutsch:                         │
│ Hallo, wie geht es dir?         │  ← Übersetzung angezeigt
│ Ich freue mich, dir zu schreiben.│
│                                  │
│ [× Übersetzung ausblenden]       │
└──────────────────────────────────┘
```

### Keyboard Shortcut:

```
Strg + Shift + T  → Übersetzungs-Dialog öffnen
                   (wähle Zielsprache)

Oder:
Strg + Shift + 1  → Übersetze zu Deutsch
Strg + Shift + 2  → Übersetze zu Englisch
Strg + Shift + 3  → Übersetze zu Französisch
... etc.
```

### C++ Implementation:

```cpp
// In MailViewWidget
void MailViewWidget::setupTranslationShortcuts() {
    // Ctrl+Shift+T → Dialog
    new QShortcut(
        Qt::CTRL + Qt::SHIFT + Qt::Key_T,
        this,
        SLOT(on_translateEmail_triggered())
    );
    
    // Ctrl+Shift+D → Deutsch
    new QShortcut(
        Qt::CTRL + Qt::SHIFT + Qt::Key_D,
        this,
        [this]() { translateEmailTo("Deutsch"); }
    );
}

void MailViewWidget::translateEmailTo(const QString &language) {
    if (!m_currentEmail) return;
    
    // Check Cache
    QString cached = TranslationCache::instance()
        .getCachedTranslation(m_currentEmail->id(), language);
    
    if (!cached.isEmpty()) {
        // Sofort zeigen (aus Cache)
        showTranslation(cached);
        return;
    }
    
    // Übersetzung starten
    ui->translationLabel->setText("⟳ Übersetzung läuft...");
    
    // Async Translation (nicht blockieren!)
    QtConcurrent::run([this, language]() {
        EmailTranslator translator;
        
        QString translated = translator.translateEmail(
            m_currentEmail->body(),
            language
        );
        
        // Cache speichern
        TranslationCache::instance().cacheTranslation(
            m_currentEmail->id(),
            language,
            translated,
            translated.length()
        );
        
        // UI Update
        QMetaObject::invokeMethod(this, [this, translated]() {
            showTranslation(translated);
        });
    });
}

void MailViewWidget::showTranslation(const QString &translatedText) {
    ui->translationLabel->setText(translatedText);
    ui->hideTranslationButton->setVisible(true);
}
```

---

## 6. Performance & Geschwindigkeit

### Wie lange dauert Übersetzung?

```
DeepL API (online):
- Netzwerk-Latenz: 200-500ms
- API-Verarbeitung: 500-1000ms
- Total: ~1-1.5 Sekunden

Ollama lokal:
- Directe Verarbeitung: 2-5 Sekunden
- Keine Netzwerk-Verzögerung
- Total: ~2-5 Sekunden

Caching (aus DB):
- Datenbank-Abfrage: 10-50ms
- Total: ~0.05 Sekunden (sofort!)
```

### Optimierung: Async Translation

```cpp
// NICHT blockieren!
void translateEmail() {
    // ❌ FALSCH:
    QString translation = translator.translateEmail(text);
    // UI friert für 1-5 Sekunden ein!
    
    // ✅ RICHTIG:
    QtConcurrent::run([this]() {
        QString translation = translator.translateEmail(text);
        
        // Async callback
        QMetaObject::invokeMethod(this, [this, translation]() {
            ui->translationLabel->setText(translation);
        });
    });
    // UI bleibt responsive!
}
```

---

## 7. Grammatik & Rechtschreibung (C++)

### LanguageTool Integration

```cpp
// src/translation/GrammarChecker.h/cpp
class GrammarChecker {
private:
    QString m_language;
    
public:
    struct GrammarIssue {
        int start;
        int length;
        QString message;
        QStringList suggestions;
    };
    
    QVector<GrammarIssue> checkGrammar(const QString &text) {
        // LanguageTool REST API aufrufen
        // http://localhost:8081/v2/check
        
        QJsonObject params;
        params["text"] = text;
        params["language"] = m_language;
        
        // Sende Anfrage
        QNetworkAccessManager manager;
        QNetworkRequest request(QUrl("http://localhost:8081/v2/check"));
        
        QNetworkReply *reply = manager.post(request, 
            QJsonDocument(params).toJson());
        
        // Parse Antwort
        // ...
    }
    
    // Visuelle Markierung
    void highlightErrors(QTextEdit *editor, 
        const QVector<GrammarIssue> &issues) {
        
        for (const auto &issue : issues) {
            // Markiere fehlerhafte Stellen mit rot welligen Linien
            QTextCursor cursor(editor->document());
            cursor.setPosition(issue.start);
            cursor.setPosition(issue.start + issue.length, 
                QTextCursor::KeepAnchor);
            
            QTextCharFormat fmt;
            fmt.setUnderlineStyle(QTextCharFormat::WaveUnderline);
            fmt.setUnderlineColor(Qt::red);
            cursor.setCharFormat(fmt);
        }
    }
};

// Nutzung beim Schreiben:
void ComposeDialog::checkGrammarWhileTyping() {
    GrammarChecker checker;
    auto issues = checker.checkGrammar(ui->textEdit->toPlainText());
    checker.highlightErrors(ui->textEdit, issues);
}
```

---

## 8. Original unverändert, nur Anzeige übersetzen

### Strategie: Zwei Text-Widgets

```cpp
class MailViewWidget {
private:
    QTextEdit *m_originalText;      // Originale Email (readonly)
    QTextEdit *m_translatedText;    // Übersetzung (readonly)
    QTabWidget *m_textTabs;
    
public:
    void setupTranslationUI() {
        m_textTabs = new QTabWidget();
        
        // Tab 1: Original
        m_originalText = new QTextEdit();
        m_originalText->setReadOnly(true);
        m_textTabs->addTab(m_originalText, "Original");
        
        // Tab 2: Übersetzung
        m_translatedText = new QTextEdit();
        m_translatedText->setReadOnly(true);
        m_textTabs->addTab(m_translatedText, "Deutsch");
        
        // Layout
        auto layout = new QVBoxLayout();
        layout->addWidget(m_textTabs);
        setLayout(layout);
    }
    
    void displayEmail(const MailMessage &email) {
        // Originale Email
        m_originalText->setPlainText(email.body());
        
        // Übersetze (async)
        QtConcurrent::run([this, email]() {
            QString translated = translator.translateEmail(
                email.body(),
                "Deutsch"
            );
            
            QMetaObject::invokeMethod(this, [this, translated]() {
                m_translatedText->setPlainText(translated);
            });
        });
    }
};
```

**Resultat:**
```
┌─────────────────────────────┐
│ [Original] [Deutsch] [...]  │  ← Tabs
├─────────────────────────────┤
│ Bonjour, comment allez-vous?│  ← Original unverändert
│ Je suis heureux...          │
│                             │
│ (Click "Deutsch" Tab)       │
│                             │
│ Hallo, wie geht es dir?     │  ← Übersetzung
│ Ich freue mich...           │
└─────────────────────────────┘
```

---

## 9. Zusammenfassung: Praktische Email-Übersetzung

### Features (Phase C):

✅ **On-Demand Übersetzung**
- Button: "Zu Deutsch übersetzen"
- Shortcut: Ctrl+Shift+D

✅ **Caching (nie doppelt übersetzen)**
- SQLite Database
- Erste Übersetzung: 1-2 Sekunden
- Cache-Hit: 0.05 Sekunden

✅ **Budget-Tracking**
- DeepL: 12.500 Zeichen/Monat
- Warnung wenn Limit nah
- Statistik: X Zeichen genutzt, Y übrig

✅ **Original + Übersetzung**
- Tabs: Original | Deutsch | Englisch | ...
- Original unverändert
- Benutzer sieht beide

✅ **Grammatik-Prüfung**
- LanguageTool (kostenlos)
- Fehler rot unterstrichen
- Vorschläge bei Hover

✅ **Async (nicht blockieren)**
- UI bleibt responsive
- Loading-Indikator

### Kosten:
- DeepL Free: 12.500 Zeichen/Monat kostenlos
- LanguageTool: Kostenlos
- **TOTAL: €0**

### Performance:
- Erste Übersetzung: 1-2 Sekunden
- Cache-Hit: Sofort (0.05s)
- Ohne Blockierung: UI responsive
