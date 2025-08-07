#include "main.h"
#include "version.h"
#include <QApplication>

int main(int argc, char *argv[]) {
#if QT_VERSION >= 0x50601
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    app.setApplicationName("BuckshotRouletteTool");
    app.setOrganizationName("BuckshotRoulette");
    app.setOrganizationDomain("hwzen.myds.me");
    app.setApplicationVersion(PROJECT_VERSION);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
