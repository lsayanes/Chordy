#include <QApplication>
#include <QStyleFactory>
#include <QIcon>
#include <QFileInfo>
#include <string>
#include <iostream>

#include "chordy.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    const char *appName =  "Chordy";

    app.setApplicationName(appName);
    app.setApplicationVersion("1.1");
    app.setOrganizationName("iDev - JalaGamaes");

    app.setStyle(QStyleFactory::create("Fusion"));

    Chordy ChordyApp;
    bool bCreated = ChordyApp.create(appName);

    if(bCreated)
    {
        ChordyApp.show();
        return app.exec();
    }

    return -1;
}
