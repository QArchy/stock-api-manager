#ifndef LOGGERTEST_H
#define LOGGERTEST_H

#include "src/tests/loggerTest/loggerworker.h"
#include <QThread>

void loggerTest() {
    QThread* thread[5];
    LoggerWorker* worker[5];
    for (int i = 0; i < 5; i++) {
        thread[i] = new QThread();
        worker[i] = new LoggerWorker(i);
        worker[i]->moveToThread(thread[i]);
        QObject::connect(thread[i], &QThread::started, worker[i], &LoggerWorker::doWork);
        thread[i]->start();
    }
}

#endif // LOGGERTEST_H
