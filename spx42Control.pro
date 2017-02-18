###############################################################################
####                                                                       ####
#### Projekt Controller für SPX42 via Bluetooth                            ####
#### Begonnen 05.09.2016                                                   ####
#### Autor: Dirk Marciniak                                                 ####
####                                                                       ####
###############################################################################
win32:VERSION_PE_HEADER                = 0.1
win32:QMAKE_TARGET_COMPANY             = submatix.com
win32:QMAKE_TARGET_COPYRIGHT           = D. Marciniak
win32:QMAKE_TARGET_PRODUCT             = SPX42 BT-Controller
win32:RC_ICONS                         = src/res/programIcon.ico
win32:VERSION                          = 0.1.0.0 # major.minor.patch.build
else:VERSION                           = 0.1.0    # major.minor.patch
macx:ICON                              = src/res/programIcon.ico
#
TARGET                                 = spx42Control
TEMPLATE                               = app
#
QT                                     += core
QT                                     += gui
QT                                     += widgets
unix:QT                                += bluetooth
QT                                     += sql
CONFIG                                 += stl
CONFIG                                 += c++11
DESTDIR                                = out
MOC_DIR                                = moc
RCC_DIR                                = rcc
UI_DIR                                 = ui
# wenn debug in der config steht, die EXE auch so benennen
build_pass:CONFIG(debug, debug|release) {
  unix: TARGET = $$join(TARGET,,,_debug)
  else: TARGET = $$join(TARGET,,,_D)
  DEBUG=DEBUG
}
unix:DEFINES                           += UNIX
DEFINES                                += $$DEBUG

SOURCES                                += \
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
                                          src/config/SPX42Defs.cpp

HEADERS                                += \
                                          src/Spx42ControlMainWin.hpp \
                                          src/config/AppConfigClass.hpp \
                                          src/config/ProjectConst.hpp \
                                          src/logging/Logger.hpp \
                                          src/config/CurrBuildDef.hpp \
                                          src/utils/AboutDialog.hpp \
                                          src/guiFragments/ConnectFragment.hpp \
                                          src/guiFragments/GasFragment.hpp \
                                          src/utils/SPX42Config.hpp \
                                          src/utils/SPX42Gas.hpp \
                                          src/config/SPX42Defs.hpp \
                                          src/guiFragments/IFragmentInterface.hpp \
                                          src/guiFragments/DeviceConfigFragment.hpp

FORMS                                  += \
                                          src/ui/Spx42ControlMainWin.ui \
                                          src/ui/AboutDialog.ui \
                                          src/ui/GasFragment.ui \
                                          src/ui/ConnectFragment.ui \
                                          src/ui/DeviceConfig.ui

RESOURCES                              = \
                                          src/res/Spx42ControlRes.qrc

TRANSLATIONS                           = \
                                          src/translations/Spx42Control_de_DE.ts


###############################################################################
#### Betriebssystemspezifische Sachen                                      ####
###############################################################################

win32 {
SOURCES                 += \

HEADERS                 += \
}

unix {
SOURCES                 += \

HEADERS                 += \
}

macx {
SOURCES                 += \

HEADERS                 += \

}
