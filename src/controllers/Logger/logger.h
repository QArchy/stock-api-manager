#ifndef LOGGER_H
#define LOGGER_H

/*******************************************************************************************/
/*                      To create new logging categories, do:                              */
/*                                                                                         */
/* 1. use Q_DECLARE_LOGGING_CATEGORY(categoryName) macro in header file.                   */
/* 2. use Q_LOGGING_CATEGORY(categoryName, "categoryName") macro in a source file.         */
/* 3. add static FILE *log<categoryName> variable in the Logger class.                     */
/* 4. initialize it like FILE * Logger::log<categoryName> = nullptr; in the source file.   */
/* 5. add static QMutex <categoryName>Mutex variable in the Logger class.                  */
/* 6. initialize it like QMutex <categoryName>Mutex; in the source file.                   */
/* 7. initialize it in initializeLogFiles() function.                                      */
/* 8. make changes in Logger::logToFile function                                           */
/* 9. make changes in Logger::install function                                             */
/*                                                                                         */
/*******************************************************************************************/

#include <QtMessageHandler>
#include <QLoggingCategory>
#include <QString>
#include <QDir>
#include <QMutex>
#include <stdio.h>
#include <stdlib.h>

Q_DECLARE_LOGGING_CATEGORY(getpost)
Q_DECLARE_LOGGING_CATEGORY(websocket)

class Logger {
private:
    static QString msgPattern;
    static FILE *logGeneral;
    static FILE *logGetPost;
    static FILE *logWebSocket;
    static QtMessageHandler originalHandler;

    static QMutex generalMutex;
    static QMutex getpostMutex;
    static QMutex websocketMutex;

    static void initializeLogFiles();
    static void logToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg);

public:
    static void install();
};

#endif // LOGGER_H
