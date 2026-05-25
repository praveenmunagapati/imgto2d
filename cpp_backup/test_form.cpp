#include <QApplication>
#include <QWidget>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QTimer>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget w;
    QFormLayout* layout = new QFormLayout(&w);
    layout->addRow(new QLabel("Header"));
    layout->addRow(new QLabel("Label1"), new QSpinBox());
    layout->addRow(new QLabel("Label2"), new QSpinBox());
    w.show();

    QTimer::singleShot(1000, [&]() {
        while(layout->rowCount() > 0) {
            layout->removeRow(0);
        }
        layout->addRow(new QLabel("New Header"));
        layout->addRow(new QLabel("NewLabel"), new QSpinBox());
        QTimer::singleShot(1000, [&]() { app.quit(); });
    });

    return app.exec();
}
