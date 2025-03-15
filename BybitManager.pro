QT += core gui widgets network

CONFIG += c++17

SOURCES += \
    src/main.cpp \
    src/models/APISettings.cpp \
    src/ui/mainwindow/mainwindow.cpp

HEADERS += \
    src/models/APISettings.h \
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
