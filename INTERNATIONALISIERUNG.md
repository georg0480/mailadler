# Mail-Adler Internationalisierung (i18n) - Deutsch-First Strategie

## 1. Design-Prinzip: Deutsch als Master-Language

### 1.1 Warum Deutsch-First?

**Problem mit Englisch-First:**
```cpp
// ❌ FALSCH: Englisch im Code
const char *text = tr("Inbox");  // Später zu "Eingang" übersetzt
// Problem: UI-Layouts nicht optimal für Deutsch
// Deutsche Wörter sind meist länger → Layout-Probleme
```

**Richtig: Deutsch im Code**
```cpp
// ✅ RICHTIG: Deutsch zuerst
const char *text = tr("Eingang");  // Master ist Deutsch
// Automatisch zu "Inbox" übersetzt für Englisch
// UI optimiert für längere deutsche Wörter von Anfang an
```

### 1.2 Vorteile

| Aspekt | Englisch-First | Deutsch-First |
|--------|---|---|
| **UI-Layout** | ❌ Zu kurz | ✅ Optimal |
| **Übersetzungsqualität** | ⚠️ KI macht Fehler | ✅ Deutsche Muttersprachler |
| **Kontext** | ❌ Verloren | ✅ Im Code klar |
| **Performance** | ❌ Übersetzungs-Overhead | ✅ Native Sprache |
| **Wartbarkeit** | ❌ Verwirrend | ✅ Klar |
| **Marktposition** | ❌ Generisch | ✅ "Für Deutsche" |

---

## 2. Code-Architektur: Deutsch-Only Source Code

### 2.1 Alle String-Konstanten in Deutsch

```cpp
// src/ui/MainWindow.h
class MainWindow : public QMainWindow {
private:
    // ✅ Deutsch in Source Code
    QString m_title = "Mail-Adler";
    QString m_statusReady = "Bereit";
    QString m_statusSyncing = "Synchronisiere...";
    QString m_errorConnection = "Verbindungsfehler";
};

// src/models/MailFolder.h
enum StandardFolder {
    FOLDER_INBOX = "Eingang",      // Nicht "Inbox"
    FOLDER_SENT = "Gesendet",      // Nicht "Sent"
    FOLDER_DRAFTS = "Entwürfe",    // Nicht "Drafts"
    FOLDER_TRASH = "Papierkorb",   // Nicht "Trash"
    FOLDER_SPAM = "Spam",          // Nicht "Junk"
    FOLDER_ARCHIVE = "Archiv"      // Nicht "Archive"
};

// src/localization/Strings.h
namespace Strings {
    constexpr auto MENU_FILE = "Datei";
    constexpr auto MENU_EDIT = "Bearbeiten";
    constexpr auto MENU_VIEW = "Ansicht";
    constexpr auto MENU_TOOLS = "Werkzeuge";
    constexpr auto MENU_HELP = "Hilfe";
    
    constexpr auto ACTION_NEW = "Neu";
    constexpr auto ACTION_OPEN = "Öffnen";
    constexpr auto ACTION_SAVE = "Speichern";
    constexpr auto ACTION_EXIT = "Beenden";
    
    constexpr auto BUTTON_OK = "OK";
    constexpr auto BUTTON_CANCEL = "Abbrechen";
    constexpr auto BUTTON_APPLY = "Anwenden";
    constexpr auto BUTTON_CLOSE = "Schließen";
};
```

### 2.2 UI-Dateien (Qt Designer) in Deutsch

```xml
<!-- forms/mainwindow.ui -->
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="windowTitle">
   <string>Mail-Adler</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <layout class="QVBoxLayout" name="verticalLayout">
    <item>
     <widget class="QMenuBar" name="menubar">
      <widget class="QMenu" name="menuDatei">
       <property name="title">
        <string>Datei</string>
       </property>
       <addaction name="actionNeu"/>
       <addaction name="actionÖffnen"/>
      </widget>
     </widget>
    </item>
   </layout>
  </widget>
 </widget>
 <action name="actionNeu">
  <property name="text">
   <string>Neu</string>
  </property>
  <property name="shortcut">
   <string>Ctrl+N</string>
  </property>
 </action>
</ui>
```

### 2.3 CMakeLists.txt - Deutsch als Standard-Sprache

```cmake
# CMakeLists.txt

# Qt Internationalization
set(CMAKE_AUTORCC ON)

# Standard-Sprache: Deutsch
set(QT_TRANSLATIONS_DEFAULT_LANGUAGE "de_DE")

# Alle .ts Dateien (Translation Source) basierend auf Deutsch
set(TS_FILES
    translations/mail-adler_de.ts    # Master (Deutsch)
    translations/mail-adler_en.ts    # English
    translations/mail-adler_fr.ts    # Français
    translations/mail-adler_es.ts    # Español
    translations/mail-adler_it.ts    # Italiano
)

# Nur eine master .ts datei (Deutsch)
qt_add_translations(mailadler_app
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX "/translations"
)
```

---

## 3. i18n System: Dynamische Übersetzungen

### 3.1 Ressourcen-basiertes System (nicht hardcoded)

```cpp
// src/localization/LocalizationManager.h
class LocalizationManager : public QObject {
    Q_OBJECT
    
public:
    static LocalizationManager& instance();
    
    void setLanguage(const QString &langCode);  // "de_DE", "en_US", "fr_FR"
    QString tr(const QString &germanText);       // Übersetze Deutsch → aktuelle Sprache
    
    bool loadTranslations(const QString &langCode);
    
private:
    QTranslator *m_translator = nullptr;
    QString m_currentLanguage = "de_DE";
};

// Beispiel:
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Standard: Deutsch
    LocalizationManager::instance().setLanguage("de_DE");
    
    // Falls System-Sprache Englisch → English laden
    QString systemLang = QLocale::system().language();
    if (systemLang == "en") {
        LocalizationManager::instance().setLanguage("en_US");
    }
    
    MainWindow window;
    window.show();
    return app.exec();
}
```

### 3.2 Verwendung im Code

```cpp
// src/ui/MainWindow.cpp
#include "localization/LocalizationManager.h"

MainWindow::MainWindow() {
    auto &i18n = LocalizationManager::instance();
    
    // ✅ Einfach - übersetzen wenn nötig
    ui->menuDatei->setTitle(i18n.tr("Datei"));
    ui->menuBearbeiten->setTitle(i18n.tr("Bearbeiten"));
    ui->menuAnsicht->setTitle(i18n.tr("Ansicht"));
    
    // Mit Pluralisierung
    int messageCount = 5;
    QString text = i18n.tr("%1 ungelesene Nachricht(en)").arg(messageCount);
    // DE: "5 ungelesene Nachrichten"
    // EN: "5 unread messages"
    // FR: "5 messages non lus"
}
```

### 3.3 Translation Source File (mail-adler_de.ts)

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de_DE">
    <context>
        <name>MainWindow</name>
        <message>
            <location filename="src/ui/mainwindow.cpp" line="123"/>
            <source>Datei</source>
            <translation type="unfinished"></translation>
        </message>
        <message>
            <location filename="src/ui/mainwindow.cpp" line="124"/>
            <source>Bearbeiten</source>
            <translation type="unfinished"></translation>
        </message>
    </context>
</TS>
```

---

## 4. Übersetzungs-Management mit KI

### 4.1 Automatische Übersetzung (mit GPT)

**Workflow:**

```bash
# 1. Deutsch Source Code → Extrahiere alle Strings
lupdate -no-obsolete src/ -ts translations/mail-adler_de.ts
# Erzeugt: translations/mail-adler_de.ts (Master)

# 2. Übersetze zu allen anderen Sprachen (mit KI)
./scripts/translate_with_ai.py \
    --source translations/mail-adler_de.ts \
    --target en_US,fr_FR,es_ES,it_IT \
    --ai-engine gpt-4 \
    --output translations/
# Erzeugt: 
#   translations/mail-adler_en.ts
#   translations/mail-adler_fr.ts
#   translations/mail-adler_es.ts
#   translations/mail-adler_it.ts

# 3. Kompiliere Übersetzungen
lrelease translations/mail-adler_*.ts
# Erzeugt: translations/mail-adler_de.qm, mail-adler_en.qm, etc.
```

### 4.2 Python-Script für KI-Übersetzungen

```python
# scripts/translate_with_ai.py
#!/usr/bin/env python3

import openai
import xml.etree.ElementTree as ET
from pathlib import Path

class AITranslator:
    def __init__(self, api_key):
        openai.api_key = api_key
        self.cache = {}
    
    def translate(self, text: str, target_lang: str) -> str:
        """Übersetze Text von Deutsch zu Zielsprache mit GPT"""
        cache_key = f"{text}::{target_lang}"
        
        if cache_key in self.cache:
            return self.cache[cache_key]
        
        lang_names = {
            'en_US': 'English',
            'fr_FR': 'French',
            'es_ES': 'Spanish',
            'it_IT': 'Italian'
        }
        
        prompt = f"""
Übersetze folgendes Deutsch in {lang_names[target_lang]}.
Nur das Übersetzungs-Ergebnis ausgeben, keine Erklärung.
Behalte Formatierung und Sonderzeichen.

Deutsch: {text}
{lang_names[target_lang]}:
"""
        
        response = openai.ChatCompletion.create(
            model="gpt-4",
            messages=[{"role": "user", "content": prompt}],
            temperature=0.3  # Niedrig für Konsistenz
        )
        
        translation = response['choices'][0]['message']['content'].strip()
        self.cache[cache_key] = translation
        return translation
    
    def translate_ts_file(self, source_ts: str, target_lang: str) -> str:
        """Übersetze komplette .ts Datei"""
        tree = ET.parse(source_ts)
        root = tree.getroot()
        
        for message in root.findall('.//message'):
            source_elem = message.find('source')
            translation_elem = message.find('translation')
            
            if source_elem is not None and translation_elem is not None:
                source_text = source_elem.text
                translated = self.translate(source_text, target_lang)
                translation_elem.text = translated
                translation_elem.set('type', 'finished')
        
        return ET.tostring(root, encoding='unicode')

if __name__ == '__main__':
    import argparse
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--source', required=True)
    parser.add_argument('--target', required=True)
    parser.add_argument('--output', required=True)
    
    args = parser.parse_args()
    
    translator = AITranslator(api_key='your-api-key')
    
    for lang in args.target.split(','):
        print(f"Übersetze zu {lang}...")
        translated_xml = translator.translate_ts_file(args.source, lang.strip())
        
        output_path = f"{args.output}/mail-adler_{lang.split('_')[0]}.ts"
        Path(output_path).write_text(translated_xml)
        print(f"Gespeichert: {output_path}")
```

### 4.3 Qualitätskontrolle vor Release

```bash
# 1. Übersetzte Strings prüfen
./scripts/validate_translations.py translations/*.ts

# 2. Context-Mismatch prüfen
./scripts/check_context_consistency.py translations/mail-adler_*.ts

# 3. Längste Strings pro Sprache prüfen (UI-Layout)
./scripts/check_string_lengths.py translations/mail-adler_*.ts
```

---

## 5. Sprachen-Fallback & Lokalisierung

### 5.1 Fallback-Kette

```cpp
// src/localization/LocalizationManager.cpp
void LocalizationManager::setLanguage(const QString &langCode) {
    QLocale locale(langCode);
    
    // Fallback-Kette:
    // 1. Gewünschte Sprache (z.B. de_AT → de)
    // 2. Basis-Sprache (z.B. de_AT → de_DE)
    // 3. Englisch (fallback)
    // 4. Deutsch (Master)
    
    QStringList fallbackList;
    fallbackList << langCode;                    // de_AT
    fallbackList << langCode.split("_").first(); // de
    fallbackList << "en_US";                     // English
    fallbackList << "de_DE";                     // Deutsch (Master)
    
    for (const QString &lang : fallbackList) {
        if (m_translator->load(lang, ":/translations")) {
            QCoreApplication::installTranslator(m_translator);
            m_currentLanguage = lang;
            emit languageChanged(lang);
            return;
        }
    }
}
```

### 5.2 Lokale Format-Strings

```cpp
// src/localization/LocaleFormatting.h
class LocaleFormatting {
public:
    // Deutsche Datumsformate
    static QString formatDate_de(const QDateTime &dt) {
        return dt.toString("d. MMMM yyyy");  // "3. Februar 2025"
    }
    
    static QString formatTime_de(const QDateTime &dt) {
        return dt.toString("HH:mm Uhr");     // "14:30 Uhr"
    }
    
    static QString formatDateTime_de(const QDateTime &dt) {
        return formatDate_de(dt) + ", " + formatTime_de(dt);
    }
    
    // Englische Formate
    static QString formatDate_en(const QDateTime &dt) {
        return dt.toString("MMMM d, yyyy");  // "February 3, 2025"
    }
    
    // Französische Formate
    static QString formatDate_fr(const QDateTime &dt) {
        return dt.toString("d MMMM yyyy");   // "3 février 2025"
    }
};

// Verwendung:
QString formatted = LocaleFormatting::formatDate_de(QDateTime::currentDateTime());
```

### 5.3 Pluralisierung

```cpp
// src/localization/Pluralization.h
class Pluralization {
public:
    static QString unreadMessages(int count) {
        auto &i18n = LocalizationManager::instance();
        
        if (count == 0) {
            return i18n.tr("Keine ungelesenen Nachrichten");
        } else if (count == 1) {
            return i18n.tr("1 ungelesene Nachricht");
        } else {
            return i18n.tr("%1 ungelesene Nachrichten").arg(count);
        }
    }
};

// Automatische Pluralisierung mit Qt:
QString text = tr("nplurals=2; plural=(n != 1);") // German rule
              + i18n.tr("%n ungelesene Nachricht(en)", "", count);
// DE: "5 ungelesene Nachrichten"
// EN: "5 unread messages"
```

---

## 6. Sprachen-Support nach Priorität

### Phase B (Aktuell)
- ✅ **Deutsch** (Master Language)
  - 100% von Anfang an
  - Native Muttersprachler
  - Vollständig getestet

### Phase C (April 2025)
- ⏳ **Englisch** (English)
  - KI-übersetzen (GPT-4)
  - Review vor Release
  - ~200 Strings

### Phase D (Mai 2025)
- ⏳ **Französisch** (Français)
- ⏳ **Italienisch** (Italiano)
- ⏳ **Spanisch** (Español)

### Phase E (Juni 2025)
- ⏳ **Niederländisch** (Nederlands)
- ⏳ **Polnisch** (Polski)
- ⏳ **Schwedisch** (Svenska)

### Nicht geplant
- ❌ Chinesisch, Japanisch, Arabisch (Zu komplex, andere Zeichensätze)
- ❌ Russisch (Politische Gründe für deutsches Projekt)

---

## 7. Workflow für Neue Strings

### 7.1 Entwickler hinzufügt neuen String

```cpp
// src/ui/AccountSetupDialog.cpp
void AccountSetupDialog::setupUI() {
    auto label = new QLabel(tr("E-Mail Adresse:"));  // ← Deutsch!
    // Nicht: tr("Email Address:")
}
```

### 7.2 Automatische Extraktion

```bash
# Täglich (via Git Hook):
lupdate src/ forms/ -ts translations/mail-adler_de.ts

# .git/hooks/pre-commit
#!/bin/bash
cd "$(git rev-parse --show-toplevel)"
lupdate src/ forms/ -ts translations/mail-adler_de.ts
git add translations/mail-adler_de.ts
```

### 7.3 Neue Strings markieren

In `mail-adler_de.ts`:
```xml
<message>
    <location filename="src/ui/accountsetupdialog.cpp" line="42"/>
    <source>E-Mail Adresse:</source>
    <translation type="unfinished"></translation>  <!-- NEUER STRING -->
</message>
```

### 7.4 Vor Release: Review & Übersetzen

```bash
# Git-Hook vor Release:
./scripts/review_untranslated.py translations/mail-adler_de.ts
# → Zeigt: 3 übersetzte, 0 unübersetzt
# Falls unübersetzt: Release blockiert!

# Dann KI-Übersetzung:
./scripts/translate_with_ai.py \
    --source translations/mail-adler_de.ts \
    --target en_US,fr_FR \
    --output translations/
```

---

## 8. Vorteile dieses Ansatzes

| Vorteil | Erklärung |
|---------|-----------|
| **Natürliche UI** | Deutsche Wörter → längere Strings → Layout optimiert |
| **Bessere Übersetzung** | KI arbeitet von Deutsch → andere Sprachen (natives Deutsch als Kontext) |
| **Einfache Maintenance** | Ein Source-of-Truth (Deutsch), keine verwirrenden Englisch-Kommentare |
| **KI-Freundlich** | GPT übersetzt besser von Deutsch als von technischem Englisch |
| **Markt-Vorteil** | "Für Deutsche gemacht" ist erkennbar und authentisch |
| **Performance** | Master-Language = Runtime-Language (kein Übersetzungs-Overhead) |
| **Branding** | Mail-Adler ist "Deutsch-zentriert", nicht "Globales Englisch-Projekt" |

---

## 9. Ressourcen-Dateien Structure

```
translations/
├─ mail-adler_de.ts      (Master - von Entwickler gepflegt)
├─ mail-adler_de.qm      (Compiled - verwendet zur Laufzeit)
├─ mail-adler_en.ts      (English - von KI generiert)
├─ mail-adler_en.qm      (Compiled)
├─ mail-adler_fr.ts      (Français - von KI generiert)
├─ mail-adler_fr.qm      (Compiled)
└─ translations.qrc      (Qt Resource File)

translations.qrc:
<?xml version="1.0" encoding="UTF-8"?>
<RCC>
    <qresource prefix="/translations">
        <file>mail-adler_de.qm</file>
        <file>mail-adler_en.qm</file>
        <file>mail-adler_fr.qm</file>
    </qresource>
</RCC>
```

---

## 10. GitHub Workflow für Übersetzungen

```yaml
# .github/workflows/translations.yml
name: Translations

on:
  push:
    paths:
      - 'src/**'
      - 'forms/**'

jobs:
  update-translations:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Update master (German) strings
        run: |
          lupdate src/ forms/ -ts translations/mail-adler_de.ts
      
      - name: Auto-translate to other languages (GPT-4)
        env:
          OPENAI_API_KEY: ${{ secrets.OPENAI_API_KEY }}
        run: |
          ./scripts/translate_with_ai.py \
              --source translations/mail-adler_de.ts \
              --target en_US,fr_FR,es_ES,it_IT \
              --output translations/
      
      - name: Compile translations
        run: |
          lrelease translations/mail-adler_*.ts
      
      - name: Commit and push
        run: |
          git config --local user.email "action@github.com"
          git config --local user.name "Translation Bot"
          git add translations/
          git commit -m "Auto-update translations from master (German)"
          git push
```

---

## 11. Fazit: "Deutsch-First" ist die Zukunft

**Klassisches Englisch-First Projekt:**
```
Englisch Code → Deutsche Übersetzung → UI passt nicht → Fixxen
```

**Mail-Adler Deutsch-First:**
```
Deutsch Code → UI perfekt → KI übersetzt → Fertig
```

**Mail-Adler wird sich als "deutsches Projekt" schneller durchsetzen** weil:
1. ✅ Native Qualität von Anfang an
2. ✅ Deutsche Nutzer fühlen sich verstanden
3. ✅ Keine verloren gehen Übersetzungs-Kontext
4. ✅ KI produziert bessere Qualität mit natürlichem Deutsch als Input
5. ✅ Markenpositionierung klar: "Open Source für Deutsche"
