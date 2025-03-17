#ifndef APISETTINGS_H
#define APISETTINGS_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

struct WebSocketsParameters {
    QString publicUrl;
    QString privateUrl;
};

struct GetPostParameters {
    QString mainNetBaseUrl;
    QString testNetBaseUrl;
    QString demoNetBaseUrl;
    QString recvWindow;
};

struct Keys {
    QString apiKeyDemo;
    QString apiSecretDemo;
    QString apiKeyTestnet;
    QString apiSecretTestnet;
};

struct BybitApi {
    WebSocketsParameters wsParams;
    GetPostParameters gpParams;
    Keys keys;
};

class ApiSettings {
public:
    static ApiSettings& getInstance() {
        static ApiSettings instance;
        if (!instance.initialized) instance.loadSettings();
        return instance;
    }
    BybitApi* getbApi();
private:
    ApiSettings();
    ApiSettings(const ApiSettings&) = delete;
    ApiSettings& operator=(const ApiSettings&) = delete;
    ~ApiSettings() = default;
    void loadSettings();

    bool initialized;
    BybitApi* bApi;
};

#endif // APISETTINGS_H
