#include "apiSettings.h"

ApiSettings::ApiSettings() { bApi = new BybitApi; initialized = false; }

void ApiSettings::loadSettings() {
    QFile file("/home/archy/MEGA/Programming/C++/BybitManager/config/ApiSettings.json");
    if (!file.open(QIODevice::ReadOnly))
        qFatal() << "Failed to open file";

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError)
        qFatal() << "Failed to parse JSON: " << parseError.errorString();

    QJsonObject jsonObj = jsonDoc.object();
    bApi->gpParams.mainNetBaseUrl = jsonObj.value("ApiMainnetBaseUrl").toString();
    bApi->gpParams.demoNetBaseUrl = jsonObj.value("ApiDemoBaseUrl").toString();
    bApi->gpParams.recvWindow = jsonObj.value("RecvWindow").toString();
    bApi->keys.apiKeyDemo = jsonObj.value("ApiKeyDemo").toString();
    bApi->keys.apiSecretDemo = jsonObj.value("ApiSecretDemo").toString();
    bApi->keys.apiKeyTestnet = jsonObj.value("ApiKeyTestnet").toString();
    bApi->keys.apiSecretTestnet = jsonObj.value("ApiSecretTestnet").toString();
    bApi->wsParams.publicUrl = jsonObj.value("PublicWsUrl").toString();
    bApi->wsParams.privateUrl = jsonObj.value("PrivateWsUrl").toString();

    qInfo() << "All settings loaded successfully";
    initialized = true;
}

BybitApi* ApiSettings::getbApi() { return bApi; }
