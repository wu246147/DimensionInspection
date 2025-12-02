#include "communicationmodbus.h"

// 2 个寄存器(大端) → float
inline float reg2float(const quint16 high, const quint16 low)
{

    quint32 u32 = (quint32(high) << 16 | low);
    // 如果本机小端，再把 u32 整体转成小端
    // #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    //     u32 = ((u32 & 0xFF000000) >> 24)
    //           | ((u32 & 0x00FF0000) >> 8)
    //           | ((u32 & 0x0000FF00) << 8)
    //           | ((u32 & 0x000000FF) << 24);
    // #endif
    // qDebug() << u32;

    float f;
    memcpy(&f, &u32, 4);
    // qDebug() << f;
    return f;
}

// float → 2 个寄存器(大端)
inline void float2reg(float f, quint16 &high, quint16 &low)
{
    quint32 u32;
    memcpy(&u32, &f, sizeof(u32));

    // qDebug() << u32;

    // #if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    //     u32 = ((u32 & 0xFF000000) >> 24)
    //           | ((u32 & 0x00FF0000) >> 8)
    //           | ((u32 & 0x0000FF00) << 8)
    //           | ((u32 & 0x000000FF) << 24);
    // #endif
    high = quint16(u32 >> 16);
    low  = quint16(u32 & 0xFFFF);
}

CommunicationModbus::CommunicationModbus() {}


bool CommunicationModbus::readInt(int serverAddress, int address, QModbusDataUnit::RegisterType registerType, int &value, std::string &msg)
{
    bool rt = false;
    value = -1;
    QModbusDataUnit readRequest(registerType, address, 1);
    if(auto *reply = modbusClient->sendReadRequest(readRequest, serverAddress))
    {
        // 同步等待
        QEventLoop loop;
        int timeout = 2000;
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);   // 防死锁
        connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
        loop.exec();                // 这里阻塞


        if(reply->error() == QModbusDevice::NoError)
        {
            msg = ("read success\r\n");
            rt = true;
            const QModbusDataUnit data = reply->result();
            value = data.value(0);
        }
        else
        {
            msg = ("read fail" + reply->errorString() + "\r\n").toStdString();

        }
        reply->deleteLater();

    }
    else
    {
        msg = ("request fail:" + modbusClient->errorString()).toStdString();
    }

    return rt;
}

bool CommunicationModbus::writeInt(int serverAddress, int address, QModbusDataUnit::RegisterType registerType, int value, std::string &msg)
{
    bool rt = false;

    QModbusDataUnit writeRequest(registerType, address, 1);
    writeRequest.setValue(0, value);

    if(auto *reply = modbusClient->sendWriteRequest(writeRequest, serverAddress))
    {
        // 同步等待
        QEventLoop loop;
        int timeout = 2000;
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);   // 防死锁
        connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
        loop.exec();                // 这里阻塞


        if(reply->error() == QModbusDevice::NoError)
        {
            rt = true;
            msg += "write success\r\n";
        }
        else
        {

            msg += ("write fail" + reply->errorString() + "\r\n").toStdString();

        }
        reply->deleteLater();


    }
    else
    {
        msg += "request fail";
    }
    return rt;
}

bool CommunicationModbus::readFloat(int serverAddress, int address, QModbusDataUnit::RegisterType registerType, float &value, std::string &msg)
{
    bool rt = false;
    value = -1;
    QModbusDataUnit readRequest(registerType, address, 2);
    if(auto *reply = modbusClient->sendReadRequest(readRequest, serverAddress))
    {
        // 同步等待
        QEventLoop loop;
        int timeout = 100;
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);   // 防死锁
        connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
        loop.exec();                // 这里阻塞

        if(!reply)
        {
            return false;
        }

        if(reply->error() == QModbusDevice::NoError)
        {
            msg += ("read success\r\n");
            rt = true;
            const QModbusDataUnit data = reply->result();
            // value = data.value(0);
            msg += ("data.value(0):" + std::to_string(data.value(0)) + "\r\n").data();
            msg += ("data.value(1):" + std::to_string(data.value(1)) + "\r\n").data();

            value = reg2float(data.value(0), data.value(1));

            msg += ("value:" + std::to_string(value) + "\r\n").data();

        }
        else
        {
            msg += ("read fail" + reply->errorString() + "\r\n").toStdString();

        }
        reply->deleteLater();

    }
    else
    {
        msg += ("request fail：" + modbusClient->errorString()).toStdString();
    }

    return rt;
}

bool CommunicationModbus::writeFloat(int serverAddress, int address, QModbusDataUnit::RegisterType registerType, float value, std::string &msg)
{
    bool rt = false;

    quint16 low, high;
    float2reg(value, high, low);

    QModbusDataUnit writeRequest(registerType, address, 2);


    writeRequest.setValue(0, high);
    writeRequest.setValue(1, low);


    if(auto *reply = modbusClient->sendWriteRequest(writeRequest, serverAddress))
    {
        // 同步等待
        QEventLoop loop;
        int timeout = 2000;
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);   // 防死锁
        connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
        loop.exec();                // 这里阻塞


        if(reply->error() == QModbusDevice::NoError)
        {
            rt = true;
            msg += "write success！\r\n";

            msg += ("data.value(0):" + std::to_string(high) + "\r\n").data();
            msg += ("data.value(1):" + std::to_string(low) + "\r\n").data();

        }
        else
        {

            msg += ("write fail" + reply->errorString() + "\r\n").toStdString();

        }
        reply->deleteLater();


    }
    else
    {
        msg += "request fail";
    }
    return rt;
}

bool CommunicationModbus::modbusConnect(std::string ip, int port, std::string &msg)
{
    bool total_rt = false;

    modbusClient->setConnectionParameter(
        QModbusDevice::NetworkAddressParameter, ip.data() // 服务器 IP
    );
    modbusClient->setConnectionParameter(
        QModbusDevice::NetworkPortParameter, port            // 默认端口
    );


    qDebug() << modbusClient->state();
    bool rt = modbusClient->connectDevice();
    // 2. 连接到服务器
    if(!rt)
    {
        msg += ("connect fail：" + modbusClient->errorString() +
                "\r\n").toStdString();
        return total_rt;
    }
    else
    {
        msg += "connect success";
        total_rt = true;
        return total_rt;
    }
}

void CommunicationModbus::modbusDisconnect()
{
    modbusClient->disconnectDevice();
}
