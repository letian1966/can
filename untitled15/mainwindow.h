// mainwindow.h
#ifndef MAINWINDOW_H
//mainwindow.h
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <qplaintextedit.h>
#include <QGroupBox>
#include "canthread.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectCAN();
    void onCalculate();
    void updateStatus(bool connected);

private:
    void createUI();
    void setupSignals();
    void loadSettings();
    void saveSettings();
    std::tuple<double, double> inverseKinematics(double theta, double gamma) const;

    // UI Components
    QDoubleSpinBox *spinAlpha;
    QDoubleSpinBox *spinBeta;
    QComboBox *comboInterface;
    QSpinBox *spinBitrate;
    QLabel *statusLabel;
    QLabel *labelResult1;
    QLabel *labelResult2;
    QPushButton *btnConnect;
    QPushButton *btnCalculate;
    QPlainTextEdit *txtCanLog;

    CANThread m_canThread;
};


#endif // MAINWINDOW_H
