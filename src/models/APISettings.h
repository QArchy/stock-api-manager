#ifndef APISETTINGS_H
#define APISETTINGS_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

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

    APISettings(QString filepath);
};


#endif // APISETTINGS_H
