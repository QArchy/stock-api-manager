#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QCoreApplication>
#include <QApplication>
#include <QTranslator>
#include <QLocale>

void translation(QApplication& a) {
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BybitManager_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
}

#endif // TRANSLATOR_H
