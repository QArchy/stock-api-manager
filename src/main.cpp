#include "src/controllers/Translator/translator.h"
#include "src/controllers/Logger/logger.h"
#include "src/ui/mainwindow/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    translation(a);
    Logger::install();
    MainWindow w;
    //w.show();
    return a.exec();
}
