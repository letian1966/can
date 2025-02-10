// canthread.cpp
#include "canthread.h"
#include <QDebug>
#include <sys/ioctl.h>
#include <cmath>
#include <string.h>
#include <QProcess>
#include <QElapsedTimer>

CANThread::CANThread(QObject *parent) : QThread(parent) {}

CANThread::~CANThread() {
    stop();
}

bool CANThread::initCAN(const QString &interface, int bitrate) {
    QMutexLocker locker(&m_mutex);

    // 关闭旧socket
    if (m_can.socket != -1) {
        ::close(m_can.socket);
        m_can.socket = -1;
    }

    // 配置CAN接口
    QStringList commands = {
        QString("sudo ip link set %1 down").arg(interface),
        QString("sudo ip link set %1 type can bitrate %2").arg(interface).arg(bitrate),
        QString("sudo ip link set %1 up").arg(interface)
    };

    foreach (const QString &cmd, commands) {
        QProcess process;
        process.start("bash", QStringList() << "-c" << cmd);
        if (!process.waitForFinished(2000)) {
            emit errorOccurred("命令超时: " + cmd);
            return false;
        }
        if (process.exitCode() != 0) {
            QString error = QString::fromLocal8Bit(process.readAllStandardError());
            emit errorOccurred("命令错误: " + cmd + "\n" + error);
            return false;
        }
    }

    // 创建socket
    if ((m_can.socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        emit errorOccurred("Socket创建失败: " + QString(strerror(errno)));
        return false;
    }

    // 绑定接口
    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface.toUtf8().constData(), IFNAMSIZ);
    if (ioctl(m_can.socket, SIOCGIFINDEX, &ifr) < 0) {
        emit errorOccurred("接口绑定失败: " + QString(strerror(errno)));
        ::close(m_can.socket);
        m_can.socket = -1;
        return false;
    }

    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(m_can.socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        emit errorOccurred("绑定失败: " + QString(strerror(errno)));
        ::close(m_can.socket);
        m_can.socket = -1;
        return false;
    }

    m_can.interface = interface;
    m_can.bitrate = bitrate;
    m_can.running = true;
    emit canStatusChanged(true);
    return true;
}

void CANThread::stop() {
    {
        QMutexLocker locker(&m_mutex);
        m_can.running = false;
    }

    // 等待线程退出
    if (isRunning()) {
        wait(500); // 最多等待500ms
    }

    QMutexLocker locker(&m_mutex);
    if (m_can.socket != -1) {
        ::close(m_can.socket);
        m_can.socket = -1;
    }
    emit canStatusChanged(false);
}

void CANThread::run() {
    struct can_frame frame;
    while (true) {
        {
            QMutexLocker locker(&m_mutex);
            if (!m_can.running || m_can.socket < 0) break;
        }

        int nbytes = read(m_can.socket, &frame, sizeof(can_frame));
        if (nbytes < 0) {
            if (errno == EAGAIN) continue;
            QMutexLocker locker(&m_mutex);
            if (m_can.running) {
                emit errorOccurred("读取错误: " + QString(strerror(errno)));
            }
            break;
        }

        if (nbytes == sizeof(can_frame)) {
            QString dataStr;
            for(int i = 0; i < frame.can_dlc; i++) {
                dataStr += QString("%1 ").arg(frame.data[i], 2, 16, QLatin1Char('0')).toUpper();
            }

            QString info = QString("[接收] ID: 0x%1 | DLC: %2 | 数据: %3")
                           .arg(QString::number(frame.can_id, 16).toUpper().rightJustified(3, '0'))
                           .arg(frame.can_dlc)
                           .arg(dataStr.trimmed());

            emit frameInfo(QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ") + info);
        }
    }
}

void CANThread::packPositionData(int id, double position, can_frame &frame) {
    const int32_t pulse = static_cast<int32_t>(std::round(position * m_pulsePerMM));

    frame.can_id =  id;
    frame.can_dlc = 8;

    frame.data[0] = 0x00;
    frame.data[1] = 0x1A;
    frame.data[2] = 0x50;
    frame.data[3] = (pulse >> 24) & 0xFF;
    frame.data[4] = (pulse >> 16) & 0xFF;
    frame.data[5] = 0x05;
    frame.data[6] = (pulse >> 8) & 0xFF;
    frame.data[7] = pulse & 0xFF;
}

void CANThread::sendPosition(int id, double position) {
    QMutexLocker locker(&m_mutex);

    if (!m_can.running || m_can.socket < 0) {
        emit errorOccurred("CAN未连接");
        return;
    }

    can_frame frame;
    packPositionData(id, position, frame);

    QString dataStr;
    for(int i = 0; i < frame.can_dlc; i++) {
        dataStr += QString("%1 ").arg(frame.data[i], 2, 16, QLatin1Char('0')).toUpper();
    }

    QString info = QString("[发送] ID: 0x%1 | DLC: %2 | 数据: %3")
                   .arg(QString::number(frame.can_id, 16).toUpper().rightJustified(3, '0'))
                   .arg(frame.can_dlc)
                   .arg(dataStr.trimmed());

    emit frameInfo(QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ") + info);

    if (write(m_can.socket, &frame, sizeof(can_frame)) != sizeof(can_frame)) {
        emit errorOccurred("发送失败: " + QString(strerror(errno)));
    } else {
        emit positionSent(id, position);
    }
}

void CANThread::initMotorSettings(int id) {
    QMutexLocker locker(&m_mutex);

    if (!m_can.running || m_can.socket < 0) {
        emit errorOccurred("CAN未连接");
        return;
    }

    // 配置帧1: 设置位置模式
    can_frame modeFrame;
    modeFrame.can_id = id;
    modeFrame.can_dlc = 8;
    modeFrame.data[0] = 0x00;
    modeFrame.data[1] = 0x1A;
    modeFrame.data[2] = 0x02;
    modeFrame.data[3] = 0x00;
    modeFrame.data[4] = 0xD0;
    modeFrame.data[5] = 0xFF;
    modeFrame.data[6] = 0xFF;
    modeFrame.data[7] = 0xFF;

    // 配置帧2: 启动电机
    can_frame enableFrame;
    enableFrame.can_id = id;
    enableFrame.can_dlc = 8;
    enableFrame.data[0] = 0x00;
    enableFrame.data[1] = 0x1A;
    enableFrame.data[2] = 0x00;
    enableFrame.data[3] = 0x00;
    enableFrame.data[4] = 0x01;
    enableFrame.data[5] = 0xFF;
    enableFrame.data[6] = 0xFF;
    enableFrame.data[7] = 0xFF;

    if (write(m_can.socket, &modeFrame, sizeof(can_frame)) != sizeof(can_frame) ||
        write(m_can.socket, &enableFrame, sizeof(can_frame)) != sizeof(can_frame))
    {
        emit errorOccurred("初始化命令发送失败");
    } else {
        emit frameInfo("[系统] 驱动器初始化完成");
    }
}
