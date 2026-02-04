# LM Studio Workflow - Deutsch-First Übersetzung

## 1. Warum Semi-Manuell besser ist

### Problem: Batch-Übersetzung
```
Deutsch: "Markiert als Spam"
Englisch von KI: "Marked as spam" ❌ Sollte "Mark as Spam" sein

Jetzt muss man suchen: "War das 'Mark as Spam' oder 'Marked as spam'?"
```

### Lösung: Wort-für-Wort mit Kontext
```
Deutsche Strings → Export mit Kontext
↓
Du kopierst reihum in LM Studio
↓
LM Studio gibt einzelne Übersetzung (sicher!)
↓
Du kopierst zurück
↓
Import → fertig

Vorteil: Du siehst GENAU welches Wort, Kontext ist klar
```

---

## 2. LM Studio Setup

### 2.1 Installation & Modell

**LM Studio Download:** https://lmstudio.ai

```
1. Download & Install (.exe)
2. Starten
3. Modelle suchen: "Mistral 7B" oder "Neural Chat"
4. Download (4-5 GB)
5. "Local Server" Tab → Start (Port 1234)

Server läuft auf: http://localhost:1234
```

### 2.2 LM Studio einrichten (einmalig)

```
LM Studio GUI:
1. Model: "Mistral 7B" wählen
2. Temperature: 0.2 (niedrig = konsistent)
3. Max Tokens: 200
4. Local Server → Start

Im Chat dann können Sie testen:
"Übersetze 'Eingang' ins Englische"
Antwort: "Inbox"
```

---

## 3. Export-Tool: Begriffe mit Kontext

### 3.1 Python-Script zum Exportieren

```python
#!/usr/bin/env python3
# scripts/export_for_translation.py

import xml.etree.ElementTree as ET
import json
import argparse
from pathlib import Path
from datetime import datetime

class TranslationExporter:
    def __init__(self, ts_file: str):
        self.ts_file = ts_file
        self.tree = ET.parse(ts_file)
        self.root = self.tree.getroot()
        self.ns = {'ts': 'http://trolltech.com/TS'}
        ET.register_namespace('', 'http://trolltech.com/TS')
    
    def export_for_manual_translation(self, target_lang: str, output_file: str):
        """
        Exportiere alle untranslatierten Strings mit Kontext
        Format: Einfaches Text-Format für LM Studio
        """
        
        lang_names = {
            'en': 'English',
            'fr': 'French',
            'es': 'Spanish',
            'pt': 'Portuguese',
            'it': 'Italian',
            'nl': 'Dutch',
            'pl': 'Polish'
        }
        
        lang_name = lang_names.get(target_lang, target_lang)
        
        # Header
        output = []
        output.append(f"{'='*70}")
        output.append(f"Mail-Adler Translation Export")
        output.append(f"Quellsprache: Deutsch")
        output.append(f"Zielsprache: {lang_name}")
        output.append(f"Exportdatum: {datetime.now().strftime('%d.%m.%Y %H:%M')}")
        output.append(f"{'='*70}")
        output.append("")
        
        # Glossar (konstante Begriffe)
        output.append("GLOSSAR (Diese Wörter IMMER so übersetzen):")
        output.append("-" * 70)
        glossar = {
            'de': ['Eingang', 'Gesendet', 'Entwürfe', 'Papierkorb', 'Spam', 'Archiv', 'Markiert'],
            'en': ['Inbox', 'Sent', 'Drafts', 'Trash', 'Spam', 'Archive', 'Flagged'],
            'fr': ['Boîte de réception', 'Envoyés', 'Brouillons', 'Corbeille', 'Spam', 'Archive', 'Marqués'],
            'es': ['Bandeja de entrada', 'Enviados', 'Borradores', 'Papelera', 'Spam', 'Archivo', 'Marcado'],
        }
        
        if target_lang in glossar:
            for de_word, trans_word in zip(glossar['de'], glossar[target_lang]):
                output.append(f"  • {de_word:20} → {trans_word}")
        output.append("")
        output.append("")
        
        # Alle Strings
        string_count = 0
        for context in self.root.findall('.//context', self.ns):
            context_name = context.find('.//name', self.ns)
            context_text = context_name.text if context_name is not None else "Unknown"
            
            output.append(f"[CONTEXT: {context_text}]")
            output.append("=" * 70)
            
            for message in context.findall('.//message', self.ns):
                source_elem = message.find('source', self.ns)
                location_elem = message.find('location', self.ns)
                translation_elem = message.find('translation', self.ns)
                
                if source_elem is None:
                    continue
                
                source_text = source_elem.text
                
                # Überspringe bereits fertig übersetzte
                if translation_elem is not None and translation_elem.text and translation_elem.get('type') != 'unfinished':
                    continue
                
                string_count += 1
                
                # Kontext (Datei + Zeilennummer)
                location_text = ""
                if location_elem is not None:
                    filename = location_elem.get('filename', '')
                    line = location_elem.get('line', '')
                    location_text = f"  ({filename}:{line})"
                
                output.append(f"")
                output.append(f"[STRING #{string_count}]")
                output.append(f"Deutsch: {source_text}")
                output.append(f"Zielsprache ({lang_name}):")
                output.append(f"Kontext: {location_text}")
                output.append("---")
                output.append("")
        
        # Speichern
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write('\n'.join(output))
        
        print(f"✅ Export fertig!")
        print(f"   Datei: {output_file}")
        print(f"   Strings: {string_count}")
        print(f"")
        print(f"Workflow:")
        print(f"1. Öffne {output_file}")
        print(f"2. Kopiere 'Deutsch: [text]'")
        print(f"3. Gebe in LM Studio ein: 'Übersetze ins {lang_name}: [text]'")
        print(f"4. Kopiere Ergebnis → ersetze '[STRING #X]' Zeile")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Export für manuelle Übersetzung')
    parser.add_argument('--source', required=True, help='mail-adler_de.ts')
    parser.add_argument('--target', required=True, help='en, fr, es, pt, it, nl, pl')
    parser.add_argument('--output', required=True, help='Ausgabedatei')
    
    args = parser.parse_args()
    
    exporter = TranslationExporter(args.source)
    exporter.export_for_manual_translation(args.target, args.output)
```

### 3.2 Export erstellen

```bash
# Export für Englisch
python3 scripts/export_for_translation.py \
  --source translations/mail-adler_de.ts \
  --target en \
  --output export_en_manual.txt

# Export für Französisch
python3 scripts/export_for_translation.py \
  --source translations/mail-adler_de.ts \
  --target fr \
  --output export_fr_manual.txt
```

**Output-Beispiel (export_en_manual.txt):**

```
======================================================================
Mail-Adler Translation Export
Quellsprache: Deutsch
Zielsprache: English
Exportdatum: 03.02.2025 14:30
======================================================================

GLOSSAR (Diese Wörter IMMER so übersetzen):
----------------------------------------------------------------------
  • Eingang              → Inbox
  • Gesendet             → Sent
  • Entwürfe             → Drafts
  • Papierkorb           → Trash
  • Spam                 → Spam
  • Archiv               → Archive
  • Markiert             → Flagged


[CONTEXT: MainWindow]
======================================================================

[STRING #1]
Deutsch: Datei
Zielsprache (English):
Kontext: (src/ui/mainwindow.cpp:123)
---

[STRING #2]
Deutsch: Bearbeiten
Zielsprache (English):
Kontext: (src/ui/mainwindow.cpp:124)
---

[STRING #3]
Deutsch: Ansicht
Zielsprache (English):
Kontext: (src/ui/mainwindow.cpp:125)
---
```

---

## 4. LM Studio Prompt-Template

### 4.1 Einfacher Workflow im Chat

**In LM Studio Chat eingeben:**

```
Kontext: Du übersetzt für die Mail-Anwendung "Mail-Adler"

GLOSSAR:
- Eingang = Inbox
- Gesendet = Sent
- Entwürfe = Drafts
- Papierkorb = Trash
- Spam = Spam
- Archiv = Archive
- Markiert = Flagged

Übersetze folgendes Deutsches Wort/Phrase ins Englische:
[DEUTSCHES WORT HIER]

Antwort (nur das übersetzte Wort, keine Erklärung):
```

### 4.2 Copy-Paste Workflow

**Schritt 1: Export öffnen**
```
export_en_manual.txt öffnen (mit Notepad/VS Code)
```

**Schritt 2: LM Studio öffnen**
```
http://localhost:1234
Chat öffnen
```

**Schritt 3: Wort-für-Wort übersetzen**

```
export_en_manual.txt:
[STRING #1]
Deutsch: Datei

↓ (kopiere "Datei")

LM Studio Chat:
[Gib Kontext & Glossar ein (einmalig)]
Übersetze ins Englische: Datei

LM Studio antwortet:
File

↓ (kopiere "File")

export_en_manual.txt (aktualisiere):
[STRING #1]
Deutsch: Datei
Englisch: File ← EINGEBEN

↓ (zum nächsten Wort)
```

### 4.3 Vordefiniertes Prompt-Template (Copy-Paste)

Einfach diesen Text in LM Studio eingeben (einmalig), dann nur noch Wörter austauschen:

```
🔧 LM Studio System Prompt (einmalig einrichten):

Kontext: Du bist Übersetzer für die Mail-Anwendung "Mail-Adler" (ein Open-Source E-Mail-Client für Deutsch sprechende Nutzer).

GLOSSAR (Diese Wörter IMMER exakt so übersetzen, auch wenn anders üblich):
- Eingang = Inbox (nicht "Postfach")
- Gesendet = Sent
- Entwürfe = Drafts (nicht "Konzepte")
- Papierkorb = Trash (nicht "Müllkorb")
- Spam = Spam
- Archiv = Archive
- Markiert = Flagged (nicht "Gekennzeichnet")
- Synchronisieren = Synchronize (oder "Sync")
- Verschlüsseln = Encrypt
- Entschlüsseln = Decrypt
- Konto = Account (nicht "Benutzerkonto")
- Anmeldedaten = Credentials

ANWEISUNG:
- Übersetze NUR das Wort/die Phrase
- KEINE Erklärung
- KEINE Sätze
- Halte Formatierung (z.B. Umlaute)
- Fachbegriffe korrekt
- Sei konsistent (nutze immer die gleiche Übersetzung)

Format für jede Übersetzung:
Übersetze ins [SPRACHE]: [DEUTSCHES WORT]
Antwort: [ÜBERSETZTES WORT]
```

---

## 5. Import-Tool: Zurück in .ts Datei

### 5.1 Script zum Importieren

```python
#!/usr/bin/env python3
# scripts/import_translated_strings.py

import xml.etree.ElementTree as ET
import argparse
import re
from pathlib import Path

class TranslationImporter:
    def __init__(self, ts_file: str):
        self.ts_file = ts_file
        self.tree = ET.parse(ts_file)
        self.root = self.tree.getroot()
        self.ns = {'ts': 'http://trolltech.com/TS'}
        ET.register_namespace('', 'http://trolltech.com/TS')
    
    def import_from_export(self, export_file: str, output_ts: str):
        """
        Importiere übersetzte Strings aus export_*.txt
        Format: 
        [STRING #X]
        Deutsch: [original]
        Englisch: [translation]
        """
        
        # Parse export file
        translations = {}
        
        with open(export_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Regex zum Extrahieren: STRING #X ... Deutsch: ... Zielsprache: ...
        pattern = r'\[STRING #(\d+)\]\s*Deutsch:\s*([^\n]+)\s*(?:Englisch|Französisch|Spanisch|Portugiesisch|Italienisch|Niederländisch|Polnisch):\s*([^\n]+)'
        
        for match in re.finditer(pattern, content):
            deutsch = match.group(2).strip()
            translation = match.group(3).strip()
            
            translations[deutsch] = translation
            print(f"✓ {deutsch:30} → {translation}")
        
        # Update .ts Datei
        updated_count = 0
        for context in self.root.findall('.//context', self.ns):
            for message in context.findall('.//message', self.ns):
                source_elem = message.find('source', self.ns)
                translation_elem = message.find('translation', self.ns)
                
                if source_elem is None or translation_elem is None:
                    continue
                
                source_text = source_elem.text
                
                if source_text in translations:
                    translation_elem.text = translations[source_text]
                    translation_elem.set('type', 'finished')
                    updated_count += 1
        
        # Speichern
        self.tree.write(output_ts, encoding='UTF-8', xml_declaration=True)
        
        print(f"\n✅ Import fertig!")
        print(f"   Aktualisierte Strings: {updated_count}")
        print(f"   Datei: {output_ts}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Import übersetzte Strings')
    parser.add_argument('--source', required=True, help='mail-adler_de.ts')
    parser.add_argument('--import', dest='import_file', required=True, help='export_*.txt')
    parser.add_argument('--output', required=True, help='mail-adler_en.ts')
    
    args = parser.parse_args()
    
    importer = TranslationImporter(args.source)
    importer.import_from_export(args.import_file, args.output)
```

### 5.2 Import durchführen

```bash
# Nach du alle Wörter übersetzt hast:
python3 scripts/import_translated_strings.py \
  --source translations/mail-adler_de.ts \
  --import export_en_manual.txt \
  --output translations/mail-adler_en.ts

# Output:
✓ Datei                        → File
✓ Bearbeiten                   → Edit
✓ Ansicht                      → View
✓ Eingang                      → Inbox
✓ Gesendet                     → Sent
...
✅ Import fertig!
   Aktualisierte Strings: 247
   Datei: translations/mail-adler_en.ts
```

---

## 6. Kompletter Workflow Schritt-für-Schritt

### 6.1 Tag 1: Englisch übersetzen

```bash
# Schritt 1: Export Deutsch → Englisch
python3 scripts/export_for_translation.py \
  --source translations/mail-adler_de.ts \
  --target en \
  --output export_en_manual.txt

# Schritt 2: LM Studio starten
# Terminal 1: LM Studio bereits laufen?
# Falls nein: starten Sie LM Studio GUI

# Schritt 3: Editor öffnen
code export_en_manual.txt  # oder Notepad

# Schritt 4: Copy-Paste Loop
# - Deutsch-Wort aus export_en_manual.txt kopieren
# - In LM Studio Chat eingeben (mit Kontext-Prompt)
# - Übersetzung zurück kopieren
# - In export_en_manual.txt eintragen
# (ca. 250 Wörter = 30-45 Minuten)

# Schritt 5: Import zurück
python3 scripts/import_translated_strings.py \
  --source translations/mail-adler_de.ts \
  --import export_en_manual.txt \
  --output translations/mail-adler_en.ts

# Schritt 6: Kompilieren
lrelease translations/mail-adler_en.ts

# Schritt 7: Git Commit & Release
git add translations/
git commit -m "Add English translation"
git push
./scripts/release_with_translation.sh en_US
```

### 6.2 Tag 2: Französisch

```bash
# Gleicher Prozess für Französisch
python3 scripts/export_for_translation.py \
  --source translations/mail-adler_de.ts \
  --target fr \
  --output export_fr_manual.txt

# ... Copy-Paste Loop mit LM Studio (45 Min)
# ... Import + Kompilieren
python3 scripts/import_translated_strings.py \
  --source translations/mail-adler_de.ts \
  --import export_fr_manual.txt \
  --output translations/mail-adler_fr.ts

# ... Commit & Release
./scripts/release_with_translation.sh fr_FR
```

---

## 7. Effizienz-Tipps

### 7.1 Mehrere LM Studio Chats parallel

```
LM Studio öffnen:
- Tab 1: Englisch-Prompt (System-Prompt gespeichert)
- Tab 2: Französisch-Prompt
- Tab 3: Spanisch-Prompt

Dann:
- Export für alle 3 Sprachen öffnen
- Wort kopieren → Tab 1 → Englisch
- Ergebnis kopieren → export_en_manual.txt
- Nächstes Wort → Tab 2 → Französisch
- ... parallel bearbeiten
```

### 7.2 Batch-Modus (wenn möglich)

Wenn ein Deutsch-Satz mehrere Wörter hat, kannst du testen:

```
export: "Email Adresse eingeben"

LM Studio-Prompt: "Übersetze ins Englische (halte zusammenhängende Wörter zusammen): Email Adresse eingeben"

LM Studio antwortet: "Email Address - Enter"

Dann manuell tracken welcher Teil was ist
```

### 7.3 Glossar aktualisieren

Wenn du merkst "Ah, 'Konto' sollte immer 'Account' sein, nicht 'User Account'":

```
1. Globales GLOSSAR.txt aktualisieren
2. Nächster Export hat korrigiertes Glossar
3. Alle Sprachen konsistent
```

---

## 8. LM Studio Vorteile für diesen Workflow

| Aspekt | Vorteil |
|--------|---------|
| **GUI** | Einfach zu bedienen, kein Terminal nötig |
| **Lokal** | Keine Daten an API gesendet |
| **Kostenlos** | Unbegrenzte Nutzung |
| **Schnell** | 1 Wort in 2-3 Sekunden |
| **Modelle** | Jederzeit testen: Mistral, Neural Chat, Orca |
| **Offline** | Funktioniert auch ohne Internet |
| **Semi-Manuell** | Du kontrollierst jedes Wort, KI assistiert |

---

## 9. Checkliste: Englisch komplett

```
✅ Export erstellt
  export_en_manual.txt existiert
  
✅ LM Studio läuft
  http://localhost:1234 erreichbar
  
✅ Glossar eingeben
  Alle Glossar-Wörter in System-Prompt
  
✅ Wort-für-Wort übersetzen
  Alle STRING #X haben englische Übersetzung
  
✅ Import durchführen
  python3 scripts/import_translated_strings.py ...
  
✅ Kompilieren
  lrelease translations/mail-adler_en.ts
  → mail-adler_en.qm existiert
  
✅ Testen
  App starten, Sprache zu Englisch wechseln
  Alle Strings korrekt angezeigt
  
✅ Commit & Release
  git push
  GitHub Action erzeugt Release
  
✅ Nutzer-Download
  Version mit English verfügbar
```

---

## 10. Zusammenfassung

**Dein Setup:**
1. ✅ LM Studio (GUI, lokal, kostenlos)
2. ✅ Export-Tool (Python-Script)
3. ✅ Copy-Paste Loop (30-45 Min pro Sprache)
4. ✅ Import-Tool (Python-Script)
5. ✅ Automatisches Rollout (GitHub Actions)

**Vorteile dieses Ansatzes:**
- 💰 Kostenlos
- 🔒 Privat (alles lokal)
- 🎯 Konsistent (du kontrollierst jedes Wort)
- ⚡ Schnell (LM Studio lädt lokal)
- 🧠 KI assistiert, du kontrollierst
- 📦 Versionierbar (Glossar + Export-Datei)

**Praxis:**
```
Montag:  Englisch (45 Min)
Dienstag: Französisch (45 Min)
Mittwoch: Spanisch + Portugiesisch (90 Min)
...

Jede Sprache = neuer Release (auto-rollout)
Nutzer laden neue Version mit neue Sprache
```
