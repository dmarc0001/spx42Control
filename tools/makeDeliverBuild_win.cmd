@echo off
:: erzeuge ein Build zum ausliefern
::
:: Definitionen/Verzeichnisse
SET PROJECTTYPE=release
SET QT_PREFIX=5.12.0
SET ONLINEINSTALLER=spx42ControlOnlineInstaller
SET OFFLINEINSTALLER=spx42ControlOfflineInstaller
::
SET PACKAGES=packages
SET PROJECTBASE=C:\DATEN\Entwicklung\QT-Projekte\spx42Control
SET QT_DIR=C:\Qt
SET QT_BASEDIR=%QT_DIR%\%QT_PREFIX%\msvc2015_64\bin
::SET QT_BASEDIR=%QT_DIR%\%QT_PREFIX%\msvc2017_64\bin
::SET QT_BASEDIR=%QT_DIR%\%QT_PREFIX%\msvc2017\bin
SET QT_TOOLS=%QT_DIR%\Tools
SET QT_INSTALLER_DIR=%QT_TOOLS%\QtInstallerFramework\3.0\bin
SET PROJECTBUILDDIR=DEPLOYDIR
SET TRANSLATION=%PROJECTBASE%\src\translations
SET INSTALLERBASE=%PROJECTBASE%\installer
SET EXTRAS=%INSTALLERBASE%\extra
SET REPOSITORY_DIR=repository
SET APP_INSTALLER_FILE_PATH=%INSTALLERBASE%\%PACKAGES%\spx42Control\data\
SET RUNTIME_INSTALLER_FILE_PATH=%INSTALLERBASE%\%PACKAGES%\qtRuntime\data\
::
SET PROJECTFILE=%PROJECTBASE%\spx42Control.pro
SET APP_INSTALLER_FILE=spx42Control.7z
SET RUNTIME_INSTALLER_FILE=qtRuntime.7z
SET QT_QMAKE=%QT_BASEDIR%\qmake.exe
SET QT_JOM=%QT_TOOLS%\QtCreator\bin\jom.exe
SET DEPLOY=%QT_BASEDIR%\windeployqt.exe
SET ARCHIVEGEN=%QT_INSTALLER_DIR%\archivegen.exe
SET REPOGEN=%QT_INSTALLER_DIR%\repogen.exe
SET BINARYCREATOR=%QT_INSTALLER_DIR%\binarycreator.exe
SET MAKE_DONE=false

echo.
echo.
pushd %cd%
cd %PROJECTBASE%

:: das Projekt Buildverzechnis testen und ggf anlegen
if exist %PROJECTBUILDDIR% (
  rmdir /q /s %PROJECTBUILDDIR%
)
mkdir %PROJECTBUILDDIR%
:: das Repository Verzeichnis testen und ggf anlegen
if exist %REPOSITORY_DIR% (
  rmdir /q /s %REPOSITORY_DIR%
)
mkdir %REPOSITORY_DIR%
cd %PROJECTBUILDDIR%

echo directory %cd%

echo Umgebung einrichten...
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
SET PATH=%PATH%;C:\Program Files (x86)\Windows Kits\10\bin\10.0.17763.0\x64
::call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
::call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community/VC\Auxiliary\Build\vcvars32.bat"

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

echo entferne alte installer
if exist "%APP_INSTALLER_FILE_PATH%%APP_INSTALLER_FILE%" (
  echo loesche alte datei "%APP_INSTALLER_FILE_PATH%%APP_INSTALLER_FILE%"
  del /F /Q "%APP_INSTALLER_FILE_PATH%%APP_INSTALLER_FILE%"
)

if exist "%RUNTIME_INSTALLER_FILE_PATH%%RUNTIME_INSTALLER_FILE%" (
  echo loesche alte datei "%RUNTIME_INSTALLER_FILE_PATH%%RUNTIME_INSTALLER_FILE%"
  del /F /Q "%RUNTIME_INSTALLER_FILE_PATH%%APP_INSTARUNTIME_INSTALLER_FILELLER_FILE%"
)

if exist "%INSTALLERBASE%\%ONLINEINSTALLER%" (
  echo loesche alte datei "%INSTALLERBASE%\%ONLINEINSTALLER%"
  del /F /Q "%INSTALLERBASE%\%ONLINEINSTALLER%"
)

if exist "%INSTALLERBASE%\%OFFLINEINSTALLER%" (
  echo loesche alte datei "%INSTALLERBASE%\%OFFLINEINSTALLER%"
  del /F /Q "%INSTALLERBASE%\%OFFLINEINSTALLER%"
)


echo erzeuge updater packages

SET MAKE_DONE=false
:: BUGFIX von visual studio/qt deploy
cp -f %EXTRAS%\ucrtbased.dll ucrtbased.dll
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
cd %INSTALLERBASE%
echo erzeuge das installerprogramm online...

echo %BINARYCREATOR% -n -c config\config.xml -p packages -i qtRuntime,spx42Control %ONLINEINSTALLER%
%BINARYCREATOR% -n -c config\config.xml -p packages -i qtRuntime,spx42Control %ONLINEINSTALLER%
:: repository generate
%REPOGEN% -p packages -i qtRuntime,spx42Control ../repository/

::sleep 2
::echo erzeuge das installerprogramm offline...
::echo %BINARYCREATOR% -f -c config\config.xml -p packages -i qtRuntime,spx42Control %OFFLINEINSTALLER%
::%BINARYCREATOR% -f -c config\config.xml -p packages -i qtRuntime,spx42Control %OFFLINEINSTALLER%

:: jetzt die META Daten zippen





:false_end
echo fertig (%MAKE_DONE%)...
echo.

popd



