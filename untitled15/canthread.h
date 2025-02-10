// canthread.h
#ifndef CANTHREAD_H
#define CANTHREAD_H

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>


class CANThread : public QThread {
    Q_OBJECT
public:
    explicit CANThread(QObject *parent = nullptr);
    ~CANThread();

    bool initCAN(const QString &interface, int bitrate = 500000);
    void sendPosition(int id, double position);
    void stop();
    void initMotorSettings(int id); // 新增初始化方法

signals:
    void canStatusChanged(bool connected);
    void errorOccurred(const QString &error);
    void positionSent(int id, int32_t position);
    void frameInfo(const QString &info);

protected:
    void run() override;

private:
    struct CanConfig {
        QString interface;
        int bitrate;
        int socket = -1;
        bool running = false;
    };

    CanConfig m_can;
    QMutex m_mutex;
    double m_pulsePerMM = 2000.0;

    void setupSocketCAN();
    void packPositionData(int id, double position, can_frame &frame);
};

#endif // CANTHREAD_H
