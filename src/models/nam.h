#ifndef NETWORKSINGLETON_H
#define NETWORKSINGLETON_H

#include <QNetworkAccessManager>

class NAM {
public:
    static NAM& getInstance() {
        static NAM instance;
        return instance;
    }
    QNetworkAccessManager* getManager() { return manager; }
private:
    QNetworkAccessManager* manager;
    NAM() : manager{new QNetworkAccessManager()} {}
    ~NAM() { delete manager; }
    NAM(const NAM&) = delete;
    NAM& operator=(const NAM&) = delete;
};

#endif // NETWORKSINGLETON_H
