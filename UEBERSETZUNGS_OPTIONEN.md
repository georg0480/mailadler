# Übersetzungsoptionen - Günstig & Praktisch

## 1. Übersetzungs-Anbieter (Vergleich)

| Anbieter | Kostenlos | Qualität | API | Limit | Empfehlung |
|----------|-----------|----------|-----|-------|------------|
| **Google Translate Free** | ✅ Kostenlos | ✅✅ Gut | ❌ Unofficial | Unbegrenzt | ✅ Einmalig |
| **DeepL Free** | ✅ 500K chars/Monat | ✅✅✅ Sehr gut | ✅ Kostenlos | 500K | ✅ BESTE Qualität |
| **Microsoft Translator** | ⚠️ 2M chars/Monat | ✅✅ Gut | ✅ Kostenlos | 2M | ✅ Viel Freiheit |
| **Yandex** | ✅ Kostenlos | ✅ Gut | ✅ Free | Unbegrenzt | ✅ Backup |
| **OpenAI GPT-4** | ❌ $0.03 pro 1K Tokens | ✅✅✅ Excellent | ✅ API | Pay-as-you-go | ⚠️ Teuer |
| **AWS Translate** | ❌ $15 pro 1M chars | ✅✅ Gut | ✅ API | Pay-as-you-go | ⚠️ Teuer |
| **Ollama lokal** | ✅ Kostenlos | ✅ Gut | ✅ Lokal | Unbegrenzt | ✅ Datenschutz |

---

## 2. EMPFEHLUNG: DeepL Free

### Warum DeepL?
```
✅ Kostenlos (500K characters/Monat)
✅ BESTE Übersetzungsqualität (besser als Google)
✅ Kostenlose API verfügbar
✅ 70 Wörter × 30 Sprachen = ~2100 chars = KOSTENLOS!
✅ Unbegrenztes Kontingent mit Free-Tier
```

### Beispiel: 70 Wörter × 30 Sprachen
```
70 Wörter durchschnittlich 6 Buchstaben = 420 Zeichen
× 30 Sprachen = 12.600 Zeichen
500.000 Zeichen/Monat → locker kostenlos!

Selbst 100 Sprachen würden passen!
```

### DeepL Free Setup:

```bash
# 1. Kostenlos registrieren
https://www.deepl.com/de/signup

# 2. Python-Library
pip install deepl

# 3. Script:
```

```python
#!/usr/bin/env python3
# scripts/deepl_translate.py

import deepl
import csv
import argparse

def translate_csv_with_deepl(csv_file: str, language_code: str):
    """
    Übersetze CSV-Spalte mit DeepL
    language_code: "en", "fr", "es", "pt", "it", "nl", "pl"
    """
    
    # DeepL kostenlos (kein API-Key nötig für Web-Interface)
    # Oder mit API-Key (kostenlos 500K chars):
    # translator = deepl.Translator("your-free-api-key")
    
    # Für Free-Tier ohne API-Key: Google Translate Alternative
    # ODER: Registriere dich für DeepL Free API
    
    translator = deepl.Translator("your-deepl-api-key")
    
    lang_map = {
        'en': 'EN-US',
        'fr': 'FR',
        'es': 'ES',
        'pt': 'PT',
        'it': 'IT',
        'nl': 'NL',
        'pl': 'PL',
        'de': 'DE'
    }
    
    target_lang = lang_map.get(language_code, 'EN-US')
    
    # Lese CSV
    with open(csv_file, 'r', encoding='utf-8') as f:
        reader = csv.reader(f)
        rows = list(reader)
    
    # Übersetze Englisch-Spalte (Index 1)
    if len(rows[0]) > 1 and rows[0][1] == 'Englisch':
        # Erste Zeile ist Header, überspringe
        for i in range(1, len(rows)):
            if len(rows[i]) > 1 and rows[i][1]:  # Wenn Englisch-Text
                english_text = rows[i][1]
                
                # Übersetze mit DeepL
                result = translator.translate_text(
                    english_text,
                    target_lang=target_lang
                )
                
                rows[i].append(result.text)
                print(f"✓ {english_text:30} → {result.text}")
    
    # Speichern
    with open(csv_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerows(rows)
    
    print(f"\n✅ Übersetzt mit DeepL!")

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--csv', required=True)
    parser.add_argument('--lang', required=True, help='en, fr, es, pt, it, nl, pl')
    
    args = parser.parse_args()
    translate_csv_with_deepl(args.csv, args.lang)
```

---

## 3. Strategie: Nur NEUE Strings übersetzen

### Problem:
```
Jedes Mal ALLE 70 Wörter übersetzen = Verschwendung
Besser: Nur neue/veränderte Strings
```

### Lösung: Delta-Übersetzung

```python
#!/usr/bin/env python3
# scripts/translate_delta.py

import deepl
import csv
import hashlib
import json
from pathlib import Path

class DeltaTranslator:
    def __init__(self, api_key: str):
        self.translator = deepl.Translator(api_key)
        self.cache_file = "translations/translation_cache.json"
        self.cache = self.load_cache()
    
    def load_cache(self):
        """Lade bereits übersetzte Wörter aus Cache"""
        if Path(self.cache_file).exists():
            with open(self.cache_file, 'r', encoding='utf-8') as f:
                return json.load(f)
        return {}
    
    def save_cache(self):
        """Speichere Cache"""
        with open(self.cache_file, 'w', encoding='utf-8') as f:
            json.dump(self.cache, f, ensure_ascii=False, indent=2)
    
    def get_hash(self, text: str) -> str:
        """Generiere Hash für Wort"""
        return hashlib.md5(text.encode()).hexdigest()
    
    def translate_csv_delta(self, csv_file: str, language_code: str):
        """
        Übersetze nur NEUE Wörter
        Cache speichert bereits übersetzte
        """
        
        lang_map = {
            'en': 'EN-US', 'fr': 'FR', 'es': 'ES', 'pt': 'PT',
            'it': 'IT', 'nl': 'NL', 'pl': 'PL'
        }
        target_lang = lang_map.get(language_code, 'EN-US')
        
        # Lese CSV
        with open(csv_file, 'r', encoding='utf-8') as f:
            reader = csv.reader(f)
            rows = list(reader)
        
        translated_count = 0
        cached_count = 0
        
        # Verarbeite Strings
        for i in range(1, len(rows)):  # Überspringe Header
            if len(rows[i]) > 1 and rows[i][1]:
                english_text = rows[i][1]
                text_hash = self.get_hash(english_text)
                
                # Check Cache
                cache_key = f"{language_code}:{text_hash}"
                
                if cache_key in self.cache:
                    # Aus Cache nehmen
                    translation = self.cache[cache_key]
                    cached_count += 1
                    print(f"⚡ (Cache) {english_text:30} → {translation}")
                else:
                    # Neu übersetzen
                    result = self.translator.translate_text(
                        english_text,
                        target_lang=target_lang
                    )
                    translation = result.text
                    self.cache[cache_key] = translation
                    translated_count += 1
                    print(f"✓ (Neu)   {english_text:30} → {translation}")
                
                rows[i].append(translation)
        
        # Speichern
        with open(csv_file, 'w', newline='', encoding='utf-8') as f:
            writer = csv.writer(f)
            writer.writerows(rows)
        
        # Cache speichern
        self.save_cache()
        
        print(f"\n✅ Fertig!")
        print(f"   Neu übersetzt: {translated_count}")
        print(f"   Aus Cache: {cached_count}")

if __name__ == '__main__':
    import argparse
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--csv', required=True)
    parser.add_argument('--lang', required=True)
    parser.add_argument('--api-key', required=True)
    
    args = parser.parse_args()
    
    translator = DeltaTranslator(args.api_key)
    translator.translate_csv_delta(args.csv, args.lang)
```

### Verwendung:

```bash
# Erste Übersetzung (alle Wörter)
python3 scripts/translate_delta.py \
  --csv translations/glossary_all.csv \
  --lang fr \
  --api-key "your-deepl-api-key"

# Output:
# ✓ (Neu)   Abbrechen                  → Annuler
# ✓ (Neu)   Anmeldedaten              → Identifiants
# ... 70 Wörter
# ✅ Fertig!
#    Neu übersetzt: 70
#    Aus Cache: 0

# Später: Nur 5 neue Wörter hinzugefügt
# Zweite Übersetzung
python3 scripts/translate_delta.py \
  --csv translations/glossary_all.csv \
  --lang fr \
  --api-key "your-deepl-api-key"

# Output:
# ⚡ (Cache) Abbrechen                  → Annuler
# ⚡ (Cache) Anmeldedaten              → Identifiants
# ... 65 cached
# ✓ (Neu)   Synchronisieren            → Synchroniser
# ✓ (Neu)   Verschlüsseln             → Chiffrer
# ... 5 neue
# ✅ Fertig!
#    Neu übersetzt: 5
#    Aus Cache: 65

# Cache-Datei: translation_cache.json
# {
#   "fr:abc123...": "Annuler",
#   "fr:def456...": "Identifiants",
#   ...
# }
```

---

## 4. Rechtschreibung & Grammatik

### Optionen:

| Tool | Kostenlos | Qualität | LLM | Einfachheit |
|------|-----------|----------|-----|-------------|
| **LanguageTool** | ✅ Kostenlos | ✅✅ Gut | ❌ | ✅✅ Einfach |
| **Grammarly API** | ❌ Bezahlt | ✅✅✅ Sehr gut | ✅ LLM | ⚠️ Komplex |
| **Ollama (lokales LLM)** | ✅ Kostenlos | ✅ Gut | ✅ Ja | ✅ Einfach |
| **ChatGPT API** | ❌ Bezahlt | ✅✅✅ Excellent | ✅ GPT-4 | ⚠️ Teuer |

### EMPFEHLUNG: LanguageTool (kostenlos)

```bash
pip install language-tool-python
```

```python
#!/usr/bin/env python3
# scripts/check_grammar.py

from language_tool_python import LanguageTool
import csv

def check_translations_grammar(csv_file: str, language_code: str):
    """
    Prüfe Rechtschreibung & Grammatik der Übersetzungen
    """
    
    # LanguageTool für verschiedene Sprachen
    lang_map = {
        'en': 'en-US',
        'fr': 'fr',
        'es': 'es',
        'pt': 'pt',
        'it': 'it',
        'nl': 'nl',
        'pl': 'pl'
    }
    
    tool = LanguageTool(lang_map.get(language_code, 'en-US'))
    
    # Lese CSV
    with open(csv_file, 'r', encoding='utf-8') as f:
        reader = csv.reader(f)
        rows = list(reader)
    
    issues_found = 0
    
    # Prüfe jede Übersetzung
    for i in range(1, len(rows)):
        if len(rows[i]) > 1 and rows[i][1]:
            original = rows[i][0]
            translation = rows[i][1]
            
            # Prüfe
            matches = tool.check(translation)
            
            if matches:
                issues_found += 1
                print(f"\n⚠️  {original}")
                print(f"    Übersetzung: {translation}")
                
                for match in matches:
                    print(f"    Fehler: {match.message}")
                    print(f"    Vorschlag: {match.replacements[:3]}")
    
    print(f"\n✅ Grammatik-Prüfung fertig!")
    print(f"   Probleme gefunden: {issues_found}")

if __name__ == '__main__':
    import argparse
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--csv', required=True)
    parser.add_argument('--lang', required=True)
    
    args = parser.parse_args()
    check_translations_grammar(args.csv, args.lang)
```

### Verwendung:

```bash
python3 scripts/check_grammar.py \
  --csv translations/glossary_all.csv \
  --lang fr

# Output:
# ⚠️  Abbrechen
#     Übersetzung: Anuler
#     Fehler: Typo or grammar error
#     Vorschlag: ['Annuler', 'Annulé', 'Annulez']

# ⚠️  Synchronisieren
#     Übersetzung: Sincroniser
#     Fehler: Word not recognized
#     Vorschlag: ['Synchroniser', 'Sincronisé']

# ✅ Grammatik-Prüfung fertig!
#    Probleme gefunden: 2
```

---

## 5. Kompletter praktischer Workflow

### Schritt 1: Englisch manuell (LM Studio)
```bash
# 70 Wörter mit LM Studio/Ollama
# 10-15 Minuten
```

### Schritt 2: Alle anderen Sprachen mit DeepL
```bash
# Englisch → 29 Sprachen
for lang in fr es pt it nl pl de sv da no; do
    python3 scripts/translate_delta.py \
      --csv translations/glossary_all.csv \
      --lang $lang \
      --api-key "your-deepl-api-key"
done

# Total: ~30 Sekunden (alles cached nach erstem Lauf)
```

### Schritt 3: Grammatik-Prüfung
```bash
python3 scripts/check_grammar.py \
  --csv translations/glossary_all.csv \
  --lang fr

# Behebe Fehler manuell in Excel
```

### Schritt 4: Import & Release
```bash
./batch_import_parallel.sh

git push
# GitHub Actions → Release
```

---

## 6. Kostenübersicht (30 Sprachen, 70 Wörter)

| Methode | Kosten/Monat | Qualität |
|---------|-------------|----------|
| **DeepL Free** | €0 | ✅✅✅ Beste |
| **Google Translate Free** | €0 | ✅✅ Gut |
| **Microsoft Translator Free** | €0 | ✅✅ Gut |
| **OpenAI GPT-4** | €0.05-0.10 | ✅✅✅ Excellent |
| **AWS Translate** | €0.30 | ✅✅ Gut |

**EMPFEHLUNG: DeepL Free + LanguageTool (€0 / 100% kostenlos)**

---

## 7. Cache-Strategie (wichtig!)

```
Ersten Monat: Alle 70 Wörter × 30 Sprachen = 500K chars
↓
Cache speichert alles
↓
Nächste Monate: 
- 5 neue Strings hinzugefügt?
- Nur diese 5 × 30 Sprachen übersetzen
- Rest aus Cache
↓
99% Kostenersparnis!
```

### Cache-Datei:
```json
{
  "fr:abc123": "Annuler",
  "es:abc123": "Cancelar",
  "pt:abc123": "Cancelar",
  "it:abc123": "Annulla",
  ...
}
```

---

## Fazit

**Dein BESTES Setup:**

```
1. Englisch: LM Studio/Ollama manuell (10 Min)
2. Rest: DeepL Free API (kostenlos, sehr gut)
3. Cache: Nur neue Strings übersetzen (99% Ersparnis)
4. Grammar: LanguageTool kostenlos prüfen
5. Import: Automatisch

TOTAL KOSTEN: €0 / 100% kostenlos!
TOTAL ZEIT: 15-20 Minuten für 30 Sprachen
QUALITÄT: Höchste (besser als Google!)
```

**Du brauchst wirklich nichts zu bezahlen!** 🎯
