#include "communication.h"

Communication::Communication() {}


void Communication::tcp_server_init()
{
    // for(int id = 0; id < jobworkers.size(); ++id)
    // {
    //     jobworkers[id].tcp_server_init();
    // }
    LOGE_HIGH("TCP start Init.");
    if(m_tcpServer == nullptr)
    {
        m_tcpServer = new QTcpServer(this);
    }
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(tcp_socker_connected()));
    QString Ip = QString(plc_ip.data());
    quint16 port = net_port;
    QHostAddress addr(Ip);
    m_tcpServer->listen(addr, port);
    LOGE_HIGH("TCP finish Init.");
}

void Communication::tcp_server_dispose()
{
    // for(int id = 0; id < jobworkers.size(); ++id)
    // {
    //     jobworkers[id].tcp_server_dispose();
    // }
    LOGE_HIGH("TCP start dispose.");
    if(m_tcpServer != nullptr)
    {
        disconnect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(tcp_socker_connected()));
        if(m_tcpServer->isListening())
        {
            m_tcpServer->close();
        }
        delete  m_tcpServer;
        m_tcpServer = nullptr;
    }
    LOGE_HIGH("TCP finish dispose.");
}

void Communication::tcp_socker_connected()
{
    QTcpSocket * m_tcpSocket = m_tcpServer->nextPendingConnection();   //创建 socket
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(tcp_socker_disconnected()));
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(tcp_ready_read()));
    m_tcpSocketList.push_back(m_tcpSocket);

    std::cout << "connect success!!!" << std::endl;
}

void Communication::tcp_socker_disconnected()
{
    std::cout << "disconnect success!!!" << std::endl;
}

void Communication::tcp_send(std::string info)
{
    for(int id = 0; id < m_tcpSocketList.count(); ++id)
    {
        QTcpSocket *m_tcpSocket = m_tcpSocketList[id];
        if(m_tcpSocket != nullptr && m_tcpSocket->isOpen())
        {
            m_tcpSocket->write((info + "\n").data());
            LOGE_HIGH("tcp send:%s", info.data());
        }
    }
}

void Communication::tcp_ready_read()
{
    QTcpSocket * m_tcpSocket = static_cast<QTcpSocket*>(sender());
    if(!m_tcpSocket)
    {
        return;
    }
    QString info = m_tcpSocket->readAll();

    try
    {
        info = info.remove('\r');
        info = info.remove('\n');
        LOGE("receive tcp:%s", info.toStdString().data());
        // std::cout <<  << std::endl;
        QStringList infoList = info.split(',');

        if(infoList[7].toInt() == 1)
        {
            //运行
            checkCamSignal(infoList[7].toInt(), infoList[8].toInt(), infoList[6].toStdString());


        }
        else if(infoList[7].toInt() == 3)
        {
            //标定
            calibrationSignal(infoList[0].toDouble(),
                              infoList[1].toDouble(),
                              infoList[2].toDouble(),
                              infoList[3].toDouble(),
                              infoList[4].toDouble(),
                              infoList[5].toDouble());

            //返回结果
            tcp_send("0,0,0,0,0,0,0,0,0,0,0,0,1,");
            // m_tcpSocket->write((info + "\n").data());

        }
        else
        {
            LOGE("not have this Instruction:%d", infoList[7].toInt());

        }

        LOGE_HIGH("finish tcp process");
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}

