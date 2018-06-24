TARGET                                 = btSecondConsole
TEMPLATE                               = app
#
QT                                     += core
#QT                                     += console
QT                                     -= gui
QT                                     -= widgets
QT                                     += bluetooth
CONFIG                                 += stl
CONFIG                                 += c++11
CONFIG                                 += app_bundle
DESTDIR                                = out
MOC_DIR                                = moc
RCC_DIR                                = rcc

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main_console.cpp \
    bluetooth/BtLocalDevicesManager.cpp \
    bluetooth/BtServiceDiscover.cpp \
    bluetooth/SPX42BtDevices.cpp \
    logging/Logger.cpp \
    ConsoleMainObject.cpp

target.path = exportpath
INSTALLS += target

HEADERS += \
    bluetooth/BtLocalDevicesManager.hpp \
    bluetooth/BtServiceDiscover.hpp \
    bluetooth/SPX42BtDevices.hpp \
    logging/Logger.hpp \
    ConsoleMainObject.hpp
