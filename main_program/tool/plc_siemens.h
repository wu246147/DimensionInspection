#ifndef PLC_SIEMENS_H
#define PLC_SIEMENS_H

#include "snap7.h"
#include <QThread>
#include <QDebug>
#include <QString>
#include <QMutex>

#include "mylog.h"

class PLC_Siemens : public QThread
{


    typedef float *pfloat;

public:
    enum dataType {eBit, eByte, eInt, eDInt, eWord, eDowrd, eReal};
    PLC_Siemens();
    PLC_Siemens(QString ipAddress, int DB_No, int DB_Length);
    ~PLC_Siemens();


    ///
    /// \brief connect 连接服务器
    /// \return
    ///
    bool connect();

    ///
    /// \brief ReadCycle 循环读取指定位置列表值
    ///
    void ReadCycle();

    ///
    /// \brief WriteValue 在指定位置写入数值
    /// \param typ
    /// \param dbNum
    /// \param offset
    /// \param val
    /// \return
    ///
    bool WriteValue(dataType typ, int dbNum, int offset,  void * val);
    // 读写时用到的锁
    QMutex mutex;

    /// byte 转 不同类型数据
    uint getUInt16(int Pos);
    uint getUInt16(byte* Buffer, int Pos);

    int getInt16(int Pos);
    int getInt16(byte* Buffer, int Pos);

    int getUInt32(int Pos);
    uint getUInt32(byte* Buffer, int Pos);

    int getInt32(int Pos);
    int getInt32(byte* Buffer, int Pos);

    double getReal(int Pos);
    double getReal(byte* Buffer, int Pos);
    // 通讯参数
    QString ipAddress = "192.168.1.5";
    int DB_No = 1;
    int DB_Length = 50;
    int DB_StartID = 0;

    //通讯对象
    TS7Client *MyS7Client = nullptr;
    //通讯结果参数
    byte DB_Buffer[200];
    //通讯连接
    bool m_exitThread = false;

// signals:
//     ///
//     /// \brief reserveChangeFileSignal 切换作业信号
//     /// \param fileName 切换作业名
//     ///
//     void reserveChangeFileSignal(std::string fileName);

//     ///
//     /// \brief reserveResetDataSignal  重置信号
//     /// \param instance_init_name 实例初始化名称
//     ///
//     void reserveResetDataSignal();

protected:
    void run();
private:
    int old_value;
};

#endif // PLC_SIEMENS_H
