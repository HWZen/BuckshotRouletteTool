#include "main.h"
#include "version.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

QFile debugLogFile;

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream out(&debugLogFile);
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("[%1] Debug: %2").arg(time, msg);
        break;
    case QtWarningMsg:
        txt = QString("[%1] Warning: %2").arg(time, msg);
        break;
    case QtCriticalMsg:
        txt = QString("[%1] Critical: %2").arg(time, msg);
        break;
    case QtFatalMsg:
        txt = QString("[%1] Fatal: %2").arg(time, msg);
        break;
    }
    out << txt << "\n";
    out.flush();
}

int main(int argc, char *argv[]) {
#if QT_VERSION >= 0x50601
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    debugLogFile.setFileName("debug.log");
    debugLogFile.open(QIODevice::Text | QIODevice::WriteOnly);
    qInstallMessageHandler(myMessageHandler);

    QApplication app(argc, argv);

    app.setApplicationName("BuckshotRouletteTool");
    app.setOrganizationName("BuckshotRoulette");
    app.setOrganizationDomain("hwzen.myds.me");
    app.setApplicationVersion(PROJECT_VERSION);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
