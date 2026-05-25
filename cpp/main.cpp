#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include "ui/main_window.h"

void customLogHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    static QFile logFile("app_crash.log");
    if (!logFile.isOpen()) {
        logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    QTextStream ts(&logFile);
    ts << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz") << " [" << type << "] " << msg << "\n";
    ts.flush();
}

int main(int argc, char* argv[]) {
    qInstallMessageHandler(customLogHandler);
    QApplication app(argc, argv);
    app.setApplicationName("imgto2d");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("imgto2d");
    app.setStyle(QStyleFactory::create("Fusion"));

    qDebug() << "Application started";

    MainWindow win;
    win.show();

    return app.exec();
}
