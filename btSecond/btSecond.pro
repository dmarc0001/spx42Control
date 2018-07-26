TARGET                                 = btSecond
TEMPLATE                               = app
#
QT                                     += core
QT                                     += gui
QT                                     += widgets
QT                                     += bluetooth
QT                                     += sql
CONFIG                                 += stl
CONFIG                                 += c++11
DESTDIR                                = out
MOC_DIR                                = moc
RCC_DIR                                = rcc
UI_DIR                                 = ui

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG(release, debug|release) {
  DEFINES                              += QT_NO_DEBUG_OUTPUT
}


SOURCES += \
        main_gui.cpp \
    logging/Logger.cpp \
    bluetooth/BtServiceDiscover.cpp \
    bluetooth/SPX42BtDevices.cpp \
    bluetooth/BtDevicesManager.cpp \
    BtDiscoverDialog.cpp \
    database/SPX42Database.cpp

HEADERS += \
    logging/Logger.hpp \
    bluetooth/BtServiceDiscover.hpp \
    bluetooth/SPX42BtDevices.hpp \
    bluetooth/BtDevicesManager.hpp \
    BtDiscoverDialog.hpp \
    database/SPX42Database.hpp

FORMS += \
    ui/BtDiscoverDialog.ui

target.path = exportpath
INSTALLS += target

DISTFILES += \
    res/working.gif

RESOURCES += \
    res/scannerResource.qrc
