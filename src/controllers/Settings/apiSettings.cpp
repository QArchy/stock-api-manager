#include "apiSettings.h"

ApiSettings::ApiSettings() { m_bApi = new BybitApi; m_initialized = false; }

void ApiSettings::loadSettings() {
    QFile file("/home/archy/MEGA/Programming/C++/BybitManager/config/ApiSettings.json");
    if (!file.open(QIODevice::ReadOnly))
        qFatal() << "Failed to open file";

    QJsonParseError parseError;
    QByteArray arr = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(arr, &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError)
        qFatal() << "Failed to parse JSON: " << parseError.errorString();

    QJsonObject jsonObj = jsonDoc.object();
    m_bApi->httpParams.mainNetBaseUrl = jsonObj.value("ApiMainnetBaseUrl").toString();
    m_bApi->httpParams.demoNetBaseUrl = jsonObj.value("ApiDemoBaseUrl").toString();
    m_bApi->httpParams.recvWindow = jsonObj.value("RecvWindow").toString();
    m_bApi->keys.apiKeyDemo = jsonObj.value("ApiKeyDemo").toString();
    m_bApi->keys.apiSecretDemo = jsonObj.value("ApiSecretDemo").toString();
    m_bApi->keys.apiKeyTestnet = jsonObj.value("ApiKeyTestnet").toString();
    m_bApi->keys.apiSecretTestnet = jsonObj.value("ApiSecretTestnet").toString();
    m_bApi->wsParams.publicUrl = jsonObj.value("PublicWsUrl").toString();
    m_bApi->wsParams.privateUrl = jsonObj.value("PrivateWsUrl").toString();

    qInfo() << "All settings loaded successfully";
    m_initialized = true;
}

BybitApi* ApiSettings::getbApi() { return m_bApi; }
