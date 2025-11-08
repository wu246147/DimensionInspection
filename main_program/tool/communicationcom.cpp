#include "communicationcom.h"


COM::COM()
{
    serial = new QSerialPort;
}

COM::~COM()
{
    delete serial;
}

int COM::connect()
{
    bool result = true;
    if(serial->isOpen())
    {
        serial->clear();
        serial->close();
    }
    serial->setPortName(port_name.data());
    if(serial->open(QIODevice::ReadOnly))
    {
        //设置波特率
        serial->setBaudRate(baud_rate);//设置波特率为115200
        serial->setDataBits(data_bits);//设置数据位8
        serial->setParity(parity);
        serial->setStopBits(stop_bits);//停止位设置为1
        serial->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制
    }
    else
    {
        result = false;
    }

    return result;
}

void COM::disconnect()
{
    if(serial->isOpen())
    {
        serial->clear();
        serial->close();
    }
}


