#ifndef LOGGERWORKER_H
#define LOGGERWORKER_H

#include <QObject>

class LoggerWorker : public QObject {
    Q_OBJECT

    int threadNum;

public:
    explicit LoggerWorker(int threadNum, QObject* parent = nullptr);

public slots:
    void doWork();

signals:
    void finished();
};

#endif // LOGGERWORKER_H
