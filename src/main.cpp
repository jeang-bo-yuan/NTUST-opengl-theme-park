#include <QWidget>
#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QWidget* w = new QWidget;
    w->show();

    return app.exec();
}
