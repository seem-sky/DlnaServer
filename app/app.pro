QT += qml quick widgets network xml sql webview

TARGET = QT_Media_Server
TEMPLATE = app

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

include (../network/network.pri)
include (../../QmlApplication/QmlApplication.prf)
include (../../Upnp/UpnpLibrary/UpnpLibrary.prf)

HEADERS += \
    myapplication.h \
    applicationcontroller.h \
    debugitem.h \
    createdatabasethread.h \
    checknetworklinkitem.h \
    serviceconnectionmanager.h \
    servicecontentdirectory.h

SOURCES += \
    main.cpp \
    myapplication.cpp \
    applicationcontroller.cpp \
    debugitem.cpp \
    createdatabasethread.cpp \
    checknetworklinkitem.cpp \
    serviceconnectionmanager.cpp \
    servicecontentdirectory.cpp

RESOURCES += \
    ressources.qrc

mac {
    ICON = icon.icns
}

DISTFILES += \
    app.iconset/icon_512x512.png \
    app.iconset/icon_512x512@2x.png \
    app.iconset/icon_128x128.png \
    app.iconset/icon_128x128@2x.png \
    app.iconset/icon_32x32.png \
    app.iconset/icon_32x32@2x.png \
    app.iconset/icon_16x16.png \
    app.iconset/icon_16x16@2x.png \
    app.iconset/icon_256x256.png \
    app.iconset/icon_256x256@2x.png \
    icon.icns \
    UPnP_AV_ConnectionManager_1.0.xml \
    UPnP_AV_ContentDirectory_1.0.xml

