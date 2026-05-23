#include <QApplication>
#include <QStyleFactory>
#include "ui/main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("imgto2d");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("imgto2d");
    app.setStyle(QStyleFactory::create("Fusion"));

    MainWindow win;
    win.show();

    return app.exec();
}
