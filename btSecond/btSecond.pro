TARGET                                 = btSecond
TEMPLATE                               = app
#
QT                                     += core
QT                                     += gui
QT                                     += widgets
QT                                     += bluetooth
CONFIG                                 += stl
CONFIG                                 += c++11
DESTDIR                                = out
MOC_DIR                                = moc
RCC_DIR                                = rcc
UI_DIR                                 = ui

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main_gui.cpp \
    MainDialog.cpp \
    logging/Logger.cpp \
    bluetooth/BtServiceDiscover.cpp \
    bluetooth/SPX42BtDevices.cpp \
    bluetooth/BtDevicesManager.cpp

HEADERS += \
    MainDialog.hpp \
    logging/Logger.hpp \
    bluetooth/BtServiceDiscover.hpp \
    bluetooth/SPX42BtDevices.hpp \
    bluetooth/BtDevicesManager.hpp

FORMS += \
    ui/MainDialog.ui

target.path = exportpath
INSTALLS += target
