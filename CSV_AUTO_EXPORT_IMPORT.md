# CSV Auto-Export/Import - Easiest Way

## 1. CSV Auto-Generieren (aus .ts)

### Problem
```
Du willst CSV mit:
- Spalte 1: Alle Deutsch-Wörter (aus mail-adler_de.ts)
- Spalte 2: Leer für neue Sprache
- Mit Kommas korrekt formatiert

Statt manuell alle Wörter zu kopieren
```

### Lösung: Export-Script

```python
#!/usr/bin/env python3
# scripts/export_to_csv.py

import xml.etree.ElementTree as ET
import csv
import argparse
from pathlib import Path

def ts_to_csv(ts_file: str, csv_output: str, language_name: str = "Neue Sprache"):
    """
    Exportiere alle Deutsch-Strings aus .ts zu CSV
    
    Output:
    Deutsch,Neue Sprache
    Eingang,
    Gesendet,
    ...
    """
    
    tree = ET.parse(ts_file)
    root = tree.getroot()
    ns = {'ts': 'http://trolltech.com/TS'}
    
    # Sammle alle Deutsch-Strings
    german_strings = []
    
    for message in root.findall('.//message', ns):
        source_elem = message.find('source', ns)
        if source_elem is not None and source_elem.text:
            german_strings.append(source_elem.text.strip())
    
    # Dedupliziere (falls gleiche Wörter mehrmals vorkommen)
    german_strings = list(dict.fromkeys(german_strings))
    german_strings.sort()
    
    # Schreibe CSV
    with open(csv_output, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        # Header
        writer.writerow(['Deutsch', language_name])
        # Alle Strings
        for word in german_strings:
            writer.writerow([word, ''])  # Zweite Spalte leer
    
    print(f"✅ Export fertig!")
    print(f"   Datei: {csv_output}")
    print(f"   Strings: {len(german_strings)}")
    print(f"")
    print(f"Nächster Schritt:")
    print(f"1. Öffne {csv_output} in Excel")
    print(f"2. Fülle die '{language_name}'-Spalte mit Übersetzungen")
    print(f"3. Speichern")
    print(f"4. Führe import_csv.py aus")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Export .ts zu CSV')
    parser.add_argument('--source', required=True, help='mail-adler_de.ts')
    parser.add_argument('--output', required=True, help='output.csv')
    parser.add_argument('--language', default='English', help='Sprachen-Name')
    
    args = parser.parse_args()
    ts_to_csv(args.source, args.output, args.language)
```

### Verwendung:

```bash
# Exportiere für Niederländisch
python3 scripts/export_to_csv.py \
  --source translations/mail-adler_de.ts \
  --output translations/glossary_nl.csv \
  --language Niederländisch

# Output: glossary_nl.csv erstellt
# CSV hat:
# - Spalte 1: "Deutsch" (alle Strings)
# - Spalte 2: "Niederländisch" (leer)
```

**glossary_nl.csv sieht so aus:**
```csv
Deutsch,Niederländisch
Abbrechen,
Anmeldedaten,
Antworten,
Ansicht,
Archive,
Archiv,
Bearbeiten,
Beenden,
...
```

---

## 2. In Excel bearbeiten

### Schritt 1: CSV öffnen

```
1. Windows: Rechts-Klick auf glossary_nl.csv
   → "Öffnen mit" → Excel
   
2. Oder: Excel → Datei → Öffnen → glossary_nl.csv
```

### Schritt 2: Niederländisch-Spalte ausfüllen

```
Excel-Tabelle:
┌─────────────────┬──────────────────┐
│ Deutsch         │ Niederländisch   │
├─────────────────┼──────────────────┤
│ Abbrechen       │ Annuleren        │
│ Anmeldedaten    │ Inloggegevens    │
│ Antworten       │ Antwoorden       │
│ Ansicht         │ Weergave         │
│ ...             │ ...              │
└─────────────────┴──────────────────┘
```

### Schritt 3: Speichern (als CSV!)

```
Excel:
1. Datei → Speichern unter
2. Format: "CSV UTF-8 (Kommagetrennt)" 
   (WICHTIG: UTF-8, nicht Standart-CSV)
3. Speichern
```

---

## 3. Import zurück zu .ts

### Import-Script

```python
#!/usr/bin/env python3
# scripts/import_csv_to_ts.py

import csv
import xml.etree.ElementTree as ET
import argparse

def csv_to_ts(csv_file: str, ts_source: str, ts_output: str, language_column: str = 1):
    """
    Importiere CSV-Übersetzungen zurück zu .ts
    
    CSV-Format:
    Deutsch,English (oder Französisch, Niederländisch, etc.)
    Eingang,Inbox
    ...
    """
    
    # 1. Lese CSV
    translations = {}
    
    with open(csv_file, 'r', encoding='utf-8') as f:
        reader = csv.reader(f)
        header = next(reader)  # Überspringe Header
        
        for row in reader:
            if len(row) >= 2:
                deutsch = row[0].strip()
                übersetzt = row[1].strip()
                
                if deutsch and übersetzt:  # Nur wenn beide gefüllt
                    translations[deutsch] = übersetzt
    
    print(f"✅ CSV geladen: {len(translations)} Übersetzungen gefunden")
    
    # 2. Parse .ts Datei
    tree = ET.parse(ts_source)
    root = tree.getroot()
    ns = {'ts': 'http://trolltech.com/TS'}
    ET.register_namespace('', 'http://trolltech.com/TS')
    
    # 3. Update Übersetzungen
    updated = 0
    missing = 0
    
    for message in root.findall('.//message', ns):
        source_elem = message.find('source', ns)
        trans_elem = message.find('translation', ns)
        
        if source_elem is None or trans_elem is None:
            continue
        
        deutsch_text = source_elem.text
        
        if deutsch_text in translations:
            trans_elem.text = translations[deutsch_text]
            trans_elem.set('type', 'finished')
            updated += 1
            print(f"  ✓ {deutsch_text:30} → {translations[deutsch_text]}")
        else:
            missing += 1
    
    # 4. Speichern
    tree.write(ts_output, encoding='UTF-8', xml_declaration=True)
    
    print(f"\n✅ FERTIG!")
    print(f"   Aktualisiert: {updated}")
    print(f"   Fehlend (nicht in CSV): {missing}")
    print(f"   Ausgabedatei: {ts_output}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Import CSV zu .ts')
    parser.add_argument('--csv', required=True, help='glossary_nl.csv')
    parser.add_argument('--source', required=True, help='mail-adler_de.ts')
    parser.add_argument('--output', required=True, help='mail-adler_nl.ts')
    
    args = parser.parse_args()
    csv_to_ts(args.csv, args.source, args.output)
```

### Verwendung:

```bash
# Importiere CSV zurück zu .ts
python3 scripts/import_csv_to_ts.py \
  --csv translations/glossary_nl.csv \
  --source translations/mail-adler_de.ts \
  --output translations/mail-adler_nl.ts

# Output:
# ✅ CSV geladen: 247 Übersetzungen gefunden
#   ✓ Abbrechen                    → Annuleren
#   ✓ Anmeldedaten                 → Inloggegevens
#   ...
# ✅ FERTIG!
#    Aktualisiert: 247
#    Fehlend: 0
#    Ausgabedatei: translations/mail-adler_nl.ts
```

---

## 4. Kompletter Workflow für neue Sprache

```bash
# 1️⃣ EXPORT: Alle Deutsch-Strings → CSV
python3 scripts/export_to_csv.py \
  --source translations/mail-adler_de.ts \
  --output translations/glossary_nl.csv \
  --language Niederländisch

# Output: glossary_nl.csv erstellt (250 leere Zeilen)

# 2️⃣ BEARBEITEN: In Excel ausfüllen
# → Öffne glossary_nl.csv in Excel
# → Fülle Niederländisch-Spalte
# → Speichern (als CSV UTF-8!)

# 3️⃣ IMPORT: CSV → .ts
python3 scripts/import_csv_to_ts.py \
  --csv translations/glossary_nl.csv \
  --source translations/mail-adler_de.ts \
  --output translations/mail-adler_nl.ts

# 4️⃣ KOMPILIEREN
lrelease translations/mail-adler_nl.ts

# 5️⃣ GIT & RELEASE
git add translations/glossary_nl.csv translations/mail-adler_nl.ts
git commit -m "Add Dutch translation"
./scripts/release_with_translation.sh nl_NL
```

---

## 5. Mit LM Studio (Copy-Paste aus Excel)

### Schneller Workflow:

```
1. Export: glossary_nl.csv erstellen
   python3 scripts/export_to_csv.py ...

2. Excel: glossary_nl.csv öffnen
   Links: Deutsch, Rechts: Niederländisch (leer)

3. LM Studio offen (http://localhost:1234)

4. Copy-Paste Loop:
   - Excel: "Abbrechen" kopieren
   - LM Studio: "Übersetze ins Niederländische: Abbrechen"
   - LM Studio antwortet: "Annuleren"
   - Excel: "Annuleren" einfügen
   - Nächst Wort...

5. Nach alle Wörter:
   Import: glossary_nl.csv → mail-adler_nl.ts
   python3 scripts/import_csv_to_ts.py ...

6. Fertig!
```

---

## 6. Mehrere Sprachen gleichzeitig (in einer Datei)

### Super praktisch: Ein CSV für alle Sprachen

```python
#!/usr/bin/env python3
# scripts/export_to_csv_multilang.py

import xml.etree.ElementTree as ET
import csv
import argparse

def ts_to_csv_multilang(ts_file: str, csv_output: str, languages: list):
    """
    Exportiere zu CSV mit mehreren Sprach-Spalten
    
    languages = ["English", "Français", "Español", "Niederländisch"]
    
    Output:
    Deutsch,English,Français,Español,Niederländisch
    Eingang,Inbox,Boîte de réception,Bandeja de entrada,Postvak IN
    ...
    """
    
    tree = ET.parse(ts_file)
    root = tree.getroot()
    ns = {'ts': 'http://trolltech.com/TS'}
    
    # Sammle Deutsch-Strings
    german_strings = []
    for message in root.findall('.//message', ns):
        source_elem = message.find('source', ns)
        if source_elem is not None and source_elem.text:
            german_strings.append(source_elem.text.strip())
    
    german_strings = list(dict.fromkeys(german_strings))
    german_strings.sort()
    
    # Schreibe CSV mit mehreren Sprachen
    with open(csv_output, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        # Header
        writer.writerow(['Deutsch'] + languages)
        # Alle Strings (nur erste Spalte gefüllt)
        for word in german_strings:
            row = [word] + ([''] * len(languages))
            writer.writerow(row)
    
    print(f"✅ Multi-Language CSV erstellt!")
    print(f"   Datei: {csv_output}")
    print(f"   Strings: {len(german_strings)}")
    print(f"   Sprachen: {', '.join(languages)}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--source', required=True, help='mail-adler_de.ts')
    parser.add_argument('--output', required=True, help='output.csv')
    parser.add_argument('--languages', required=True, 
                        help='Comma-separated: "English,Français,Español,Niederländisch"')
    
    args = parser.parse_args()
    langs = [l.strip() for l in args.languages.split(',')]
    ts_to_csv_multilang(args.source, args.output, langs)
```

### Verwendung:

```bash
python3 scripts/export_to_csv_multilang.py \
  --source translations/mail-adler_de.ts \
  --output translations/glossary_all.csv \
  --languages "English,Français,Español,Niederländisch,Portugiesisch,Italienisch"

# Output: glossary_all.csv mit 6 leeren Sprach-Spalten
```

**Ergebnis (in Excel):**
```csv
Deutsch,English,Français,Español,Niederländisch,Portugiesisch,Italienisch
Abbrechen,Cancel,Annuler,Cancelar,Annuleren,Cancelar,Annulla
Anmeldedaten,Credentials,Identifiants,Credenciales,Inloggegevens,Credenciais,Credenziali
...
```

**Jetzt kannst du alle Sprachen in EINER Datei übersetzen!**

---

## 7. Import für jede einzelne Spalte

```bash
# Nach du alle Spalten in Excel gefüllt hast:

# Englisch extrahieren & importieren
python3 scripts/import_csv_column_to_ts.py \
  --csv translations/glossary_all.csv \
  --column English \
  --source translations/mail-adler_de.ts \
  --output translations/mail-adler_en.ts

# Französisch
python3 scripts/import_csv_column_to_ts.py \
  --csv translations/glossary_all.csv \
  --column Français \
  --source translations/mail-adler_de.ts \
  --output translations/mail-adler_fr.ts

# Niederländisch
python3 scripts/import_csv_column_to_ts.py \
  --csv translations/glossary_all.csv \
  --column Niederländisch \
  --source translations/mail-adler_de.ts \
  --output translations/mail-adler_nl.ts

# ... für alle Sprachen
```

**Script dafür:**

```python
#!/usr/bin/env python3
# scripts/import_csv_column_to_ts.py

import csv
import xml.etree.ElementTree as ET
import argparse

def csv_column_to_ts(csv_file: str, column_name: str, ts_source: str, ts_output: str):
    """
    Importiere eine bestimmte Spalte aus CSV zu .ts
    """
    
    # Lese CSV & finde Spalte
    translations = {}
    
    with open(csv_file, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)  # Nutzt Header als Keys
        
        for row in reader:
            deutsch = row['Deutsch'].strip()
            übersetzt = row.get(column_name, '').strip()
            
            if deutsch and übersetzt:
                translations[deutsch] = übersetzt
    
    print(f"✅ Spalte '{column_name}' geladen: {len(translations)} Übersetzungen")
    
    # Update .ts
    tree = ET.parse(ts_source)
    root = tree.getroot()
    ns = {'ts': 'http://trolltech.com/TS'}
    ET.register_namespace('', 'http://trolltech.com/TS')
    
    updated = 0
    for message in root.findall('.//message', ns):
        source_elem = message.find('source', ns)
        trans_elem = message.find('translation', ns)
        
        if source_elem is not None and trans_elem is not None:
            deutsch_text = source_elem.text
            if deutsch_text in translations:
                trans_elem.text = translations[deutsch_text]
                trans_elem.set('type', 'finished')
                updated += 1
    
    tree.write(ts_output, encoding='UTF-8', xml_declaration=True)
    
    print(f"✅ {updated} Strings aktualisiert → {ts_output}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--csv', required=True)
    parser.add_argument('--column', required=True, help='Spalten-Name')
    parser.add_argument('--source', required=True)
    parser.add_argument('--output', required=True)
    
    args = parser.parse_args()
    csv_column_to_ts(args.csv, args.column, args.source, args.output)
```

---

## 8. Batch-Script für alle Sprachen

```bash
#!/bin/bash
# scripts/batch_import_all_languages.sh

CSV="translations/glossary_all.csv"
SOURCE="translations/mail-adler_de.ts"
LANGUAGES=("English" "Français" "Español" "Niederländisch" "Portugiesisch" "Italienisch")
LANG_CODES=("en" "fr" "es" "nl" "pt" "it")

for i in "${!LANGUAGES[@]}"; do
    LANG="${LANGUAGES[$i]}"
    CODE="${LANG_CODES[$i]}"
    
    echo "🌍 Importiere $LANG..."
    
    python3 scripts/import_csv_column_to_ts.py \
      --csv "$CSV" \
      --column "$LANG" \
      --source "$SOURCE" \
      --output "translations/mail-adler_${CODE}.ts"
    
    lrelease "translations/mail-adler_${CODE}.ts"
done

echo "✅ Alle Sprachen importiert & kompiliert!"
```

### Verwendung:

```bash
chmod +x scripts/batch_import_all_languages.sh
./scripts/batch_import_all_languages.sh

# Output:
# 🌍 Importiere English...
# ✅ 247 Strings aktualisiert → translations/mail-adler_en.ts
# 🌍 Importiere Français...
# ✅ 247 Strings aktualisiert → translations/mail-adler_fr.ts
# ...
# ✅ Alle Sprachen importiert & kompiliert!
```

---

## 9. Zusammenfassung: Der EASIEST Workflow

### Super Einfach (für dich perfekt):

**Schritt 1: EXPORT (Auto)**
```bash
python3 scripts/export_to_csv_multilang.py \
  --source translations/mail-adler_de.ts \
  --output translations/glossary_all.csv \
  --languages "English,Français,Español,Niederländisch,Portugiesisch,Italienisch"
```

**Schritt 2: BEARBEITEN (Excel)**
```
Öffne glossary_all.csv in Excel
Fülle alle Spalten mit Übersetzungen
(oder nutze LM Studio: Copy-Paste jedes Wort)
Speichern (Format: CSV UTF-8!)
```

**Schritt 3: IMPORT (Auto)**
```bash
./scripts/batch_import_all_languages.sh
```

**Schritt 4: RELEASE (Auto)**
```bash
git add translations/
git commit -m "Add all translations"
git push
# GitHub Action macht den Rest
```

**Fertig! Keine .ts-Bearbeitung, keine komplexe Formate, nur Excel!**
