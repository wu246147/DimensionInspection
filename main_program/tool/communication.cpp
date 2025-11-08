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
        LOGE_HIGH("receive infoList len:%d", infoList.length());
        foreach(auto info_item, infoList)
        {
            LOGE_HIGH("info_item:%s", info_item.toStdString().data());
        }
        //添加通讯逻辑
        if(infoList[0].startsWith("T"))
        {
            LOGE_HIGH("start change mode");
            //手动定位
            if(infoList[1].compare("S") == 0)
            {
                LOGE_HIGH("start change mode");

                if(infoList[2].compare("0") == 0)
                {
                    // isAutoMode = true;
                    changeRunModelSignal(infoList[0].mid(1, 1).toInt() - 1, true);
                }
                else if(infoList[2].compare("1") == 0)
                {
                    // isAutoMode = false;
                    changeRunModelSignal(infoList[0].mid(1, 1).toInt() - 1, false);
                }


                //结果发送
                std::string info = "T" + infoList[0].mid(1, 1).toStdString() + ",S," + infoList[2].toStdString() + ",0\r";
                tcp_send(info);
            }
            else if(infoList[1].compare("DIS") == 0)
            {
                LOGE_HIGH("set centerLine distance");

                changeCenterLineDistSignal(infoList[0].mid(1, 1).toInt() - 1,
                                           infoList[2].toInt());

            }
            else if(infoList[1].compare("AF") == 0)
            {
                //先关掉实时
                changeRunModelSignal(infoList[0].mid(1, 1).toInt() - 1, true);
                //定位
                if(infoList[0].mid(1, 1).compare("1") == 0)
                {
                    LOGE_HIGH("focusing cam 1");

                    focusingSignal(0);
                }
                else if(infoList[0].mid(1, 1).compare("2") == 0)
                {
                    LOGE_HIGH("focusing cam 2");

                    focusingSignal(1);
                }
            }
            else
            {
                LOGE_HIGH("start check cam");
                if(infoList.size() == 4)
                {
                    //先关掉实时
                    changeRunModelSignal(infoList[0].mid(1, 1).toInt() - 1, true);
                    //自动标定
                    if(infoList[0].mid(1, 1).compare("1") == 0)
                    {
                        calibrationSignal(0, infoList[2].toFloat(), infoList[3].toFloat());
                    }
                    else if(infoList[0].mid(1, 1).compare("2") == 0)
                    {
                        calibrationSignal(1, infoList[2].toFloat(), infoList[3].toFloat());
                    }
                }
                else
                {
                    //先关掉实时
                    changeRunModelSignal(infoList[0].mid(1, 1).toInt() - 1, true);
                    //定位
                    if(infoList[0].mid(1, 1).compare("1") == 0)
                    {
                        LOGE_HIGH("check cam 1");

                        checkCamSignal(0);
                    }
                    else if(infoList[0].mid(1, 1).compare("2") == 0)
                    {
                        LOGE_HIGH("check cam 2");

                        checkCamSignal(1);
                    }
                }

            }
        }
        //切换display
        else if(infoList[0].compare("SWC") == 0)
        {
            LOGE_HIGH("start change Display");
            int tmp_cam_id =  infoList[1].toInt() - 1;
            changeDisplaySignal(tmp_cam_id);
            std::string info = "SWC," + infoList[1].toStdString() + ",0\r";
            tcp_send(info);
        }
        //切换模式
        else if(infoList[0].compare("SWM") == 0)
        {
            //先关掉实时
            changeRunModelSignal(0, true);
            changeRunModelSignal(1, true);
            QThread::msleep(500);
            LOGE_HIGH("start change show Model");
            if(infoList[1].compare("R") == 0)
            {
                changeShowModeSignal(true);
            }
            else
            {
                changeShowModeSignal(false);
            }
            std::string info = "SWM," + infoList[1].toStdString() + ",0\r";
            tcp_send(info);
        }
        //切换作业
        else if(infoList[0].compare("SWP") == 0)
        {
            //先关掉实时
            changeRunModelSignal(0, true);
            changeRunModelSignal(1, true);
            QThread::msleep(500);
            LOGE_HIGH("start change File");
            changeFileSignal(infoList[1].toStdString());
        }
        LOGE_HIGH("finish tcp process");
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}

