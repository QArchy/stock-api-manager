#ifndef APISETTINGS_H
#define APISETTINGS_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

class APISettings {
public:
    static QString ApiMainnetBaseUrl;
    static QString ApiDemoBaseUrl;
    static QString RecvWindow;
    static QString ApiKeyDemo;
    static QString ApiSecretDemo;
    static QString ApiKeyTestnet1;
    static QString ApiSecretTestnet1;
    static QString ApiKeyTestnet2;
    static QString ApiSecretTestnet2;

    APISettings() {
        QFile file(":/json/ApiSettings.json");

        file.open(QIODevice::ReadOnly);
        QJsonObject jsonObj = QJsonDocument::fromJson(file.readAll()).object();
        file.close();

        this->ApiMainnetBaseUrl = jsonObj.value("ApiMainnetBaseUrl").toString();
        this->ApiDemoBaseUrl = jsonObj.value("ApiDemoBaseUrl").toString();
        this->RecvWindow = jsonObj.value("RecvWindow").toString();
        this->ApiKeyDemo = jsonObj.value("ApiKeyDemo").toString();
        this->ApiSecretDemo = jsonObj.value("ApiSecretDemo").toString();
        this->ApiKeyTestnet1 = jsonObj.value("ApiKeyTestnet1").toString();
        this->ApiSecretTestnet1 = jsonObj.value("ApiSecretTestnet1").toString();
        this->ApiKeyTestnet2 = jsonObj.value("ApiKeyTestnet2").toString();
        this->ApiSecretTestnet2 = jsonObj.value("ApiSecretTestnet2").toString();
    }
};

#endif // APISETTINGS_H
