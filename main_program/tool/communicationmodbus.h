#ifndef COMMUNICATIONMODBUS_H
#define COMMUNICATIONMODBUS_H


#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>




class CommunicationModbus: public QObject
{
    Q_OBJECT
public:

    //plc 通讯ip
    std::string plc_ip = "192.168.1.5";
    //plc 通讯 port
    int net_port = 9094;

    // 机器人通讯，modbus TCPIP

    QModbusTcpClient *modbusClient = new QModbusTcpClient;


    CommunicationModbus();


    // 机器人通讯
    bool readInt(int serverAddress, int address, QModbusDataUnit::RegisterType registerType, int &value, std::string &msg);
    bool writeInt(int serverAddress, int address, QModbusDataUnit::RegisterType registerType, int value, std::string &msg);


    bool readFloat(int serverAddress, int address, QModbusDataUnit::RegisterType registerType, float &value, std::string &msg);
    bool writeFloat(int serverAddress, int address, QModbusDataUnit::RegisterType registerType, float value, std::string &msg);

    bool modbusConnect(std::string ip, int port, std::string &msg);
    void modbusDisconnect();

};

#endif // COMMUNICATIONMODBUS_H
