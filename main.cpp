#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QDebug>
#include <vector>

#include <videofilter.h>

#include <registro.h>
#include <scene.h>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType< VideoFilter >( "com.calibration.opencv", 1, 0, "VideoFilter" );

//    QQmlApplicationEngine engine;

//    engine.load( QUrl( QStringLiteral("qrc:/main.qml")));

    Scene s;
    s.show();

    return app.exec();
}

