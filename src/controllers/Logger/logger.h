#ifndef LOGGER_H
#define LOGGER_H

#include <QLoggingCategory>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QStandardPaths>

Q_DECLARE_LOGGING_CATEGORY(networkGetPost)
Q_DECLARE_LOGGING_CATEGORY(networkWebSocket)

class Logger {
public:
    static void initialize();
    static void shutdown();

private:
    static QFile generalLog;
    static QTextStream logStream;
    static QMutex logMutex;
    static QtMessageHandler originalHandler;

    static void messageHandler(QtMsgType type,
                               const QMessageLogContext &context,
                               const QString &msg);
};

#endif // LOGGER_H
