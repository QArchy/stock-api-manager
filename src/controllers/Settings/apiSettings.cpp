#include "apiSettings.h"

QString APISettings::apiMainnetBaseUrl;
QString APISettings::apiDemoBaseUrl;
QString APISettings::recvWindow;
QString APISettings::apiKeyDemo;
QString APISettings::apiSecretDemo;
QString APISettings::apiKeyTestnet1;
QString APISettings::apiSecretTestnet1;
QString APISettings::apiKeyTestnet2;
QString APISettings::apiSecretTestnet2;

void APISettings::load(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly))
        qFatal() << "Failed to open file: " << qPrintable(filepath);

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError)
        qFatal() << "Failed to parse JSON: " << qPrintable(parseError.errorString());

    QJsonObject jsonObj = jsonDoc.object();
    apiMainnetBaseUrl = jsonObj.value("ApiMainnetBaseUrl").toString();
    apiDemoBaseUrl = jsonObj.value("ApiDemoBaseUrl").toString();
    recvWindow = jsonObj.value("RecvWindow").toString();
    apiKeyDemo = jsonObj.value("ApiKeyDemo").toString();
    apiSecretDemo = jsonObj.value("ApiSecretDemo").toString();
    apiKeyTestnet1 = jsonObj.value("ApiKeyTestnet1").toString();
    apiSecretTestnet1 = jsonObj.value("ApiSecretTestnet1").toString();
    apiKeyTestnet2 = jsonObj.value("ApiKeyTestnet2").toString();
    apiSecretTestnet2 = jsonObj.value("ApiSecretTestnet2").toString();

    qInfo() << "All settings loaded successfully.";
}

QString APISettings::getApiMainnetBaseUrl() { return apiMainnetBaseUrl; }
QString APISettings::getApiDemoBaseUrl() { return apiDemoBaseUrl; }
QString APISettings::getRecvWindow() { return recvWindow; }
QString APISettings::getApiKeyDemo() { return apiKeyDemo; }
QString APISettings::getApiSecretDemo() { return apiSecretDemo; }
QString APISettings::getApiKeyTestnet1() { return apiKeyTestnet1; }
QString APISettings::getApiSecretTestnet1() { return apiSecretTestnet1; }
QString APISettings::getApiKeyTestnet2() { return apiKeyTestnet2; }
QString APISettings::getApiSecretTestnet2() { return apiSecretTestnet2; }
