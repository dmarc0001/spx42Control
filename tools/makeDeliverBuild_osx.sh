#!/bin/bash
#
# erstelle ein instll build

#
# Definitionen
#
ARCH_PREFIX=0.5.2
QT_PREFIX=5.12.0
PROJECTBASE=/Users/dmarcini/entwicklung/spx42Control
PROJECTFILE=$PROJECTBASE/spx42Control.pro
PROJECTBUILDDIR=DEPLOYDIR
PROJECTTYPE=release
TRANSLATION=$PROJECTBASE/src/translations
INSTALLERBASE=$PROJECTBASE/installer
TRANSLATION=$PROJECTBASE/src/translations
INSTALLERBASE=$PROJECTBASE/installer
APP_INSTALLER_FILE=$ARCH_PREFIXspx42Control.7z
APP_INSTALLER_FILE_PATH=$INSTALLERBASE/packages/app/data/
RUNTIME_INSTALLER_FILE=$QT_PREFIXqtRuntime_5120.7z
RUNTIME_INSTALLER_FILE_PATH=$INSTALLERBASE/packages/runtime/data/

QT_BASEDIR=/Users/dmarcini/Qt/$QT_PREFIX/clang_64/bin
QT_QMAKE=$QT_BASEDIR/qmake
MAKE=/usr/bin/make
DEPLOY=$QT_BASEDIR/macdeployqt
ARCHIVEGEN=/Users/dmarcini/Qt/Tools/QtInstallerFramework/3.0/bin/archivegen
BINARYCREATOR=/Users/dmarcini/Qt/Tools/QtInstallerFramework/3.0/bin/binarycreator
MAKE_DONE=false

#
# merke mir das Verzeichnis
#
pushd `pwd`
cd $PROJECTBASE
# 
# Ausgabeverzeichnis erzeugen, falls notwendig
#
if [ ! -e $PROJECTBUILDDIR ] 
then
  echo "erzeuge deliver builddir..." 
  mkdir -p $PROJECTBUILDDIR
fi

#
# in das Verzeichnis wechseln und komplett leeren
#
cd $PROJECTBUILDDIR
# TODO: entkommentieren  rm -rf ./*

#
# mit qmake das make konfigurieren
#
echo "qmake ausfuehren..."
$QT_QMAKE $PROJECTFILE  -Wall -nocache -spec macx-clang  "CONFIG += $PROJECTTYPE" "CONFIG+=x86_64" 
# das eigentliche compilieren
echo "build Projekt $PROJECTTYPE"
$MAKE qmake_all first && MAKE_DONE=true

#
# falls das falsch ist, zum Ende kommen
#
if [ "$MAKE_DONE" == "false" ] 
then
  echo "Fehler beim compilieren..."
  popd
  exit
fi

#
# jettz deployment...
#
echo "Deployment..."
MAKE_DONE=false
# cd out
pwd
$DEPLOY  out/spx42Control.app -verbose=1 -always-overwrite -appstore-compliant  && MAKE_DONE=true



popd


