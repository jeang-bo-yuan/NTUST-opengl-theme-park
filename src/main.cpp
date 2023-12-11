#include "MainWindow.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    MainWindow* w = new MainWindow(nullptr);
    QTimer::singleShot(10, w, [w]() { w->show(); });

    return app.exec();
}
