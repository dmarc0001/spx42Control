#!/bin/bash
#
# Installer wieder bereinigen
#

#
# Einstellungen
#    
BUILDDIR=build
RELEASEDIR=release
INSTALLERDIR=installer
PACK_UPDATER=$INSTALLERDIR/packages/btupdater/data
PACK_RUNTIME=$INSTALLERDIR/packages/runtime/data
PACK_FIRMWARE=$INSTALLERDIR/packages/firmware/data
REPO_DIR=$INSTALLERDIR/repository
INST_RELEASE_DIR=$INSTALLERDIR/release
INST_DEBUG_DIR=$INSTALLERDIR/debug

pushd `pwd`
cd ..

if [ -d $REPO_DIR ] ; then
  echo "remove repository directory..."
  rm -rf $REPO_DIR
fi

if [ -d $BUILDDIR ] ; then
  echo "remove build directory..."
  rm -rf $BUILDDIR
fi

if [ -d $RELEASEDIR ] ; then
  echo "remove release directory..."
  rm -rf $RELEASEDIR
fi

if [ -d $INST_DEBUG_DIR ] ; then 
  echo "remove installation debug directory..."
  rm -rf $INST_DEBUG_DIR
fi

if [ -d $INST_RELEASE_DIR ] ; then
  echo "remove installation release directory..."
  rm -rf $INST_RELEASE_DIR
fi

echo "remove installation Makefiles..." 
rm -f $INSTALLERDIR/Makefile*  

cd ..
echo "remove package 7zip files..."
rm -f $PACK_UPDATER/*.7z
rm -f $PACK_RUNTIME/*.7z
rm -f $PACK_FIRMWARE/*.7z


popd   