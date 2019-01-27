###############################################################################
####                                                                       ####
#### Projekt Controller für SPX42 via Bluetooth                            ####
#### Begonnen 05.09.2016                                                   ####
#### Autor: Dirk Marciniak                                                 ####
####                                                                       ####
###############################################################################
MAJOR                                  = 0 # Major Verion Nummer
MINOR                                  = 5 # Minor Version Number
PATCH                                  = 5 # Patch Version Number
BUILD                                  = 0 # win build number

win32:VERSION_PE_HEADER                = $${MAJOR}.$${MINOR}
win32:QMAKE_TARGET_COMPANY             = submatix.com
win32:QMAKE_TARGET_COPYRIGHT           = D. Marciniak
win32:QMAKE_TARGET_PRODUCT             = SPX42 BT-Controller
win32:RC_ICONS                         = src/res/programIcon.ico
<<<<<<< HEAD
<<<<<<< HEAD
win32:VERSION                          = 0.5.2.0  # major.minor.patch.build
else:VERSION                           = 0.5.2    # major.minor.patch
=======
win32:VERSION                          = 0.5.4.0  # major.minor.patch.build
else:VERSION                           = 0.5.4    # major.minor.patch
>>>>>>> dev/stabilizing
=======
win32:VERSION                          = $${MAJOR}.$${MINOR}.$${PATCH}.$${BUILD} # major.minor.patch.build
else:VERSION                           = $${MAJOR}.$${MINOR}.$${PATCH}    # major.minor.patch
>>>>>>> dev/stabilizing
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
    src/SPX42ControlMainWin.cpp \
    src/ControlMain.cpp \
    src/config/AppConfigClass.cpp \
    src/config/ProjectConst.cpp \
    src/logging/Logger.cpp \
    src/utils/AboutDialog.cpp \
    src/guiFragments/ConnectFragment.cpp \
    src/guiFragments/GasFragment.cpp \
    src/spx42/SPX42Config.cpp \
    src/spx42/SPX42Gas.cpp \
    src/guiFragments/IFragmentInterface.cpp \
    src/guiFragments/DeviceConfigFragment.cpp \
    src/spx42/SPX42Defs.cpp \
    src/guiFragments/LogFragment.cpp \
    src/utils/IDataSeriesGenerator.cpp \
    src/utils/DebugDataSeriesGenerator.cpp \
    src/utils/DiveDataSeriesGenerator.cpp \
    src/guiFragments/ChartsFragment.cpp \
    src/database/SPX42Database.cpp \
    src/bluetooth/SPX42RemotBtDevice.cpp \
    src/spx42/SPX42Commands.cpp \
    src/bluetooth/BtLocalDevicesManager.cpp \
    src/bluetooth/BtDiscoverRemoteDevice.cpp \
    src/bluetooth/BtDiscoverRemoteService.cpp \
    src/bluetooth/SPX42BtDevicesManager.cpp \
    src/spx42/SPX42SingleCommand.cpp \
    src/utils/HelpDialog.cpp \
    src/guiFragments/DeviceInfoFragment.cpp \
    src/spx42/SPX42LogDirectoryEntry.cpp \
    src/database/LogDetailWalker.cpp \
    src/charts/DiveMiniChart.cpp \
<<<<<<< HEAD
    src/database/ChartDataWorker.cpp
=======
    src/database/ChartDataWorker.cpp \
<<<<<<< HEAD
    src/database/spx42databaseconstants.cpp
>>>>>>> dev/stabilizing
=======
    src/database/spx42databaseconstants.cpp \
    src/uddf/spx42uddfexport.cpp
>>>>>>> dev/stabilizing

HEADERS                 += \
    src/SPX42ControlMainWin.hpp \
    src/config/AppConfigClass.hpp \
    src/config/ProjectConst.hpp \
    src/logging/Logger.hpp \
    src/utils/AboutDialog.hpp \
    src/guiFragments/ConnectFragment.hpp \
    src/guiFragments/GasFragment.hpp \
    src/spx42/SPX42Config.hpp \
    src/spx42/SPX42Gas.hpp \
    src/spx42/SPX42Defs.hpp \
    src/guiFragments/IFragmentInterface.hpp \
    src/guiFragments/DeviceConfigFragment.hpp \
    src/guiFragments/LogFragment.hpp \
    src/ControlMain.hpp \
    src/utils/IDataSeriesGenerator.hpp \
    src/utils/DebugDataSeriesGenerator.hpp \
    src/utils/DiveDataSeriesGenerator.hpp \
    src/guiFragments/ChartsFragment.hpp \
    src/spx42/SPX42Commands.hpp \
    src/database/SPX42Database.hpp \
    src/bluetooth/SPX42RemotBtDevice.hpp \
    src/bluetooth/BtTypes.hpp \
    src/spx42/SPX42CommandDef.hpp \
    src/bluetooth/BtLocalDevicesManager.hpp \
    src/bluetooth/BtDiscoverRemoteDevice.hpp \
    src/bluetooth/BtDiscoverRemoteService.hpp \
    src/bluetooth/SPX42BtDevicesManager.hpp \
    src/spx42/SPX42SingleCommand.hpp \
    src/utils/HelpDialog.hpp \
    src/guiFragments/DeviceInfoFragment.hpp \
    src/config/CurrBuildDef.hpp \
    src/spx42/SPX42LogDirectoryEntry.hpp \
    src/database/LogDetailWalker.hpp \
    src/charts/DiveMiniChart.hpp \
<<<<<<< HEAD
    src/database/ChartDataWorker.hpp
=======
    src/database/ChartDataWorker.hpp \
<<<<<<< HEAD
    src/database/spx42databaseconstants.hpp
>>>>>>> dev/stabilizing
=======
    src/database/spx42databaseconstants.hpp \
    src/uddf/spx42uddfexport.hpp
>>>>>>> dev/stabilizing

FORMS                   += \
    src/ui/SPX42ControlMainWin.ui \
    src/ui/AboutDialog.ui \
    src/ui/GasFragment.ui \
    src/ui/LogFragment.ui \
    src/ui/DeviceConfigFragment.ui \
    src/ui/ChartsFragment.ui \
    src/ui/ConnectFragment.ui \
    src/ui/HelpDialog.ui \
    src/ui/DeviceInfoFragment.ui

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
    src/res/TlwgTypewriter.ttf


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
