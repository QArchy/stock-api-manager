#ifndef NETWORKSINGLETON_H
#define NETWORKSINGLETON_H

#include <QNetworkAccessManager>

class Network {
public:
    static Network& getInstance() {
        static Network instance;
        return instance;
    }
    QNetworkAccessManager* getManager() { return manager; }
private:
    QNetworkAccessManager* manager;
    Network() : manager{new QNetworkAccessManager()} {}
    ~Network() { delete manager; }
    Network(const Network&) = delete;
    Network& operator=(const Network&) = delete;
};

#endif // NETWORKSINGLETON_H
