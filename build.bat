@echo off
echo.🪟 Windows Installer oluşturuluyor...

REM Python ortamı
python -m venv venv
call venv\Scripts\activate.bat

REM Bağımlılıkları yükle
pip install -r requirements.txt
pip install pyinstaller

REM PyInstaller ile executable yap
pyinstaller --onefile --windowed ^
  --name=ScreenMirror ^
  --icon=assets\icon.ico ^
  --add-data=config.json;. ^
  app_windows.py

REM NSIS Installer yap (NSIS yüklü olması gerekir)
if exist "C:\Program Files (x86)\NSIS\makensis.exe" (
  "C:\Program Files (x86)\NSIS\makensis.exe" installer.nsi
  echo.✅ Installer hazır: ScreenMirror-Setup.exe
) else (
  echo.⚠️ NSIS yüklü değil. Sadece exe oluşturuldu.
  echo.NSIS indirmek için: https://nsis.sourceforge.io/
)

pause
