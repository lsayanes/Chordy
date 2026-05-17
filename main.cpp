#include <QApplication>
#include <QStyleFactory>
#include <QIcon>
#include <QFile>
#include <string>
#include <iostream>

#include "chordy.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    const char *appName =  "Chordy";

    app.setApplicationName(appName);
    app.setApplicationVersion("1.2");
    app.setOrganizationName("iDev - JalaGamaes");

    app.setStyle(QStyleFactory::create("Fusion"));

    // El ícono se empaqueta vía qrc (chordy.qrc) y funciona en cualquier
    // plataforma (desktop y Android) sin asumir un layout de filesystem.
    const QString iconPath = QStringLiteral(":/resources/chordy.png");
    if (QFile::exists(iconPath))
        app.setWindowIcon(QIcon(iconPath));

    Chordy ChordyApp;
    bool bCreated = ChordyApp.create(appName);

    if(bCreated)
    {
#ifdef Q_OS_ANDROID
        // En Android la app no es una "ventana": tiene que ocupar toda la pantalla
        // del device para que el grid use todo el alto disponible.
        ChordyApp.showMaximized();
#else
        ChordyApp.show();
#endif
        return app.exec();
    }

    return -1;
}
