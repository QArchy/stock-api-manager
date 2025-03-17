#ifndef APISETTINGS_H
#define APISETTINGS_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

/*******************************************************************************************/
/*                                  To add new setting, do:                                */
/*                                                                                         */
/* 1. add private static QString variableName in the APISettings class.                    */
/* 2. initialize it like QString APISettings::variableName; in the source file.            */
/* 3. add private static QString get<variableName> function in the APISettings class.      */
/* 4. initialize its definition in the source file.                                        */
/* 5. make changes in Logger::logToFile function                                           */
/*                                                                                         */
/*******************************************************************************************/

class APISettings {
public:
    static void load(const QString& filepath);

    static QString getApiMainnetBaseUrl();
    static QString getApiDemoBaseUrl();
    static QString getRecvWindow();
    static QString getApiKeyDemo();
    static QString getApiSecretDemo();
    static QString getApiKeyTestnet1();
    static QString getApiSecretTestnet1();
    static QString getApiKeyTestnet2();
    static QString getApiSecretTestnet2();

private:
    static QString apiMainnetBaseUrl;
    static QString apiDemoBaseUrl;
    static QString recvWindow;
    static QString apiKeyDemo;
    static QString apiSecretDemo;
    static QString apiKeyTestnet1;
    static QString apiSecretTestnet1;
    static QString apiKeyTestnet2;
    static QString apiSecretTestnet2;
};

#endif // APISETTINGS_H
