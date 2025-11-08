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
    /// \brief checkCamSignal
    /// \param cam_id
    ///
    void checkCamSignal(int cam_id);

    ///
    /// \brief focusingSignal
    /// \param cam_id
    ///
    void focusingSignal(int cam_id);

    ///
    /// \brief calibrationSignal
    /// \param cam_id
    /// \param robotX
    /// \param robotY
    ///
    void calibrationSignal(int cam_id, float robotX, float robotY);



    ///
    /// \brief changeDisplaySignal
    /// \param cam_id
    ///
    void changeDisplaySignal(int cam_id);

    ///
    /// \brief changeShowModeSignal
    /// \param isShowForm
    ///
    void changeShowModeSignal(bool isShowForm);

    ///
    /// \brief changeRunModelSignal
    /// \param isAutoMode
    ///
    void changeRunModelSignal(int camid, bool isAutoMode);

    ///
    /// \brief changeRunModelSignal
    /// \param isAutoMode
    ///
    void changeCenterLineDistSignal(int camid, int centerDist);

    ///
    /// \brief changeFileSignal
    /// \param fileName
    ///
    void changeFileSignal(std::string fileName);

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
