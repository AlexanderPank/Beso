#include "MainWindow.h"
#include "core/QLogStream.h"
#include "models/Feature.h"

#include <QApplication>



int main(int argc, char *argv[])
{
    Logger::setOutputType(Logger::Console); // Выводить и в консоль, и в файл

    qRegisterMetaType<Feature*>("Feature*");

    QApplication app(argc, argv);

    // Загрузка темы QDarkStyleSheet
    QFile styleFile(":/styles/dark_theme.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    MainWindow w;
    w.show();
    return app.exec();
}
