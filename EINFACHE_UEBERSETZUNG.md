# Einfache Übersetzung - Deutsch ↔ Andere Sprachen

## 1. Vergiss .ts - Arbeite mit einfachen Text-Dateien

### Problem mit .ts
```xml
<!-- .ts ist XML - kompliziert -->
<message>
    <location filename="src/ui/mainwindow.cpp" line="123"/>
    <source>Eingang</source>
    <translation type="unfinished"></translation>
</message>
```

### Lösung: Einfache Text-Datei
```
Deutsch | English
Eingang | Inbox
Gesendet | Sent
Entwürfe | Drafts
Papierkorb | Trash
...
```

**VIEL schneller und einfacher!**

---

## 2. Format-Optionen (du wählst)

### Option A: CSV (Empfohlen - für Excel)

**File: `translations/glossary_en.csv`**
```csv
Deutsch,English
Eingang,Inbox
Gesendet,Sent
Entwürfe,Drafts
Papierkorb,Trash
Spam,Spam
Archiv,Archive
Markiert,Flagged
Synchronisieren,Synchronize
Verschlüsseln,Encrypt
Entschlüsseln,Decrypt
Konto,Account
Anmeldedaten,Credentials
Neue Nachricht,New Message
Antworten,Reply
Allen antworten,Reply All
Weiterleiten,Forward
Löschen,Delete
Zurück,Back
OK,OK
Abbrechen,Cancel
Speichern,Save
Beenden,Exit
Einstellungen,Settings
Hilfe,Help
...
```

### Option B: Einfache Text-Datei (noch schneller zum Tippen)

**File: `translations/glossary_en.txt`**
```
Eingang = Inbox
Gesendet = Sent
Entwürfe = Drafts
Papierkorb = Trash
Spam = Spam
Archiv = Archive
Markiert = Flagged
Synchronisieren = Synchronize
Verschlüsseln = Encrypt
Entschlüsseln = Decrypt
Konto = Account
Anmeldedaten = Credentials
Neue Nachricht = New Message
Antworten = Reply
Allen antworten = Reply All
Weiterleiten = Forward
Löschen = Delete
Zurück = Back
OK = OK
Abbrechen = Cancel
Speichern = Save
Beenden = Exit
Einstellungen = Settings
Hilfe = Help
```

### Option C: JSON (für Struktur)

**File: `translations/glossary_en.json`**
```json
{
  "ui": {
    "Eingang": "Inbox",
    "Gesendet": "Sent",
    "Entwürfe": "Drafts"
  },
  "actions": {
    "Antworten": "Reply",
    "Allen antworten": "Reply All",
    "Weiterleiten": "Forward"
  }
}
```

**EMPFEHLUNG: CSV (Option A) - du kannst es in Excel öffnen und bearbeiten!**

---

## 3. Einfaches Python-Script: CSV → .ts

```python
#!/usr/bin/env python3
# scripts/update_translations_from_csv.py

import csv
import xml.etree.ElementTree as ET
import argparse
from pathlib import Path

def csv_to_ts(csv_file: str, ts_source: str, ts_output: str):
    """
    Lese CSV-Datei und aktualisiere .ts Datei
    
    CSV-Format:
    Deutsch,English
    Eingang,Inbox
    ...
    """
    
    # 1. Lese CSV
    translations = {}
    with open(csv_file, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            deutsch = row['Deutsch'].strip()
            übersetzt = row['English'].strip()  # oder 'Français', 'Español', etc.
            translations[deutsch] = übersetzt
    
    print(f"✅ CSV geladen: {len(translations)} Übersetzungen")
    
    # 2. Parse .ts Datei
    tree = ET.parse(ts_source)
    root = tree.getroot()
    ns = {'ts': 'http://trolltech.com/TS'}
    ET.register_namespace('', 'http://trolltech.com/TS')
    
    # 3. Update Übersetzungen
    updated = 0
    skipped = 0
    
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
            skipped += 1
    
    # 4. Speichern
    tree.write(ts_output, encoding='UTF-8', xml_declaration=True)
    
    print(f"\n✅ FERTIG!")
    print(f"   Aktualisiert: {updated}")
    print(f"   Übersprungen: {skipped}")
    print(f"   Ausgabedatei: {ts_output}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='CSV → .ts Converter')
    parser.add_argument('--csv', required=True, help='glossary_en.csv')
    parser.add_argument('--source', required=True, help='mail-adler_de.ts')
    parser.add_argument('--output', required=True, help='mail-adler_en.ts')
    
    args = parser.parse_args()
    csv_to_ts(args.csv, args.source, args.output)
```

### Verwendung:

```bash
# 1. CSV bearbeiten (in Excel oder Notepad)
# translations/glossary_en.csv

# 2. Script ausführen
python3 scripts/update_translations_from_csv.py \
  --csv translations/glossary_en.csv \
  --source translations/mail-adler_de.ts \
  --output translations/mail-adler_en.ts

# 3. Fertig!
# mail-adler_en.ts ist aktualisiert
```

---

## 4. Noch schneller: Einfache Text-Datei (mit =)

### Python-Script für .txt Format

```python
#!/usr/bin/env python3
# scripts/update_translations_from_txt.py

import xml.etree.ElementTree as ET
import argparse
import re

def txt_to_ts(txt_file: str, ts_source: str, ts_output: str):
    """
    Lese einfache .txt Datei (Deutsch = English)
    und aktualisiere .ts Datei
    """
    
    # 1. Lese .txt Datei
    translations = {}
    with open(txt_file, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):  # Überspringe Kommentare
                continue
            
            # Format: Deutsch = English
            if '=' in line:
                deutsch, englisch = line.split('=', 1)
                deutsch = deutsch.strip()
                englisch = englisch.strip()
                translations[deutsch] = englisch
    
    print(f"✅ TXT geladen: {len(translations)} Übersetzungen")
    
    # 2-4. Gleich wie CSV-Script
    tree = ET.parse(ts_source)
    root = tree.getroot()
    ns = {'ts': 'http://trolltech.com/TS'}
    ET.register_namespace('', 'http://trolltech.com/TS')
    
    updated = 0
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
    
    tree.write(ts_output, encoding='UTF-8', xml_declaration=True)
    
    print(f"\n✅ FERTIG!")
    print(f"   Aktualisiert: {updated}")
    print(f"   Ausgabedatei: {ts_output}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='TXT → .ts Converter')
    parser.add_argument('--txt', required=True, help='glossary_en.txt')
    parser.add_argument('--source', required=True, help='mail-adler_de.ts')
    parser.add_argument('--output', required=True, help='mail-adler_en.ts')
    
    args = parser.parse_args()
    txt_to_ts(args.txt, args.source, args.output)
```

### Verwendung:

```bash
# 1. Öffne Notepad
# Bearbeite: translations/glossary_en.txt

Eingang = Inbox
Gesendet = Sent
...

# 2. Speichern & Script ausführen
python3 scripts/update_translations_from_txt.py \
  --txt translations/glossary_en.txt \
  --source translations/mail-adler_de.ts \
  --output translations/mail-adler_en.ts

# 3. Fertig!
```

---

## 5. Kompletter Workflow (EINFACH)

### Schritt-für-Schritt

```bash
# 1. Glossary-Datei erstellen (einmalig)
cat > translations/glossary_en.txt << 'EOF'
# Englisch Glossar für Mail-Adler
# Format: Deutsch = English

Eingang = Inbox
Gesendet = Sent
Entwürfe = Drafts
Papierkorb = Trash
Spam = Spam
Archiv = Archive
Markiert = Flagged
Synchronisieren = Synchronize
Verschlüsseln = Encrypt
Entschlüsseln = Decrypt
Konto = Account
Anmeldedaten = Credentials
Neue Nachricht = New Message
Antworten = Reply
Allen antworten = Reply All
Weiterleiten = Forward
Löschen = Delete
...
EOF

# 2. Bei LM Studio: Wörter hinzufügen
# Öffne translations/glossary_en.txt
# Kopiere "Eingang =" 
# Füge in LM Studio ein: "Übersetze: Eingang"
# LM Studio antwortet: "Inbox"
# Ersetze "Eingang = " mit "Eingang = Inbox"

# 3. Nach alle Wörter übersetzt sind:
python3 scripts/update_translations_from_txt.py \
  --txt translations/glossary_en.txt \
  --source translations/mail-adler_de.ts \
  --output translations/mail-adler_en.ts

# 4. Kompilieren
lrelease translations/mail-adler_en.ts

# 5. Commit & Release
git add translations/glossary_en.txt translations/mail-adler_en.ts
git commit -m "Add English translation"
./scripts/release_with_translation.sh en_US
```

---

## 6. Mit LM Studio: Copy-Paste Flow

**Workflow:**

```
1. VS Code öffnen: translations/glossary_en.txt
2. LM Studio öffnen: http://localhost:1234
3. Wort-für-Wort:

   VS Code:
   Eingang = [KOPIEREN: "Eingang"]
   
   LM Studio Chat:
   "Übersetze ins Englische: Eingang"
   → Antwortet: "Inbox"
   
   VS Code:
   Eingang = Inbox [EINFÜGEN: "Inbox"]
   
   ... nächstes Wort
```

**Pro Sprache 30-45 Minuten**

---

## 7. Mehrsprachig (Englisch, Französisch, Spanisch, etc.)

```
translations/
├─ glossary_de.txt      (Master - deine Deutsch-Strings)
├─ glossary_en.txt      (Englisch - deine Copy-Paste Übersetzungen)
├─ glossary_fr.txt      (Französisch)
├─ glossary_es.txt      (Spanisch)
├─ glossary_pt.txt      (Portugiesisch)
└─ glossary_it.txt      (Italienisch)
```

**Script für alle Sprachen:**

```bash
#!/bin/bash
# scripts/update_all_translations.sh

LANGUAGES=("en" "fr" "es" "pt" "it")

for LANG in "${LANGUAGES[@]}"; do
    echo "🌍 Update $LANG..."
    
    python3 scripts/update_translations_from_txt.py \
      --txt translations/glossary_${LANG}.txt \
      --source translations/mail-adler_de.ts \
      --output translations/mail-adler_${LANG}.ts
    
    lrelease translations/mail-adler_${LANG}.ts
done

echo "✅ Alle Sprachen aktualisiert!"
```

---

## 8. Excel-Workflow (noch schneller)

Wenn du lieber in Excel arbeiten möchtest:

**translations/glossary_all.csv**
```csv
Deutsch,English,Français,Español,Português,Italiano
Eingang,Inbox,Boîte de réception,Bandeja de entrada,Caixa de entrada,Posta in arrivo
Gesendet,Sent,Envoyés,Enviados,Enviados,Inviati
Entwürfe,Drafts,Brouillons,Borradores,Rascunhos,Bozze
Papierkorb,Trash,Corbeille,Papelera,Lixo,Cestino
...
```

**Excel-Script:**

```python
#!/usr/bin/env python3
# scripts/update_from_excel.py

import pandas as pd
import xml.etree.ElementTree as ET
import argparse

def excel_to_ts(excel_file: str, language: str, ts_source: str, ts_output: str):
    """
    Lese Excel/CSV und schreibe eine bestimmte Sprach-Spalte in .ts
    """
    
    # Lese Excel
    df = pd.read_csv(excel_file)
    
    # Extrahiere Sprach-Spalte
    translations = dict(zip(df['Deutsch'], df[language]))
    
    # Update .ts (wie oben)
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
                trans_elem.text = str(translations[deutsch_text])
                trans_elem.set('type', 'finished')
                updated += 1
    
    tree.write(ts_output, encoding='UTF-8', xml_declaration=True)
    print(f"✅ {language}: {updated} Strings aktualisiert")

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--excel', required=True)
    parser.add_argument('--language', required=True, help='English, Français, Español, etc.')
    parser.add_argument('--source', required=True)
    parser.add_argument('--output', required=True)
    
    args = parser.parse_args()
    excel_to_ts(args.excel, args.language, args.source, args.output)
```

**Nutzung:**
```bash
# Alle Sprachen aus einer Excel-Datei
python3 scripts/update_from_excel.py --excel translations/glossary_all.csv --language English --source translations/mail-adler_de.ts --output translations/mail-adler_en.ts
python3 scripts/update_from_excel.py --excel translations/glossary_all.csv --language Français --source translations/mail-adler_de.ts --output translations/mail-adler_fr.ts
python3 scripts/update_from_excel.py --excel translations/glossary_all.csv --language Español --source translations/mail-adler_de.ts --output translations/mail-adler_es.ts
```

---

## 9. Zusammenfassung: Einfache Optionen

### Schnellste Variante: TXT-Datei

```
1. Öffne Notepad
2. Bearbeite: translations/glossary_en.txt
   Eingang = Inbox
   Gesendet = Sent
   ...
3. Script: python3 scripts/update_translations_from_txt.py ...
4. Fertig!
```

### Professionellste Variante: Excel/CSV

```
1. Öffne Excel
2. Alle Sprachen in einer Datei
   Deutsch | English | Français | Español
   Eingang | Inbox   | Boîte... | Bandeja...
3. Script: python3 scripts/update_from_excel.py ...
4. Fertig!
```

### Beide Varianten = Keine .ts-Bearbeitung nötig!

**Du arbeitest nur mit:**
- ✅ Notepad/Word/Excel
- ✅ LM Studio (Chat)
- ✅ Python-Script (einmal klicken)

**Nicht mit:**
- ❌ .ts XML-Dateien
- ❌ Komplexe Formate
- ❌ Manuelle .ts-Bearbeitung
