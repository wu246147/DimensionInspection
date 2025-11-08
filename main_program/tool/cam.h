#ifndef CAM_H
#define CAM_H

//相机库
#include "../MvCamera.h"
#include "string"
#include <QThread>
#include <QtConcurrent>
//opencv 库
#include "opencv2/opencv.hpp"
// my 库
#include "mylog.h"
//boost库
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/serialization/split_free.hpp"
#include "boost/serialization/vector.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

///
/// \brief IsBayer8PixelFormat 判断是否灰度图
/// \param enType 输入类别
/// \return 返回判断结果
///
bool IsBayer8PixelFormat(MvGvspPixelType enType);

///
/// \brief IsColorPixelFormat 判断是否彩图
/// \param enType 输入类别
/// \return 返回判断结果
///
bool IsColorPixelFormat(MvGvspPixelType enType);

///
/// \brief IsMonoPixelFormat 判断是否灰度图
/// \param enType 输入类别
/// \return 返回判断结果
///
bool IsMonoPixelFormat(MvGvspPixelType enType);

class Cam: public QObject
{
    Q_OBJECT
public:
    Cam();
    Cam(int _cam_id);
    Cam(const Cam &_cam);

    Cam &operator = (const Cam &_cam);

    ~Cam();
public:
    ///
    /// 保存参数
    ///

    //相机id
    int cam_id;
    //worker id
    int worker_id = 0;

    //仿真路径，只访问当前路径下的图片
    std::string src_image_dir;
    //取象触发延迟时间
    float trigger_delay = 0;
    //相机曝光时间
    float exposure_time = 100;
    //相机增益
    float gain = 1.0;


    ///
    /// 不保存参数
    ///
    ///
    cv::Mat img;
    int acq_count = 0;

    //相机类
    CMvCamera *m_pcMyCamera = nullptr; // ch:CMyCamera封装了常用接口 | en:CMyCamera packed commonly used interface

    //是否打开相机设备
    bool                    b_open_device = false;                         // ch:是否打开设备 | en:Whether to open device
    //是否开始抓图
    bool                    b_start_grabbing = false;                     // ch:是否开始抓图 | en:Whether to start grabbing
    //取流线程句柄
    void                   *h_grab_thread = nullptr;                         // ch:取流线程句柄 | en:Grab thread handle
    //取像线程状态
    bool                    b_thread_state;
    //相机完成取向线程
    QFuture<void> fut;

//函数
public:
    ///
    /// \brief acqfifo_complete 取像完成事件
    ///
    void acqfifo_complete();
    //相机图像类型判读

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
            name = "CamEnd";
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
                    else if(name.compare("CamEnd") == 0)
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
                    LOGE_HIGH(e.what());
                    break;
                }
            }
        }
    }


public slots:


signals:
    ///
    /// \brief finish_acq 完成取像信号
    /// \param acq_count 当前取像个数
    /// \para img 当前采集图片
    ///
    void finish_acq(int _cam_id, int worker_id);



};

#endif // CAM_H
