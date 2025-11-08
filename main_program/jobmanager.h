#ifndef JOBMANAGER_H
#define JOBMANAGER_H
#include "jobworker.h"
#include "tool/cam.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QtConcurrent>
#include <QMessageBox>
#include <QFile>



#pragma execution_character_set("utf-8")

///
/// \brief The cam_connect_type enum 相机连接模式
///
enum cam_connect_type
{
    mvs_network = 1,
    mvs_gentl = 2
};


///
/// \brief The cam_image_type enum 相机取像模式
///
enum cam_image_type
{
    line_type = 1,
    area_type = 2
};

///
/// \brief pointToLineDistance3D 3d点P到线AB的距离
/// \param A
/// \param B
/// \param P
/// \return
///
float pointToLineDistance3D(const cv::Vec3f& A,
                            const cv::Vec3f& B,
                            const cv::Vec3f& P);

class JobManager: public QObject
{
    Q_OBJECT
public:
    JobManager();
    ///
    /// \brief JobManager 作业管理器
    /// \param camCount 相机个数
    ///
    JobManager(int _cam_count, int _jobworker_count);

    JobManager &operator = (const JobManager &jobmanager);

    //参数
public:

    ///
    /// 保存参数
    ///

    //相机作业工具
    std::vector<JobWorker> jobworkers;
    //相机作业工具
    std::vector<Cam> cams;

    // 光源控制通讯




    //作业个数
    int jobworker_num = 0;

    //相机个数
    int cam_num = 0;

    // 结果
    double distance_thre_min = 0;
    double distance_thre_max = 99999;


    // 标定参数
    cv::Mat T_Cam2Gripper = cv::Mat::eye(cv::Size(4, 4), CV_64FC1);
    cv::Mat H_Pix2Tool = cv::Mat::eye(cv::Size(3, 3), CV_64FC1);

    cv::Mat distanceMtx = cv::Mat::eye(cv::Size(1, 1), CV_64FC1);

    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) <<
                            1, 0.0, 0,
                            0.0, 1, 0,
                            0.0, 0.0, 1.0);
    cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << 0, 0,  0, 0, 0);


    cv::Mat H_gripper2base1 = cv::Mat::eye(cv::Size(4, 4), CV_64FC1);
    cv::Mat H_gripper2base2 = cv::Mat::eye(cv::Size(4, 4), CV_64FC1);

    //矫正系数
    double k = 1.0;



    //io输出参数

    //工位1剔除延时
    int station_1_delay = 300;
    //工位2剔除延时
    int station_2_delay = 1000;
    //剔除产品持续时间
    int output_delay = 50;
    //工位1剔除信号输出相机
    int station_1_output_cam = 0;
    //工位1剔除信号输出路线
    int station_1_output_line = 1;


    //不保存参数


    //相机驱动，用于获取相机驱动信息
    MV_CC_DEVICE_INFO_LIST  m_stDevList;
    //相机类型，目前为面阵相机
    cam_image_type m_cam_image_type = cam_image_type::area_type;
    //相机连接方式，目前为网线连接
    cam_connect_type m_cam_connect_type = cam_connect_type::mvs_network;

    //ok产品个数记录
    int ok_count = 0;
    //ng产品个数记录
    int ng_count = 0;

    //结果
    float x_offset = 0;
    float y_offset = 0;
    float r_offset = 0;

    float dist = 0;
    bool dist_result = true;


    //是否正在检测
    bool is_busy = false;
    //是否启用工位1输出信号
    bool station_1_output_enable = true;

    // //网络socket通信
    // QTcpServer *m_tcpServer = nullptr;
    // QTcpSocket *m_tcpSocket = nullptr;

    // //通信锁
    // std::mutex socket_locker;


public slots:
    // ///
    // /// \brief tcp_socker_connected 新tcp客户端连接
    // ///
    // void tcp_socker_connected();

    // ///
    // /// \brief tcp_socker_disconnected tcp客户端断开连接
    // ///
    // void tcp_socker_disconnected();

    // ///
    // /// \brief tcp_ready_read 接收tcp客户端信息
    // ///
    // void tcp_ready_read();

    ///
    /// \brief reg_acqfifo_complete_event 部署取像完成事件
    ///
    void reg_acqfifo_complete_event(int cam_id);

    ///
    /// \brief remove_acqfifo_complete_event 清除取像完成事件
    ///
    void remove_acqfifo_complete_event(int cam_id);

// signals:

//     ///
//     /// \brief changeDisplaySignal
//     /// \param cam_id
//     ///
//     void changeDisplaySignal(int cam_id);

//     ///
//     /// \brief changeShowModeSignal
//     /// \param isShowForm
//     ///
//     void changeShowModeSignal(bool isShowForm);

//     ///
//     /// \brief changeRunModelSignal
//     /// \param isAutoMode
//     ///
//     void changeRunModelSignal(bool isAutoMode);

    //函数
public:
    ///
    /// \brief acq_init 相机初始化
    ///
    void acq_init();

    ///
    /// \brief checkCam
    /// \param cam_id 相机id
    /// \param worker_id joworker id
    /// \return
    ///
    int checkCam(int cam_id, int worker_id);

    ///
    /// \brief focusingCam
    /// \param cam_id
    /// \return
    ///
    int focusingCam(int cam_id);

    ///
    /// \brief calibrationCam
    /// \param cam_id
    /// \param robotX
    /// \param robotY
    /// \return
    ///
    int calibrationCam(int cam_id, float robotX, float robotY);

    ///
    /// \brief mvs_open_device 相机驱动打开函数
    /// \return
    ///
    int mvs_open_device();

    ///
    /// \brief mvs_close_device 相机驱动关闭函数
    /// \return
    ///
    int mvs_close_device();

    ///
    /// \brief mvs_start_grab 相机开始取像函数
    ///
    void mvs_start_grab();

    ///
    /// \brief mvs_stop_grab  相机关闭取像函数
    ///
    void mvs_stop_grab();

    ///
    /// \brief acqfifo_complete_event 取像完成事件
    /// \param cam_id 相机id
    ///
    void acqfifo_complete_event(int cam_id, int worker_id);

    ///
    /// \brief set_cam_para 设置相机参数函数
    ///
    void set_cam_para();

    ///
    /// \brief rundemo 仿真运行单个相机图片
    /// \param id 运行相机id
    ///
    void rundemo(int id);

    ///
    /// \brief TongJi 统计整个作业结果
    /// \param rt 整个作业结果
    ///
    void TongJi(bool rt);

    ///
    /// \brief TongJi 统计相机作业结果
    /// \param i    相机id
    /// \param rt   相机结果
    ///
    void TongJi(int i, bool rt);

    //结果

    void getDistance();

    ///
    /// \brief result 整个作业结果
    /// \return
    ///
    bool result();

    ///
    /// \brief all_count 整个总统计个数
    /// \return
    ///
    int all_count();

    ///
    /// \brief station_1_output 工位一结果输出函数，未使用
    ///
    void station_1_output();

    ///
    /// \brief station_2_output 工位二结果输出函数，未使用
    ///
    void station_2_output();

    ///
    /// \brief station_1_output 工位一结果输出函数
    /// \param delay_time 剔除延迟时间
    ///
    void station_1_output(int delay_time);

    // ///
    // /// \brief tcp_server_init tcp服务器通讯初始化
    // ///
    // void tcp_server_init();

    // ///
    // /// \brief tcp_server_dispose tcp服务器关闭
    // ///
    // void tcp_server_dispose();



    ///
    /// 保存参数，每次添加保存参数后，都要更新该函数
    ///
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        if(Archive::is_saving::value)
        {
            std::string name;
            name = "jobworkers";
            ar &name;
            ar &jobworkers;
            name = "cams";
            ar &name;
            ar &cams;
            name = "jobworker_num";
            ar &name;
            ar &jobworker_num;
            name = "cam_num";
            ar &name;
            ar &cam_num;
            name = "station_1_delay";
            ar &name;
            ar &station_1_delay;
            name = "station_1_output_cam";
            ar &name;
            ar &station_1_output_cam;
            name = "station_1_output_line";
            ar &name;
            ar &station_1_output_line;
            name = "station_2_delay";
            ar &name;
            ar &station_2_delay;
            name = "output_delay";
            ar &name;
            ar &output_delay;

            name = "distance_thre_min";
            ar &name;
            ar &distance_thre_min;
            name = "distance_thre_max";
            ar &name;
            ar &distance_thre_max;

            name = "k";
            ar &name;
            ar &k;

            // name = "T_Tool2Base";
            // ar &name;
            // ar &T_Tool2Base;
            // name = "H_Pix2Tool";
            // ar &name;
            // ar &H_Pix2Tool;


            name = "JobManagerEnd";
            ar &name;
        }
        if(Archive::is_loading::value)
        {
            while(true)
            {
                try
                {
                    std::string name;
                    ar &name;
                    if(name.compare("jobworkers") == 0)
                    {
                        ar &jobworkers;
                    }
                    else if(name.compare("cams") == 0)
                    {
                        ar &cams;
                    }
                    else if(name.compare("jobworker_num") == 0)
                    {
                        ar &jobworker_num;
                    }
                    else if(name.compare("cam_num") == 0)
                    {
                        ar &cam_num;
                    }
                    else if(name.compare("station_1_delay") == 0)
                    {
                        ar &station_1_delay;
                    }
                    else if(name.compare("station_1_output_cam") == 0)
                    {
                        ar &station_1_output_cam;
                    }
                    else if(name.compare("station_1_output_line") == 0)
                    {
                        ar &station_1_output_line;
                    }
                    else if(name.compare("station_2_delay") == 0)
                    {
                        ar &station_2_delay;
                    }
                    else if(name.compare("output_delay") == 0)
                    {
                        ar &output_delay;
                    }

                    else if(name.compare("distance_thre_min") == 0)
                    {
                        ar &distance_thre_min;
                    }
                    else if(name.compare("distance_thre_max") == 0)
                    {
                        ar &distance_thre_max;
                    }
                    else if(name.compare("k") == 0)
                    {
                        ar &k;
                    }
                    // else if(name.compare("T_Tool2Base") == 0)
                    // {
                    //     ar &T_Tool2Base;
                    // }
                    // else if(name.compare("H_Pix2Tool") == 0)
                    // {
                    //     ar &H_Pix2Tool;
                    // }


                    else if(name.compare("JobManagerEnd") == 0)
                    {
                        break;
                    }
                    else
                    {
                        std::string tmp_obj;
                        ar &tmp_obj;
                    }
                }
                catch(boost::archive::archive_exception &e)
                {
                    //                    std::cout << "OnlineCheck deserialization:" << e.what() << std::endl;
                    LOGE_HIGH(e.what());
                    break;
                }
            }
        }
    }

};






#endif // JOBMANAGER_H
