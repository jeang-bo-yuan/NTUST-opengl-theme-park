#include "ViewWidget.h"
#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    ViewWidget* w = new ViewWidget(nullptr);
    w->show();

    return app.exec();
}
