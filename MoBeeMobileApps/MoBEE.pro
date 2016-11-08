TEMPLATE = app
TARGET = MoBEE
QT += network \
    xml \
    webkit
INCLUDEPATH += ../../src/multimedia \
    ../../src/systeminfo
include(../examples.pri)
CONFIG += mobility
MOBILITY = multimedia \
    location \
    bearer \
    systeminfo
CONFIG += console
RESOURCES = resources.qrc
HEADERS = nearestservices.h \
    currentpositiononthemap.h \
    sym_iap_util.h \
    CurrentMap.h \
    PictureItem.h \
    WindowMap.h \
    GoogleMapLoader.h \
    XMLParser.h \
    MoBEE.h \
    settings.h \
    ../satellitedialog/satellitedialog.h
SOURCES = nearestservices.cpp \
    currentpositiononthemap.cpp \
    CurrentMap.cpp \
    PictureItem.cpp \
    WindowMap.cpp \
    GoogleMapLoader.cpp \
    XMLParser.cpp \
    main.cpp \
    MoBEE.cpp \
    settings.cpp \
    ../satellitedialog/satellitedialog.cpp
symbian: { 
    TARGET.CAPABILITY = NetworkServices \
        Location \
        ReadUserData \
        WriteUserData \
        LocalServices \
        UserEnvironment \
        ReadDeviceData \
        WriteDeviceData \
        TrustedUI
    FORMS += nearestservices.ui \
        currentpositiononthemap.ui \
        MoBEE.ui \
        settings_s60.ui
    LIBS += -lesock \
        -linsock
    TARGET.EPOCHEAPSIZE = 0x20000 \
        0x2000000
}
else:FORMS += MoBEE.ui \
    settings.ui
