# Raspberry Pi 5 als GitHub Actions Self-Hosted Runner

## Voraussetzungen
- Raspberry Pi 5 (8GB) mit Raspberry Pi OS (64-bit)
- Internetverbindung
- GitHub Repository Admin-Zugang

## 1. Dependencies installieren (einmalig)

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y \
  build-essential cmake ninja-build git curl \
  qt6-base-dev qt6-websockets-dev \
  libgl1-mesa-dev libxkbcommon-dev \
  libxcb-xinerama0-dev libxcb-cursor0 \
  libsqlite3-dev libssl-dev
```

## 2. GitHub Runner installieren

```bash
# Verzeichnis erstellen
mkdir -p ~/actions-runner && cd ~/actions-runner

# Runner herunterladen (ARM64 Version)
curl -o actions-runner-linux-arm64-2.321.0.tar.gz -L \
  https://github.com/actions/runner/releases/download/v2.321.0/actions-runner-linux-arm64-2.321.0.tar.gz

# Entpacken
tar xzf ./actions-runner-linux-arm64-2.321.0.tar.gz
```

## 3. Runner registrieren

1. Gehe zu: https://github.com/georg0480/mailadler/settings/actions/runners/new
2. Wähle "Linux" und "ARM64"
3. Kopiere den Token aus der Anleitung
4. Führe aus:

```bash
./config.sh --url https://github.com/georg0480/mailadler --token DEIN_TOKEN
```

Bei der Konfiguration:
- **Runner group**: Enter (default)
- **Runner name**: `pi5-dns` (oder anderer Name)
- **Labels**: `self-hosted,linux,ARM64` (wichtig!)
- **Work folder**: Enter (default: `_work`)

## 4. Als Systemd-Service einrichten (läuft automatisch)

```bash
sudo ./svc.sh install
sudo ./svc.sh start

# Status prüfen
sudo ./svc.sh status
```

## 5. Testen

Gehe zu: https://github.com/georg0480/mailadler/actions

Klicke auf "build-mailadler-linux-arm64" → "Run workflow"

Der Build sollte auf deinem Pi laufen!

## Ressourcen-Tipps für DNS-Server + Runner

Da der Pi auch DNS-Server ist:

```bash
# Runner-Priority senken (nice)
sudo nano /etc/systemd/system/actions.runner.*.service
```

Füge hinzu unter `[Service]`:
```ini
Nice=10
CPUQuota=75%
MemoryMax=6G
```

```bash
sudo systemctl daemon-reload
sudo systemctl restart actions.runner.*.service
```

## Logs anschauen

```bash
journalctl -u actions.runner.* -f
```

## Runner stoppen/deaktivieren

```bash
sudo ./svc.sh stop
sudo ./svc.sh uninstall
```
