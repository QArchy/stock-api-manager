QT += core gui widgets network websockets

CONFIG += c++17

SOURCES += \
    src/controllers/Logger/logger.cpp \
    src/main.cpp \
    src/controllers/Settings/apiSettings.cpp \
    src/models/GetPostManager/getpostmanager.cpp \
    src/models/WebSocketManager/websocketmanager.cpp \
    src/ui/mainwindow/mainwindow.cpp

HEADERS += \
    src/controllers/Logger/logger.h \
    src/controllers/Settings/apiSettings.h \
    src/controllers/Translator/translator.h \
    src/models/GetPostManager/getpostmanager.h \
    src/models/WebSocketManager/websocketmanager.h \
    src/models/networkSingleton.h \
    src/ui/mainwindow/mainwindow.h

TRANSLATIONS += \
    resources/translations/BybitManager_en_US.ts

CONFIG += lrelease embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    src/ui/mainwindow/mainwindow.ui

RESOURCES += \
    resources/resources.qrc
