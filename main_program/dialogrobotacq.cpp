#include "dialogrobotacq.h"
#include "ui_dialogrobotacq.h"

DialogRobotAcq::DialogRobotAcq(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRobotAcq)
{
    ui->setupUi(this);
}

DialogRobotAcq::~DialogRobotAcq()
{
    delete ui;
}

DialogRobotAcq::DialogRobotAcq(JobManager *j, CommunicationModbus *c, COM *com, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogRobotAcq)
{
    ui->setupUi(this);
    jobmanager = j;
    communicationModbus = c;

    communicationCOM = com;
}

void DialogRobotAcq::on_pushButton_acq_clicked()
{
    //开灯
    communicationCOM->serial->write("SC0255#");
    QThread::msleep(10);

    int r = jobmanager->checkCam(0, 0);

    //关灯
    int sleepTime = jobmanager->cams[0].exposure_time / 1000;
    sleepTime += 10;
    QThread::msleep(sleepTime);
    communicationCOM->serial->write("SC0000#");
}


void DialogRobotAcq::on_pushButton_read_coordinates_clicked()
{
    float x, y, z, rx, ry, rz;
    std::string msg;
    bool rt  = communicationModbus->readFloat(1, 128, QModbusDataUnit::InputRegisters, x, msg);
    if(rt)
    {
        LOGE("x:%f", x);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus->readFloat(1, 130, QModbusDataUnit::InputRegisters, y, msg);
    if(rt)
    {
        LOGE("y:%f", y);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus->readFloat(1, 132, QModbusDataUnit::InputRegisters, z, msg);
    if(rt)
    {
        LOGE("z:%f", z);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus->readFloat(1, 134, QModbusDataUnit::InputRegisters, rx, msg);
    if(rt)
    {
        LOGE("rx:%f", rx);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus->readFloat(1, 136, QModbusDataUnit::InputRegisters, ry, msg);
    if(rt)
    {
        LOGE("ry:%f", ry);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus->readFloat(1, 138, QModbusDataUnit::InputRegisters, rz, msg);
    if(rt)
    {
        LOGE("rz:%f", rz);
    }
    else
    {
        LOGE("读取坐标失败");
    }

    robotPoints.clear();
    robotPoints.push_back(x);
    robotPoints.push_back(y);
    robotPoints.push_back(z);
    robotPoints.push_back(rx);
    robotPoints.push_back(ry);
    robotPoints.push_back(rz);

    //更新界面
    ui->doubleSpinBox_calib_robotX->setValue(robotPoints[0]);
    ui->doubleSpinBox_calib_robotY->setValue(robotPoints[1]);
    ui->doubleSpinBox_calib_robotZ->setValue(robotPoints[2]);
    ui->doubleSpinBox_calib_robotRX->setValue(robotPoints[3]);
    ui->doubleSpinBox_calib_robotRY->setValue(robotPoints[4]);
    ui->doubleSpinBox_calib_robotRZ->setValue(robotPoints[5]);

}


void DialogRobotAcq::on_pushButton_move_and_acq_clicked()
{
    int wait_count = 10;
    int cumulative_count = 0;
    //发送移动坐标
    float centerx, centery, centerz, centerrx, centerry, centerrz;
    float dx, dy, dz, drx, dry, drz;

    centerx = ui->doubleSpinBox_calib_centerX->value();
    centery = ui->doubleSpinBox_calib_centerY->value();
    centerz = ui->doubleSpinBox_calib_centerZ->value();

    centerrx = ui->doubleSpinBox_calib_centerRX->value();
    centerry = ui->doubleSpinBox_calib_centerRY->value();
    centerrz = ui->doubleSpinBox_calib_centerRZ->value();

    dx = ui->doubleSpinBox_calib_moveX->value();
    dy = ui->doubleSpinBox_calib_moveY->value();
    dz = ui->doubleSpinBox_calib_moveZ->value();

    drx = ui->doubleSpinBox_calib_moveRX->value();
    dry = ui->doubleSpinBox_calib_moveRY->value();
    drz = ui->doubleSpinBox_calib_moveRZ->value();

    centerPoints.clear();
    centerPoints.push_back(centerx);
    centerPoints.push_back(centery);
    centerPoints.push_back(centerz);
    centerPoints.push_back(centerrx);
    centerPoints.push_back(centerry);
    centerPoints.push_back(centerrz);

    movePoints.clear();
    movePoints.push_back(dx);
    movePoints.push_back(dy);
    movePoints.push_back(dz);
    movePoints.push_back(drx);
    movePoints.push_back(dry);
    movePoints.push_back(drz);

    std::string msg;
    bool rt;
    float x, y, z, rx, ry, rz;
    x = centerx + dx;
    y = centery + dy;
    z = centerz + dz;

    rx = centerrx + drx;
    ry = centerry + dry;
    rz = centerrz + drz;


    rt = communicationModbus->writeFloat(1, 132, QModbusDataUnit::HoldingRegisters, x, msg);
    if(rt)
    {
        LOGE("x:%f", x);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus->writeFloat(1, 134, QModbusDataUnit::HoldingRegisters, y, msg);
    if(rt)
    {
        LOGE("y:%f", y);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus->writeFloat(1, 136, QModbusDataUnit::HoldingRegisters, z, msg);
    if(rt)
    {
        LOGE("z:%f", z);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus->writeFloat(1, 138, QModbusDataUnit::HoldingRegisters, rx, msg);
    if(rt)
    {
        LOGE("rx:%f", rx);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus->writeFloat(1, 140, QModbusDataUnit::HoldingRegisters, ry, msg);
    if(rt)
    {
        LOGE("ry:%f", ry);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus->writeFloat(1, 142, QModbusDataUnit::HoldingRegisters, rz, msg);
    if(rt)
    {
        LOGE("rz:%f", rz);
    }
    else
    {
        LOGE("读取坐标失败");
    }

    //发送启动信号
    rt  = communicationModbus->writeInt(1, 50, QModbusDataUnit::Coils, 1, msg);

    //等待取像信号
    wait_count = 100;
    cumulative_count = 0;
    while(1)
    {
        int value = -1;
        rt  = communicationModbus->readInt(1, 28, QModbusDataUnit::DiscreteInputs, value, msg);
        if(value == 1)
        {
            break;
        }
        QThread::msleep(100);
        cumulative_count += 1;
        if(cumulative_count >= wait_count)
        {
            // std::cout << "没有收到取像开始信号" << std::endl;
            LOGE("没有收到取像开始信号");
            break;
        }

    }
    //关闭启动信号
    rt  = communicationModbus->writeInt(1, 50, QModbusDataUnit::Coils, 0, msg);

    //接收坐标信号
    on_pushButton_read_coordinates_clicked();

    //发送取像完成信号
    rt  = communicationModbus->writeInt(1, 51, QModbusDataUnit::Coils, 1, msg);

    //等待取像信号结束
    wait_count = 10;
    cumulative_count = 0;
    while(1)
    {
        int value = -1;
        rt  = communicationModbus->readInt(1, 28, QModbusDataUnit::DiscreteInputs, value, msg);
        if(value == 0)
        {
            break;
        }
        QThread::msleep(100);
        cumulative_count += 1;
        if(cumulative_count >= wait_count)
        {
            // std::cout << "没有收到取像结束信号" << std::endl;
            LOGE("没有收到取像结束信号");
            break;
        }
    }

    //取像
    on_pushButton_acq_clicked();

    QThread::msleep(100);

    //关闭取像完成信号
    rt  = communicationModbus->writeInt(1, 51, QModbusDataUnit::Coils, 0, msg);

}


void DialogRobotAcq::on_pushButton_save_data_clicked()
{
    if(jobmanager->jobworkers[cam_id].img.empty())
    {
        LOGE("标定图片为空");
        return;
    }

    //读取机器人坐标
    float x, y, z, rx, ry, rz;
    x = robotPoints[0];
    y = robotPoints[1];
    z = robotPoints[2];
    rx = robotPoints[3];
    ry = robotPoints[4];
    rz = robotPoints[5];


    //保存图片 calibration/time/2d.bmp
    QDateTime currentDateTime = QDateTime::currentDateTime();
    std::string create_time = currentDateTime.toString("yyyyMMdd_hhmmss").toStdString();
    LOGE("create_time:%s", create_time.data());

    std::string save_img_path = "calibration/" + create_time + "/2d.bmp";


    createMultipleFolders(QFileInfo(save_img_path.data()).absolutePath());

    cv::imwrite(save_img_path, jobmanager->jobworkers[0].img);


    //保存数据 calibration/robotCoordinate.csv
    std::string save_file_path = "calibration/robotCoordinate.csv";

    createMultipleFolders(QFileInfo(save_file_path.data()).absolutePath());
    QList<QStringList>dataList;
    QStringList data;
    data.append(create_time.data());
    data.append(std::to_string(x).data());
    data.append(std::to_string(y).data());
    data.append(std::to_string(z).data());
    data.append(std::to_string(rx).data());
    data.append(std::to_string(ry).data());
    data.append(std::to_string(rz).data());

    dataList.append(data);
    saveCsvSimple(save_file_path.data(), dataList);
}

