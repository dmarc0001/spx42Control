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
}
unix:DEFINES                += UNIX

SOURCES                                += \
                                          src/spx42ControlMainWin.cpp \
                                          src/controlMain.cpp \
                                          src/config/AppConfigClass.cpp \
                                          src/config/ProjectConst.cpp \
                                          src/logging/Logger.cpp \
                                          src/utils/aboutDialog.cpp \
                                          src/guiFragments/connectFragment.cpp \
                                          src/guiFragments/gasFragment.cpp \
                                          src/utils/SPX42Config.cpp \
                                          src/utils/SPX42Gas.cpp

HEADERS                                += \
                                          src/spx42ControlMainWin.hpp \
                                          src/config/AppConfigClass.hpp \
                                          src/config/ProjectConst.hpp \
                                          src/logging/Logger.hpp \
                                          src/config/currBuildDef.hpp \
                                          src/utils/aboutDialog.hpp \
                                          src/guiFragments/connectFragment.hpp \
                                          src/guiFragments/gasFragment.hpp \
                                          src/utils/SPX42Config.hpp \
                                          src/utils/SPX42Gas.hpp \
                                          src/utils/SPX42Defs.hpp

FORMS                                  += \
                                          src/ui/spx42controlmainwin.ui \
                                          src/ui/aboutdialog.ui \
                                          src/ui/gasFragment.ui \
                                          src/ui/connectFragment.ui

RESOURCES                              = \
                                          src/res/spx42ControlRes.qrc

TRANSLATIONS                           = \
                                          src/translations/spx42Control_de_DE.ts


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
