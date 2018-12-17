@echo off
:: erzeuge ein Build zum ausliefern
::
:: Definitionen/Verzeichnisse
SET ARCH_PREFIX=0.5.2
SET QT_PREFIX=5.12.0
SET PROJECTBASE=C:\DATEN\Entwicklung\QT-Projekte\spx42Control
SET QT_DIR=C:\Qt
SET QT_BASEDIR=%QT_DIR%\%QT_PREFIX%\msvc2017_64\bin
SET QT_TOOLS=%QT_DIR%\Tools
SET PROJECTBUILDDIR=DEPLOYDIR
SET TRANSLATION=%PROJECTBASE%\src\translations
SET INSTALLERBASE=%PROJECTBASE%\installer
SET APP_INSTALLER_FILE_PATH=%INSTALLERBASE%\packages\app\data\
SET RUNTIME_INSTALLER_FILE_PATH=%INSTALLERBASE%\packages\qtRuntime\data\

SET PROJECTTYPE=release
SET PROJECTFILE=%PROJECTBASE%\spx42Control.pro
SET APP_INSTALLER_FILE=%ARCH_PREFIX%spx42Control.7z
SET RUNTIME_INSTALLER_FILE=%QT_PREFIX%qtRuntime_5120.7z
SET QT_QMAKE=%QT_BASEDIR%\qmake.exe
SET QT_JOM=%QT_TOOLS%\QtCreator\bin\jom.exe
SET DEPLOY=%QT_DIR%\%QT_PREFIX%\msvc2017_64\bin\windeployqt.exe
SET ARCHIVEGEN=%QT_TOOLS%\QtInstallerFramework\3.0\bin\archivegen.exe
SET BINARYCREATOR=%QT_TOOLS%\QtInstallerFramework\3.0\bin\binarycreator.exe
SET MAKE_DONE=false

echo.
echo.

pushd %cd%
cd ..
if not exist %PROJECTBUILDDIR% (
  mkdir %PROJECTBUILDDIR%
)
cd %PROJECTBUILDDIR%

echo directory %cd%

echo Umgebung einrichten...
call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvars64.bat"

echo.
echo qmake ausfuehren
%QT_QMAKE% %PROJECTFILE%  -Wall -nocache -spec win32-msvc "CONFIG += %PROJECTTYPE%"

echo build Projekt %PROJECTTYPE%
%QT_JOM% qmake_all first && SET MAKE_DONE=true

:: falls das falsch ist, zum Ende kommen
if "%MAKE_DONE%" == "false" goto false_end

echo Deployment... 
SET MAKE_DONE=false
%DEPLOY% --%PROJECTTYPE% --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-opengl-sw "out" && SET MAKE_DONE=true

:: falls das falsch ist, zum Ende kommen
if "%MAKE_DONE%" == "false" goto false_end

:: Sprachdateien kopieren
echo kopiere translations (%TRANSLATION%\*.qm)  ...
if exist "%TRANSLATION%\*.qm" (
  copy /Y  "%TRANSLATION%\*.qm"  ".\out\"
)

cd out
echo erzeuge updater packages

if exist "%APP_INSTALLER_FILE_PATH%%APP_INSTALLER_FILE%" (
  echo loesche alte datei "%APP_INSTALLER_FILE_PATH%%APP_INSTALLER_FILE%"
  del /F /Q "%APP_INSTALLER_FILE_PATH%%APP_INSTALLER_FILE%"
)

SET MAKE_DONE=false
:: BUGFIX von visual studio/qt deploy
cp -f %INSTALLERBASE%\ucrtbased.dll ucrtbased.dll
echo %ARCHIVEGEN% %APP_INSTALLER_FILE_PATH%\%APP_INSTALLER_FILE% *.exe *.qm *.ilk *.pdb ucrtbased.dll
%ARCHIVEGEN% %APP_INSTALLER_FILE_PATH%\%APP_INSTALLER_FILE% *.exe *.qm *.ilk *.pdb ucrtbased.dll && SET MAKE_DONE=true

:: falls das falsch ist, zum Ende kommen
if "%MAKE_DONE%" == "false" goto false_end

del /f /q  spx42Control.*
del /f /q  *.qm
del /f /q ucrtbased.dll
del /f /q "%RUNTIME_INSTALLER_FILE_PATH%%RUNTIME_INSTALLER_FILE%"

SET MAKE_DONE=false
echo erzeuge runtime package (%RUNTIME_INSTALLER_FILE_PATH%%RUNTIME_INSTALLER_FILE%)...
%ARCHIVEGEN% %RUNTIME_INSTALLER_FILE_PATH%%RUNTIME_INSTALLER_FILE% * && SET MAKE_DONE=true

cd ..

:: falls das falsch ist, zum Ende kommen
if "%MAKE_DONE%" == "false" goto false_end

:: den installer erzeugen
echo erzeuge das installerprogramm
cd %INSTALLERBASE%
%BINARYCREATOR% --online-only -c config\config.xml -p packages spx42ControlOnlineInstaller
%BINARYCREATOR% --offline-only -c config\config.xml -p packages spx42ControlOfflineInstaller

:false_end
echo fertig (%MAKE_DONE%)...
echo.

popd



