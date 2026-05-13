#!/usr/bin/env python3
"""
Installer Builder for Screen Mirror App
Windows ve Linux için exe/deb paketleri oluştur
"""

import os
import sys
import subprocess
import shutil
from pathlib import Path

class InstallerBuilder:
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.dist_dir = self.project_root / "dist"
        self.build_dir = self.project_root / "build"
        
    def build_linux(self):
        """Linux için .deb paketi oluştur"""
        print("🐧 Linux Installer oluşturuluyor...")
        
        # PyInstaller ile executable yap
        subprocess.run([
            "pyinstaller", "--onefile", "--windowed",
            "--name=ScreenMirror",
            "--icon=assets/icon.png",
            "app_linux.py"
        ], check=True)
        
        # FPM ile deb paketi oluştur
        subprocess.run([
            "fpm", "-s", "dir", "-t", "deb",
            "-n", "screen-mirror-app",
            "-v", "1.0.0",
            "-C", str(self.dist_dir / "ScreenMirror"),
            "-p", "ScreenMirror-Linux.deb",
            "."
        ], check=True)
        
        print("✅ Linux Installer hazır: ScreenMirror-Linux.deb")
        
    def build_windows(self):
        """Windows için .exe installer oluştur"""
        print("🪟 Windows Installer oluşturuluyor...")
        
        # PyInstaller ile executable yap
        subprocess.run([
            "pyinstaller", "--onefile", "--windowed",
            "--name=ScreenMirror",
            "--icon=assets/icon.ico",
            "--add-data=config.json;.",
            "app_windows.py"
        ], check=True)
        
        # NSIS ile installer yap
        subprocess.run([
            "makensis",
            "installer.nsi"
        ], check=True)
        
        print("✅ Windows Installer hazır: ScreenMirror-Setup.exe")
        
    def build(self):
        """Build başlat"""
        if sys.platform == "linux":
            self.build_linux()
        elif sys.platform == "win32":
            self.build_windows()
        else:
            print("❌ Bilinmeyen platform")

if __name__ == "__main__":
    builder = InstallerBuilder()
    builder.build()
