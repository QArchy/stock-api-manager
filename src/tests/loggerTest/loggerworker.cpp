#include "loggerworker.h"
#include "src/controllers/Logger/logger.h"
#include <QDebug>
LoggerWorker::LoggerWorker(int threadNum, QObject *parent) : QObject(parent), threadNum(threadNum) {}

void LoggerWorker::doWork() {
    for (int i = 0; i < 100; i++) {
        qDebug(networkGetPost) << "Thread #" << threadNum << ": debug #" << i;
        qInfo(networkGetPost) << "Thread #" << threadNum << ": debug #" << i;
        qWarning(networkWebSocket) << "Thread #" << threadNum << ": debug #" << i;
        qCritical(networkWebSocket) << "Thread #" << threadNum << ": debug #" << i;
    }
    emit finished();
}
