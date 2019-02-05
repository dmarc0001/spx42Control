###############################################################################
####                                                                       ####
#### Projekt Controller für SPX42 via Bluetooth                            ####
#### Begonnen 05.09.2016                                                   ####
#### Autor: Dirk Marciniak                                                 ####
####                                                                       ####
###############################################################################
MAJOR                                  = 0 # Major Verion Nummer
MINOR                                  = 6 # Minor Version Number
PATCH                                  = 4 # Patch Version Number
BUILD                                  = 0 # win build number

win32:VERSION_PE_HEADER                = $${MAJOR}.$${MINOR}
win32:QMAKE_TARGET_COMPANY             = submatix.com
win32:QMAKE_TARGET_COPYRIGHT           = D. Marciniak
win32:QMAKE_TARGET_PRODUCT             = SPX42 BT-Controller
win32:RC_ICONS                         = src/res/programIcon.ico
win32:VERSION                          = $${MAJOR}.$${MINOR}.$${PATCH}.$${BUILD} # major.minor.patch.build
else:VERSION                           = $${MAJOR}.$${MINOR}.$${PATCH}    # major.minor.patch
macx:ICON                              = src/res/programIcon.ico
#
TARGET                                 = spx42Control
TEMPLATE                               = app
#
QT                                     += core
QT                                     += gui
QT                                     += widgets
QT                                     += bluetooth
QT                                     += sql
QT                                     += charts
QT                                     += concurrent
QT                                     += xml
CONFIG                                 += stl
CONFIG                                 += c++14
CONFIG                                 += lrelease
INCLUDEPATH                            += src
DESTDIR                                = out
MOC_DIR                                = moc
RCC_DIR                                = rcc
UI_DIR                                 = ui

# %{CurrentProject:NativePath}\%{CurrentKit:FileSystemName}\%{CurrentBuild:Name}
# %{CurrentProject:NativePath}/%{CurrentKit:FileSystemName}/%{CurrentBuild:Name}
#
# build:
#
# python(3)
# %{CurrentProject:NativePath}/tools/makeBuildTemplate.py  --srcdir  %{CurrentProject:NativePath}/src/config --build Debug|Release
# %{CurrentProject:NativePath}\tools
#
# deployment
#
# cp
# -f  %{CurrentProject:NativePath}/src/translations/*.qm %{CurrentProject:NativePath}/%{CurrentKit:FileSystemName}/%{CurrentBuild:Name}/out/
# %{buildDir}
#


#
# momentan noch als TESTVERSION markieren
#
DEFINES                                += TESTVERSION
DEFINES                                += VMAJOR=$$MAJOR
DEFINES                                += VMINOR=$$MINOR
DEFINES                                += VPATCH=$$PATCH
DEFINES                                += QT_DEPRECATED_WARNINGS

CONFIG(debug, debug|release) {
  DEFINES                              += DEBUG
  #unix: TARGET                         = $$join(TARGET,,,_debug)
  #else: TARGET                         = $$join(TARGET,,,_D)
}

*msvc* {
  #
  # visual studio spec filter
  # then you not need -j8
  #
  QMAKE_CXXFLAGS += -MP
}

unix:!macos {
    # benutze ccache aber nicht auf mac
    QMAKE_CXX                          = ccache g++
}

SOURCES                 += \
    src/bluetooth/BtDiscoverRemoteDevice.cpp \
    src/bluetooth/BtDiscoverRemoteService.cpp \
    src/bluetooth/BtLocalDevicesManager.cpp \
    src/bluetooth/SPX42BtDevicesManager.cpp \
    src/bluetooth/SPX42RemotBtDevice.cpp \
    src/config/AppConfigClass.cpp \
    src/config/ProjectConst.cpp \
    src/ControlMain.cpp \
    src/database/ChartDataWorker.cpp \
    src/database/LogDetailWalker.cpp \
    src/database/SPX42Database.cpp \
    src/database/spx42databaseconstants.cpp \
    src/guiFragments/ChartsFragment.cpp \
    src/guiFragments/ConnectFragment.cpp \
    src/guiFragments/DeviceConfigFragment.cpp \
    src/guiFragments/DeviceInfoFragment.cpp \
    src/guiFragments/GasFragment.cpp \
    src/guiFragments/IFragmentInterface.cpp \
    src/guiFragments/LogFragment.cpp \
    src/logging/Logger.cpp \
    src/spx42/SPX42Commands.cpp \
    src/spx42/SPX42Config.cpp \
    src/spx42/SPX42Defs.cpp \
    src/spx42/SPX42Gas.cpp \
    src/spx42/SPX42LogDirectoryEntry.cpp \
    src/spx42/SPX42SingleCommand.cpp \
    src/SPX42ControlMainWin.cpp \
    src/uddf/spx42uddfexport.cpp \
    src/utils/AboutDialog.cpp \
    src/utils/DebugDataSeriesGenerator.cpp \
    src/utils/DiveDataSeriesGenerator.cpp \
    src/utils/HelpDialog.cpp \
    src/utils/IDataSeriesGenerator.cpp \
    src/utils/OptionsDialog.cpp

HEADERS                 += \
    src/bluetooth/BtDiscoverRemoteDevice.hpp \
    src/bluetooth/BtDiscoverRemoteService.hpp \
    src/bluetooth/BtLocalDevicesManager.hpp \
    src/bluetooth/BtTypes.hpp \
    src/bluetooth/SPX42BtDevicesManager.hpp \
    src/bluetooth/SPX42RemotBtDevice.hpp \
    src/config/AppConfigClass.hpp \
    src/config/CurrBuildDef.hpp \
    src/config/ProjectConst.hpp \
    src/ControlMain.hpp \
    src/database/ChartDataWorker.hpp \
    src/database/LogDetailWalker.hpp \
    src/database/SPX42Database.hpp \
    src/database/spx42databaseconstants.hpp \
    src/guiFragments/ChartsFragment.hpp \
    src/guiFragments/ConnectFragment.hpp \
    src/guiFragments/DeviceConfigFragment.hpp \
    src/guiFragments/DeviceInfoFragment.hpp \
    src/guiFragments/GasFragment.hpp \
    src/guiFragments/IFragmentInterface.hpp \
    src/guiFragments/LogFragment.hpp \
    src/logging/Logger.hpp \
    src/spx42/SPX42CommandDef.hpp \
    src/spx42/SPX42Commands.hpp \
    src/spx42/SPX42Config.hpp \
    src/spx42/SPX42Defs.hpp \
    src/spx42/SPX42Gas.hpp \
    src/spx42/SPX42LogDirectoryEntry.hpp \
    src/spx42/SPX42SingleCommand.hpp \
    src/SPX42ControlMainWin.hpp \
    src/uddf/spx42uddfexport.hpp \
    src/utils/AboutDialog.hpp \
    src/utils/DebugDataSeriesGenerator.hpp \
    src/utils/DiveDataSeriesGenerator.hpp \
    src/utils/HelpDialog.hpp \
    src/utils/IDataSeriesGenerator.hpp \
    src/utils/OptionsDialog.hpp

FORMS                   += \
    src/ui/SPX42ControlMainWin.ui \
    src/ui/AboutDialog.ui \
    src/ui/GasFragment.ui \
    src/ui/LogFragment.ui \
    src/ui/DeviceConfigFragment.ui \
    src/ui/ChartsFragment.ui \
    src/ui/ConnectFragment.ui \
    src/ui/HelpDialog.ui \
    src/ui/DeviceInfoFragment.ui \
    src/ui/OptionsDialog.ui

RESOURCES               = \
    src/res/SPX42ControlRes.qrc

TRANSLATIONS            = \
    src/translations/SPX42Control_de_DE.ts

DISTFILES               += \
    src/res/spx42Control.css \
    src/res/no_working.gif \
    src/res/working.gif \
    src/res/ic_bluetooth_black_24dp_1x.png \
    src/res/ic_bluetooth_black_24dp_2x.png \
    src/res/DejaVuSansMono.ttf \
    src/res/Hack-Bold.ttf \
    src/res/Hack-BoldItalic.ttf \
    src/res/Hack-Italic.ttf \
    src/res/Hack-Regular.ttf \
    src/res/spx42Control.css \
    src/res/spx42ControlMac.css \
    src/res/TlwgTypewriter.ttf \
    src/res/bahnschrift.ttf \
    tools/makeDeliverBuild_osx.sh \
    tools/makeDeliverBuild_win.cmd \
    tools/makeBuildTemplate.py \
    tools/setAppVersion.py


###############################################################################
#### Betriebssystemspezifische Sachen                                      ####
###############################################################################

win32 {
SOURCES                 +=

HEADERS                 += \

INCLUDEPATH             += \

LIBS                    += \

}

unix {
SOURCES                 += \

HEADERS                 += \

INCLUDEPATH             += \

}

macx {
SOURCES                 += \

HEADERS                 += \

INCLUDEPATH             += \


}




message( app version $$VERSION kit $${KIT} )
