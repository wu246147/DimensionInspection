#ifndef COMMUNICATIONCOM_H
#define COMMUNICATIONCOM_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


class COM
{

public:
    COM();
    ~COM();
    QSerialPort *serial;
    int connect();
    void disconnect();

    std::string port_name = "COM1";
    QSerialPort::BaudRate baud_rate = QSerialPort::Baud19200;
    QSerialPort::DataBits data_bits = QSerialPort::Data8;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stop_bits = QSerialPort::OneStop;


};

#endif // COMMUNICATIONCOM_H
