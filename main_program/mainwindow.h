#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QtXml>
#include <QDomDocument>
#include <QMessageBox>
#include <QDir>
#include <QGridLayout>
#include <QSplashScreen>
#include <QFileDialog>
#include <QDebug>
#include <QThread>
#include <QtConcurrent>
#include <QMainWindow>

#include <QDesktopServices>
#include <QFontDatabase>
#include<mutex>
#include "jobworker.h"
#include "jobmanager.h"
#include "dialogsimulationset.h"
#include "dialoglinecam.h"
#include "dialogoutputset.h"
#include "dialogtoolpara.h"
#include "forminterface.h"
#include "dialogcalibration.h"
//log
#include "mylog.h"
#include "forminterface.h"
#include "SharePara.h"
#include "tool/communication.h"
#include "tool/communicationcom.h"
#include "tool/communicationmodbus.h"
#include "tool/projecttool.h"

#include "dialogrobotacq.h"

#pragma execution_character_set("utf-8")

void get_files(std::string dir, std::vector<std::string> *file_paths);

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void extracted();
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 是否显示结果窗口
    bool isShowForm = false;

    //操作模式 ture为自动模式，false为手动选点发送模式。
    // bool isAutoMode = true;

    int cam_id = 0;

    // 运行线程
    bool isRunning = true;
    QFuture<void> runFut;

    void runThread();



    // 作业

    ///
    /// \brief new_file 新建作业
    ///
    void new_file();

    ///
    /// \brief open_file 打开作业
    ///
    void open_file();

    ///
    /// \brief save_file 保存作业
    ///
    void save_file();

    ///
    /// \brief save_as 另存作业
    ///
    void save_as();

    // ///
    // /// \brief close 关闭软件，当点击菜单关闭软件时调用
    // ///
    // void close();

    ///
    /// \brief do_after_loadfile 读取文件后调用函数
    ///
    void do_after_loadfile();

    ///
    /// \brief deserilize 作业读取
    /// \param name 作业文件名
    /// \return 返回读取结果
    ///
    bool deserilize(QString name);

    ///
    /// \brief serilize 作业保存
    /// \param name 作业文件名
    /// \return 返回保存结果
    ///
    bool serilize(QString name);

    ///
    /// \brief make_all_dir 创建所有必要的文件夹
    ///
    void make_all_dir();

    ///
    /// \brief readCommunicationPara 读取communication文件
    /// \return
    ///
    bool readCommunicationPara();

    ///
    /// \brief writeCommunicationPara 保存communication文件
    /// \return
    ///
    bool writeCommunicationPara();

    ///
    /// \brief read_config 读取config文件
    /// \return
    ///
    bool read_config();

    ///
    /// \brief write_config 保存config文件
    /// \return
    ///
    bool write_config();

    // 设置

    ///
    /// \brief para_setting 参数设置，未启用
    ///
    void para_setting();

    ///
    /// \brief cam_setting 相机设置，未启用
    ///
    void cam_setting();

    ///
    /// \brief help 帮助，未启用
    ///
    void help();

    // 显示

    ///
    /// \brief set_display 部署显示界面
    ///
    void set_display();

    ///
    /// \brief reg_jobover_event 部署作业完成事件
    ///
    void reg_jobover_event();

    ///
    /// \brief remove_jobover_event 清除作业完成事件
    ///
    void remove_jobover_event();

    ///
    /// \brief reg_jobstart_event 部署作业开始事件
    ///
    void reg_jobstart_event();

    ///
    /// \brief remove_jobstart_event 清除作业开始事件
    ///
    void remove_jobstart_event();

    ///
    /// \brief reg_update_display_img_event 部署显示图像更新事件
    ///
    void reg_update_display_img_event();

    ///
    /// \brief remove_update_display_img_event 清除显示图像更新事件
    ///
    void remove_update_display_img_event();

    ///
    /// \brief regCheckCamEvent 部署切换显示窗口事件
    ///
    void regCheckCamEvent();

    ///
    /// \brief removeCheckCamEvent 清除切换显示窗口事件
    ///
    void removeCheckCamEvent();

    // ///
    // /// \brief regChangeDisplayEvent 部署切换显示窗口事件
    // ///
    // void regChangeDisplayEvent();

    // ///
    // /// \brief removeChangeDisplayEvent 清除切换显示窗口事件
    // ///
    // void removeChangeDisplayEvent();

    // ///
    // /// \brief regChangeShowModeEvent 部署切换显示模式事件
    // ///
    // void regChangeShowModeEvent();

    // ///
    // /// \brief removeChangeShowModeEvent 清除切换显示模式事件
    // ///
    // void removeChangeShowModeEvent();

    // ///
    // /// \brief regChangeRunModelEvent 部署切换运行模式事件
    // ///
    // void regChangeRunModelEvent();

    // ///
    // /// \brief removeChangeRunModelEvent 清除切换运行模式事件
    // ///
    // void removeChangeRunModelEvent();

    // ///
    // /// \brief regChangeFileEvent 部署切换作业事件
    // ///
    // void regChangeFileEvent();

    // ///
    // /// \brief removeChangeFileEvent 清除切换作业事件
    // ///
    // void removeChangeFileEvent();

    // ///
    // /// \brief regChangeFileEvent 部署切换作业事件
    // ///
    // void regSendImagePixelEvent();

    // ///
    // /// \brief removeChangeFileEvent 清除切换作业事件
    // ///
    // void removeSendImagePixelEvent();



    ///
    /// \brief reset_data 重置数据函数
    ///
    void reset_data();

    ///
    /// \brief update_count_view 更新数据统计界面函数
    ///
    void update_count_view();

    // mvs cam

    ///
    /// \brief set_cam_mode 设置相机模式函数，自动和手动模式
    ///
    void set_cam_mode();

    // 计时

    ///
    /// \brief init_clock 初始化记时函数，
    ///
    void init_clock();

    ///
    /// \brief set_work_mode_type 设置工作模式，自动、手动、仿真模式
    /// \param new_work_mode_type 当前工作模式
    ///
    void set_work_mode_type(work_mode_type new_work_mode_type);

    ///
    /// \brief show_log_result 日志显示函数，未启用
    /// \param work_id
    ///
    void show_log_result(int work_id);

    ///
    /// \brief get_files 获取当前路径文件列表函数
    /// \param dir 输入当前路径
    /// \param file_paths 返回文件列表
    ///
    void get_files(std::string dir, std::vector<std::string> *file_paths);

    ///
    /// \brief licence_check 许可证查询
    ///
    void licence_check();

    // 推理线程

    // 自动运行线程，已禁用
    QFuture<void> run_fut;
    // 自动运行线程列表
    std::vector<QFuture<void> *> run_fut_list;

    ///
    /// \brief auto_run_station2 自动运行函数，已禁用
    ///
    void auto_run_station2();

    ///
    /// \brief auto_run_jobworker 自动运行函数
    /// \param cam_id 相机id
    ///
    void auto_run_jobworker(int cam_id);

    ///
    /// \brief auto_run_jobworkers 自动运行函数，已禁用
    ///
    void auto_run_jobworkers();

    // 是否启用自动运行，正常在切换成自动模式后启用
    bool keep_run = false;

    ///
    /// \brief Append 添加信息事件，用于绑定控件显示
    /// \param text 显示信息
    ///
    void Append(const QString &text);

    ///
    /// \brief job_result_output 结果输出函数
    /// \param work_id 相机id
    ///
    void job_result_output(int work_id);

    void show_jobname();

    void setTTF();

    QFont loadFont(QString name, int fontSize);

    void addStatisticsData(int cam_id);

    void showForm();

    void changeDisplay();

    void changeShowMode();

    void changeFile(std::string file_base_name);

    void showCenterLine(int work_id);

    ///
    /// \brief continueGrab 实时取像
    ///
    void continueGrab(int cam_id);
    QFuture<void> continueGrabFut;
    bool isGrabFinish = false;

    void enableMenubar(bool enable);

private slots:
    void on_actionopen_triggered();

    void on_actionsave_triggered();

    void on_action_save_as_triggered();

    void on_actionclose_triggered();

    void on_actionpara_setting_triggered();

    void on_actioncam_setting_triggered();

    void on_actionoutput_setting_triggered();

    void on_actionabout_nengge_triggered();

    void on_actionnew_file_triggered();

    void dispose_slot();

    void on_action_simulation_triggered();

    ///
    /// \brief jobover_event 作业结束事件
    /// \param work_id 相机id
    ///
    void jobover_event(int work_id);

    ///
    /// \brief jobstart_event 作业开始事件
    /// \param work_id 相机id
    ///
    void jobstart_event(int work_id);

    ///
    /// \brief update_display_img_even 更新画面显示事件
    /// \param work_id 相机id
    ///
    void update_display_img_even(int work_id);

    void on_action_reset_triggered();

    void on_action_next_img_triggered();

    void on_action_last_img_triggered();

    ///
    /// \brief circulate_even 循环检测事件，用于测试高速检测效果
    ///
    void circulate_even();

    //    void on_action_auto_mode_triggered();

    //    void on_action_hand_mode_triggered();

    //    void on_action_simulation_mode_triggered();

    void on_action_check_one_triggered();

    void on_action_simulation_path_setting_triggered();

    ///
    /// \brief add_log_even 添加日志事件
    /// \param message 输入日志
    ///
    void add_log_even(QString message);

    ///
    /// \brief clean_log_even 清空日志事件
    ///
    void clean_log_even();

    void on_action_save_img_triggered(bool checked);

    ///
    /// \brief qss_init 界面风格初始化函数，用于切换界面风格
    ///
    void qss_init();

    void on_action_auto_mode_triggered(bool checked);

    void on_action_hand_mode_triggered(bool checked);

    void on_action_simulation_mode_triggered(bool checked);

    void on_action_not_save_image_triggered(bool checked);

    void on_action_save_ng_image_triggered(bool checked);

    void on_actionng_triggered();

    void on_pushButton_min_clicked();

    void on_pushButton_max_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_3_clicked();


    void on_pushButton_start_clicked(bool checked);

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_6_clicked();

    void changeDisplayEven(int _cam_id);

    void changeShowModeEven(bool _isAutoMode);

    void changeRunModelEven(int cam_id, bool _isAutoMode);
    void changeFileEven(std::string file_base_name);

    void sendImagePixelEven1(float x, float y, int r, int g, int b);

    void sendImagePixelEven2(float x, float y, int r, int g, int b);

    void on_pushButton_7_clicked();

    void on_action_calibration_triggered();

    int calibrationCamTest(int cam_id, float robotX, float robotY);

    void changeCenterLineDistEven(int cam_id, int centerLineDist);


    //添加鼠标拖动事件
    void  mousePressEvent(QMouseEvent* event)  override;

    void  mouseMoveEvent(QMouseEvent* event) override;

    void on_pushButton_test_clicked();

    void on_pushButton_sim_clicked(bool checked);

    void on_pushButton_save_calibration_data_clicked();

    void on_pushButton_correction_clicked();

    int changeFileAndCheckCam(int cam_id, int worker_id, std::string fileName);


    void on_pushButton_sim_2_clicked(bool checked);

    void robotGrab();


signals:

    ///
    /// \brief add_log_signal 添加日志信号
    /// \param message 添加日志
    ///
    void add_log_signal(QString message);

    ///
    /// \brief clean_log_signal 清空日志信号
    ///
    void clean_log_signal();

private:


    void closeEvent(QCloseEvent *event);


    void useTransformLocationPoints(); // 示例函数，用于演示如何使用transform_location_points
    Ui::MainWindow *ui;

    //工作模式
    work_mode_type m_work_mode_type = work_mode_type::hand_mode;

    //相机类型
    int cam_type = 2;   //1是大华，2是海康

    //显示窗口布局控件
    QGridLayout *display_grid_layout = nullptr;

    //统计结果布局控件
    QGridLayout *result_table_grid_layout = nullptr;

    //统计对象名显示列表
    std::vector<QLabel *> count_name_label_list;

    //统计取像个数显示列表，未启用
    std::vector<QLabel *> check_label_list;
    //统计等待个数显示列表，未启用
    std::vector<QLabel *> wait_for_label_list;
    //统计ng个数显示列表
    std::vector<QLabel *> ng_count_label_list;
    //统计ok个数显示列表
    std::vector<QLabel *> ok_count_label_list;
    //统计总个数显示列表
    std::vector<QLabel *> total_count_label_list;
    //统计通过概率显示列表
    std::vector<QLabel *> total_pass_rate_label_list;

    //统计发送产品坐标个数显示列表
    std::vector<QLabel *> product_count_label_list;
    //统计发送产品坐标个数显示列表
    std::vector<QLabel *> send_count_label_list;


    //统计整个作业ng个数显示控件
    QLabel  *all_ng_count_label = nullptr;
    //统计整个作业ok个数显示控件
    QLabel  *all_ok_count_label = nullptr;
    //统计整个作业总个数显示控件
    QLabel  *all_total_count_label = nullptr;
    //统计整个作业通过概率显示控件
    QLabel  *all_total_pass_rate_label = nullptr;

    //作业保存路径
    QString job_root_path;
    //作业保存名称
    QString job_name;
    //作业相机个数，默认为5，正常不能改变
    const int cam_count = 1;
    const int jobworker_count = 2;

    //整个作业管理对象
    JobManager jobmanager;

    //机器人通讯管理对象
    CommunicationModbus communicationModbus;
    //判断线程是否正在处理
    bool isProcessingModbus = false;

    //机器人2通讯管理对象
    Communication communication;


    //光源通讯管理对象
    COM communicationCOM;


    //    string target_image_path = "D:/data/project/corrugated_paper/normal/2(K55530295)_test/Image_20221201114402274.bmp";
    //    string src_image_dir = "D:/data/project/corrugated_paper/normal/2(K55530295)_test";

    //作业完成锁
    std::mutex jobover_locker;

    //相机完成个数统计，用于判断是否工位已经完成
    int handled_cam_number = 0;
    //工位3相机完成个数统计，用于判断是否工位已经完成，已停用
    int handled_cam_number_thrid = 0;
    //工位3相机个数，已停用
    const int handled_cam_count = 3;
    //工位1相机结果保存列表，已停用
    std::vector<bool> handled_cam_0_result ;




    //现在只要一个，要改成数组
    //    int tmp_src_img_id = 1;
    //仿真图像id记录列表，用于记录仿真时跑到第几张图片
    std::vector<int> tmp_src_img_id_list;
    //相机
    //    CMvCamera              *m_pcMyCamera;                           // ch:CMyCamera封装了常用接口 | en:CMyCamera packed commonly used interface
    //未启用
    QFuture<void> fut;

    //用时统计，未启用
    clock_t jobmanager_start_time, jobmanager_end_time;
    double jobmanager_total_use_time;
    std::vector<clock_t> jobworkers_start_time, jobworkers_end_time;
    std::vector<double> jobworkers_total_use_time;

    //循环跑测试
    bool circulate = false;
    //判断是否完成一次循环
    bool jobmanager_finished = false;

    // //licence 工具
    // License_control License_control_tool;
    //强行关闭，如果为true，关闭时不需要作任何提醒
    bool fouce_close = false;


    //色调读取文件
    QFile *qssFile;

    // // 假设有一个外部变量来跟踪ID
    // int last_id = 0; // 静态变量，确保它在函数调用之间保持状态

    //相机实时
    std::vector<bool> isContineGrab{false, false};

    std::vector<bool> isContineGrabFinish{true, true};

    //鼠标拖动事件
    QPoint diff_pos;  // 鼠标和窗口的相对位移
    QPoint window_pos;
    QPoint mouse_pos;

};

///
/// \brief save_jobmanager 保存作业函数
/// \param es 作业管理对象
/// \param file_path 保存路径
/// \return
///
bool save_jobmanager(const JobManager &es, std::string file_path);

///
/// \brief load_jobmanager 读取作业函数
/// \param es  作业管理对象
/// \param file_path 读取作业路径
/// \return
///
bool load_jobmanager(JobManager &es, std::string file_path);


#endif // MAINWINDOW_H
