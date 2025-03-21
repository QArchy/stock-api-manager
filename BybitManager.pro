QT += core gui widgets network websockets

CONFIG += c++17

SOURCES += \
    src/controllers/Logger/logger.cpp \
    src/main.cpp \
    src/controllers/Settings/apiSettings.cpp \
    src/network/HttpManager/httpmanager.cpp \
    src/network/WebSocketManager/websocketmanager.cpp \
    src/tests/loggerTest/loggerworker.cpp \
    src/ui/mainwindow/mainwindow.cpp

HEADERS += \
    src/controllers/Logger/logger.h \
    src/controllers/Settings/apiSettings.h \
    src/controllers/Translator/translator.h \
    src/network/HttpManager/httpmanager.h \
    src/network/WebSocketManager/websocketmanager.h \
    src/network/nam.h \
    src/tests.h \
    src/tests/loggerTest/loggerTest.h \
    src/tests/loggerTest/loggerworker.h \
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
