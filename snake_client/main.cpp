#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "snakeModel.h"
#include "client_connection.h"

#include <vector>
#include <thread>

using namespace std::chrono_literals;

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setOrganizationName("MOS");
    app.setOrganizationDomain("MOS");

    qmlRegisterType<SnakeModel>("SnakeModel", 0, 1, "SnakeModel");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
