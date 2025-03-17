#include "logger.h"

#ifdef QT_DEBUG
QString Logger::msgPattern = "["
    "%{time yyyyMMdd h:mm:ss.zzz t}"
    "%{if-debug}D%{endif}"
    "%{if-info}I%{endif}"
    "%{if-warning}W%{endif}"
    "%{if-critical}C%{endif}"
    "%{if-fatal}F%{endif}"
    "] %{category}\t%{file}:%{line} - %{message}";
#else
QString Logger::msgPattern = "["
    "%{time yyyyMMdd h:mm:ss.zzz t}"
    "%{if-debug}D%{endif}"
    "%{if-info}I%{endif}"
    "%{if-warning}W%{endif}"
    "%{if-critical}C%{endif}"
    "%{if-fatal}F%{endif}"
    "] %{category}\t- %{message}";
#endif
FILE* Logger::logGeneral = nullptr;
FILE* Logger::logGetPost = nullptr;
FILE* Logger::logWebSocket = nullptr;
QtMessageHandler Logger::originalHandler = nullptr;

QMutex Logger::generalMutex;
QMutex Logger::getpostMutex;
QMutex Logger::websocketMutex;

Q_LOGGING_CATEGORY(getpost, "getpost")
Q_LOGGING_CATEGORY(websocket, "websocket")


void Logger::initializeLogFiles() {
    QDir logDir(QDir::currentPath() + QDir::separator() + "logFiles");
    if (!logDir.exists()) logDir.mkpath(".");
    logGeneral = fopen(logDir.absoluteFilePath("logGeneral.txt").toUtf8().constData(), "a");
    logGetPost = fopen(logDir.absoluteFilePath("logGetPost.txt").toUtf8().constData(), "a");
    logWebSocket = fopen(logDir.absoluteFilePath("logWebSocket.txt").toUtf8().constData(), "a");
}

void Logger::logToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QString message = qFormatLogMessage(type, context, msg);
    QString category = QString::fromUtf8(context.category);

    if (category == "getpost") {
        QMutexLocker lock(&getpostMutex);
        if (logGetPost) {
            fprintf(logGetPost, "%s\n", qPrintable(message));
            fflush(logGetPost);
        }
    } else if (category == "websocket") {
        QMutexLocker lock(&websocketMutex);
        if (logWebSocket) {
            fprintf(logWebSocket, "%s\n", qPrintable(message));
            fflush(logWebSocket);
        }
    }

    {
        QMutexLocker lock(&generalMutex);
        if (logGeneral) {
            fprintf(logGeneral, "%s\n", qPrintable(message));
            fflush(logGeneral);
        }
    }

    if (originalHandler) {
        (*originalHandler)(type, context, msg);
    }
}

void Logger::install() {
    initializeLogFiles();
    qSetMessagePattern(msgPattern);
    originalHandler = qInstallMessageHandler(logToFile);

    qDebug() << "New session";
    qDebug(getpost) << "New session";
    qDebug(websocket) << "New session";
}
