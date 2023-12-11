#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->radioSineWave, &QRadioButton::clicked, ui->view, &ViewWidget::use_sine_wave);
    connect(ui->radioRipple, &QRadioButton::clicked, ui->view, &ViewWidget::use_ripple);
    connect(ui->radioHeightMap, &QRadioButton::clicked, ui->view, &ViewWidget::use_height_map);

    connect(ui->checkBoxWireframe, &QCheckBox::toggled, ui->view, &ViewWidget::toggle_wireframe);
}

MainWindow::~MainWindow()
{
    delete ui;
}
