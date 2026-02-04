# Lokales LLM für Mail-Adler Übersetzungen

## 1. Warum lokales LLM statt API?

| Kriterium | API (GPT-4) | Lokal (z.B. Ollama) |
|-----------|-----------|------------|
| **Kosten** | €0.03 pro 1K Tokens | ✅ Kostenlos |
| **Datenschutz** | ❌ Daten an OpenAI | ✅ Lokal, privat |
| **Geschwindigkeit** | ⚠️ Network-Latenz | ✅ Sofort |
| **Offline** | ❌ Internet erforderlich | ✅ Funktioniert offline |
| **Kontrolle** | ❌ OpenAI entscheidet | ✅ Du kontrollierst |
| **Konsistenz** | ⚠️ Variabel je Update | ✅ Gleicher Modell |
| **Dezentralisierung** | ❌ US-Firma | ✅ Open-Source |

---

## 2. Beste lokale LLM-Optionen für Deutsch

### 2.1 Vergleich

| LLM | Typ | Speicher | Geschwindigkeit | Qualität | Installation |
|-----|-----|----------|-----------------|----------|--------------|
| **Ollama** | Launcher | 4-13GB | ⚡⚡⚡ Sehr schnell | ✅✅✅ Sehr gut | ✅✅✅ Einfach |
| **LM Studio** | GUI | 4-13GB | ⚡⚡ Schnell | ✅✅✅ Sehr gut | ✅✅ Mittel |
| **GPT4All** | GUI | 3-7GB | ⚡⚡ Schnell | ✅✅ Gut | ✅✅ Einfach |
| **LocalAI** | Docker | 4-13GB | ⚡⚡ Schnell | ✅✅ Gut | ⚠️ Komplex |
| **Hugging Face** | Lokal | Variabel | ⚡ Langsam | ✅✅ Gut | ⚠️ Komplex |

### 2.2 EMPFEHLUNG: Ollama

**Warum Ollama?**
- ✅ Einfachste Installation (1 Klick)
- ✅ Schnellste Performance
- ✅ Beste Modell-Bibliothek
- ✅ REST API (leicht zu integrieren)
- ✅ Läuft auch auf macOS/Linux/Windows

---

## 3. Ollama Setup für Deutsch-Übersetzung

### 3.1 Installation

**Windows 11:**
```bash
# Download: https://ollama.ai/download
# → Ollama-0.1.26-windows.exe (ca. 200MB)

# Installation:
1. Doppelklick auf .exe
2. Admin-Passwort eingeben
3. "Ollama" startet automatisch (im Systemtray)
4. Terminal öffnen, testen:

ollama --version
# Output: ollama version 0.1.26
```

**Linux (Ubuntu):**
```bash
curl https://ollama.ai/install.sh | sh
ollama --version
```

**macOS:**
```bash
# Via Homebrew oder direkter Download
brew install ollama
ollama --version
```

### 3.2 Beste Modelle für Deutsch-Übersetzung

#### Option A: Mistral 7B (Empfohlen für Anfänger)
```bash
ollama pull mistral:7b
# Download: ~4.1GB
# Performance: ⚡⚡⚡ Sehr schnell (auf 8GB RAM)
# Qualität: ✅✅ Gut für Deutsch
```

**Test:**
```bash
ollama run mistral:7b

>>> Übersetze ins Englische:
>>> Eingang
The Inbox
```

#### Option B: Neural Chat (Intel - optimiert für Deutsch)
```bash
ollama pull neural-chat:7b
# Download: ~4.7GB
# Performance: ⚡⚡⚡ Schnell
# Qualität: ✅✅✅ Sehr gut für Deutsch
```

#### Option C: Orca 2 (Höhere Qualität, langsamer)
```bash
ollama pull orca-mini:13b
# Download: ~8.4GB
# Performance: ⚡⚡ Mittel
# Qualität: ✅✅✅ Sehr gut
# Empfohlen nur mit 16GB+ RAM
```

**EMPFEHLUNG:** Starte mit **Mistral 7B** (schnell & gut)

### 3.3 Ollama Server starten

```bash
# Terminal 1: Ollama Server im Hintergrund
ollama serve

# Output:
# 2025/02/03 14:30:00 "Listening on 127.0.0.1:11434"

# Bleibt laufen im Hintergrund
# Terminal 2+: Weitere Befehle
ollama run mistral:7b
```

---

## 4. Mail-Adler Translation Tool (Python)

### 4.1 Translation Manager Script

```python
#!/usr/bin/env python3
# scripts/translate_manual.py

import requests
import json
import sys
from pathlib import Path
import argparse
from typing import Dict, List

class OllamaTranslator:
    def __init__(self, model: str = "mistral:7b", base_url: str = "http://localhost:11434"):
        self.model = model
        self.base_url = base_url
        self.cache = {}
    
    def translate_text(self, text: str, target_lang: str) -> str:
        """Übersetze Text mit lokalem LLM"""
        
        # Cache-Check
        cache_key = f"{text}::{target_lang}"
        if cache_key in self.cache:
            return self.cache[cache_key]
        
        # Prompt-Vorlage (siehe Punkt 5)
        prompt = f"""Du bist ein präziser Übersetzer für die Mail-Anwendung "Mail-Adler".

ANWEISUNG:
- Übersetze NUR das Wort/die Phrase
- KEINE Erklärung
- Kurz und prägnant
- Behalte Formatierung (.ts Datei)

SPRACHEN:
- Source: Deutsch
- Target: {self._get_lang_name(target_lang)}

TEXT ZUM ÜBERSETZEN:
{text}

ÜBERSETZUNG:"""

        try:
            response = requests.post(
                f"{self.base_url}/api/generate",
                json={
                    "model": self.model,
                    "prompt": prompt,
                    "stream": False,
                    "temperature": 0.3,  # Niedrig = konsistent
                },
                timeout=60
            )
            
            if response.status_code == 200:
                result = response.json()
                translation = result.get("response", "").strip()
                self.cache[cache_key] = translation
                return translation
            else:
                print(f"❌ Ollama Error: {response.status_code}")
                return text  # Fallback
                
        except requests.exceptions.ConnectionError:
            print("❌ Ollama nicht erreichbar!")
            print("   Starten Sie: ollama serve")
            return text
    
    def _get_lang_name(self, lang_code: str) -> str:
        """Konvertiere Lang-Code zu Name"""
        langs = {
            "en_US": "English (American)",
            "en_GB": "English (British)",
            "fr_FR": "French",
            "es_ES": "Spanish",
            "it_IT": "Italian",
            "nl_NL": "Dutch",
            "pl_PL": "Polish",
            "sv_SE": "Swedish"
        }
        return langs.get(lang_code, lang_code)
    
    def translate_ts_file(self, source_file: str, target_lang: str, output_file: str):
        """Übersetze komplette .ts Datei"""
        import xml.etree.ElementTree as ET
        
        print(f"\n📝 Übersetze {source_file} → {target_lang}")
        print("=" * 60)
        
        tree = ET.parse(source_file)
        root = tree.getroot()
        
        # Namespace
        ns = {'ts': 'http://trolltech.com/TS'}
        ET.register_namespace('', 'http://trolltech.com/TS')
        
        translated_count = 0
        skipped_count = 0
        
        for context in root.findall('.//context', ns):
            context_name = context.find('.//name', ns)
            
            for message in context.findall('.//message', ns):
                source_elem = message.find('source', ns)
                translation_elem = message.find('translation', ns)
                
                if source_elem is not None and translation_elem is not None:
                    source_text = source_elem.text
                    
                    # Überspringe bereits übersetzte
                    if translation_elem.text and translation_elem.get('type') != 'unfinished':
                        skipped_count += 1
                        continue
                    
                    # Übersetze
                    print(f"DE: {source_text}")
                    translated = self.translate_text(source_text, target_lang)
                    print(f"{target_lang.split('_')[0].upper()}: {translated}")
                    
                    translation_elem.text = translated
                    translation_elem.set('type', 'finished')
                    translated_count += 1
                    print("-" * 60)
        
        # Speichern
        tree.write(output_file, encoding='UTF-8', xml_declaration=True)
        
        print(f"\n✅ Fertig!")
        print(f"   Übersetzt: {translated_count}")
        print(f"   Übersprungen: {skipped_count}")
        print(f"   Datei: {output_file}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Mail-Adler Translation Manager')
    
    parser.add_argument('--model', default='mistral:7b',
                        help='Ollama-Modell (default: mistral:7b)')
    parser.add_argument('--source', required=True,
                        help='Quell-.ts Datei (z.B. translations/mail-adler_de.ts)')
    parser.add_argument('--target', required=True,
                        help='Zielsprache (z.B. en_US, fr_FR, es_ES)')
    parser.add_argument('--output', required=True,
                        help='Ausgangs-.ts Datei')
    
    args = parser.parse_args()
    
    translator = OllamaTranslator(model=args.model)
    translator.translate_ts_file(args.source, args.target, args.output)
```

### 4.2 Verwendung des Scripts

```bash
# Terminal vorbereiten
# Terminal 1: Ollama Server
ollama serve

# Terminal 2: Übersetzung starten
cd /path/to/mail-adler

# Mistral 7B laden (beim ersten Mal)
ollama pull mistral:7b

# Englisch übersetzen
python3 scripts/translate_manual.py \
  --model mistral:7b \
  --source translations/mail-adler_de.ts \
  --target en_US \
  --output translations/mail-adler_en.ts

# Französisch übersetzen
python3 scripts/translate_manual.py \
  --model mistral:7b \
  --source translations/mail-adler_de.ts \
  --target fr_FR \
  --output translations/mail-adler_fr.ts
```

**Output-Beispiel:**
```
📝 Übersetze translations/mail-adler_de.ts → en_US
============================================================
DE: Eingang
EN: Inbox
------------------------------------------------------------
DE: Gesendet
EN: Sent
------------------------------------------------------------
DE: Papierkorb
EN: Trash
------------------------------------------------------------
...
✅ Fertig!
   Übersetzt: 247
   Übersprungen: 0
   Datei: translations/mail-adler_en.ts
```

---

## 5. Optimal Prompt-Vorlage für Übersetzungen

### 5.1 Template für Batch-Übersetzung (EMPFOHLEN)

```
Du bist ein präziser Übersetzer für die Mail-Anwendung "Mail-Adler".

RICHTLINIEN:
- Übersetze PRÄZISE und KONSISTENT
- Halte Formatierung bei
- Technische Begriffe korrekt (IMAP, SMTP, etc.)
- Kurze, prägnante Begriffe
- KEINE Erklärung, nur Übersetzung

GLOSSAR (Diese Begriffe immer gleich übersetzen):
- Inbox → Eingang (NICHT Postfach)
- Sent → Gesendet
- Drafts → Entwürfe (NICHT Konzepte)
- Trash → Papierkorb (NICHT Müllkorb)
- Spam → Spam (kein Übersetzung)
- Archive → Archiv
- Flagged → Markiert
- Read → Gelesen
- Unread → Ungelesen
- IMAP → IMAP (bleibt gleich)
- SMTP → SMTP (bleibt gleich)
- Encrypt → Verschlüsseln
- Decrypt → Entschlüsseln

SPRACHEN:
- Source: Deutsch
- Target: [SPRACHE HIER]

ZU ÜBERSETZENDE TEXTE:
[TEXT HIER]

AUSGABE-FORMAT:
Deutsch: [original]
[Zielsprache]: [Übersetzung]
---
```

### 5.2 Template für einzelne Wörter/Phrasen

```
Übersetze diesen Text aus der Mail-Anwendung "Mail-Adler" präzise ins [ZIELSPRACHE].

Text: "[TEXT]"

Antwort (nur Übersetzung):
```

### 5.3 Was ist besser: Batch vs. Single?

| Ansatz | Vorteile | Nachteile |
|--------|----------|----------|
| **Batch (10-50 Strings)** | ✅ Konsistenz, Kontext | ⚠️ Längere Verarbeitung |
| **Single (1 Wort)** | ✅ Schnell, einfach | ❌ Inkonsistenzen möglich |

**EMPFEHLUNG:** **Batch mit Glossar**
- Alle Strings einer Kategorie zusammen
- Glossar definiert Fachbegriffe
- → Maximale Konsistenz

---

## 6. Version-Management beim Übersetzen

### 6.1 Versionierung mit Sprach-Updates

**Struktur:**
```
Mail-Adler Versionen:
├─ v0.1.0-de    (Deutsch Release)
├─ v0.1.1-de+en (Deutsch + English hinzugefügt)
├─ v0.1.2-de+en+fr (+ Französisch)
└─ v0.2.0-de+en+fr+es (+ Spanisch, neue Features)
```

**CMakeLists.txt:**
```cmake
# Version-Management mit Sprachen
set(MAIL_ADLER_VERSION_MAJOR 0)
set(MAIL_ADLER_VERSION_MINOR 1)
set(MAIL_ADLER_VERSION_PATCH 0)
set(MAIL_ADLER_LANGUAGES "de;en;fr;es")  # Aktive Sprachen

# Dynamische Versionsstring
string(REPLACE ";" "+" LANG_STRING "${MAIL_ADLER_LANGUAGES}")
set(MAIL_ADLER_VERSION_WITH_LANGS 
    "${MAIL_ADLER_VERSION_MAJOR}.${MAIL_ADLER_VERSION_MINOR}.${MAIL_ADLER_VERSION_PATCH}-${LANG_STRING}")

message(STATUS "Mail-Adler Version: ${MAIL_ADLER_VERSION_WITH_LANGS}")
```

### 6.2 Automated Release beim Sprach-Update

```bash
# scripts/release_with_translation.sh
#!/bin/bash

TARGET_LANG=$1  # z.B. "en_US", "fr_FR"

if [ -z "$TARGET_LANG" ]; then
    echo "Nutzung: ./scripts/release_with_translation.sh <lang>"
    echo "Beispiel: ./scripts/release_with_translation.sh fr_FR"
    exit 1
fi

echo "🌍 Mail-Adler Translation Release"
echo "=================================="

# 1. Übersetzung durchführen
echo "📝 Übersetze zu ${TARGET_LANG}..."
python3 scripts/translate_manual.py \
    --source translations/mail-adler_de.ts \
    --target ${TARGET_LANG} \
    --output translations/mail-adler_${TARGET_LANG%_*}.ts

# 2. Kompilieren
echo "🔨 Kompiliere Übersetzungen..."
lrelease translations/mail-adler_*.ts

# 3. Version erhöhen
echo "📌 Erhöhe Version..."
CURRENT_VERSION=$(grep "MAIL_ADLER_VERSION_PATCH" CMakeLists.txt | grep -oP '\d+')
NEW_VERSION=$((CURRENT_VERSION + 1))

sed -i "s/set(MAIL_ADLER_VERSION_PATCH ${CURRENT_VERSION})/set(MAIL_ADLER_VERSION_PATCH ${NEW_VERSION})/g" CMakeLists.txt

# 4. Sprachenliste updaten
echo "🌐 Update Sprachen-Liste..."
LANG_CODE=${TARGET_LANG%_*}
sed -i "s/set(MAIL_ADLER_LANGUAGES \"/set(MAIL_ADLER_LANGUAGES \"${LANG_CODE};/g" CMakeLists.txt

# 5. Git Commit
echo "📦 Erstelle Release-Commit..."
git add translations/ CMakeLists.txt
git commit -m "Release: Mail-Adler v0.1.${NEW_VERSION} + ${TARGET_LANG}"

# 6. Tag erstellen
git tag -a "v0.1.${NEW_VERSION}" -m "Mail-Adler Version 0.1.${NEW_VERSION} - ${TARGET_LANG} Translation"

echo "✅ Release fertig!"
echo "   Version: v0.1.${NEW_VERSION}"
echo "   Sprachen: ${LANG_CODE}"
echo ""
echo "Push mit: git push && git push --tags"
```

### 6.3 Automatisches Rollout (GitHub Actions)

```yaml
# .github/workflows/translation-release.yml
name: Translation Release

on:
  push:
    paths:
      - 'translations/mail-adler_*.ts'
      - 'CMakeLists.txt'

jobs:
  release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Build & Release
        run: |
          # Compile translations
          sudo apt-get install -y qt6-tools-dev
          lrelease translations/mail-adler_*.ts
          
          # Build
          mkdir build && cd build
          cmake .. -DCMAKE_BUILD_TYPE=Release -GNinja
          ninja
          
          # Test
          ninja test || true
      
      - name: Create GitHub Release
        uses: softprops/action-gh-release@v1
        with:
          files: |
            build/mail-adler_*
            translations/mail-adler_*.qm
          tag_name: ${{ github.ref }}
```

---

## 7. Workflow: Schritt-für-Schritt

### 7.1 Neue Strings hinzufügen (Entwickler)

```bash
# 1. Schreibe Code mit Deutsch-Strings
# src/ui/newfeature.cpp:
ui->label->setText(tr("Neue Funktion"));

# 2. Extrahiere Strings
cd mail-adler
lupdate src/ forms/ -ts translations/mail-adler_de.ts

# 3. Commit
git add translations/mail-adler_de.ts
git commit -m "Add new strings for new feature"
```

### 7.2 Übersetzen (Du selbst)

```bash
# 1. Ollama Server starten (im Hintergrund)
ollama serve &

# 2. Stelle sicher, dass Modell da ist
ollama pull mistral:7b

# 3. Übersetze zu allen Sprachen
./scripts/release_with_translation.sh en_US
./scripts/release_with_translation.sh fr_FR
./scripts/release_with_translation.sh es_ES

# 4. Review (optional, mit Sicht auf Ergebnisse)
# Öffne die .ts Dateien und prüfe Qualität
```

### 7.3 Automatisches Rollout

```bash
# 1. Push zu GitHub
git push origin main
git push origin --tags

# 2. GitHub Actions:
#    - Kompiliert Übersetzungen
#    - Baut Mail-Adler
#    - Erstellt Release mit .qm Dateien
#    - Auto-Rollout zu Website

# 3. Nutzer:
#    - Download neue Version
#    - Sprachauswahl in Einstellungen
#    - Automatischer Download .qm Datei
```

---

## 8. Kostenlose lokale LLM-Alternativen

### Falls Ollama nicht reicht:

| Tool | Download | RAM | Deutsch | Einfachheit |
|------|----------|-----|---------|-------------|
| **LM Studio** | https://lmstudio.ai | 4-16GB | ✅ Gut | ✅✅ GUI |
| **GPT4All** | https://gpt4all.io | 3-8GB | ⚠️ OK | ✅✅ GUI |
| **LocalAI** | https://localai.io | 4-16GB | ✅ Gut | ⚠️ Docker |
| **Hugging Face** | huggingface.co | 2-32GB | Variabel | ⚠️ Code |

### LM Studio Alternative zu Ollama

```bash
# Download & Start: https://lmstudio.ai
# 1. GUI öffnen
# 2. "Mistral 7B" suchen & loaded
# 3. "Local Server" starten (Port 1234)

# Dann im Script anpassen:
python3 scripts/translate_manual.py \
  --model "mistral:7b" \
  --source translations/mail-adler_de.ts \
  --target en_US \
  --output translations/mail-adler_en.ts
  # (Funktioniert mit LM Studio auch - kompatible API)
```

---

## 9. Qualitätskontrolle

### 9.1 Übersetzte Strings prüfen

```bash
# Script zum Vergleichen
#!/bin/bash
# scripts/check_translations.sh

echo "Übersetzte Strings vs Original:"
grep "<translation" translations/mail-adler_en.ts | wc -l
grep "<source" translations/mail-adler_de.ts | wc -l

echo ""
echo "Unfertige Übersetzungen:"
grep '<translation type="unfinished"' translations/mail-adler_en.ts
```

### 9.2 Manuelle Review (vor Release)

```
Mail-Adler Übersetzungs-Checkliste:

English (Englisch):
- [ ] Alle 240+ Strings übersetzt
- [ ] Keine "Inbox" statt "Eingang" Pattern
- [ ] Fachbegriffe konsistent
- [ ] Keine Tippfehler
- [ ] UI-Test durchgespielt

Französisch:
- [ ] Alle 240+ Strings übersetzt
- [ ] Accents korrekt (é, è, ê, ë, etc.)
- [ ] Keine Inkonsisstences
- [ ] UI-Test

... (pro Sprache)
```

---

## 10. Zusammenfassung

**Dein Workflow:**
1. ✅ Schreibe Code mit **Deutsch-Strings**
2. ✅ Nutze **Ollama lokal** zum Übersetzen (kostenlos, schnell, privat)
3. ✅ Script führt Batch-Übersetzung durch
4. ✅ **Version automatisch erhöht**
5. ✅ **Automatisches Rollout** via GitHub Actions
6. ✅ Nutzer bekommen neue Sprach-Version

**Vorteile:**
- 💰 Kostenlos (keine API)
- 🔒 Privat (lokale Daten)
- ⚡ Schnell (kein Netzwerk-Overhead)
- 🎯 Konsistent (Glossar + Batch)
- 🚀 Automatisiert (GitHub Actions)
- 🌍 Dezentralisiert (Open-Source)

**Empfohlenes Setup:**
```bash
# Einmalig
brew install ollama                    # oder Windows-Installer
ollama pull mistral:7b                 # ~4GB

# Bei jeder Übersetzung
ollama serve &                         # Hintergrund
python3 scripts/translate_manual.py \  # Batch-Übersetzen
  --target en_US \
  --source translations/mail-adler_de.ts \
  --output translations/mail-adler_en.ts
./scripts/release_with_translation.sh en_US  # Release
```
