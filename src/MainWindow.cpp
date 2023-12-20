#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->buttonUnwind->hide();

    // Water
    connect(ui->radioSineWave, &QRadioButton::clicked, ui->view, &ViewWidget::use_sine_wave);
    connect(ui->radioRipple, &QRadioButton::clicked, ui->view, &ViewWidget::use_ripple);
    connect(ui->radioHeightMap, &QRadioButton::clicked, ui->view, &ViewWidget::use_height_map);

    // Train
    ui->buttonDeleteCP->setDisabled(true);
    connect(ui->view, &ViewWidget::is_point_selected, ui->buttonDeleteCP, &QPushButton::setEnabled);
    connect(ui->buttonAddCP, &QPushButton::clicked, ui->view, &ViewWidget::add_train_CP);
    connect(ui->buttonDeleteCP, &QPushButton::clicked, ui->view, &ViewWidget::delete_train_CP);

    // Misc
    connect(ui->checkBoxWireframe, &QCheckBox::toggled, ui->view, &ViewWidget::toggle_wireframe);
}

MainWindow::~MainWindow()
{
    delete ui;
}
