#include "app/Application.h"

#include <QGuiApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("TypeBoost"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));
    app.setOrganizationName(QStringLiteral("TypeBoost"));
    app.setWindowIcon(QIcon(QStringLiteral(":/TypeBoost/resources/icon.png")));

    Application typeboost;
    return typeboost.run();
}
