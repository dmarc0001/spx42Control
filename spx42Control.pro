###############################################################################
####                                                                       ####
#### Projekt Controller für SPX42 via Bluetooth                            ####
#### Begonnen 05.09.2016                                                   ####
#### Autor: Dirk Marciniak                                                 ####
####                                                                       ####
###############################################################################
win32:VERSION_PE_HEADER                = 0.2
win32:QMAKE_TARGET_COMPANY             = submatix.com
win32:QMAKE_TARGET_COPYRIGHT           = D. Marciniak
win32:QMAKE_TARGET_PRODUCT             = SPX42 BT-Controller
win32:RC_ICONS                         = src/res/programIcon.ico
win32:VERSION                          = 0.2.0.0 # major.minor.patch.build
else:VERSION                           = 0.2.0    # major.minor.patch
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
CONFIG                                 += stl
CONFIG                                 += c++11
INCLUDEPATH                            += src
DESTDIR                                = out
MOC_DIR                                = moc
RCC_DIR                                = rcc
UI_DIR                                 = ui


# wenn debug in der config steht, die EXE auch so benennen
build_pass:CONFIG(debug, debug|release) {
  unix: TARGET = $$join(TARGET,,,_debug)
  else: TARGET = $$join(TARGET,,,_D)
}
DEFINES                 += $$DEBUG

SOURCES                 += \
    src/Spx42ControlMainWin.cpp \
    src/ControlMain.cpp \
    src/config/AppConfigClass.cpp \
    src/config/ProjectConst.cpp \
    src/logging/Logger.cpp \
    src/utils/AboutDialog.cpp \
    src/guiFragments/ConnectFragment.cpp \
    src/guiFragments/GasFragment.cpp \
    src/utils/SPX42Config.cpp \
    src/utils/SPX42Gas.cpp \
    src/guiFragments/IFragmentInterface.cpp \
    src/guiFragments/DeviceConfigFragment.cpp \
    src/config/SPX42Defs.cpp \
    src/guiFragments/LogFragment.cpp \
    src/utils/IDataSeriesGenerator.cpp \
    src/utils/DebugDataSeriesGenerator.cpp \
    src/utils/DiveDataSeriesGenerator.cpp \
    src/guiFragments/ChartsFragment.cpp \
    src/bluetooth/PinDialog.cpp \
    src/spx42/Spx42Commands.cpp \
    src/spx42/SpxCommandDef.cpp \
    src/bluetooth/BtDevicesManager.cpp \
    src/bluetooth/BtServiceDiscover.cpp \
    src/bluetooth/SPX42BtDevices.cpp \
    src/bluetooth/BtDiscoverDialog.cpp \
    src/database/SPX42Database.cpp

HEADERS                 += \
    src/Spx42ControlMainWin.hpp \
    src/config/AppConfigClass.hpp \
    src/config/ProjectConst.hpp \
    src/logging/Logger.hpp \
    src/utils/AboutDialog.hpp \
    src/guiFragments/ConnectFragment.hpp \
    src/guiFragments/GasFragment.hpp \
    src/utils/SPX42Config.hpp \
    src/utils/SPX42Gas.hpp \
    src/config/SPX42Defs.hpp \
    src/guiFragments/IFragmentInterface.hpp \
    src/guiFragments/DeviceConfigFragment.hpp \
    src/guiFragments/LogFragment.hpp \
    src/ControlMain.hpp \
    src/utils/IDataSeriesGenerator.hpp \
    src/utils/DebugDataSeriesGenerator.hpp \
    src/utils/DiveDataSeriesGenerator.hpp \
    src/guiFragments/ChartsFragment.hpp \
    src/bluetooth/PinDialog.hpp \
    src/spx42/Spx42Commands.hpp \
    src/spx42/SpxCommandDef.hpp \
    src/bluetooth/BtDevicesManager.hpp \
    src/bluetooth/BtServiceDiscover.hpp \
    src/bluetooth/SPX42BtDevices.hpp \
    src/bluetooth/BtDiscoverDialog.hpp \
    src/database/SPX42Database.hpp \
    src/config/currBuildDef.hpp

FORMS                   += \
    src/ui/Spx42ControlMainWin.ui \
    src/ui/AboutDialog.ui \
    src/ui/GasFragment.ui \
    src/ui/ConnectFragment.ui \
    src/ui/LogFragment.ui \
    src/ui/DeviceConfigFragment.ui \
    src/ui/ChartsFragment.ui \
    src/ui/PinDialog.ui \
    src/ui/BtDiscoverDialog.ui

RESOURCES               = \
    src/res/Spx42ControlRes.qrc

TRANSLATIONS            = \
    src/translations/Spx42Control_de_DE.ts

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
    src/res/spx42Control.css


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

