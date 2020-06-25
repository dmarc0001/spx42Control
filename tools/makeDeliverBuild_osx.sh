#!/bin/bash
#
# erstelle ein instll build

#
# Definitionen
#
QT_PREFIX=5.12.1
ONLINEINSTALLER=spx42ControlOnlineInstallerMac.dmg
OFFLINEINSTALLER=spx42ControlOfflineInstallerMac.dmg
PROJECTBASE=/Users/dmarcini/entwicklung/spx42Control
PROJECTFILE=$PROJECTBASE/spx42Control.pro
PROJECTBUILDDIR=DEPLOYDIR
PROJECTTYPE=release
TRANSLATION=$PROJECTBASE/src/translations
INSTALLERBASE=$PROJECTBASE/installer
TRANSLATION=$PROJECTBASE/src/translations
INSTALLERBASE=$PROJECTBASE/installer
HELPSYSTEM=$PROJECTBASE/helpsystem
HELPDESTINATION=$PROJECTBASE/$PROJECTBUILDDIR/out/spx42Control.app/Contents/MacOS
REPOSITORY_DIR=repository
APP_INSTALLER_FILE=spx42Control.7z
APP_INSTALLER_FILE_PATH=$INSTALLERBASE/packages/spx42ControlMac/data/
APP_INSTALLER_CONFIG=config_darwin.xml

QT_BASEDIR=/Users/dmarcini/Qt/$QT_PREFIX/clang_64/bin
QT_QMAKE=$QT_BASEDIR/qmake
MAKE=/usr/bin/make
DEPLOY=$QT_BASEDIR/macdeployqt
ARCHIVEGEN=/Users/dmarcini/Qt/Tools/QtInstallerFramework/3.0/bin/archivegen
BINARYCREATOR=/Users/dmarcini/Qt/Tools/QtInstallerFramework/3.0/bin/binarycreator
REPOGEN=/Users/dmarcini/Qt/Tools/QtInstallerFramework/3.0/bin/repogen
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
# das Repository Verzeichnis testen und ggf anlegen
#
if [ -d $REPOSITORY_DIR ] 
then
  rm -r  $REPOSITORY_DIR
fi
mkdir $REPOSITORY_DIR

#
# in das Verzeichnis wechseln und komplett leeren
#
cd $PROJECTBUILDDIR

rm -rf ./out/*
# rm -rf ./*

#
# mit qmake das make konfigurieren
#
echo "qmake ausfuehren..."
$QT_QMAKE $PROJECTFILE  -Wall -nocache -spec macx-clang  "CONFIG += $PROJECTTYPE" "CONFIG+=x86_64" 
# das eigentliche compilieren
echo "build Projekt $PROJECTTYPE"
$MAKE -j4 qmake_all first && MAKE_DONE=true

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
# übersetzungen machen und kopieren
# TODO: Übersetzung updaten...
#
echo "kopiere translations ($TRANSLATION\*.qm)..."
if [ -e $TRANSLATION/*.qm ] 
then
  cp -f $TRANSLATION/*.qm out/spx42Control.app/Contents/MacOS/
fi

#
# kopieren des Hilfesystems in die App
#
echo "kopiere helpsystem ($HELPSYSTEM nach $HELPDESTINATION )..."
if [ -d $HELPSYSTEM ] 
then
  cp -rf $HELPSYSTEM $HELPDESTINATION/
  find $HELPDESTINATION -type f -name "*.xcf" -exec rm -f "{}" \;
fi

#
# jettz deployment...
#
echo "Deployment..."
MAKE_DONE=false
cd out
pwd
$DEPLOY  spx42Control.app -verbose=1 -always-overwrite -appstore-compliant  && MAKE_DONE=true
#$DEPLOY  spx42Control.app -dmg -verbose=1 -always-overwrite -appstore-compliant  && MAKE_DONE=true


echo "create package..."
echo $ARCHIVEGEN $APP_INSTALLER_FILE_PATH/$APP_INSTALLER_FILE 
$ARCHIVEGEN $APP_INSTALLER_FILE_PATH/$APP_INSTALLER_FILE *


cd $INSTALLERBASE
pwd
echo "make repository..."
$REPOGEN -p packages -i spx42ControlMac ../repository

echo "make binary creator..."
echo $BINARYCREATOR -n -c config/$APP_INSTALLER_CONFIG -p packages -i spx42ControlMac $ONLINEINSTALLER
$BINARYCREATOR -n -c config/$APP_INSTALLER_CONFIG -p packages -i spx42ControlMac $ONLINEINSTALLER


popd


