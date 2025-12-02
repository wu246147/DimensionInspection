#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QtConcurrent>

#include "mylog.h"

class Communication: public QObject
{
    Q_OBJECT
public:
    Communication();

    //plc 通讯ip
    std::string plc_ip = "192.168.1.5";
    //plc 通讯 port
    int net_port = 9094;



    //网络socket通信
    QTcpServer *m_tcpServer = nullptr;
    QList<QTcpSocket *>m_tcpSocketList;

    //通信锁
    std::mutex socket_locker;


public slots:
    ///
    /// \brief tcp_socker_connected 新tcp客户端连接
    ///
    void tcp_socker_connected();

    ///
    /// \brief tcp_socker_disconnected tcp客户端断开连接
    ///
    void tcp_socker_disconnected();

    ///
    /// \brief tcp_ready_read 接收tcp客户端信息
    ///
    void tcp_ready_read();

signals:

    ///
    /// \brief checkCamSignal 触发运行
    /// \param cam_id   相机id
    /// \param fileName 作业名称
    ///
    void checkCamSignal(int cam_id, int work_id, std::string fileName);


    ///
    /// \brief calibrationSignal 触发标定
    /// \param cam_id   相机id
    /// \param robotX   机器人当前X坐标
    /// \param robotY   机器人当前Y坐标
    /// \param robotZ   机器人当前Z坐标
    /// \param robotRX   机器人当前RX坐标
    /// \param robotRY   机器人当前RY坐标
    /// \param robotRZ   机器人当前RZ坐标
    ///
    void calibrationSignal(float robotX, float robotY, float robotZ,
                           float robotRX, float robotRY, float robotRZ);





public:
    ///
    /// \brief tcp_server_init tcp服务器通讯初始化
    ///
    void tcp_server_init();

    ///
    /// \brief tcp_server_dispose tcp服务器关闭
    ///
    void tcp_server_dispose();


    void tcp_send(std::string info);
};

#endif // COMMUNICATION_H
