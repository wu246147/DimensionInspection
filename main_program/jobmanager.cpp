#include "jobmanager.h"

// #include <QtGlobal>   // 里面定义 Q_BIG_ENDIAN / Q_LITTLE_ENDIAN



float pointToLineDistance3D(const cv::Vec3f& A,
                            const cv::Vec3f& B,
                            const cv::Vec3f& P)
{
    // 方向向量 AB
    cv::Vec3f AB = B - A;
    // 向量 AP
    cv::Vec3f AP = P - A;

    // 叉乘的模长 ||AB × AP||
    cv::Vec3f cross = AB.cross(AP);
    float normCross = std::sqrt(cross.dot(cross));

    // 方向向量模长 ||AB||
    float normAB = std::sqrt(AB.dot(AB));

    // 距离 = ||AB × AP|| / ||AB||
    return (normAB > 1e-6f) ? (normCross / normAB) : 0.0f;
}

/**************************************************
* @brief    检查是否是旋转矩阵
* @note
* @param
* @param
* @param
* @return  true : 是旋转矩阵， false : 不是旋转矩阵
**************************************************/
bool isRotatedMatrix(cv::Mat& R)        //旋转矩阵的转置矩阵是它的逆矩阵，逆矩阵 * 矩阵 = 单位矩阵
{
    cv::Mat temp33 = R({ 0, 0, 3, 3 }); //无论输入是几阶矩阵，均提取它的三阶矩阵
    cv::Mat Rt;
    cv::transpose(temp33, Rt);  //转置矩阵
    cv::Mat shouldBeIdentity = Rt * temp33;//是旋转矩阵则乘积为单位矩阵
    cv::Mat I = cv::Mat::eye(3, 3, shouldBeIdentity.type());
    return cv::norm(I, shouldBeIdentity) < 1e-6;
}
/**************************************************
* @brief   欧拉角转换为旋转矩阵
* @note
* @param    const std::string& seq  指定欧拉角的排列顺序；（机械臂的位姿类型有xyz,zyx,zyz几种，需要区分）
* @param    const Mat& eulerAngle   欧拉角（1*3矩阵）, 角度值
* @param
* @return   返回3*3旋转矩阵
**************************************************/
cv::Mat eulerAngleToRotateMatrix(const cv::Mat& eulerAngle, const std::string& seq)
{
    CV_Assert(eulerAngle.rows == 1 && eulerAngle.cols == 3);//检查参数是否正确
    eulerAngle /= (180 / CV_PI);        //度转弧度
    cv::Matx13d m(eulerAngle);                //<double, 1, 3>
    auto rx = m(0, 0), ry = m(0, 1), rz = m(0, 2);
    auto rxs = sin(rx), rxc = cos(rx);
    auto rys = sin(ry), ryc = cos(ry);
    auto rzs = sin(rz), rzc = cos(rz);
    //XYZ方向的旋转矩阵
    cv::Mat RotX = (cv::Mat_<double>(3, 3) << 1, 0, 0,
                    0, rxc, -rxs,
                    0, rxs, rxc);
    cv::Mat RotY = (cv::Mat_<double>(3, 3) << ryc, 0, rys,
                    0,      1, 0,
                    -rys, 0, ryc);
    cv::Mat RotZ = (cv::Mat_<double>(3, 3) << rzc, -rzs, 0,
                    rzs, rzc, 0,
                    0, 0, 1);
    //按顺序合成后的旋转矩阵
    cv::Mat rotMat;
    if(seq == "zyx")
    {
        rotMat = RotX * RotY * RotZ;
    }
    else if(seq == "yzx")
    {
        rotMat = RotX * RotZ * RotY;
    }
    else if(seq == "zxy")
    {
        rotMat = RotY * RotX * RotZ;
    }
    else if(seq == "yxz")
    {
        rotMat = RotZ * RotX * RotY;
    }
    else if(seq == "xyz")
    {
        rotMat = RotZ * RotY * RotX;
    }
    else if(seq == "xzy")
    {
        rotMat = RotY * RotZ * RotX;
    }
    else
    {
        std::cout << "Euler Angle Sequence string is wrong...";
    }
    if(!isRotatedMatrix(rotMat))         //欧拉角特殊情况下会出现死锁
    {
        std::cout << "Euler Angle convert to RotatedMatrix failed..." << std::endl;
        exit(-1);
    }
    std::cout << RotX << std::endl;
    std::cout << RotY << std::endl;
    std::cout << RotZ << std::endl;
    return rotMat;
}

/**************************************************
* @brief   将四元数转换为旋转矩阵
* @note
* @param   const Vec4d& q   归一化的四元数: q = q0 + q1 * i + q2 * j + q3 * k;
* @return  返回3*3旋转矩阵R
**************************************************/
cv::Mat quaternionToRotatedMatrix(const cv::Vec4d& q)
{
    double q0 = q[0], q1 = q[1], q2 = q[2], q3 = q[3];
    double q0q0 = q0 * q0, q1q1 = q1 * q1, q2q2 = q2 * q2, q3q3 = q3 * q3;
    double q0q1 = q0 * q1, q0q2 = q0 * q2, q0q3 = q0 * q3;
    double q1q2 = q1 * q2, q1q3 = q1 * q3;
    double q2q3 = q2 * q3;
    //根据公式得来
    cv::Mat RotMtr = (cv::Mat_<double>(3, 3) << (q0q0 + q1q1 - q2q2 - q3q3), 2 * (q1q2 + q0q3), 2 * (q1q3 - q0q2),
                      2 * (q1q2 - q0q3), (q0q0 - q1q1 + q2q2 - q3q3), 2 * (q2q3 + q0q1),
                      2 * (q1q3 + q0q2), 2 * (q2q3 - q0q1), (q0q0 - q1q1 - q2q2 + q3q3));
    //这种形式等价
    /*Mat RotMtr = (Mat_<double>(3, 3) << (1 - 2 * (q2q2 + q3q3)), 2 * (q1q2 - q0q3), 2 * (q1q3 + q0q2),
                                         2 * (q1q2 + q0q3), 1 - 2 * (q1q1 + q3q3), 2 * (q2q3 - q0q1),
                                         2 * (q1q3 - q0q2), 2 * (q2q3 + q0q1), (1 - 2 * (q1q1 + q2q2)));*/
    return RotMtr;
}

/**************************************************
* @brief      将采集的原始数据转换为齐次矩阵（从机器人控制器中获得的）
* @note
* @param      Mat& m    1*6//1*10矩阵 ， 元素为： x,y,z,rx,ry,rz  or x,y,z, q0,q1,q2,q3,rx,ry,rz
* @param      bool useQuaternion      原始数据是否使用四元数表示
* @param      string& seq         原始数据使用欧拉角表示时，坐标系的旋转顺序
* @return      返回转换完的齐次矩阵
**************************************************/
void attitudeVectorToMatrix(const cv::Mat& m, bool useQuaternion, const char* seq, cv::Mat &temp)
{
    CV_Assert(m.total() == 6 || m.total() == 10);
    //if (m.cols == 1)    //转置矩阵为行矩阵
    //    m = m.t();
    temp = cv::Mat::eye(4, 4, CV_64FC1);
    if(useQuaternion)
    {
        cv::Vec4d quaternionVec = m({ 3, 0, 4, 1 }).clone(); //读取存储的四元数
        quaternionToRotatedMatrix(quaternionVec).copyTo(temp({0, 0, 3, 3}));
    }
    else
    {
        // std::cout << "4441"  << std::endl;
        cv::Mat rotVec;
        if(m.total() == 6)
        {
            // std::cout << "44411"  << std::endl;
            // std::cout << "m:" << m  << std::endl;
            // std::cout << "m size:" << m.size()  << std::endl;
            rotVec = m({ 3, 0, 3, 1 }).clone(); //读取存储的欧拉角
            // std::cout << "44412"  << std::endl;
        }
        if(m.total() == 10)
        {
            rotVec = m({ 7, 0, 3, 1 }).clone();
        }
        // std::cout << "4442"  << std::endl;
        //如果seq为空，表示传入的是3*1旋转向量，否则，传入的是欧拉角
        if(0 == std::string(seq).compare(""))
        {
            // std::cout << "4443"  << std::endl;
            Rodrigues(rotVec, temp({ 0, 0, 3, 3 })); //罗德利斯转换
            // std::cout << "4444"  << std::endl;
        }
        else
        {
            eulerAngleToRotateMatrix(rotVec, seq).copyTo(temp({ 0, 0, 3, 3 }));
        }
    }
    //存入平移矩阵
    temp({ 3, 0, 1, 3 }) = m({ 0, 0, 3, 1 }).t();
}


void readXml(std::string filePath, std::vector<double>&xList, std::vector<double>&yList, std::vector<double>&zList,
             std::vector<double>&rxList, std::vector<double>&ryList, std::vector<double>&rzList)
{
    QFile file(filePath.data());
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream stream(&file);
    QString line = stream.readLine();
    while(!stream.atEnd())
    {
        line = stream.readLine();
        // std::cout << line.toStdString().data() << std::endl;
        QStringList row = line.split(',', Qt::SkipEmptyParts);
        xList.push_back(row[1].toDouble());
        yList.push_back(row[2].toDouble());
        zList.push_back(row[3].toDouble());
        rxList.push_back(row[4].toDouble());
        ryList.push_back(row[5].toDouble());
        rzList.push_back(row[6].toDouble());
    }
    file.close();
}


JobManager::JobManager()
{
}

JobManager::JobManager(int _cam_count, int _jobworker_count)
{
    jobworker_num = _jobworker_count;
    for(int i = 0; i < jobworker_num; i++)
    {
        JobWorker jobworker(i);
        jobworkers.push_back(jobworker);
    }
    cam_num = _cam_count;
    for(int i = 0; i < cam_num; i++)
    {
        Cam cam(i);
        cams.push_back(cam);
    }

    //读取标定参数
    std::string path = "calib/Homo_cam2base.tiff";
    if(QFile(path.data()).exists())
    {
        T_Cam2Gripper = cv::imread(path, -1);
        // std::cout << "T_Cam2Gripper:" << cv::format(T_Cam2Gripper, cv::Formatter::FMT_NUMPY) << std::endl;
    }

    std::string path2 = "calib/mtx9.tiff";

    if(QFile(path2.data()).exists())
    {
        H_Pix2Tool = cv::imread(path2, -1);
        // std::cout << "H_Pix2Tool:" << cv::format(H_Pix2Tool, cv::Formatter::FMT_NUMPY) << std::endl;

    }

    //读取取像坐标
    std::string path3 = "calib/robotCoordinate.csv";

    if(QFile(path3.data()).exists())
    {
        std::vector<double>xList;
        std::vector<double>yList;
        std::vector<double>zList;
        std::vector<double>rxList;
        std::vector<double>ryList;
        std::vector<double>rzList ;
        readXml(path3, xList, yList, zList, rxList, ryList, rzList);

        cv::Mat robotVector = cv::Mat::zeros(cv::Size(6, 1), CV_64FC1);

        robotVector.at<double>(0, 0) = xList[0];
        robotVector.at<double>(0, 1) = yList[0];
        robotVector.at<double>(0, 2) = zList[0];
        robotVector.at<double>(0, 3) = rxList[0];
        robotVector.at<double>(0, 4) = ryList[0];
        robotVector.at<double>(0, 5) = rzList[0];
        attitudeVectorToMatrix(robotVector, false, "xyz", H_gripper2base1);
        // std::cout << "robotVector:" << cv::format(robotVector, cv::Formatter::FMT_NUMPY) << std::endl;

        robotVector.at<double>(0, 0) = xList[1];
        robotVector.at<double>(0, 1) = yList[1];
        robotVector.at<double>(0, 2) = zList[1];
        robotVector.at<double>(0, 3) = rxList[1];
        robotVector.at<double>(0, 4) = ryList[1];
        robotVector.at<double>(0, 5) = rzList[1];
        attitudeVectorToMatrix(robotVector, false, "xyz", H_gripper2base2);
        // std::cout << "robotVector:" << cv::format(robotVector, cv::Formatter::FMT_NUMPY) << std::endl;

    }

    // 读取标定板距离
    std::string path4 = "calib/distanceMtx.tiff";

    if(QFile(path4.data()).exists())
    {
        distanceMtx = cv::imread(path4, -1);
        std::cout << "distanceMtx:" << cv::format(distanceMtx, cv::Formatter::FMT_NUMPY) << std::endl;

    }

    // 读取相机内参
    std::string path5 = "calib/mtx.tiff";

    if(QFile(path5.data()).exists())
    {
        cameraMatrix = cv::imread(path5, -1);

    }

    // 读取相机畸变系数
    std::string path6 = "calib/dist.tiff";

    if(QFile(path6.data()).exists())
    {
        distCoeffs = cv::imread(path6, -1);
    }
}

JobManager &JobManager::operator =(const JobManager &jobmanager)
{
    this->jobworker_num = jobmanager.jobworker_num;
    this->jobworkers = jobmanager.jobworkers;
    this->cam_num = jobmanager.cam_num;
    this->cams = jobmanager.cams;


    this->distance_thre_min = jobmanager.distance_thre_min;
    this->distance_thre_max = jobmanager.distance_thre_max;
    this->station_1_delay = jobmanager.station_1_delay;
    this->station_2_delay = jobmanager.station_2_delay;
    this->output_delay = jobmanager.output_delay;
    this->station_1_output_cam = jobmanager.station_1_output_cam;
    this->station_1_output_line = jobmanager.station_1_output_line;

    this->T_Cam2Gripper = jobmanager.T_Cam2Gripper;
    this->H_Pix2Tool = jobmanager.H_Pix2Tool;
    this->H_gripper2base1 = jobmanager.H_gripper2base1;
    this->H_gripper2base2 = jobmanager.H_gripper2base2;

    this->distanceMtx = jobmanager.distanceMtx;

    this->cameraMatrix = jobmanager.cameraMatrix;
    this->distCoeffs = jobmanager.distCoeffs;

    return *this;
}

int JobManager::mvs_open_device()
{
    LOGE_HIGH("mvs_open_device");
    bool rt = true;
    // 枚举Interface
    if(m_cam_connect_type == cam_connect_type::mvs_network)
    {
        int nRet = CMvCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);
        for(int i = 0; i < cams.size() ; i++)
        {
            LOGE_HIGH("m_stDevList.nDeviceNum:%d", m_stDevList.nDeviceNum);
            //遍历设备，配置ip段位与jobworker id相匹配的相机
            int cam_index = -1;
            for(int j = 0; j < m_stDevList.nDeviceNum ; j++)
            {
                int ip_port = -1;
                // ip_port = int((m_stDevList.pDeviceInfo[j]->SpecialInfo.stGigEInfo.nNetExport - 192 * 256 * 256 * 256 - 168 * 256 * 256) / 256);
                auto cam_name = m_stDevList.pDeviceInfo[j]->SpecialInfo.stGigEInfo.chUserDefinedName;
                LOGE("cam_name:%s。", cam_name);
                // if(ip_port == i)
                // {
                //     cam_index = j;
                // }
                char _name[256];
                sprintf(_name, "cam%d", i + 1);
                std::string _name_str = _name;
                if(_name_str.compare(reinterpret_cast<char * >(cam_name)) == 0)
                {
                    cam_index = j;
                }
            }
            if(cam_index == -1)
            {
                LOGE("无法找到到相机%d，请确认相机是否已经启动，或ip是否设置正确。", i);
                continue;
            }
            cams[i].m_pcMyCamera = new CMvCamera;
            if(NULL == cams[i].m_pcMyCamera)
            {
                rt = false;
            }
            //连接相机
            nRet = cams[i].m_pcMyCamera->Open(m_stDevList.pDeviceInfo[cam_index]);
            if(MV_OK != nRet)
            {
                delete cams[i].m_pcMyCamera;
                cams[i].m_pcMyCamera = NULL;
                LOGE("相机%d无法连接，请确认相机是否被其他软件占用中。", i);
                // LOGE(std::string("相机" + std::to_string(i) + " 无法连接，请确认相机是否被其他软件占用中。").data());
                // QMessageBox::warning(this, "无法连接到相机.", ("相机" + std::to_string(i) + " 无法连接，请确认相机是否被其他软件占用中。").data(), QMessageBox::StandardButton::Ok);
                rt = false;
            }
            cams[i].b_open_device = true;
        }
    }
    else
    {
        rt = false;
        LOGE(("没有这样的相机类型。请确认你选择的相机类型没有错."));
        // QMessageBox::warning(this, "没有这样的相机类型.", "没有这样的相机类型。请确认你选择的相机类型没有错.", QMessageBox::StandardButton::Ok);
    }
    return rt;
}

int JobManager::mvs_close_device()
{
    bool rt = true;
    for(int i = 0; i < cams.size() ; i++)
    {
        cams[i].b_thread_state = false;
        //关闭线程
        if(cams[i].h_grab_thread)
        {
            cams[i].fut.waitForFinished();
            cams[i].h_grab_thread = NULL;
        }
        if(cams[i].m_pcMyCamera)
        {
            cams[i].m_pcMyCamera->Close();
            delete cams[i].m_pcMyCamera;
            cams[i].m_pcMyCamera = NULL;
        }
        cams[i].b_start_grabbing = false;
        cams[i].b_open_device = false;
    }
    return rt;
}

void JobManager::mvs_stop_grab()
{
    for(int i = 0; i < cams.size() ; i++)
    {
        if(false == cams[i].b_open_device ||
                false == cams[i].b_start_grabbing ||
                NULL == cams[i].m_pcMyCamera)
        {
            continue;
        }
        cams[i].b_thread_state = false;
        if(cams[i].h_grab_thread)
        {
            cams[i].fut.waitForFinished();
            cams[i].h_grab_thread = NULL;
        }
        int nRet = cams[i].m_pcMyCamera->StopGrabbing();
        if(MV_OK != nRet)
        {
            continue;
        }
        cams[i].b_start_grabbing = false;
    }
}

void JobManager::acqfifo_complete_event(int cam_id, int worker_id)
{
    jobworkers[worker_id].acqfifo_complete(cams[cam_id].img);
    // if(cam_id == 0)
    // {
    //     jobworkers[0].acqfifo_complete(cams[0].img);
    //     cams[1].acq_count = 0;
    // }
    // else
    // {
    //     if(cams[1].acq_count % 2 == 1)
    //     {
    //         jobworkers[1].acqfifo_complete(cams[1].img);
    //     }
    //     else
    //     {
    //         jobworkers[2].acqfifo_complete(cams[1].img);
    //     }
    // }
}

void JobManager::mvs_start_grab()
{
    for(int i = 0; i < cams.size() ; i++)
    {
        if(false == cams[i].b_open_device ||
                true == cams[i].b_start_grabbing ||
                NULL == cams[i].m_pcMyCamera)
        {
            continue;
        }
        cams[i].b_thread_state = true;
        cams[i].fut = QtConcurrent::run(&cams[i], &Cam::acqfifo_complete);
        //添加取像完毕信号槽关联
        reg_acqfifo_complete_event(i);
        int nRet = cams[i]. m_pcMyCamera->StartGrabbing();
        if(MV_OK != nRet)
        {
            cams[i].b_thread_state = false;
            continue;
        }
        cams[i].b_start_grabbing = true;
    }
}


void JobManager::set_cam_para()
{
    for(int i = 0; i < cams.size(); i++)
    {
        if(cams[i].m_pcMyCamera == nullptr)
        {
            continue;
        }
        try
        {
            //设置曝光
            cams[i].m_pcMyCamera->SetFloatValue("ExposureTime", cams[i].exposure_time);
            //设置触发延时
            cams[i].m_pcMyCamera->SetFloatValue("TriggerDelay", cams[i].trigger_delay);
            //设置增益
            cams[i].m_pcMyCamera->SetFloatValue("Gain", cams[i].gain);
        }
        catch(std::exception &e)
        {
            LOGE_PROFESSIONAL(e.what());
        }
    }
}


void JobManager::station_1_output()
{
    if(cams[station_1_output_cam].m_pcMyCamera == nullptr)
    {
        return;
    }
    //大华相机
    // QThread::msleep(station_1_delay);
    // jobworkers[station_1_output_cam].m_pcMyCamera->SetEnumValue("LineSelector", 0);
    // jobworkers[station_1_output_cam].m_pcMyCamera->SetBoolValue("LineOutputValue", false);
    // jobworkers[station_1_output_cam].m_pcMyCamera->SetBoolValue("LineInverter", true);
    // QThread::msleep(output_delay);
    // jobworkers[station_1_output_cam].m_pcMyCamera->SetBoolValue("LineInverter", false);
    //海康相机
    QThread::msleep(station_1_delay);
    cams[station_1_output_cam].m_pcMyCamera->SetEnumValue("LineSelector", station_1_output_line);
    cams[station_1_output_cam].m_pcMyCamera->SetEnumValue("LineSource", 5);
    cams[station_1_output_cam].m_pcMyCamera->SetBoolValue("StrobeEnable", true);
    cams[station_1_output_cam].m_pcMyCamera->SetIntValue("StrobeLineDuration", output_delay * 1000);
    cams[station_1_output_cam].m_pcMyCamera->CommandExecute("LineTriggerSoftware");
}

void JobManager::station_2_output()
{
    if(cams[2].m_pcMyCamera == nullptr)
    {
        return;
    }
    //延时剔除
    QThread::msleep(station_2_delay);
    //设置输出接口
    cams[2].m_pcMyCamera->SetEnumValue("LineSelector", 0);
    //设置输出低电平
    cams[2].m_pcMyCamera->SetBoolValue("LineOutputValue", false);
    //开始输出
    cams[2].m_pcMyCamera->SetBoolValue("LineInverter", true);
    QThread::msleep(output_delay);
    //停止输出
    cams[2].m_pcMyCamera->SetBoolValue("LineInverter", false);
}

void JobManager::station_1_output(int delay_time)
{
    if(cams[station_1_output_cam].m_pcMyCamera == nullptr)
    {
        return;
    }
    try
    {
        //大华相机
        //    QThread::msleep(delay_time);
        //    jobworkers[station_1_output_cam].m_pcMyCamera->SetEnumValue("LineSelector", station_1_output_line);
        //    jobworkers[station_1_output_cam].m_pcMyCamera->SetBoolValue("LineOutputValue", false);
        //    jobworkers[station_1_output_cam].m_pcMyCamera->SetBoolValue("LineInverter", true);
        //    QThread::msleep(output_delay);
        //    jobworkers[station_1_output_cam].m_pcMyCamera->SetBoolValue("LineInverter", false);
        //海康相机
        //    std::cout << "start output info" << std::endl;
        QThread::msleep(delay_time);
        cams[station_1_output_cam].m_pcMyCamera->SetEnumValue("LineSelector", station_1_output_line);
        cams[station_1_output_cam].m_pcMyCamera->SetEnumValue("LineSource", 5);
        cams[station_1_output_cam].m_pcMyCamera->SetBoolValue("StrobeEnable", true);
        cams[station_1_output_cam].m_pcMyCamera->SetIntValue("StrobeLineDuration", output_delay * 1000);
        cams[station_1_output_cam].m_pcMyCamera->CommandExecute("LineTriggerSoftware");
        //    std::cout << "finish output info" << std::endl;
        //    jobworkers[station_1_output_cam].m_pcMyCamera->SetBoolValue("LineOutputValue", false);
        //    jobworkers[station_1_output_cam].m_pcMyCamera->SetBoolValue("LineInverter", true);
        //    QThread::msleep(output_delay);
        //    jobworkers[station_1_output_cam].m_pcMyCamera->SetBoolValue("LineInverter", false);
    }
    catch(cv::Exception &e)
    {
        LOGE_HIGH(e.what());
    }
    catch(std::exception &e)
    {
        LOGE_HIGH(e.what());
    }
    catch(QException &e)
    {
        LOGE_HIGH(e.what());
    }
    catch(...)
    {
        LOGE_HIGH("other error.");
    }
}



// void JobManager::tcp_server_init()
// {
//     // for(int id = 0; id < jobworkers.size(); ++id)
//     // {
//     //     jobworkers[id].tcp_server_init();
//     // }
//     LOGE_HIGH("TCP start Init.");
//     if(m_tcpServer == nullptr)
//     {
//         m_tcpServer = new QTcpServer(this);
//     }
//     connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(tcp_socker_connected()));
//     QString Ip = QString(plc_ip.data());
//     quint16 port = net_port;
//     QHostAddress addr(Ip);
//     m_tcpServer->listen(addr, port);
//     LOGE_HIGH("TCP finish Init.");
// }

// void JobManager::tcp_server_dispose()
// {
//     // for(int id = 0; id < jobworkers.size(); ++id)
//     // {
//     //     jobworkers[id].tcp_server_dispose();
//     // }
//     LOGE_HIGH("TCP start dispose.");
//     if(m_tcpServer != nullptr)
//     {
//         disconnect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(tcp_socker_connected()));
//         if(m_tcpServer->isListening())
//         {
//             m_tcpServer->close();
//         }
//         delete  m_tcpServer;
//         m_tcpServer = nullptr;
//     }
//     LOGE_HIGH("TCP finish dispose.");
// }

// void JobManager::tcp_socker_connected()
// {
//     m_tcpSocket = m_tcpServer->nextPendingConnection();   //创建 socket
//     connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(tcp_socker_disconnected()));
//     connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(tcp_ready_read()));
//     std::cout << "connect success!!!" << std::endl;
// }

// void JobManager::tcp_socker_disconnected()
// {
//     std::cout << "disconnect success!!!" << std::endl;
// }

// void JobManager::tcp_ready_read()
// {
//     try
//     {
//         QString info = m_tcpSocket->readAll();
//         info = info.remove('\r');
//         std::cout << "receive tcp:" << info.toStdString() << std::endl;
//         QStringList infoList = info.split(',');
//         //添加通讯逻辑
//         if(infoList[0].startsWith("T") == 0)
//         {
//             //手动定位
//             if(infoList[1].compare("S") == 0)
//             {
//                 if(infoList[2].compare("0") == 0)
//                 {
//                     // isAutoMode = true;
//                     changeRunModelSignal(true);
//                 }
//                 else if(infoList[2].compare("1") == 0)
//                 {
//                     // isAutoMode = false;
//                     changeRunModelSignal(false);
//                 }
//                 //结果发送
//                 std::string info = "T" + infoList[0].mid(1, 1).toStdString() + ",S," + infoList[2].toStdString() + ",0\r";
//                 m_tcpSocket->write(info.data());
//             }
//             else
//             {
//                 //自动标定/定位
//                 if(infoList[0].mid(1, 1).compare("1") == 0)
//                 {
//                     int r = checkCam(0);
//                     if(r == -1)
//                     {
//                         //
//                         std::string info = "T1,0,0,0,0\r";
//                         m_tcpSocket->write(info.data());
//                     }
//                 }
//                 else if(infoList[0].mid(1, 1).contains("2") == 0)
//                 {
//                     int r = checkCam(1);
//                     if(r == -1)
//                     {
//                         std::string info = "T2,0,0,0,0\r";
//                         m_tcpSocket->write(info.data());
//                     }
//                 }
//             }
//         }
//         //切换display
//         else if(infoList[0].compare("SWC") == 0)
//         {
//             int tmp_cam_id =  infoList[1].toInt();
//             changeDisplaySignal(tmp_cam_id);
//             std::string info = "SWC," + infoList[1].toStdString() + ",0\r";
//             m_tcpSocket->write(info.data());
//         }
//         //切换模式
//         else if(infoList[0].compare("SWM") == 0)
//         {
//             if(infoList[1].compare("R") == 0)
//             {
//                 changeRunModelSignal(true);
//             }
//             else
//             {
//                 changeRunModelSignal(false);
//             }
//             std::string info = "SWM," + infoList[1].toStdString() + ",0\r";
//             m_tcpSocket->write(info.data());
//         }
//         //切换作业
//         else if(infoList[0].compare("SWP") == 0)
//         {
//         }
//     }
//     catch(std::exception &e)
//     {
//         std::cout << e.what() << std::endl;
//     }
// }

void JobManager::reg_acqfifo_complete_event(int cam_id)
{
    remove_acqfifo_complete_event(cam_id);
    connect(&cams[cam_id], &Cam::finish_acq, this, &JobManager::acqfifo_complete_event);
}

void JobManager::remove_acqfifo_complete_event(int cam_id)
{
    disconnect(&cams[cam_id], &Cam::finish_acq, this, &JobManager::acqfifo_complete_event);
}

int JobManager::checkCam(int cam_id, int worker_id)
{
    if(cams[cam_id].m_pcMyCamera == nullptr)
    {
        LOGE_PROFESSIONAL("cam%d not init", cam_id);
        return -1;
    }

    //取像
    LOGE_PROFESSIONAL("cam%d TriggerSoftware", cam_id);

    cams[cam_id].worker_id = worker_id;

    cams[cam_id].m_pcMyCamera->CommandExecute("TriggerSoftware");
    return 0;
}

int JobManager::focusingCam(int cam_id)
{
    // if(cams[cam_id].m_pcMyCamera == nullptr)
    // {
    //     LOGE_PROFESSIONAL("cam%d not init", cam_id);
    //     return -1;
    // }
    // LOGE_PROFESSIONAL("cam%d TriggerSoftware", cam_id);
    // jobworkers[cam_id].isFocusing = true;
    // jobworkers[cam_id].isSendFocusing = true;

    // cams[cam_id].m_pcMyCamera->CommandExecute("TriggerSoftware");
    return 0;
}

int JobManager::calibrationCam(int cam_id, float robotX, float robotY)
{
    // if(cams[cam_id].m_pcMyCamera == nullptr)
    // {
    //     LOGE_PROFESSIONAL("cam%d not init", cam_id);
    //     return -1;
    // }
    // LOGE_PROFESSIONAL("cam%d TriggerSoftware", cam_id);
    // cams[cam_id].m_pcMyCamera->CommandExecute("TriggerSoftware");
    // jobworkers[cam_id].robotCurrentCoordinates.x = robotX;
    // jobworkers[cam_id].robotCurrentCoordinates.y = robotY;

    return 0;
}

void JobManager::rundemo(int id)
{
    int CamID = id;
    jobworkers[CamID].is_busy = true;
    jobworkers[CamID].run();
}

void JobManager::TongJi(bool rt)
{
    if(rt)
    {
        ok_count += 1;
    }
    else
    {
        ng_count += 1;
    }
}

void JobManager::TongJi(int i, bool rt)
{
    if(rt)
    {
        jobworkers[i].ok_count += 1;
    }
    else
    {
        jobworkers[i].ng_count += 1;
    }
}

// 输入：
//   uv        : 图像坐标 (u,v) （像素）
//   depth     : 该点的深度 Z （单位任意，与输出一致）
//   K         : 3×3 内参矩阵
//   dist      : 1×4/1×5/1×8 畸变系数 (k1,k2,p1,p2[,k3[,k4,k5,k6]])
// 输出：
//   X,Y,Z     : 相机坐标系下的 3D 坐标
cv::Point3d uv2camera(cv::Point2d uv, double depth,
                      const cv::Mat& K, const cv::Mat& dist)
{
    // 1. 去畸变（得到归一化平面坐标 x', y'）
    std::vector<cv::Point2f> in = {cv::Point2f(uv.x, uv.y)};
    std::vector<cv::Point2f> undist;
    cv::undistortPoints(in, undist, K, dist);

    // std::cout << "K:" << cv::format(K, cv::Formatter::FMT_NUMPY) << std::endl;
    // std::cout << "dist:" << cv::format(dist, cv::Formatter::FMT_NUMPY) << std::endl;

    // std::ostringstream os;
    // os << cv::format(K, cv::Formatter::FMT_NUMPY);
    // std::string txt = os.str();
    // LOGE("K:%s", txt.data());

    // std::ostringstream os2;
    // os2 << cv::format(dist, cv::Formatter::FMT_NUMPY);
    // txt = os2.str();
    // LOGE("dist:%s", txt.data());


    LOGE("undist:%f,%f", undist[0].x, undist[0].y);
    LOGE("depth:%f", depth);

    double xn = undist[0].x;  // 归一化 x'
    double yn = undist[0].y;  // 归一化 y'

    // 2. 乘深度 → 相机坐标
    return cv::Point3d(xn * depth, yn * depth, depth);
}

void JobManager::getDistance()
{
    dist = -1;
    dist_result = true;
    if(!jobworkers[0].result() || !jobworkers[1].result())
    {
        dist_result = false;
        return;
    }
    try
    {
        ///获取第一次拍照的中点和第二次拍照的两个点
        cv::Point2f A2D, B2D, P2D;

        double k_line1 = jobworkers[0]. fitline[1] / jobworkers[0].fitline[0];
        double centerx_P = (jobworkers[0].CaliperROIList[0].x + jobworkers[0].CaliperROIList[1].x) / 2;
        P2D = cv::Point(centerx_P, k_line1 * (centerx_P - jobworkers[0]. fitline[2]) + jobworkers[0]. fitline[3]);


        double k_line2 = jobworkers[1]. fitline[1] / jobworkers[1].fitline[0];
        A2D = cv::Point(jobworkers[1].CaliperROIList[0].x, k_line2 * (jobworkers[1].CaliperROIList[0].x - jobworkers[1]. fitline[2]) + jobworkers[1]. fitline[3]);
        B2D = cv::Point(jobworkers[1].CaliperROIList[1].x, k_line2 * (jobworkers[1].CaliperROIList[1].x - jobworkers[1]. fitline[2]) + jobworkers[1]. fitline[3]);

        LOGE("A2D:%f,%f", A2D.x, A2D.y);
        LOGE("B2D:%f,%f", B2D.x, B2D.y);
        LOGE("P2D:%f,%f", P2D.x, P2D.y);

        /// 转3d坐标
        cv::Point3f A3D, B3D, P3D;

        // std::cout << "H_Pix2Tool:" << cv::format(H_Pix2Tool, cv::Formatter::FMT_NUMPY) << std::endl;

        // // 像素通过九点标定，映射成实际尺寸，相当于相机坐标系的坐标
        // std::vector<cv::Point2f> originPoints2D{A2D, B2D, P2D};
        // std::vector<cv::Point2f> transformPoints2D;
        // cv::perspectiveTransform(originPoints2D, transformPoints2D, H_Pix2Tool);


        // LOGE("H_Pix2Tool:[[%f,%f,%f],[%f,%f,%f],[%f,%f,%f]]", H_Pix2Tool.at<double>(0, 0),
        //      H_Pix2Tool.at<double>(0, 1),
        //      H_Pix2Tool.at<double>(0, 2),
        //      H_Pix2Tool.at<double>(1, 0),
        //      H_Pix2Tool.at<double>(1, 1),
        //      H_Pix2Tool.at<double>(1, 2),
        //      H_Pix2Tool.at<double>(2, 0),
        //      H_Pix2Tool.at<double>(2, 1),
        //      H_Pix2Tool.at<double>(2, 2));

        // LOGE("transformPoints2D[0]:%f,%f", transformPoints2D[0].x, transformPoints2D[0].y);
        // LOGE("transformPoints2D[1]:%f,%f", transformPoints2D[1].x, transformPoints2D[1].y);
        // LOGE("transformPoints2D[2]:%f,%f", transformPoints2D[2].x, transformPoints2D[2].y);

        // 通过内参和标定板距离，转为相机坐标系坐标
        std::vector<cv::Point2f> transformPoints2D;

        cv::Point2d uv(A2D.x, A2D.y);
        cv::Point3d camPoint = uv2camera(uv, distanceMtx.at<double>(0), cameraMatrix, distCoeffs);
        transformPoints2D.push_back(cv::Point2f(camPoint.x, camPoint.y));
        uv = cv::Point2d(B2D.x, B2D.y);
        camPoint = uv2camera(uv, distanceMtx.at<double>(0), cameraMatrix, distCoeffs);
        transformPoints2D.push_back(cv::Point2f(camPoint.x, camPoint.y));
        uv = cv::Point2d(P2D.x, P2D.y);
        camPoint = uv2camera(uv, distanceMtx.at<double>(0), cameraMatrix, distCoeffs);
        transformPoints2D.push_back(cv::Point2f(camPoint.x, camPoint.y));

        LOGE("transformPoints2D[0]:%f,%f", transformPoints2D[0].x, transformPoints2D[0].y);
        LOGE("transformPoints2D[1]:%f,%f", transformPoints2D[1].x, transformPoints2D[1].y);
        LOGE("transformPoints2D[2]:%f,%f", transformPoints2D[2].x, transformPoints2D[2].y);


        // 通过手眼标定转换矩阵，转成
        cv::Mat robotVectorA = cv::Mat::zeros(cv::Size(6, 1), CV_64FC1);
        cv::Mat robotVectorB = cv::Mat::zeros(cv::Size(6, 1), CV_64FC1);
        cv::Mat robotVectorP = cv::Mat::zeros(cv::Size(6, 1), CV_64FC1);

        cv::Mat H_tool2camA, H_tool2camB, H_tool2camP;
        robotVectorA.at<double>(0, 0) = transformPoints2D[0].x;
        robotVectorA.at<double>(0, 1) = transformPoints2D[0].y;
        robotVectorA.at<double>(0, 2) = 0;
        robotVectorA.at<double>(0, 3) = 0;
        robotVectorA.at<double>(0, 4) = 0;
        robotVectorA.at<double>(0, 5) = 0;

        attitudeVectorToMatrix(robotVectorA, false, "xyz", H_tool2camA);

        robotVectorB.at<double>(0, 0) = transformPoints2D[1].x;
        robotVectorB.at<double>(0, 1) = transformPoints2D[1].y;
        robotVectorB.at<double>(0, 2) = 0;
        robotVectorB.at<double>(0, 3) = 0;
        robotVectorB.at<double>(0, 4) = 0;
        robotVectorB.at<double>(0, 5) = 0;

        attitudeVectorToMatrix(robotVectorB, false, "xyz", H_tool2camB);

        robotVectorP.at<double>(0, 0) = transformPoints2D[2].x;
        robotVectorP.at<double>(0, 1) = transformPoints2D[2].y;
        robotVectorP.at<double>(0, 2) = 0;
        robotVectorP.at<double>(0, 3) = 0;
        robotVectorP.at<double>(0, 4) = 0;
        robotVectorP.at<double>(0, 5) = 0;

        attitudeVectorToMatrix(robotVectorP, false, "xyz", H_tool2camP);

        cv::Mat H_tool2baseA, H_tool2baseB, H_tool2baseP;

        H_tool2baseA = H_gripper2base2 * T_Cam2Gripper * H_tool2camA;
        H_tool2baseB = H_gripper2base2 * T_Cam2Gripper * H_tool2camB;
        H_tool2baseP = H_gripper2base1 * T_Cam2Gripper * H_tool2camP;


        A3D = cv::Point3f(H_tool2baseA.at<double>(0, 3), H_tool2baseA.at<double>(1, 3), H_tool2baseA.at<double>(2, 3));
        B3D = cv::Point3f(H_tool2baseB.at<double>(0, 3), H_tool2baseB.at<double>(1, 3), H_tool2baseB.at<double>(2, 3));
        P3D = cv::Point3f(H_tool2baseP.at<double>(0, 3), H_tool2baseP.at<double>(1, 3), H_tool2baseP.at<double>(2, 3));
        LOGE("A3D:%f,%f,%f", A3D.x, A3D.y, A3D.z);
        LOGE("B3D:%f,%f,%f", B3D.x, B3D.y, B3D.z);
        LOGE("P3D:%f,%f,%f", P3D.x, P3D.y, P3D.z);



        /// 求距离
        dist = pointToLineDistance3D(A3D, B3D, P3D);

        dist *= k;

        LOGE("dist:%f", dist);

        ///判断距离
        if(dist < distance_thre_min || dist > distance_thre_max)
        {
            dist_result = false;
        }

    }
    catch(cv::Exception &e)
    {
        dist = 0;
        dist_result = false;
        LOGE_HIGH(e.what());
    }
    catch(std::exception &e)
    {
        dist = 0;
        dist_result = false;
        LOGE_HIGH(e.what());
    }
    catch(QException &e)
    {
        dist = 0;
        dist_result = false;
        LOGE_HIGH(e.what());
    }
    catch(...)
    {
        dist = 0;
        dist_result = false;
        LOGE_HIGH("other error.");
    }


}

bool JobManager::result()
{
    bool rt = true;
    for(int i = 0; i < jobworker_num; i++)
    {
        if(!jobworkers[i].result())
        {
            rt = false;
        }
    }
    if(!dist_result)
    {
        rt = false;
    }
    return rt;
}

int JobManager::all_count()
{
    return ok_count + ng_count;
}





