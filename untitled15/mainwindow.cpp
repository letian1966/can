// mainwindow.cpp
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSettings>
#include <QMessageBox>
#include <QFontDatabase>
#include <QtMath>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      spinAlpha(new QDoubleSpinBox()),
      spinBeta(new QDoubleSpinBox()),
      comboInterface(new QComboBox()),
      spinBitrate(new QSpinBox()),
      statusLabel(new QLabel("状态: 未连接")),
      labelResult1(new QLabel("结果1: -")),
      labelResult2(new QLabel("结果2: -")),
      btnConnect(new QPushButton("连接CAN")),
      btnCalculate(new QPushButton("计算并发送")),
      txtCanLog(new QPlainTextEdit())
{
    setWindowTitle("CAN 运动控制");
    resize(800, 600);
    createUI();
    setupSignals();
    loadSettings();

    // 强制设置默认参数（覆盖配置文件）
    comboInterface->setCurrentText("can0");
    spinBitrate->setValue(500000);
    // 新增：启动时自动尝试连接CAN
    QTimer::singleShot(100, this, &MainWindow::onConnectCAN); // 延迟100ms确保UI初始化完成

}

MainWindow::~MainWindow() {
    saveSettings();
}

void MainWindow::createUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 运动参数组
    QGroupBox *inputGroup = new QGroupBox("运动参数");
    QFormLayout *inputLayout = new QFormLayout();
    spinAlpha->setRange(-45, 45);
    spinAlpha->setDecimals(1);
    spinAlpha->setSingleStep(0.5);
    spinBeta->setRange(-45, 45);
    spinBeta->setDecimals(1);
    spinBeta->setSingleStep(0.5);
    inputLayout->addRow("Alpha (°):", spinAlpha);
    inputLayout->addRow("Beta (°):", spinBeta);
    inputGroup->setLayout(inputLayout);

    // CAN配置组
    QGroupBox *canGroup = new QGroupBox("CAN配置");
    QFormLayout *canLayout = new QFormLayout();
    comboInterface->addItems({"can0", "can1"});
    spinBitrate->setRange(10000, 1000000);
    spinBitrate->setSingleStep(100000);
    spinBitrate->setValue(500000);
    canLayout->addRow("接口:", comboInterface);
    canLayout->addRow("波特率:", spinBitrate);
    canGroup->setLayout(canLayout);

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(btnConnect);
    buttonLayout->addWidget(btnCalculate);

    // 结果展示
    QGroupBox *resultGroup = new QGroupBox("计算结果");
    QVBoxLayout *resultLayout = new QVBoxLayout();
    resultLayout->addWidget(labelResult1);
    resultLayout->addWidget(labelResult2);
    resultGroup->setLayout(resultLayout);

    // 通信日志
    QGroupBox *logGroup = new QGroupBox("通信日志");
    txtCanLog->setReadOnly(true);
    txtCanLog->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    txtCanLog->setMaximumBlockCount(1000);
    QVBoxLayout *logLayout = new QVBoxLayout();
    logLayout->addWidget(txtCanLog);
    logGroup->setLayout(logLayout);

    // 主布局
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(canGroup);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(resultGroup);
    mainLayout->addWidget(logGroup);
    mainLayout->addWidget(statusLabel);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void MainWindow::setupSignals() {
    connect(btnConnect, &QPushButton::clicked, this, &MainWindow::onConnectCAN);
    connect(btnCalculate, &QPushButton::clicked, this, &MainWindow::onCalculate);
    connect(&m_canThread, &CANThread::canStatusChanged, this, &MainWindow::updateStatus);
    connect(&m_canThread, &CANThread::frameInfo, txtCanLog, &QPlainTextEdit::appendPlainText);
}

// 在mainwindow.cpp中的连接回调添加初始化
void MainWindow::onConnectCAN() {
    const QString interface = comboInterface->currentText();
    const int bitrate = spinBitrate->value();

    if (m_canThread.initCAN(interface, bitrate)) {
        statusLabel->setText("已连接: " + interface);
        QTimer::singleShot(200, this, [this]() { // 延迟确保线程启动
            m_canThread.initMotorSettings(1);
            m_canThread.initMotorSettings(2);
        });
    }
}

void MainWindow::onCalculate() {
    try {
        const double alpha = spinAlpha->value();
        const double beta = spinBeta->value();

        auto [delta1, delta2] = inverseKinematics(
            qDegreesToRadians(alpha),
            qDegreesToRadians(beta)
        );

        // 直接传递double类型的位置值（单位：毫米，含两位小数）
        m_canThread.sendPosition(1, delta1);
        m_canThread.sendPosition(2, delta2);

        // 更新显示结果（保留两位小数）
        labelResult1->setText(QString("结果1: %1 mm").arg(delta1, 0, 'f', 2));
        labelResult2->setText(QString("结果2: %1 mm").arg(delta2, 0, 'f', 2));
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "错误", e.what());
    }
}

std::tuple<double, double> MainWindow::inverseKinematics(double theta, double gamma) const {
    const double r = 189.04;
    const double R = 306.25;
    const double h0 = 831.278;
    const double L0 = 839.5;

    const double c_theta = cos(theta);
    const double s_theta = sin(theta);
    const double c_gamma = cos(gamma);
    const double s_gamma = sin(gamma);

    const double T[3][3] = {
        {c_theta, s_theta * s_gamma, s_theta * c_gamma},
        {0,       c_gamma,          -s_gamma},
        {-s_theta, c_theta * s_gamma, c_theta * c_gamma}
    };

    const double q1[3] = {r, 0, 0};
    const double q2[3] = {0, r, 0};
    const double p1[3] = {R, 0, 0};
    const double p2[3] = {0, R, 0};
    const double t[3] = {0, 0, h0};

    auto transform = [&](const double q[3]) {
        return std::array<double, 3>{
            T[0][0]*q[0] + T[0][1]*q[1] + T[0][2]*q[2] + t[0],
            T[1][0]*q[0] + T[1][1]*q[1] + T[1][2]*q[2] + t[1],
            T[2][0]*q[0] + T[2][1]*q[1] + T[2][2]*q[2] + t[2]
        };
    };

    auto calculateLength = [](const auto &a, const auto &b) {
        double sum = 0;
        for(int i = 0; i < 3; ++i) {
            double diff = a[i] - b[i];
            sum += diff * diff;
        }
        return sqrt(sum);
    };

    auto u_q1 = transform(q1);
    auto u_q2 = transform(q2);

    double l1 = calculateLength(u_q1, p1);
    double l2 = calculateLength(u_q2, p2);

    return {l1 - L0, l2 - L0};
}

void MainWindow::loadSettings() {
    QSettings settings("config.ini", QSettings::IniFormat);
    comboInterface->setCurrentText(settings.value("CAN/Interface", "can0").toString());
    spinBitrate->setValue(settings.value("CAN/Bitrate", 500000).toInt());
}

void MainWindow::saveSettings() {
    QSettings settings("config.ini", QSettings::IniFormat);
    settings.setValue("CAN/Interface", comboInterface->currentText());
    settings.setValue("CAN/Bitrate", spinBitrate->value());
}

void MainWindow::updateStatus(bool connected) {
    statusLabel->setText(connected ? "状态: 已连接" : "状态: 已断开");
    statusLabel->setStyleSheet(QString("color: %1;").arg(connected ? "green" : "red"));
}
