#include "plc_siemens.h"





PLC_Siemens::PLC_Siemens()
{
    m_exitThread = false;

    MyS7Client = new TS7Client();
    connect();
}

PLC_Siemens::PLC_Siemens(QString ipAddress, int DB_No, int DB_Length)
{
    m_exitThread = false;

    MyS7Client = new TS7Client();
    this->ipAddress = ipAddress;
    this->DB_No = DB_No;
    this->DB_Length = DB_Length;
    connect();
}

PLC_Siemens::~PLC_Siemens()
{
    m_exitThread = false;
}

bool PLC_Siemens::connect()
{
    // LOGE_HIGH("start MyS7Client connect");
    // LOGE_HIGH("MyS7Client->Connected() :%d", MyS7Client->Connected());

    if(!MyS7Client->Connected())
    {
        // LOGE_HIGH("MyS7Client reconnect");

        return MyS7Client->ConnectTo(ipAddress.toStdString().c_str(), 0, 0);
    }
    else
    {
        // LOGE_HIGH("MyS7Client->Connected() :%d", MyS7Client->Connected());

        return MyS7Client->Connected();
    }
    // LOGE_HIGH("MyS7Client connect finish");
}

void PLC_Siemens::ReadCycle()
{
    int res;
    /// demo
    // res = MyS7Client->DBRead(1, 0, 30, &DB_Buffer);
    // if (res!=0)
    // {
    //     qDebug()<< "Error read from DB:" << "1 "<<QString::number(res);
    // }

    /// change jobworker


    // 所有结果
    // std::fill(std::begin(DB_Buffer), std::end(DB_Buffer), byte{0});
    res = MyS7Client->DBRead(DB_No, DB_StartID, DB_Length, &DB_Buffer);
    if(res != 0)
    {


    }
    this->msleep(ulong(100)); // 100000us=100ms
}

bool PLC_Siemens::WriteValue(dataType typ, int dbNum, int offset, void *val)
{
    // LOGE_HIGH("111");
    QMutexLocker locker(&mutex);
    // LOGE_HIGH("start Write to DB %d.DBB%d :%d", dbNum, offset, val);
    // LOGE_HIGH("222");

    if(MyS7Client->Connected())
    {
        // LOGE_HIGH("233");

        if(typ == eByte)
        {
            // LOGE_HIGH("234");
            int res = MyS7Client->DBWrite(dbNum, offset, 1, val);
            // qDebug()<< "Write to DB" << dbNum <<".DBB"<<offset << ", res= " <<QString::number(res);
            // LOGE_HIGH("Write to DB %d.DBB%d :%d, res=  %d", dbNum, offset, val, res);
            return res;
        }
        else if(typ == eReal)
        {

            // 1. 把 float 拆成 4 字节
            uint8_t bytes[4];
            memcpy(bytes, val, 4);

            LOGE("bytes[0]:%d", bytes[0]);
            LOGE("bytes[1]:%d", bytes[1]);
            LOGE("bytes[2]:%d", bytes[2]);
            LOGE("bytes[3]:%d", bytes[3]);


            // 2. 调整字节序为 Big-Endian（S7 要求）
            std::reverse(bytes, bytes + 4);  // 小端 → 大端

            LOGE("bytes[0]:%d", bytes[0]);
            LOGE("bytes[1]:%d", bytes[1]);
            LOGE("bytes[2]:%d", bytes[2]);
            LOGE("bytes[3]:%d", bytes[3]);

            // 3. 写入 4 字节
            int res = MyS7Client->DBWrite(dbNum, offset, 4, bytes);

            return res;
        }
        else
        {
            // LOGE_HIGH("235");
            return false;
        }
    }
    else
    {
        // LOGE_HIGH("244");

        //connect();
        return false;
    }
    // LOGE_HIGH("333");
    // return true;
}


uint PLC_Siemens::getUInt16(int Pos)
{
    return getUInt16(DB_Buffer, Pos);
}

uint PLC_Siemens::getUInt16(byte *Buffer, int Pos)
{
    return ((Buffer[Pos] << 8) | Buffer[Pos + 1]);
}

int PLC_Siemens::getInt16(byte *Buffer, int Pos)
{
    return ((Buffer[Pos] << 8) | Buffer[Pos + 1]);
}

int PLC_Siemens::getUInt32(int Pos)
{
    return PLC_Siemens::getUInt32(DB_Buffer, Pos);
}

uint PLC_Siemens::getUInt32(byte* Buffer, int Pos)
{
    uint Result;
    Result = Buffer[Pos];
    Result <<= 8;
    Result |= Buffer[Pos + 1];
    Result <<= 8;
    Result |= Buffer[Pos + 2];
    Result <<= 8;
    Result |= Buffer[Pos + 3];
    return Result;
}

int PLC_Siemens::getInt32(byte *Buffer, int Pos)
{
    int Result;
    Result = Buffer[Pos];
    Result <<= 8;
    Result += Buffer[Pos + 1];
    Result <<= 8;
    Result += Buffer[Pos + 2];
    Result <<= 8;
    Result += Buffer[Pos + 3];
    return Result;
}

double PLC_Siemens::getReal(byte *Buffer, int Pos)
{
    uint dw = getUInt32(Buffer, Pos);
    return *(pfloat(&dw));
}

void PLC_Siemens::run()
{
    while(m_exitThread == true)
    {

        if(MyS7Client->Connected())
        {
            QMutexLocker locker(&mutex);
            ReadCycle();
        }
        else
        {
            connect();
        }
        this->sleep(ulong(0.1 * 10e-6)); // 100000us=100ms
    }
}
