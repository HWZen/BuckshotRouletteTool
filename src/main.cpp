#include "main.h"
#include "qmainwindow.h"
#include "version.h"
#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[]) {
#if QT_VERSION >= 0x50601
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);

    app.setApplicationName("BuckshotRouletteTool");
    app.setOrganizationName("BuckshotRoulette");
    app.setOrganizationDomain("hwzen.myds.me");
    app.setApplicationVersion(PROJECT_VERSION);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle(QString("BuckshotRouletteTool %1").arg(PROJECT_VERSION));
    mainWindow.setMinimumSize(800, 600);

    mainWindow.show();



    return app.exec();
}
