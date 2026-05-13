#!/bin/bash

echo "🐧 Linux Installer oluşturuluyor..."

# Virtual environment oluştur
python3 -m venv venv
source venv/bin/activate

# Bağımlılıkları yükle
pip install -r requirements.txt
pip install pyinstaller fpm

# PyInstaller ile executable yap
pyinstaller --onefile --windowed \
  --name=ScreenMirror \
  --icon=assets/icon.png \
  app_linux.py

# DEB paketi oluştur
mkdir -p build/screen-mirror-app_1.0.0/usr/bin
mkdir -p build/screen-mirror-app_1.0.0/usr/share/applications

cp dist/ScreenMirror build/screen-mirror-app_1.0.0/usr/bin/

# Desktop dosyası
cat > build/screen-mirror-app_1.0.0/usr/share/applications/screen-mirror.desktop << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Screen Mirror App
Exec=/usr/bin/ScreenMirror
Icon=screen-mirror
Categories=Utility;
EOF

# DEB paketi oluştur
fpm -s dir -t deb \
  -n screen-mirror-app \
  -v 1.0.0 \
  -C build/screen-mirror-app_1.0.0 \
  -p ScreenMirror-Setup.deb \
  -m "Efe-Elite-Dev" \
  --description "Screen Mirroring App with Chromecast, SmartView, Miracast" \
  .

echo "✅ Installer hazır: ScreenMirror-Setup.deb"
