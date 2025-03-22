QT += core gui widgets network websockets

CONFIG += c++17

SOURCES += \
    src/controllers/Logger/logger.cpp \
    src/dao/OrderBook/orderbook.cpp \
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
    src/dao/OrderBook/orderbook.h \
    src/libs/Eigen/Cholesky \
    src/libs/Eigen/Cholmodsupport \
    src/libs/Eigen/Core \
    src/libs/Eigen/Dense \
    src/libs/Eigen/Eigen \
    src/libs/Eigen/Eigenvalues \
    src/libs/Eigen/Geometry \
    src/libs/Eigen/Householder \
    src/libs/Eigen/Iterativelinearsolvers \
    src/libs/Eigen/Jacobi \
    src/libs/Eigen/Klusupport \
    src/libs/Eigen/LU \
    src/libs/Eigen/Metissupport \
    src/libs/Eigen/Orderingmethods \
    src/libs/Eigen/Pardisosupport \
    src/libs/Eigen/Pastixsupport \
    src/libs/Eigen/QR \
    src/libs/Eigen/Qtalignedmalloc \
    src/libs/Eigen/SVD \
    src/libs/Eigen/Sparse \
    src/libs/Eigen/Sparsecholesky \
    src/libs/Eigen/Sparsecore \
    src/libs/Eigen/Sparselu \
    src/libs/Eigen/Sparseqr \
    src/libs/Eigen/Spqrsupport \
    src/libs/Eigen/Stddeque \
    src/libs/Eigen/Stdlist \
    src/libs/Eigen/Stdvector \
    src/libs/Eigen/Superlusupport \
    src/libs/Eigen/Umfpacksupport \
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
