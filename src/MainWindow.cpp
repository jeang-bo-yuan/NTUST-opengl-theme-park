#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->buttonUnwind->hide();
    this->resize(1200, 800);

    // Water
    connect(ui->radioSineWave, &QRadioButton::clicked, ui->view, &ViewWidget::use_sine_wave);
    connect(ui->radioRipple, &QRadioButton::clicked, ui->view, &ViewWidget::use_ripple);
    connect(ui->radioHeightMap, &QRadioButton::clicked, ui->view, &ViewWidget::use_height_map);

    // Train
    ui->buttonDeleteCP->setDisabled(true);
    ui->sliderAlpha->setDisabled(true);
    ui->sliderBeta->setDisabled(true);
    connect(ui->view, &ViewWidget::is_point_selected, this, [this](bool selected) {
        ui->sliderAlpha->setEnabled(selected);
        ui->sliderBeta->setEnabled(selected);
        ui->buttonDeleteCP->setEnabled(selected);

        if (!selected) return;

        // 有控制點被選中，且原本的控制點超過4個，則啟用刪除鍵
        ui->buttonDeleteCP->setEnabled(ui->view->get_train().get_cp_num() > 4);

        float alpha, beta;
        ui->view->get_train().orient_of_selected_CP(alpha, beta);
        ui->sliderAlpha->setValue(glm::degrees(alpha));
        ui->sliderBeta->setValue(glm::degrees(beta));
    });
    connect(ui->buttonAddCP, &QPushButton::clicked, ui->view, &ViewWidget::add_train_CP);
    connect(ui->buttonDeleteCP, &QPushButton::clicked, ui->view, &ViewWidget::delete_train_CP);
    connect(ui->sliderAlpha, &QSlider::valueChanged, this, &MainWindow::update_orient_for_cp);
    connect(ui->sliderBeta, &QSlider::valueChanged, this, &MainWindow::update_orient_for_cp);
    connect(ui->buttonExport, &QPushButton::clicked, ui->view, &ViewWidget::export_control_points);
    connect(ui->buttonImport, &QPushButton::clicked, ui->view, &ViewWidget::import_control_points);
    connect(ui->buttonResetCP, &QPushButton::clicked, this, [this]() { ui->view->get_train().reset_CP(); });

    // Train Spline
    ui->sliderTension->setFixedWidth(120);
    connect(ui->sliderTension, &QSlider::valueChanged, this, [this](int value) {
        ui->labelTension->setNum((double)value / 10.);
        ui->view->get_train().set_tension((float)value / 10.f);
    });
    connect(ui->radioLinear, &QRadioButton::clicked, this, [this]() {
        ui->view->get_train().set_line_type(SplineType::LINEAR);
    });
    connect(ui->radioCubicB, &QRadioButton::clicked, this, [this]() {
        ui->view->get_train().set_line_type(SplineType::CUBIC_B);
    });
    connect(ui->radioCardinal, &QRadioButton::clicked, this, [this]() {
        ui->view->get_train().set_line_type(SplineType::CARDINAL);
    });

    // Train: 火車
    connect(ui->checkBoxTrackingTrain, &QCheckBox::toggled, ui->view, &ViewWidget::toggle_tracking_train);
    connect(ui->buttonAddCart, &QPushButton::clicked, this, [this]() { ui->view->get_train().add_cart(); });
    connect(ui->buttonDeleteCart, &QPushButton::clicked, this, [this]() { ui->view->get_train().delete_cart(); });
    connect(ui->buttonClearCart, &QPushButton::clicked, this, [this]() { ui->view->get_train().clear_cart(); });
    connect(ui->sliderSpeed, &QSlider::valueChanged, ui->view, &ViewWidget::set_train_speed);

    // Misc
    connect(ui->checkBoxWireframe, &QCheckBox::toggled, ui->view, &ViewWidget::toggle_wireframe);
    //
    connect(ui->checkBoxCel, &QCheckBox::toggled, ui->view, &ViewWidget::toggle_Cel_Shading);
    connect(ui->spinCelLevels, QOverload<int>::of(&QSpinBox::valueChanged), ui->view, &ViewWidget::set_Cel_Levels);
    //
    connect(ui->radioNoProcess, &QRadioButton::clicked, this, [this]() { ui->view->set_post_process_type(PostProcessor::Type::NoProcess); });
    connect(ui->radioPixelization, &QRadioButton::clicked, this, [this]() { ui->view->set_post_process_type(PostProcessor::Type::Pixelization); });
    connect(ui->radioGrayscale, &QRadioButton::clicked, this, [this]() { ui->view->set_post_process_type(PostProcessor::Type::Grayscale); });
    connect(ui->radioDepthImage, &QRadioButton::clicked, this, [this]() { ui->view->set_post_process_type(PostProcessor::Type::DepthImage); });
    connect(ui->radioSobelOperator, &QRadioButton::clicked, this, [this]() { ui->view->set_post_process_type(PostProcessor::Type::SobelOperator); });
    connect(ui->radioSpeedLine, &QRadioButton::clicked, this, [this]() { ui->view->set_post_process_type(PostProcessor::Type::SpeedLine); });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_orient_for_cp()
{
    float alpha = glm::radians<float>(ui->sliderAlpha->value());
    float beta = glm::radians<float>(ui->sliderBeta->value());
    ui->view->get_train().set_orient_for_selected_CP(alpha, beta);
}
