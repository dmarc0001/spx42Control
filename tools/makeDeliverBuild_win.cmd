@echo off
:: erzeuge ein Build zum ausliefern
::
:: Definitionen/Verzeichnisse
SET PROJECTTYPE=release
SET QT_PREFIX=5.12.1
SET ONLINEINSTALLER=spx42ControlOnlineInstaller
SET OFFLINEINSTALLER=spx42ControlOfflineInstaller
::
SET WINKIT=C:\Program Files (x86)\Windows Kits\10\bin\10.0.17763.0\x64
SET PACKAGES=packages
SET PROJECTBASE=C:\DATEN\Entwicklung\QT-Projekte\submatix\spx42Control
SET QT_DIR=C:\localProg\Qt
SET QT_BASEDIR=%QT_DIR%\%QT_PREFIX%\msvc2017_64\bin
SET QT_TOOLS=%QT_DIR%\Tools
SET QT_INSTALLER_DIR=%QT_TOOLS%\QtInstallerFramework\3.2\bin
SET PROJECTBUILDDIR=DEPLOYDIR
SET HELPSOURCE=%PROJECTBASE%\helpsystem
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
SET APP_INSTALLER_CONFIG=config_windows.xml

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
SET PATH=%PATH%;%WINKIT%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo.
echo qmake ausfuehren
%QT_QMAKE% %PROJECTFILE%  -Wall -nocache -spec win32-msvc "CONFIG += %PROJECTTYPE%"

echo build Projekt %PROJECTTYPE%
%QT_JOM% qmake_all first && SET MAKE_DONE=true

:: falls das falsch ist, zum Ende kommen
if "%MAKE_DONE%" == "false" goto false_end

echo Deployment... 
SET MAKE_DONE=false
%DEPLOY% --%PROJECTTYPE% --no-quick-import --no-system-d3d-compiler --no-compiler-runtime --no-opengl-sw "out" && SET MAKE_DONE=true
::%DEPLOY% --%PROJECTTYPE% --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-opengl-sw "out" && SET MAKE_DONE=true

:: falls das falsch ist, zum Ende kommen
if "%MAKE_DONE%" == "false" goto false_end

:: Sprachdateien kopieren
echo kopiere translations (%TRANSLATION%\*.qm)  ...
if exist "%TRANSLATION%\*.qm" (
  copy /Y  "%TRANSLATION%\*.qm"  ".\out\"
)

:: Hilfadateien kopieren, gimp vorlagen wieder entfernen
echo kopiere hilfedateien (%HELPSOURCE%) ...
if exist %HELPSOURCE% (
  xcopy /S  %HELPSOURCE% ".\out\helpsystem\"
  del /F /Q ".\out\helpsystem\de\*.xcf"
  del /F /Q ".\out\helpsystem\en\*.xcf"
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
cp -f %EXTRAS%\libEGL.dll libEGL.dll
cp -f %EXTRAS%\libGLESv2.dll libGLESv2.dll
cp -f %EXTRAS%\msvcp140.dll msvcp140.dll
cp -f %EXTRAS%\opengl32sw.dll opengl32sw.dll
cp -f %EXTRAS%\vccorlib140.dll vccorlib140.dll
cp -f %EXTRAS%\vcruntime140.dll vcruntime140.dll

echo %ARCHIVEGEN% %APP_INSTALLER_FILE_PATH%\%APP_INSTALLER_FILE% *.exe *.qm *.ilk *.pdb ucrtbased.dll libEGL.dll libGLESv2.dll msvcp140.dll opengl32sw.dll vccorlib140.dll vcruntime140.dll
%ARCHIVEGEN% %APP_INSTALLER_FILE_PATH%\%APP_INSTALLER_FILE% *.exe *.qm *.ilk *.pdb ucrtbased.dll libEGL.dll libGLESv2.dll msvcp140.dll opengl32sw.dll vccorlib140.dll vcruntime140.dll && SET MAKE_DONE=true

:: falls das falsch ist, zum Ende kommen
if "%MAKE_DONE%" == "false" goto false_end

del /f /q  spx42Control.*
del /f /q  *.qm
del /f /q ucrtbased.dll
del /f /q libEGL.dll
del /f /q libGLESv2.dll
del /f /q msvcp140.dll
del /f /q opengl32sw.dll
del /f /q vccorlib140.dll
del /f /q vcruntime140.dll
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

echo %BINARYCREATOR% --online-only -c config\%APP_INSTALLER_CONFIG% -p packages -i qtRuntime,spx42Control %ONLINEINSTALLER%
%BINARYCREATOR% --online-only -c config\%APP_INSTALLER_CONFIG% -p packages -i qtRuntime,spx42Control %ONLINEINSTALLER%
:: repository generate
echo %REPOGEN% -p packages -i qtRuntime,spx42Control ../repository/
%REPOGEN% -p packages -i qtRuntime,spx42Control ../repository/

::sleep 2
::echo erzeuge das installerprogramm offline...
::echo %BINARYCREATOR% -f -c config\%APP_INSTALLER_CONFIG% -p packages -i qtRuntime,spx42Control %OFFLINEINSTALLER%
::%BINARYCREATOR% -f -c config\%APP_INSTALLER_CONFIG% -p packages -i qtRuntime,spx42Control %OFFLINEINSTALLER%

:: jetzt die META Daten zippen





:false_end
echo fertig (%MAKE_DONE%)...
echo.

popd



