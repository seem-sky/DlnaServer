#include <QtGui/QGuiApplication>
#include <QtQml>

#include "application.h"
#include "requestlistmodel.h"

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // initialisation of the application where HMI is defined in QML
    Application c_application(&engine);

    // load the qml file describing the application
    if (c_application.load(QUrl("qrc:/qml/main.qml")) == -1)
        return -1;

    return app.exec();
}