#ifndef NETWORKSINGLETON_H
#define NETWORKSINGLETON_H

#include <QNetworkAccessManager>

class NAM {
public:
    static NAM& getInstance() {
        static NAM instance;
        return instance;
    }
    QNetworkAccessManager* getManager() { return m_manager; }
private:
    QNetworkAccessManager* m_manager;
    NAM() : m_manager{new QNetworkAccessManager()} {}
    ~NAM() { delete m_manager; }
    NAM(const NAM&) = delete;
    NAM& operator=(const NAM&) = delete;
};

#endif // NETWORKSINGLETON_H
