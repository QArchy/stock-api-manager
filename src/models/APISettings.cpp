#include "APISettings.h"

QString APISettings::ApiMainnetBaseUrl;
QString APISettings::ApiDemoBaseUrl;
QString APISettings::RecvWindow;
QString APISettings::ApiKeyDemo;
QString APISettings::ApiSecretDemo;
QString APISettings::ApiKeyTestnet1;
QString APISettings::ApiSecretTestnet1;
QString APISettings::ApiKeyTestnet2;
QString APISettings::ApiSecretTestnet2;

APISettings::APISettings(QString filepath) {
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly)) {
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
        qInfo() << "Values loaded: " << jsonObj;
    } else {
        qFatal() << "Failed to open file: " << filepath;
    }
}
