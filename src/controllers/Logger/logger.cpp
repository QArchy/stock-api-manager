#include "logger.h"
#include <QDateTime>
#include <QDir>

Q_LOGGING_CATEGORY(networkGetPost, "network.getpost")
Q_LOGGING_CATEGORY(networkWebSocket, "network.websocket")

QFile Logger::generalLog;
QTextStream Logger::logStream;
QMutex Logger::logMutex;
QtMessageHandler Logger::originalHandler = nullptr;

void Logger::initialize() {
    QDir logDir;

    generalLog.setFileName(logDir.filePath("application.log"));
    if(!generalLog.open(QIODevice::WriteOnly | QIODevice::Append)) qFatal() << "Log not open";
    logStream.setDevice(&generalLog);

    qSetMessagePattern("[%{time yyyy-MM-dd hh:mm:ss.zzz}]%{if-category}%{category}%{endif}\t%{type} %{message}");

    originalHandler = qInstallMessageHandler(messageHandler);
}

void Logger::shutdown() {
    if (generalLog.isOpen()) {
        logStream.flush();
        generalLog.close();
    }
    qInstallMessageHandler(originalHandler);
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QMutexLocker locker(&logMutex);
    const QString formatted = qFormatLogMessage(type, context, msg);
    logStream << formatted << Qt::endl;
    if (originalHandler) {
        originalHandler(type, context, msg);
    }
}
