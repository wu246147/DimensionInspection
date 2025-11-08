#ifndef JOBWORKER_H
#define JOBWORKER_H
//基本库
#include <iostream>
#include <fstream>
#include <vector>
#include <QObject>
#include <QtMath>
#include <QDebug>
#include <QtConcurrent>
#include <QThread>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

#include <QTcpServer>
#include <QTcpSocket>


//boost库
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/serialization/split_free.hpp"
#include "boost/serialization/vector.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
//图像算法库
#include <opencv2/opencv.hpp>
// #include "wafer_align.h"
#include "imgprocesstool.h"
#include "calipertool.h"
#include "shapematch.h"
//相机库
#include "MvCamera.h"
//交互库
#include "mygraphicrectitem.h"
#include "mygraphicsview.h"
#include "mygraphic_polygon_item.h"
#include "mywidgetview.h"
#include "mygraphicpointitem.h"
#include "mygraphiclineitem.h"

#include "mylog.h"
#include <type_traits>

#pragma execution_character_set("utf-8")

///
/// \brief The work_mode_type enum 工作模式
///
enum work_mode_type
{
    simulation_mode = 1,
    auto_mode = 2,
    hand_mode = 3
};

///
/// cv::Mat对象保存
///
BOOST_SERIALIZATION_SPLIT_FREE(cv::Mat)
namespace boost
{
    namespace serialization
    {
        /*** Mat ***/
        template<class Archive>
        void save(Archive &ar, const cv::Mat &m, const unsigned int version)
        {
            size_t elemSize = m.elemSize(), elemType = m.type();
            ar &m.cols;
            ar &m.rows;
            ar &elemSize;
            ar &elemType;  // element type.
            size_t dataSize = m.cols * m.rows * m.elemSize();
            //cout <<"Writing matrix data rows, cols, elemSize, type, datasize: (" << m.rows <<"," << m.cols <<"," << m.elemSize() <<"," << m.type() <<"," << dataSize <<")" << endl;
            for(size_t dc = 0; dc < dataSize; ++dc)
            {
                ar &m.data[dc];
            }
        }

        template<class Archive>
        void load(Archive &ar, cv::Mat &m, const unsigned int version)
        {
            int cols, rows;
            size_t elemSize, elemType;
            ar &cols;
            ar &rows;
            ar &elemSize;
            ar &elemType;
            m.create(rows, cols, elemType);
            size_t dataSize = m.cols * m.rows * elemSize;
            //cout <<"reading matrix data rows, cols, elemSize, type, datasize: (" << m.rows <<"," << m.cols <<"," << m.elemSize() <<"," << m.type() <<"," << dataSize <<")" << endl;
            for(size_t dc = 0; dc < dataSize; ++dc)
            {
                ar &m.data[dc];
            }
        }
    }
}

///
/// cv::Rect对象保存
///
BOOST_SERIALIZATION_SPLIT_FREE(cv::Rect)
namespace boost
{
    namespace serialization
    {
        /*** Rect ***/
        template<class Archive>
        void save(Archive &ar, const cv::Rect  &m, const unsigned int version)
        {
            ar &m.x;
            ar &m.y;
            ar &m.width;
            ar &m.height;
        }

        template<class Archive>
        void load(Archive &ar, cv::Rect  &m, const unsigned int version)
        {
            ar &m.x;
            ar &m.y;
            ar &m.width;
            ar &m.height;
        }
    }
}

///
/// cv::Point对象保存
///
BOOST_SERIALIZATION_SPLIT_FREE(cv::Point)
namespace boost
{
    namespace serialization
    {
        /*** Point ***/
        template<class Archive>
        void save(Archive &ar, const cv::Point  &m, const unsigned int version)
        {
            ar &m.x;
            ar &m.y;
        }

        template<class Archive>
        void load(Archive &ar, cv::Point  &m, const unsigned int version)
        {
            ar &m.x;
            ar &m.y;
        }
    }
}

///
/// cv::Point2f对象保存
///
BOOST_SERIALIZATION_SPLIT_FREE(cv::Point2f)
namespace boost
{
    namespace serialization
    {
        /*** Point ***/
        template<class Archive>
        void save(Archive &ar, const cv::Point2f  &m, const unsigned int version)
        {
            ar &m.x;
            ar &m.y;
        }

        template<class Archive>
        void load(Archive &ar, cv::Point2f  &m, const unsigned int version)
        {
            ar &m.x;
            ar &m.y;
        }
    }
}
///
/// cv::RotatedRect
///
BOOST_SERIALIZATION_SPLIT_FREE(cv::RotatedRect)
namespace boost
{
    namespace serialization
    {
        /*** RotatedRect ***/
        template<class Archive>
        void save(Archive &ar, const cv::RotatedRect  &m, const unsigned int version)
        {
            ar &m.center.x;
            ar &m.center.y;
            ar &m.angle;
            ar &m.size.width;
            ar &m.size.height;
        }

        template<class Archive>
        void load(Archive &ar, cv::RotatedRect  &m, const unsigned int version)
        {
            ar &m.center.x;
            ar &m.center.y;
            ar &m.angle;
            ar &m.size.width;
            ar &m.size.height;
        }
    }
}


///
/// \brief MatToQImage mat图片转QImage图片
/// \param cvImage 输入mat图片
/// \return 返回QImage图片
///
QImage MatToQImage(const cv::Mat &cvImage);

///
/// \brief MatToPixmap mat图片转Pixmap图片
/// \param cvImage 输入mat图片
/// \return 返回Pixmap图片
///
QPixmap MatToPixmap(const cv::Mat &cvImage);

///
/// \brief grab_thread 取流线程
/// \param pUser
/// \return
///
// unsigned int __stdcall grab_thread(void *pUser);

///
/// \brief createMultipleFolders 创建文件夹，未使用
/// \param path 文件夹路线
/// \return
///
QString createMultipleFolders(const QString path);


class JobWorker: public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit JobWorker(JobWorker *parent = nullptr);
    JobWorker(const JobWorker &jobworker);

    JobWorker &operator =(const JobWorker &jobworker);
    ///
    /// \brief JobWorker 相机作用类
    /// \param id 相机id
    ///
    JobWorker(int &id);

private:
    //保存参数

public:
    ///
    /// 保存参数
    ///

    //相机id
    int cam_id;
    //仿真路径，只访问当前路径下的图片
    std::string src_image_dir;
    //取象触发延迟时间
    float trigger_delay = 0;
    //相机曝光时间
    float exposure_time = 0;
    //相机增益
    float gain = 0;

    const std::string edgeType = "long";

    ///初定位参数
    int circleThre = 45;
    int chipThre = 158;
    int chipAreaMin = 10000;
    int chipAreaMax = 100000;

    bool lightChannel = false;

    //尺寸检测范围
    std::vector<float> caliper_distances_limit = {99999, 99999};

    //判断是否已经设置好训练图片并训练过
    bool is_train = false;
    //记录训练图片
    cv::Mat train_image;

    //记录训练模板框
    cv::Rect model_box = cv::Rect(100, 100, 100, 100);
    // cv::Mat train_image_cut;
    //记录训练边缘框
    std::vector<cv::Rect> boxes = {cv::Rect(100, 100, 100, 100), cv::Rect(100, 100, 100, 100), cv::Rect(100, 100, 100, 100), cv::Rect(100, 100, 100, 100)};
    //记录训练中间框
    cv::Rect center_box = cv::Rect(100, 100, 100, 100);

    ///卡尺参数
    //记录ROI

    std::vector<cv::Point2f>CaliperROIList =
    {
        cv::Point2f(100, 100),
        cv::Point2f(200, 200),
    };

    std::vector<cv::Point2f>CaliperTransformROIList =
    {
        cv::Point2f(100, 100),
        cv::Point2f(200, 200),
    };
    int CaliperROIW = 5;
    int CaliperROIH = 100;
    int CaliperROIC = 10;
    int caliperPolarity = 0;   //卡尺极值，0为黑到白、1为白到黑
    int caliperSelectPosition = 0;//卡尺找边模式，0为第一边、1为最后一边
    uchar caliperThre = 10;//卡尺找边阈值

    /// 定位参数

    void *ModelID = nullptr;
    cv::RotatedRect shapeMatchROI = cv::RotatedRect(cv::Point2f(100, 100), cv::Size2f(100, 100), 0);
    //检测参数
    float AngleStart = 0;
    float AngleExtent = 360;
    float AngleStep = 0.5;
    int Optimization = 1;
    int Metric = 0;
    int LowThreshold = 25;
    int HighThreshold = 45;
    int MinContrast = 15;
    int MinComponentSize = 5;
    int Pregeneration = 0;
    int QuantizedOrientations = 1;
    float MaxOverlap = 0;
    bool SubPixel = false;
    int NumLevels = 0;
    float Greediness = 0.9;
    bool ExpandBorder = false;
    bool MoreAggressiveCandidateStrategy = false;
    int maxMatches = 1;
    float MinScore = 0.5;

    // float locationResultColLast = 0;
    // float locationResultRowLast = 0;
    // float locationResultAngleLast = 0;

    /// 标定参数
    cv::Mat transformH = cv::Mat::eye(cv::Size(3, 3), CV_64F);               //计算转换矩阵，取5~9个包含旋转平移的点，来进行转换矩阵计算。
    cv::Point2f standardRotateCenter ;  //机器人旋转中心坐标 ，取多几个单纯旋转的点来计算旋转中心。起码两个以上
    cv::Point2f standardPoint = cv::Point2f(0, 0);        //标准位置
    float standardAngle = 0;              //标准角度

    cv::Point2f robotCurrentCoordinates;//机器人当前坐标，通过标定通讯端口获取

    ///输出结果
    int resultType = 0; //0为像素坐标，1为实际坐标，2为相对标准位置坐标

    ///
    /// 不保存参数
    ///

    //当前图片
    cv::Mat img;
    // cv::Mat train_img;

    //定位检测结果
    //转换矩阵结果
    cv::Mat locationTransformH; //shapeMatch转换矩阵
    // cv::Mat locationTransformH2; //结合卡尺检测的直线角度后的，得到的转换矩阵

    cv::Point locationPosition;             //定位的像素坐标结果
    double locationAngle;
    double locationAngleOutPut;
    double shapeMatchAngle;
    cv::Point2f locationPositionTransform;  //定位的世界坐标结果
    cv::Point2f locationPositionRelative;  //定位的相对坐标结果
    cv::Point2f locationPositionOutput;  //定位的输出坐标结果

    bool locationResult = true;
    bool locationShapeResult  = true;
    bool locationCaliperResult  = true;

    //不用了

    //是否保存图片
    bool is_save_img = false;
    //是否保存ng图片，只有同时启用保存图片及保存ng图片才会保存ng图。
    bool is_save_ng_img = false;
    //目前保存ng图的限制个数
    const int save_ng_img_limit_count = 50;



    //判断是否正在检测
    bool is_busy = false;

    //判断是否在进行相机对焦处理
    bool isFocusing = false;
    double clarity_quantization_value = 0;
    bool isSendFocusing = false; //判断是否需要输出对焦结果

    //判断是否在实时取像
    bool isGrabbing = false;

    //交互类

    //图像显示窗口
    myWidgetView *display = nullptr;
    //训练框，未启用
    MyGraphicRectItem *train_roi = nullptr;

    QGraphicsLineItem *centerLineY = nullptr;
    QGraphicsLineItem *centerLineX = nullptr;

    QGraphicsLineItem *centerLineY2 = nullptr;
    QGraphicsLineItem *centerLineY3 = nullptr;

    //结果

    //取像个数
    int check_count = 0;
    //等待个数
    int wait_for_count = 0;
    //完成ok产品个数
    int ok_count = 0;
    //完成ng产品个数
    int ng_count = 0;
    //记录相机作业启动和结束时间
    clock_t jobworker_start_time, jobworker_end_time;
    //记录相机作业总用时时间
    double jobworker_total_use_time;

    //运行状态
    work_mode_type m_work_mode_type = work_mode_type::hand_mode;

    //图片缓存列表
    std::vector<cv::Mat> tmp_img_list;
    //图片缓存锁
    std::mutex tmp_img_list_mutex;


    //输出结果缓存列表
    std::vector<bool> tmp_result_list;
    //输出结果缓存锁
    std::mutex tmp_result_list_mutex;

    // //采集图片时间记录列表
    // std::vector<std::chrono::system_clock::time_point> tmp_img_time_list;
    // //采集图片时间记录锁
    // std::mutex tmp_img_time_list_mutex;

    //显示缓存
    //    vector<QGraphicsItem*> item_list;

    //网络socket通信
    QTcpServer *m_tcpServer = nullptr;
    QTcpSocket *m_tcpSocket = nullptr;

    //通信锁
    std::mutex socket_locker;

    //plc 通讯ip
    std::string plc_ip = "192.168.1.5";
    //plc 通讯 port
    int net_port = 9094;

    int rotateType = -1;

    double xb = 0;
    double yb = 0;
    double rb = 0;

    double xa = 1;
    double ya = 1;
    double ra = 1;

    //中心线间距

    int centerLineD = 0;

    //操作模式 ture为自动模式，false为手动选点发送模式。
    bool isAutoMode = true;

    //卡尺定位结果
    cv::Vec4f fitline = {1.0f, 1.0f, 1.0f, 1.0f};
    std::vector<cv::Point2f> caliperLocationOriginPoints;
    std::vector<bool> caliperLocationResult;

    bool isDebug = false;



public slots:
    ///
    /// \brief tcp_socker_connected 新tcp客户端连接
    ///
    void tcp_socker_connected();

    ///
    /// \brief tcp_socker_disconnected tcp客户端断开连接
    ///
    void tcp_socker_disconnected();

    ///
    /// \brief tcp_ready_read 接收tcp客户端信息
    ///
    void tcp_ready_read();

signals:
    ///
    /// \brief jobworker_start_signed 作业运行开始信号
    /// \param work_id 相机id
    ///
    void jobworker_start_signed(int work_id);

    ///
    /// \brief jobworker_end_signed 作业运行完成信号
    /// \param work_id  相机id
    ///
    void jobworker_end_signed(int work_id);

    ///
    /// \brief update_display_img_signed 更新界面图片信号
    /// \param work_id  相机id
    ///
    void update_display_img_signed(int work_id);

    //函数
public:

    ///
    /// \brief init 相机作业初始化函数
    ///
    void init();

    ///
    /// \brief acqfifo_complete 取像完成事件
    ///
    void acqfifo_complete(cv::Mat tmp_img);

    ///
    /// \brief cam_result_process 未启用
    ///
    void cam_result_process();

    ///
    /// \brief run 作业运行函数
    ///
    void run();

    ///
    /// \brief get_channel_img 获取通道图片
    ///
    void get_channel_img();


    ////
    /// \brief set_train_img 设置训练图片
    /// \param _img
    ///
    void set_train_img(cv::Mat _img);


    ///
    /// \brief show_shape 显示检测框函数，未启用
    ///
    void show_shape();

    ///
    /// \brief show_train_result
    ///
    void show_train_result();

    ///
    /// \brief show_caliper_roi 显示找边卡尺的位置
    ///
    void show_caliper_roi();

    ///
    /// \brief show_result 显示结果函数
    ///
    void show_result();

    ///
    /// \brief result 获取作业结果函数
    /// \return 返回当次检测结果
    ///
    bool result();



    ///
    /// \brief reset_count 重置统计
    ///
    void reset_count();

    ///
    /// \brief all_count 总检测个数
    /// \return
    ///
    int all_count();

    ///
    /// \brief save_img 保存图片
    ///
    void save_img();

    ///
    /// \brief save_ng_img 保存ng图片
    ///
    void save_ng_img();

    ///
    /// \brief dispose 释放函数，未启用
    ///
    void dispose();

    ///
    /// \brief tcp_server_init tcp服务器通讯初始化
    ///
    void tcp_server_init();

    ///
    /// \brief tcp_server_dispose tcp服务器关闭
    ///
    void tcp_server_dispose();


    ///
    /// \brief get_remove_duplicate_b 获取去重补偿
    ///
    void get_remove_duplicate_b();

    ///
    /// \brief get_transform_mat
    /// \param detect_origin_x
    /// \param detect_origin_y
    /// \param diff_origin_phi
    /// \return
    ///
    cv::Mat get_transform_mat(float detect_origin_x, float detect_origin_y, float diff_origin_phi);



    ///
    /// \brief updataTransformCaliperRoiFromCaliperRoi
    /// \param H
    ///
    void updataTransformCaliperRoiFromCaliperRoi();

    ///
    /// \brief updataCaliperRoiFromTransformCaliperRoi
    /// \param H
    ///
    void updataCaliperRoiFromTransformCaliperRoi();


    ///
    /// 保存参数，每次添加保存参数后，都要更新该函数
    ///
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        if(Archive::is_saving::value)
        {
            std::string name;
            name = "cam_id";
            ar &name;
            ar &cam_id;
            name = "src_image_dir";
            ar &name;
            ar &src_image_dir;
            name = "trigger_delay";
            ar &name;
            ar &trigger_delay;
            name = "gain";
            ar &name;
            ar &gain;
            name = "exposure_time";
            ar &name;
            ar &exposure_time;
            name = "is_train";
            ar &name;
            ar &is_train;
            name = "train_image";
            ar &name;
            ar &train_image;
            name = "model_box";
            ar &name;
            ar &model_box;
            name = "boxes";
            ar &name;
            ar &boxes;
            name = "center_box";
            ar &name;
            ar &center_box;

            /// 卡尺参数
            name = "CaliperROIList";
            ar &name;
            ar &CaliperROIList;

            name = "CaliperTransformROIList";
            ar &name;
            ar &CaliperTransformROIList;

            name = "CaliperROIW";
            ar &name;
            ar &CaliperROIW;
            name = "CaliperROIH";
            ar &name;
            ar &CaliperROIH;
            name = "CaliperROIC";
            ar &name;
            ar &CaliperROIC;


            name = "caliperPolarity";
            ar &name;
            ar &caliperPolarity;
            name = "caliperSelectPosition";
            ar &name;
            ar &caliperSelectPosition;
            name = "caliperThre";
            ar &name;
            ar &caliperThre;
            /// 粗定位参数

            name = "circleThre";
            ar &name;
            ar &circleThre;
            name = "chipThre";
            ar &name;
            ar &chipThre;
            name = "chipAreaMin";
            ar &name;
            ar &chipAreaMin;
            name = "chipAreaMax";
            ar &name;
            ar &chipAreaMax;
            name = "lightChannel";
            ar &name;
            ar &lightChannel;

            /// 定位参数

            name = "shapeMatchROI";
            ar &name;
            ar &shapeMatchROI;
            //检测参数
            name = "LowThreshold";
            ar &name;
            ar &LowThreshold;
            name = "HighThreshold";
            ar &name;
            ar &HighThreshold;
            name = "MinContrast";
            ar &name;
            ar &MinContrast;


            /// 标定参数

            name = "transformH";
            ar &name;
            ar &transformH;

            name = "standardRotateCenter";
            ar &name;
            ar &standardRotateCenter;

            name = "standardPoint";
            ar &name;
            ar &standardPoint;

            name = "standardAngle";
            ar &name;
            ar &standardAngle;

            ///输出结果
            name = "resultType";
            ar &name;
            ar &resultType;

            name = "rotateType";
            ar &name;
            ar &rotateType;

            //补偿
            name = "xb";
            ar &name;
            ar &xb;
            name = "yb";
            ar &name;
            ar &yb;
            name = "rb";
            ar &name;
            ar &rb;

            name = "xa";
            ar &name;
            ar &xa;
            name = "ya";
            ar &name;
            ar &ya;
            name = "ra";
            ar &name;
            ar &ra;

            //中心线间距
            name = "centerLineD";
            ar &name;
            ar &centerLineD;


            ///通讯
            name = "plc_ip";
            ar &name;
            ar &plc_ip;
            name = "net_port";
            ar &name;
            ar &net_port;
            name = "JobWorkerEnd";
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
                    if(name.compare("cam_id") == 0)
                    {
                        ar &cam_id;
                    }
                    else if(name.compare("src_image_dir") == 0)
                    {
                        ar &src_image_dir;
                    }
                    else if(name.compare("trigger_delay") == 0)
                    {
                        ar &trigger_delay;
                    }
                    else if(name.compare("gain") == 0)
                    {
                        ar &gain;
                    }
                    else if(name.compare("exposure_time") == 0)
                    {
                        ar &exposure_time;
                    }
                    else if(name.compare("is_train") == 0)
                    {
                        ar &is_train;
                    }
                    else if(name.compare("train_image") == 0)
                    {
                        ar &train_image;
                    }
                    else if(name.compare("model_box") == 0)
                    {
                        ar &model_box;
                    }
                    else if(name.compare("boxes") == 0)
                    {
                        ar &boxes;
                    }
                    else if(name.compare("center_box") == 0)
                    {
                        ar &center_box;
                    }
                    ///粗定位

                    else if(name.compare("circleThre") == 0)
                    {
                        ar &circleThre;
                    }
                    else if(name.compare("chipThre") == 0)
                    {
                        ar &chipThre;
                    }
                    else if(name.compare("chipAreaMin") == 0)
                    {
                        ar &chipAreaMin;
                    }
                    else if(name.compare("chipAreaMax") == 0)
                    {
                        ar &chipAreaMax;
                    }
                    else if(name.compare("lightChannel") == 0)
                    {
                        ar &lightChannel;
                    }
                    /// 卡尺参数
                    else if(name.compare("CaliperROIList") == 0)
                    {
                        ar &CaliperROIList;
                    }
                    else if(name.compare("CaliperTransformROIList") == 0)
                    {
                        ar &CaliperTransformROIList;
                    }
                    else if(name.compare("caliperPolarity") == 0)
                    {
                        ar &caliperPolarity;
                    }
                    else if(name.compare("caliperSelectPosition") == 0)
                    {
                        ar &caliperSelectPosition;
                    }
                    else if(name.compare("caliperThre") == 0)
                    {
                        ar &caliperThre;
                    }

                    else if(name.compare("CaliperROIW") == 0)
                    {
                        ar &CaliperROIW;
                    }
                    else if(name.compare("CaliperROIH") == 0)
                    {
                        ar &CaliperROIH;
                    }
                    else if(name.compare("CaliperROIC") == 0)
                    {
                        ar &CaliperROIC;
                    }

                    /// 定位参数
                    else if(name.compare("shapeMatchROI") == 0)
                    {
                        ar &shapeMatchROI;
                    }
                    else if(name.compare("LowThreshold") == 0)
                    {
                        ar &LowThreshold;
                    }
                    else if(name.compare("HighThreshold") == 0)
                    {
                        ar &HighThreshold;
                    }
                    else if(name.compare("MinContrast") == 0)
                    {
                        ar &MinContrast;
                    }



                    /// 标定参数
                    else if(name.compare("transformH") == 0)
                    {
                        ar &transformH;
                    }
                    else if(name.compare("standardRotateCenter") == 0)
                    {
                        ar &standardRotateCenter;
                    }
                    else if(name.compare("standardPoint") == 0)
                    {
                        ar &standardPoint;
                    }
                    else if(name.compare("standardAngle") == 0)
                    {
                        ar &standardAngle;
                    }

                    else if(name.compare("resultType") == 0)
                    {
                        ar &resultType;
                    }

                    //补偿
                    else if(name.compare("xb") == 0)
                    {
                        ar &xb;
                    }

                    else if(name.compare("yb") == 0)
                    {
                        ar &yb;
                    }
                    else if(name.compare("rb") == 0)
                    {
                        ar &rb;
                    }
                    else if(name.compare("xa") == 0)
                    {
                        ar &xa;
                    }
                    else if(name.compare("ya") == 0)
                    {
                        ar &ya;
                    }
                    else if(name.compare("ra") == 0)
                    {
                        ar &ra;
                    }

                    //中心线间距

                    else if(name.compare("centerLineD") == 0)
                    {
                        ar &centerLineD;
                    }

                    else if(name.compare("plc_ip") == 0)
                    {
                        ar &plc_ip;
                    }
                    else if(name.compare("rotateType") == 0)
                    {
                        ar &rotateType;
                    }
                    else if(name.compare("net_port") == 0)
                    {
                        ar &net_port;
                    }
                    else if(name.compare("JobWorkerEnd") == 0)
                    {
                        break;
                    }
                    else
                    {
                        LOGE("deserialization false,could find %s", name.data());
                        std::string tmp_obj;
                        ar &tmp_obj;
                    }
                }
                catch(boost::archive::archive_exception &e)
                {
                    //                    std::cout << "OnlineCheck deserialization:" << e.what() << std::endl;
                    //                    std::cout << e.what() << std::endl;
                    LOGE_HIGH(e.what());
                    break;
                }
            }
        }
    }

    //    void save_img(Yolo::BoxArray boxs);

    ///
    /// \brief runShapeMatch
    ///
    void runShapeMatch(cv::Mat img);

    ///
    /// \brief runCaliperTool
    ///
    void runCaliperTool(cv::Mat img);


    ///
    /// \brief runLocation
    ///
    void runLocation(cv::Mat img);

    ///
    /// \brief train_location
    ///
    void train_location();

    ///
    /// \brief show_train_model
    ///
    void show_train_model();

    ///
    /// \brief show_chips_contour
    ///
    void show_chips_contour();
    void runCoarsePositioning(cv::Mat img);
};

#endif // JOBWORKER_H
