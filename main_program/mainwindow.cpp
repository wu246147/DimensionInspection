#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qDebug"
#include "jobworker.h"


template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    int nn = n ;
    std::ostringstream out;
    out.setf(std::ios::fixed);
    out << std::setprecision(nn) << a_value;
    return out.str();
}


void get_files(std::string dir, std::vector<std::string> *file_paths)
{
    try
    {
        QDir d(dir.data());

        if(d.exists())
        {
            d.setFilter(QDir::Files | QDir::NoSymLinks);
            QFileInfoList list = d.entryInfoList();
            for(int i = 0; i < list.size(); i++)
            {
                QFileInfo info = list.at(i);
                file_paths->push_back(dir + "/" + info.fileName().toStdString());
            }

        }
    }
    catch(std::exception e)
    {
        LOGE("error:%s", e.what());

    }
}



void MainWindow::licence_check()
{
    // // 检查license文件
    // // return 1:    license文件不存在
    // // return 2:    license文件被修改过
    // // return 3:    当前时间小于上次使用时间
    // // return 4:    当前时间大于有效期
    // int result = License_control_tool.checkExpire();
    // if(result == 1)
    // {
    //     QMessageBox::warning(this, "License检测警告.", "没有找到软件License文件，请确认是否已经购买软件License.", QMessageBox::StandardButton::Ok);
    //     fouce_close = true;
    //     exit(0);
    // }
    // else if(result == 2)
    // {
    //     QMessageBox::warning(this, "License检测警告.", "软件License文件可能被修改，请向供应商确认情况.", QMessageBox::StandardButton::Ok);
    //     fouce_close = true;
    //     exit(0);
    // }
    // else if(result == 3)
    // {
    //     QMessageBox::warning(this, "License检测警告.", "当前时间比上次运行时间早，请确认系统时间设置是否存在问题.", QMessageBox::StandardButton::Ok);
    //     fouce_close = true;
    //     exit(0);
    // }
    // else if(result == 4)
    // {
    //     QMessageBox::warning(this, "License检测警告.", "软件License已经过期，请向供应商购买软件License.", QMessageBox::StandardButton::Ok);
    //     fouce_close = true;
    //     exit(0);
    // }
}

void MainWindow::auto_run_station2()
{
    //    qDebug()<<"start auto_run_station2,keep_run:"<<keep_run<<".this->isVisible()"<<this->isVisible();
    while(keep_run && this->isVisible())
    {
        bool is_run = true;
        for(int i = 2; i < jobmanager.jobworkers.size(); i++)
        {
            if(jobmanager.jobworkers[i].tmp_img_list.size() == 0)
            {
                is_run = false;
            }
        }
        //        qDebug()<<"is_run:"<<is_run;
        if(is_run)
        {
            //            qDebug() << "555";
            for(int i = 2; i < jobmanager.jobworkers.size(); i++)
            {
                //                qDebug() << "666";
                jobmanager.jobworkers[i].tmp_img_list_mutex.lock();
                jobmanager.jobworkers[i].img = jobmanager.jobworkers[i].tmp_img_list[0];
                jobmanager.jobworkers[i].tmp_img_list.erase(jobmanager.jobworkers[i].tmp_img_list.begin());
                jobmanager.jobworkers[i].tmp_img_list_mutex.unlock();
                //                qDebug() << "777";
                //                qDebug()<<"width:"<<jobmanager.jobworkers[i].img.size().width<<",height:"<<jobmanager.jobworkers[i].img.size().height;
                emit jobmanager.jobworkers[i].update_display_img_signed(i);
                jobmanager.jobworkers[i].is_busy = true;
                jobmanager.jobworkers[i].run();
                //                qDebug() << "888";
            }
        }
        else
        {
            QThread::msleep(1);
        }
    }
}

void MainWindow::auto_run_jobworker(int cam_id)
{
    while(keep_run && this->isVisible())
    {
        try
        {
            //        std::cout << "cam_id:" << cam_id << std::endl;
            bool is_run = true;
            //        if(jobmanager.jobworkers[cam_id].tmp_img_list.size() > 0)
            //        {
            //            std::cout << "cam_id:" << cam_id << std::endl;
            //        }
            if(jobmanager.jobworkers[cam_id].tmp_img_list.size() == 0 || jobmanager.jobworkers[cam_id].is_busy)
            {
                is_run = false;
            }
            if(is_run)
            {
                LOGE_HIGH("cam %d start auto run", cam_id);
                //            std::cout << "cam_id:" << cam_id << " 111" << std::endl;
                jobmanager.jobworkers[cam_id].tmp_img_list_mutex.lock();
                jobmanager.jobworkers[cam_id].img = jobmanager.jobworkers[cam_id].tmp_img_list[0];
                jobmanager.jobworkers[cam_id].tmp_img_list.erase(jobmanager.jobworkers[cam_id].tmp_img_list.begin());
                jobmanager.jobworkers[cam_id].tmp_img_list_mutex.unlock();
                //            std::cout << "cam_id:" << cam_id << " 222" << std::endl;
                emit jobmanager.jobworkers[cam_id].update_display_img_signed(cam_id);
                //            std::cout << "cam_id:" << cam_id << " 333" << std::endl;
                jobmanager.jobworkers[cam_id].is_busy = true;
                //            std::cout << "cam_id:" << cam_id << " 444" << std::endl;
                jobmanager.jobworkers[cam_id].run();
                //            std::cout << "cam_id:" << cam_id << " 555" << std::endl;
                LOGE_HIGH("cam %d finish auto run", cam_id);
            }
            else
            {
                QThread::msleep(1);
            }
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
}



void MainWindow::auto_run_jobworkers()
{
    while(keep_run && this->isVisible())
    {
        for(int cam_id = 0; cam_id < jobmanager.jobworkers.size(); cam_id++)
        {
            bool is_run = true;
            if(jobmanager.jobworkers[cam_id].tmp_img_list.size() == 0 || jobmanager.jobworkers[cam_id].is_busy)
            {
                is_run = false;
            }
            if(is_run)
            {
                //                std::cout << "cam_id:" << cam_id << " 111" << std::endl;
                jobmanager.jobworkers[cam_id].tmp_img_list_mutex.lock();
                jobmanager.jobworkers[cam_id].img = jobmanager.jobworkers[cam_id].tmp_img_list[0];
                jobmanager.jobworkers[cam_id].tmp_img_list.erase(jobmanager.jobworkers[cam_id].tmp_img_list.begin());
                jobmanager.jobworkers[cam_id].tmp_img_list_mutex.unlock();
                //                std::cout << "cam_id:" << cam_id << " 222" << std::endl;
                emit jobmanager.jobworkers[cam_id].update_display_img_signed(cam_id);
                //                std::cout << "cam_id:" << cam_id << " 333" << std::endl;
                jobmanager.jobworkers[cam_id].is_busy = true;
                //                std::cout << "cam_id:" << cam_id << " 444" << std::endl;
                jobmanager.jobworkers[cam_id].run();
                //                QtConcurrent::run(&jobmanager.jobworkers[cam_id], &JobWorker::run);
                //                std::cout << "cam_id:" << cam_id << " 555" << std::endl;
            }
            else
            {
                QThread::msleep(1);
            }
        }
    }
}

void MainWindow::Append(const QString &text)
{
    emit add_log_signal(text);
}


QFont MainWindow::loadFont(QString name, int fontSize)
{
    //加入字体，并获取字体ID
    int fontId = QFontDatabase::addApplicationFont(name);
    //获取字体名称
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont iconFont = QFont(fontName);
    iconFont.setPixelSize(fontSize);
    return iconFont;
    // QFont iconFont;
    // return iconFont;
}

void MainWindow::show_jobname()
{
    char tmp_job_name[256];
    sprintf(tmp_job_name, "%s", job_name.toStdString().data());
    ui->label_jobname->setText(tmp_job_name);
}

void MainWindow::setTTF()
{

    // QFont iconFont = loadFont("./qss/iconfont.ttf", 14);
    // iconFont.setPointSize(14); // 设置字体大小

    // ui->pushButton_min->setFont(iconFont);
    // ui->pushButton_max->setFont(iconFont);
    // ui->pushButton_close->setFont(iconFont);

    // ui->label_10->setFont(iconFont);
    // ui->label_button_region->setFont(iconFont);
    // ui->label_log_region->setFont(iconFont);
    // ui->label_det_region->setFont(iconFont);
    // ui->label_start->setFont(iconFont);
    // ui->label_stop->setFont(iconFont);

    std::string ttf_path = "./qss/iconfont.ttf";
    QFontDatabase database;
    int fontId = database.addApplicationFont(ttf_path.data());
    QString fontName = database.applicationFontFamilies(fontId).at(0);


    QString styleSheetPushButton = QString("QPushButton{font: 18pt \"%1\";}").arg(fontName);
    QString styleSheetLabel = QString("QLabel {font: 18pt \"%1\";color: #0078dc;border-bottom: 0px;}").arg(fontName);

    ui->pushButton_min->setStyleSheet(styleSheetPushButton);
    ui->pushButton_max->setStyleSheet(styleSheetPushButton);
    ui->pushButton_close->setStyleSheet(styleSheetPushButton);

    ui->label_10->setStyleSheet(styleSheetLabel);
    ui->label_button_region->setStyleSheet(styleSheetLabel);
    ui->label_log_region->setStyleSheet(styleSheetLabel);
    ui->label_det_region->setStyleSheet(styleSheetLabel);
    ui->label_start->setStyleSheet(styleSheetLabel);
    ui->label_stop->setStyleSheet(styleSheetLabel);


    ui->label_10->setText(QChar(0xE60B));
    ui->pushButton_min->setText(QChar(0xe65a));
    ui->pushButton_max->setText(QChar(0xe653));
    ui->pushButton_close->setText(QChar(0xeca0));
    ui->label_button_region->setText(QChar(0xe67e));
    ui->label_log_region->setText(QChar(0xe654));
    ui->label_det_region->setText(QChar(0xe600));
    ui->label_start->setText(QChar(0xe658));
    ui->label_stop->setText(QChar(0xe686));
}

void MainWindow::showForm()
{
    FormInterface *form = new FormInterface(&jobmanager);
    // form->setWindowFlags(Qt::FramelessWindowHint);
    form->show();
}

void MainWindow::changeDisplay()
{
    if(isShowForm)
    {
        for(int i = 0; i < this->display_grid_layout->count(); i++)
        {
            QWidget *p = this->display_grid_layout->itemAt(i)->widget();
            if(i == cam_id)
            {
                p->setVisible(true);
            }
            else
            {
                p->setVisible(false);
            }
        }
    }
    else
    {
        for(int i = 0; i < this->display_grid_layout->count(); i++)
        {
            QWidget *p = this->display_grid_layout->itemAt(i)->widget();
            p->setVisible(true);
        }
    }
}

void MainWindow::changeShowMode()
{
    LOGE_HIGH("start changeShowMode");
    if(isShowForm)
    {
        ui->widget_3->setVisible(false);
        ui->widget_4->setVisible(false);
        ui->widget_14->setVisible(false);
        ui->widget_16->setVisible(false);
        ui->widget_17->setVisible(false);
        this->showNormal();
        // this->setFixedSize(800, 600);
        this->resize(800, 600);
    }
    else
    {
        ui->widget_3->setVisible(true);
        ui->widget_4->setVisible(true);
        ui->widget_14->setVisible(true);
        ui->widget_16->setVisible(true);
        ui->widget_17->setVisible(true);

        setMinimumSize(0, 0);
        setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));

        this->showMaximized();
    }
    changeDisplay();
    if(jobmanager.jobworkers[cam_id].isAutoMode)
    {
        ui->label_mode->setText("自动模式");
    }
    else
    {
        ui->label_mode->setText("手动模式");
    }
    LOGE_HIGH("finish changeShowMode");
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    LOGE_HIGH("MainWindow start init")
    ui->setupUi(this);
    //licence
    licence_check();
    //色调
    // qss_init();
    //全局线程池最大线程数修改
    QThreadPool::globalInstance()->setMaxThreadCount(100);
    //界面部署
    ui->splitter->setStretchFactor(0, 5);
    ui->splitter->setStretchFactor(1, 1);

    // ui->splitter_2->setStretchFactor(0, -1);
    // ui->splitter_2->setStretchFactor(1, 1);

    QList<int> widgetSizes;
    widgetSizes << 0 << 600;
    // ui->splitter_2->setSizes(widgetSizes);
    // ui->toolBar->setVisible(false);
    setTTF();
    ui->pushButton_max->setVisible(true);
    ui->groupBox->setVisible(false);
    ui->pushButton_7->setVisible(false);
    LOGE_HIGH("MainWindow finish UI setting")
    //菜单栏
    //    this->setStyleSheet("QMenuBar{background-color: rgb(197, 235, 255);}");
    //工具栏
    //初始化
    make_all_dir();
    //读取通讯信息
    readCommunicationPara();
    LOGE_HIGH("MainWindow finish readCommunicationPara")
    //读取xml文件
    bool xml_rt = read_config();
    if(!xml_rt)
    {
        QMessageBox::warning(this, "无法找到config文件!!!", "无法找到config文件或上次打开的作业文件不存在，导致无法打开上次的作业文件。请自己打开上次运行的作业文件。",
                             QMessageBox::StandardButton::Ok);
    }
    //初始化jobmanager
    jobmanager = JobManager(cam_count, jobworker_count);
    LOGE_HIGH("MainWindow finish jobmanager init")
    //    write_config();
    // 计时初始化
    init_clock();
    tmp_src_img_id_list = std::vector<int>(jobmanager.jobworkers.size(), 0);
    //判断是否记录上次运行作业，没有则自己创建一个新文件保存
    if(xml_rt)
    {
        //        qDebug() << "start loading file";
        LOGE_PROFESSIONAL("start loading file");
        // std::cout << "H_Pix2Tool:" << cv::format(jobmanager.H_Pix2Tool, cv::Formatter::FMT_NUMPY) << std::endl;

        deserilize(job_name);
        // std::cout << "H_Pix2Tool:" << cv::format(jobmanager.H_Pix2Tool, cv::Formatter::FMT_NUMPY) << std::endl;

    }
    else
    {
        job_name = "tmp";
        serilize(job_name);
        write_config();
        for(int i = 0; i < jobmanager.jobworkers.size(); i++)
        {
            jobmanager.jobworkers[i].init();
        }
    }

    // std::cout << "H_Pix2Tool:" << cv::format(jobmanager.H_Pix2Tool, cv::Formatter::FMT_NUMPY) << std::endl;

    LOGE_HIGH("MainWindow finish jobmanager serilize")
    //界面display初始化
    set_display();
    //事件
    reg_jobover_event();
    reg_jobstart_event();
    reg_update_display_img_event();

    //机器人通讯事件
    // regSendImagePixelEvent();
    regCheckCamEvent();
    // regChangeDisplayEvent();
    // regChangeRunModelEvent();
    // regChangeShowModeEvent();
    // regChangeFileEvent();


    connect(this, &MainWindow::destroyed, this, &MainWindow::dispose_slot);
    connect(this, &MainWindow::add_log_signal, this, &MainWindow::add_log_even);
    connect(this, &MainWindow::clean_log_signal, this, &MainWindow::clean_log_even);
    LOGE_HIGH("MainWindow finish signal even connect")
    //相机初始化
    LOGE_HIGH("mvs_open_device start");
    bool cam_rt = jobmanager.mvs_open_device();
    LOGE_HIGH("MainWindow finish cam init")

    //机器人通讯

    //tcp通讯初始化
    communication.tcp_server_init();
    LOGE_HIGH("MainWindow finish tcp init")
    //机器人通讯
    std::string msg;
    LOGE_HIGH("communicationModbus.plc_ip:%s", communicationModbus.plc_ip.data());
    LOGE_HIGH("communicationModbus.net_port:%d", communicationModbus.net_port);

    communicationModbus.modbusConnect(communicationModbus.plc_ip, communicationModbus.net_port, msg);

    LOGE_HIGH("msg:%s", msg.data());

    communicationCOM.connect();
    //模式
    set_work_mode_type(work_mode_type::hand_mode);
    if(cam_rt)
    {
        jobmanager.set_cam_para();
        jobmanager.mvs_start_grab();
    }
    //显示作业名
    show_jobname();
    // //end
    // for(int id = 0; id < 100; ++id)
    // {
    //     ui->tableWidget->insertRow(0);//在最后插入一行
    //     ui->tableWidget->setItem(0, 0, new QTableWidgetItem(QString(std::to_string(id + 0.01).data()))); //设置单元格内容
    //     ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString(std::to_string(id + 0.01).data()))); //设置单元格内容
    //     ui->tableWidget->setItem(0, 2, new QTableWidgetItem(QString(std::to_string(id + 0.01).data()))); //设置单元格内容
    // }
    //隐藏界面
    changeShowMode();
    LOGE_HIGH("MainWindow finish init")

    // ui->action_calibration->setVisible(false);



}


void MainWindow::runThread()
{
    // std::string msg;
    // bool rt;

    LOGE("heartBeatThread start")
    // rt  = communicationModbus.writeInt(1, 51, QModbusDataUnit::Coils, 0, msg);
    // LOGE("msg：%s", msg.data());

    while(isRunning)
    {
        try
        {
            //等待机器人触发信号
            QThread::msleep(100);

            QMetaObject::invokeMethod(this, "robotGrab",  Qt::QueuedConnection);

        }
        catch(cv::Exception &e)
        {
            LOGE("error:%s", e.what());
        }
        catch(std::exception &e)
        {
            LOGE("error:%s", e.what());

        }
        catch(QException &e)
        {
            LOGE("error:%s", e.what());

        }
        catch(...)
        {
            LOGE("other error.");
        }
    }
}

void MainWindow::robotGrab()
{
    //这个只是用来做一下demo，实际应用不合适，因为把操作和等待都放到主线程进行了。
    std::string msg;
    bool rt;

    if(isProcessingModbus)
    {
        return;
    }
    //读取取像信号
    int value = -1;
    rt  = communicationModbus.readInt(1, 28, QModbusDataUnit::DiscreteInputs, value, msg);
    // LOGE("msg: %s", msg.data());
    // LOGE("value: %d", value);

    if(value != 1)
    {
        return;
    }

    isProcessingModbus = true;

    LOGE("reserve grab")

    //读取取像相机号

    int workerID = -1;
    rt  = communicationModbus.readInt(1, 29, QModbusDataUnit::DiscreteInputs, workerID, msg);
    LOGE("msg：%s", msg.data());



    LOGE("workerID: %d", workerID)

    //取像
    jobmanager.checkCam(0, workerID);

    LOGE("finish checkCam")

    QThread::msleep(100);
    //发送取像完成信号
    rt  = communicationModbus.writeInt(1, 51, QModbusDataUnit::Coils, 1, msg);
    LOGE("msg：%s", msg.data());

    LOGE("send finish checkCam")

    //等待取像信号结束
    int wait_count = 30;
    int cumulative_count = 0;
    while(1)
    {
        int value = -1;
        rt  = communicationModbus.readInt(1, 28, QModbusDataUnit::DiscreteInputs, value, msg);

        if(value == 0)
        {
            break;
        }
        QThread::msleep(100);
        cumulative_count += 1;
        if(cumulative_count >= wait_count)
        {
            // std::cout << "没有收到取像结束信号" << std::endl;
            LOGE("not reserve cam end finish signal");
            break;
        }
    }

    LOGE("get finish grab")

    //关闭取像完成信号
    rt  = communicationModbus.writeInt(1, 51, QModbusDataUnit::Coils, 0, msg);
    LOGE("msg：%s", msg.data());

    LOGE("send end finish checkCam")

    isProcessingModbus = false;
}


MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::new_file()
{
    //创建保存文件
    QString file_path = QFileDialog::getSaveFileName(this, tr("Save a jobfile!"), job_root_path, tr("Jobfile(*.data)"));
    if(NULL == file_path)
    {
        return;
    }
    //判断创建文件是否在job_root_path路径下。
    QFileInfo file_info(file_path);
    QString absolute_dir = file_info.absoluteDir().path();
    if(absolute_dir != job_root_path)
    {
        QMessageBox::warning(this, "作业文件没有保存到软件根目录下.", "作业文件没有保存到软件根目录下。请把作业文件保存到： " + job_root_path, QMessageBox::StandardButton::Ok);
        return ;
    }
    QString file_base_name = file_info.baseName();
    //前期工作
    //断开相机连接
    jobmanager.mvs_stop_grab();
    jobmanager.mvs_close_device();
    //读取作业
    job_name = file_base_name.split(".")[0];
    reset_data();
    //后期工作
    jobmanager = JobManager(cam_count, jobworker_count);
    init_clock();
    tmp_src_img_id_list = std::vector<int>(jobmanager.jobworkers.size(), 0);
    do_after_loadfile();
    serilize(job_name);
    write_config();
    for(int i = 0; i < jobmanager.jobworkers.size(); i++)
    {
        jobmanager.jobworkers[i].init();
    }
}



void MainWindow::changeFile(std::string file_name)
{
    //前期工作
    //断开相机连接
    jobmanager.mvs_stop_grab();
    jobmanager.mvs_close_device();
    // //断开连接
    // jobmanager.tcp_server_dispose();
    //读取作业
    job_name = QString(file_name.data());
    reset_data();
    //显示等待画面
    int center_x = int(this->frameGeometry().x() + this->frameGeometry().width() / 2);
    int center_y = int(this->frameGeometry().y()) + this->frameGeometry().height() / 2;
    QPixmap pixmap("./img/loading.gif");
    QSplashScreen splash(pixmap);
    splash.setGeometry(center_x - 130, center_y - 130, 260, 260);
    splash.setWindowFlag(Qt::WindowStaysOnTopHint, true); //
    splash.show();
    QCoreApplication::processEvents();
    QFuture<void> fut_thread = QtConcurrent::run(this, &MainWindow::deserilize, job_name);
    fut_thread.waitForFinished();
    //    deserilize(job_name);
    splash.hide();
    splash.close();
    //后期工作
    do_after_loadfile();
    write_config();
}

void MainWindow::open_file()
{
    QString file_path = QFileDialog::getOpenFileName(this, tr("Load a jobfile!"), job_root_path, tr("Jobfile(*.data)"));
    //判断是否选择到文件
    if(NULL == file_path)
    {
        QMessageBox::warning(this, "没有选择打开文件.", "没有选择打开文件.", QMessageBox::StandardButton::Ok);
        return;
    }
    //判断文件是否在job_root_path路径下
    QFileInfo file_info(file_path);
    QString file_base_name = file_info.baseName();
    QString job_path = QString(job_root_path + "/" + file_base_name.split(".")[0] + ".data");
    if(!QFile(job_path).exists())
    {
        QMessageBox::warning(this, "作业文件没有保存到软件根目录下.", "作业文件没有保存到软件根目录下。请把作业文件保存到： " + job_root_path, QMessageBox::StandardButton::Ok);
        return ;
    }
    changeFile(file_base_name.split(".")[0].toStdString());
}

void MainWindow::save_file()
{
    //判断之前是否已经创建了保存文件
    if(job_name == "")
    {
        save_as();
    }
    else
    {
        //显示等待画面
        int center_x = int(this->frameGeometry().x() + this->frameGeometry().width() / 2);
        int center_y = int(this->frameGeometry().y()) + this->frameGeometry().height() / 2;
        QPixmap pixmap("./img/loading.gif");
        QSplashScreen splash(pixmap);
        splash.setGeometry(center_x - 130, center_y - 130, 260, 260);
        splash.setWindowFlag(Qt::WindowStaysOnTopHint, true);
        splash.show();
        QCoreApplication::processEvents();
        QFuture<void> fut_thread = QtConcurrent::run(this, &MainWindow::serilize, job_name);
        fut_thread.waitForFinished();
        //    serilize(job_name);
        splash.hide();
        splash.close();
    }
}

void MainWindow::save_as()
{
    QString file_path = QFileDialog::getSaveFileName(this, tr("Save a jobfile!"), job_root_path, tr("Jobfile(*.data)"));
    if(NULL == file_path)
    {
        return;
    }
    QFileInfo file_info(file_path);
    if(file_info.absoluteDir() != job_root_path)
    {
        QMessageBox::warning(this, "作业文件没有保存到软件根目录下.", "作业文件没有保存到软件根目录下。请把作业文件保存到： " + job_root_path, QMessageBox::StandardButton::Ok);
        return ;
    }
    QString file_base_name = file_info.baseName();
    job_name = file_base_name.split(".")[0];
    serilize(job_name);
    write_config();
    //显示作业名
    show_jobname();
}

// void MainWindow::close()
// {
//     bool is_close = false;
//     if(!fouce_close)
//     {
//         int nRet = QMessageBox::warning(this, "作业保存提醒!!!", "请先确认作业是否已经保存."\
//                                         "如果坚持要关闭程序，请按OK按键. 否则请按取消并在关闭程序前，先保存好作业.",
//                                         QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel);
//         switch(nRet)
//         {
//             case QMessageBox::StandardButton::Ok:
//                 is_close = true;
//                 break;
//             case QMessageBox::StandardButton::Cancel:
//                 is_close = false;
//                 break;
//         }
//     }
//     else
//     {
//         is_close = true;
//     }
//     if(is_close)
//     {
//         // //断开连接
//         // jobmanager.tcp_server_dispose();
//         //断开相机连接
//         mvs_stop_grab();
//         mvs_close_device();
//         //关闭事件
//         remove_jobstart_event();
//         remove_jobover_event();
//         remove_update_display_img_event();
//         this->destroy();
//     }
// }

void MainWindow::do_after_loadfile()
{
    //界面display初始化
    set_display();
    //事件
    reg_jobover_event();
    reg_jobstart_event();
    reg_update_display_img_event();
    // regSendImagePixelEvent();
    // regCheckCamEvent();
    // regChangeDisplayEvent();
    // regChangeRunModelEvent();
    // regChangeShowModeEvent();
    // regChangeRunModelEvent();
    //相机初始化
    jobmanager.mvs_open_device();
    jobmanager.set_cam_para();
    jobmanager.mvs_start_grab();
    // tcp通讯初始化
    // jobmanager.tcp_server_init();
    //显示作业名
    show_jobname();
}

void MainWindow::para_setting()
{
}

void MainWindow::cam_setting()
{
}

void MainWindow::help()
{
}

bool MainWindow::deserilize(QString name)
{
    bool rt = true;
    QString job_path = QString(job_root_path + "/" + name + ".data");
    rt = load_jobmanager(jobmanager, job_path.toStdString());
    //    if(rt)
    //    {
    for(int i = 0; i < jobmanager.jobworkers.size(); i++)
    {
        jobmanager.jobworkers[i].init();
    }
    //    //测试，训练模型
    //    for(int i = 0; i < jobmanager.jobworkers.size(); i++)
    //    {
    //        jobmanager.jobworkers[i].train();
    //    }
    //    }
    return rt;
}

bool MainWindow::serilize(QString name)
{
    bool rt = true;
    QString job_path = QString(job_root_path + "/" + name + ".data");
    // LOGE("111");
    rt = save_jobmanager(jobmanager, job_path.toStdString());
    // LOGE("222");
    return rt;
}

void MainWindow::make_all_dir()
{
    job_root_path = QString(QDir::currentPath() + "/job_file");
    QDir jobfile_dir(job_root_path);
    if(!jobfile_dir.exists())
    {
        jobfile_dir.mkdir(job_root_path);
    }
}

// QString createMultipleFolders(const QString path)
// {
//     QString pathReplace = path;
//     pathReplace.replace("\\", "/");
//     QDir dir(pathReplace);
//     if(pathReplace == "" || dir.exists(pathReplace))
//     {
//         return pathReplace;
//     }
//     QString parentDir = createMultipleFolders(pathReplace.mid(0, pathReplace.lastIndexOf('/')));
//     QString dirName = pathReplace.mid(pathReplace.lastIndexOf('/') + 1);
//     QDir parentPath(parentDir);
//     if(!dirName.isEmpty())
//     {
//         parentPath.mkpath(dirName);
//     }
//     return parentDir + "/" + dirName;
// }

bool MainWindow::readCommunicationPara()
{
    bool rt = true;
    QString file_path = QString(QDir::currentPath() + "/communicationPara.xml");
    QFile file(file_path);
    if(!file.exists())
    {
        LOGE_HIGH("Could not exist CommunicationPara")

        rt = false;
        return rt;
    }
    if(!file.open(QFile::ReadOnly))
    {
        LOGE_HIGH("Could not open CommunicationPara")

        rt = false;
        return rt;
    }
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        LOGE_HIGH("Could not open CommunicationPara")

        file.close();
        rt = false;
        return rt;
    }
    file.close();
    QDomElement bookStoreElement = doc.documentElement(); //返回根元素
    QDomNodeList bookNodeList = bookStoreElement.childNodes();
    //读取文件名
    QDomElement bookElement = bookNodeList.at(0).toElement();
    communication.plc_ip = bookElement.attribute("plc_ip", "").toStdString();
    communication.net_port = bookElement.attribute("net_port", "").toInt();

    LOGE_HIGH("communicationModbus.plc_ip:%s", communicationModbus.plc_ip.data());
    LOGE_HIGH("communicationModbus.net_port:%d", communicationModbus.net_port);

    QDomElement bookElement2 = bookNodeList.at(1).toElement();
    communicationModbus.plc_ip = bookElement2.attribute("plc_ip", "").toStdString();
    communicationModbus.net_port = bookElement2.attribute("net_port", "").toInt();
    LOGE_HIGH("communicationModbus.plc_ip:%s", communicationModbus.plc_ip.data());
    LOGE_HIGH("communicationModbus.net_port:%d", communicationModbus.net_port);


    QDomElement bookElement3 = bookNodeList.at(2).toElement();
    communicationCOM.port_name = bookElement3.attribute("port_name", "").toStdString();
    LOGE_HIGH("communicationCOM.port_name:%s", communicationCOM.port_name.data());

    return rt;
}

bool MainWindow::writeCommunicationPara()
{
    bool rt = true;
    QString file_path = QString(QDir::currentPath() + "/communicationPara.xml");
    QFile file(file_path); //相对路径、绝对路径、资源路径都可以
    if(!file.open(QFile::WriteOnly | QFile::Truncate))    //可以用QIODevice，Truncate表示清空原来的内容
    {
        rt = false;
        return rt;
    }
    QDomDocument doc;
    QDomProcessingInstruction instruction; //添加xml说明
    instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement("communtication_config"); //创建根元素
    doc.appendChild(root); //添加根元素
    QDomElement job_element = doc.createElement("communtication_config"); //一级子元素
    job_element.setAttribute("plc_ip", communication.plc_ip.data()); //方式一：创建属性  其中键值对的值可以是各种类型
    job_element.setAttribute("net_port", communication.net_port);//方式一：创建属性  其中键值对的值可以是各种类型
    root.appendChild(job_element);



    QDomElement job_element2 = doc.createElement("communticationModbus_config"); //一级子元素
    job_element2.setAttribute("plc_ip", communicationModbus.plc_ip.data()); //方式一：创建属性  其中键值对的值可以是各种类型
    job_element2.setAttribute("net_port", communicationModbus.net_port);//方式一：创建属性  其中键值对的值可以是各种类型
    root.appendChild(job_element2);

    QDomElement job_element3 = doc.createElement("communticationCOM_config"); //一级子元素
    job_element3.setAttribute("port_name", communicationCOM.port_name.data()); //方式一：创建属性  其中键值对的值可以是各种类型
    root.appendChild(job_element3);


    QTextStream out_stream(&file); //输出到文件
    doc.save(out_stream, 2); //缩进4格
    file.close();
    return rt;
}

bool MainWindow::read_config()
{
    bool rt = true;
    QString file_path = QString(QDir::currentPath() + "/config.xml");
    QFile file(file_path);
    if(!file.open(QFile::ReadOnly))
    {
        rt = false;
        return rt;
    }
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        rt = false;
        return rt;
    }
    file.close();
    QDomElement bookStoreElement = doc.documentElement(); //返回根元素
    QDomNodeList bookNodeList = bookStoreElement.childNodes();
    //读取文件名
    QDomElement bookElement = bookNodeList.at(0).toElement();
    job_name = bookElement.attribute("file_name", "");
    QString job_path = QString(job_root_path + "/" + job_name + ".data");
    QFileInfo file_info(job_path);
    if(!file_info.exists())
    {
        rt = false;
        return rt;
    }
    return rt;
}

bool MainWindow::write_config()
{
    bool rt = true;
    QString file_path = QString(QDir::currentPath() + "/config.xml");
    QFile file(file_path); //相对路径、绝对路径、资源路径都可以
    if(!file.open(QFile::WriteOnly | QFile::Truncate))    //可以用QIODevice，Truncate表示清空原来的内容
    {
        rt = false;
        return rt;
    }
    QDomDocument doc;
    QDomProcessingInstruction instruction; //添加xml说明
    instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement("job_config"); //创建根元素
    doc.appendChild(root); //添加根元素
    QDomElement job_element = doc.createElement("file_config"); //一级子元素
    job_element.setAttribute("file_name", job_name); //方式一：创建属性  其中键值对的值可以是各种类型
    root.appendChild(job_element);
    QTextStream out_stream(&file); //输出到文件
    doc.save(out_stream, 2); //缩进4格
    file.close();
    return rt;
}

void MainWindow::set_display()
{
    //display 清空
    for(int i = 0; i < jobmanager.jobworkers.size() ; i++)
    {
        if(jobmanager.jobworkers[i].display != nullptr)
        {
            delete jobmanager.jobworkers[i].display;
            jobmanager.jobworkers[i].display = nullptr;
        }
    }
    if(this->display_grid_layout != nullptr)
    {
        delete  this->display_grid_layout;
        this->display_grid_layout = nullptr;
    }
    this->display_grid_layout = new QGridLayout(this);
    int row_num, col_num;
    // col_num = qCeil(qSqrt(cam_count));
    // row_num = int(qSqrt(cam_count));
    row_num = 1;
    col_num = jobworker_count;
    for(int i = 0; i < jobmanager.jobworkers.size() ; i++)
    {
        jobmanager.jobworkers[i].display = new myWidgetView();
        jobmanager.jobworkers[i].display->myGraphicsView->setStyleSheet("background-color:rgb(161, 161, 161);border:1px solid black;");
        this->display_grid_layout->addWidget(this->jobmanager.jobworkers[i].display, int(i / col_num), i % col_num);
    }
    ui->widget->setLayout(this->display_grid_layout);
    ui->widget->layout()->activate();
    //结果界面清空
    // for(int i = 0 ; i < count_name_label_list.size(); i++)
    // {
    //     QLabel *count_label = count_name_label_list[i];
    //     delete  count_label;
    // }
    // for(int i = 0 ; i < check_label_list.size(); i++)
    // {
    //     QLabel *count_label = check_label_list[i];
    //     delete  count_label;
    // }
    for(int i = 0 ; i < ng_count_label_list.size(); i++)
    {
        QLabel *count_label = ng_count_label_list[i];
        delete  count_label;
    }
    for(int i = 0 ; i < ok_count_label_list.size(); i++)
    {
        QLabel *count_label = ok_count_label_list[i];
        delete  count_label;
    }
    for(int i = 0 ; i < total_count_label_list.size(); i++)
    {
        QLabel *count_label = total_count_label_list[i];
        delete  count_label;
    }
    // for(int i = 0 ; i < total_pass_rate_label_list.size(); i++)
    // {
    //     QLabel *count_label = total_pass_rate_label_list[i];
    //     delete  count_label;
    // }
    // for(int i = 0 ; i < send_count_label_list.size(); i++)
    // {
    //     QLabel *count_label = send_count_label_list[i];
    //     delete  count_label;
    // }
    // for(int i = 0 ; i < product_count_label_list.size(); i++)
    // {
    //     QLabel *count_label = product_count_label_list[i];
    //     delete  count_label;
    // }
    // check_label_list.clear();
    ng_count_label_list.clear();
    ok_count_label_list.clear();
    total_count_label_list.clear();
    // count_name_label_list.clear();
    // total_pass_rate_label_list.clear();
    // send_count_label_list.clear();
    // product_count_label_list.clear();
    if(this->all_ng_count_label != nullptr)
    {
        delete  this->all_ng_count_label;
        this->all_ng_count_label = nullptr;
    }
    if(this->all_ok_count_label != nullptr)
    {
        delete  this->all_ok_count_label;
        this->all_ok_count_label = nullptr;
    }
    if(this->all_total_count_label != nullptr)
    {
        delete  this->all_total_count_label;
        this->all_total_count_label = nullptr;
    }
    // if(this->all_total_pass_rate_label != nullptr)
    // {
    //     delete  this->all_total_pass_rate_label;
    //     this->all_total_pass_rate_label = nullptr;
    // }
    if(this->result_table_grid_layout != nullptr)
    {
        delete  this->result_table_grid_layout;
        this->result_table_grid_layout = nullptr;
    }
    this->result_table_grid_layout = new QGridLayout(this);
    for(int i = 0 ; i < jobmanager.jobworkers.size(); i++)
    {
        QLabel *count_label = new QLabel(this);
        count_label->setStyleSheet("border:1px solid black;");
        count_label->setAlignment(Qt::AlignCenter);
        count_label->setText("Cam" + QString(std::to_string(i + 1).data()));
        count_name_label_list.push_back(count_label);
        this->result_table_grid_layout->addWidget(count_label, i, 0);
    }
    QLabel *count_name_label = new QLabel(this);
    count_name_label->setStyleSheet("border:1px solid black;");
    count_name_label->setAlignment(Qt::AlignCenter);
    count_name_label->setText("总计");
    this->result_table_grid_layout->addWidget(count_name_label, jobmanager.jobworkers.size(), 0);
    for(int i = 0 ; i < jobmanager.jobworkers.size(); i++)
    {
        QLabel *count_label = new QLabel(this);
        count_label->setStyleSheet("border:1px solid black;");
        count_label->setAlignment(Qt::AlignCenter);
        count_label->setText(QString(std::to_string(0).data()));
        ok_count_label_list.push_back(count_label);
        this->result_table_grid_layout->addWidget(count_label, i, 1);
    }
    for(int i = 0 ; i < jobmanager.jobworkers.size(); i++)
    {
        QLabel *count_label = new QLabel(this);
        count_label->setStyleSheet("border:1px solid black;");
        count_label->setAlignment(Qt::AlignCenter);
        count_label->setText(QString(std::to_string(0).data()));
        ng_count_label_list.push_back(count_label);
        this->result_table_grid_layout->addWidget(count_label, i, 2);
    }
    for(int i = 0 ; i < jobmanager.jobworkers.size(); i++)
    {
        QLabel *count_label = new QLabel(this);
        count_label->setStyleSheet("border:1px solid black;");
        count_label->setAlignment(Qt::AlignCenter);
        count_label->setText(QString(std::to_string(0).data()));
        total_count_label_list.push_back(count_label);
        this->result_table_grid_layout->addWidget(count_label, i, 3);
    }
    // for(int i = 0 ; i < jobmanager.jobworkers.size(); i++)
    // {
    //     QLabel *count_label = new QLabel(this);
    //     count_label->setStyleSheet("border:1px solid black;");
    //     count_label->setAlignment(Qt::AlignCenter);
    //     count_label->setText(QString(std::to_string(0).data()));
    //     total_pass_rate_label_list.push_back(count_label);
    //     this->result_table_grid_layout->addWidget(count_label, i, 4);
    // }
    // for(int i = 0 ; i < jobmanager.jobworkers.size(); i++)
    // {
    //     QLabel *count_label = new QLabel(this);
    //     count_label->setStyleSheet("border:1px solid black;");
    //     count_label->setAlignment(Qt::AlignCenter);
    //     count_label->setText(QString(std::to_string(0).data()));
    //     check_label_list.push_back(count_label);
    //     this->result_table_grid_layout->addWidget(count_label, i, 1);
    // }
    // for(int i = 0 ; i < jobmanager.jobworkers.size(); i++)
    // {
    //     QLabel *count_label = new QLabel(this);
    //     count_label->setStyleSheet("border:1px solid black;");
    //     count_label->setAlignment(Qt::AlignCenter);
    //     count_label->setText(QString(std::to_string(0).data()));
    //     send_count_label_list.push_back(count_label);
    //     this->result_table_grid_layout->addWidget(count_label, i, 3);
    // }
    // for(int i = 0 ; i < jobmanager.jobworkers.size(); i++)
    // {
    //     QLabel *count_label = new QLabel(this);
    //     count_label->setStyleSheet("border:1px solid black;");
    //     count_label->setAlignment(Qt::AlignCenter);
    //     count_label->setText(QString(std::to_string(0).data()));
    //     product_count_label_list.push_back(count_label);
    //     this->result_table_grid_layout->addWidget(count_label, i, 2);
    // }
    all_ng_count_label = new QLabel(this);
    all_ng_count_label->setStyleSheet("border:1px solid black;");
    all_ng_count_label->setAlignment(Qt::AlignCenter);
    all_ng_count_label->setText(QString(std::to_string(0).data()));
    this->result_table_grid_layout->addWidget(all_ng_count_label, jobmanager.jobworkers.size(), 2);
    all_ok_count_label = new QLabel(this);
    all_ok_count_label->setStyleSheet("border:1px solid black;");
    all_ok_count_label->setAlignment(Qt::AlignCenter);
    all_ok_count_label->setText(QString(std::to_string(0).data()));
    this->result_table_grid_layout->addWidget(all_ok_count_label, jobmanager.jobworkers.size(), 1);
    all_total_count_label = new QLabel(this);
    all_total_count_label->setStyleSheet("border:1px solid black;");
    all_total_count_label->setAlignment(Qt::AlignCenter);
    all_total_count_label->setText(QString(std::to_string(0).data()));
    this->result_table_grid_layout->addWidget(all_total_count_label, jobmanager.jobworkers.size(), 3);
    // all_total_pass_rate_label = new QLabel(this);
    // all_total_pass_rate_label->setStyleSheet("border:1px solid black;");
    // all_total_pass_rate_label->setAlignment(Qt::AlignCenter);
    // all_total_pass_rate_label->setText(QString(std::to_string(0).data()));
    // this->result_table_grid_layout->addWidget(all_total_pass_rate_label, jobmanager.jobworkers.size(), 4);
    ui->widget_static->setLayout(this->result_table_grid_layout);
    ui->widget_static->layout()->activate();
}

void MainWindow::reg_jobover_event()
{
    remove_jobover_event();
    for(int i = 0; i < jobmanager.jobworkers.size() ; i++)
    {
        connect(&jobmanager.jobworkers[i], &JobWorker::jobworker_end_signed, this, &MainWindow::jobover_event);
    }
}

void MainWindow::remove_jobover_event()
{
    for(int i = 0; i < jobmanager.jobworkers.size() ; i++)
    {
        disconnect(&jobmanager.jobworkers[i], &JobWorker::jobworker_end_signed, this, &MainWindow::jobover_event);
    }
}

void MainWindow::reg_jobstart_event()
{
    remove_jobstart_event();
    for(int i = 0; i < jobmanager.jobworkers.size() ; i++)
    {
        connect(&jobmanager.jobworkers[i], &JobWorker::jobworker_start_signed, this, &MainWindow::jobstart_event);
    }
}

void MainWindow::remove_jobstart_event()
{
    for(int i = 0; i < jobmanager.jobworkers.size() ; i++)
    {
        disconnect(&jobmanager.jobworkers[i], &JobWorker::jobworker_start_signed, this, &MainWindow::jobstart_event);
    }
}

void MainWindow::reg_update_display_img_event()
{
    remove_update_display_img_event();
    for(int i = 0; i < jobmanager.jobworkers.size() ; i++)
    {
        connect(&jobmanager.jobworkers[i], &JobWorker::update_display_img_signed, this, &MainWindow::update_display_img_even);
    }
}

void MainWindow::remove_update_display_img_event()
{
    for(int i = 0; i < jobmanager.jobworkers.size() ; i++)
    {
        disconnect(&jobmanager.jobworkers[i], &JobWorker::update_display_img_signed, this, &MainWindow::update_display_img_even);
    }
}

void MainWindow::regCheckCamEvent()
{
    removeCheckCamEvent();
    connect(&communication, &Communication::checkCamSignal, this, &MainWindow::changeFileAndCheckCam);
    // connect(&communication, &Communication::focusingSignal, &jobmanager, &JobManager::focusingCam);
    // connect(&communication, &Communication::calibrationSignal, &jobmanager, &JobManager::calibrationCam);
    //仿真测试
    // connect(&communication, &Communication::calibrationSignal, this, &MainWindow::calibrationCamTest);

}

void MainWindow::removeCheckCamEvent()
{
    disconnect(&communication, &Communication::checkCamSignal, this, &MainWindow::changeFileAndCheckCam);
    // disconnect(&communication, &Communication::focusingSignal, &jobmanager, &JobManager::focusingCam);
    // disconnect(&communication, &Communication::calibrationSignal, &jobmanager, &JobManager::calibrationCam);
    // disconnect(&communication, &Communication::calibrationSignal, this, &MainWindow::calibrationCamTest);

}

// void MainWindow::regChangeDisplayEvent()
// {
//     removeChangeDisplayEvent();
//     connect(&communication, &Communication::changeDisplaySignal, this, &MainWindow::changeDisplayEven);
// }

// void MainWindow::removeChangeDisplayEvent()
// {
//     disconnect(&communication, &Communication::changeDisplaySignal, this, &MainWindow::changeDisplayEven);
// }

// void MainWindow::regChangeShowModeEvent()
// {
//     removeChangeShowModeEvent();
//     connect(&communication, &Communication::changeShowModeSignal, this, &MainWindow::changeShowModeEven);
//     connect(&communication, &Communication::changeCenterLineDistSignal, this, &MainWindow::changeCenterLineDistEven);

// }

// void MainWindow::removeChangeShowModeEvent()
// {
//     disconnect(&communication, &Communication::changeShowModeSignal, this, &MainWindow::changeShowModeEven);
//     disconnect(&communication, &Communication::changeCenterLineDistSignal, this, &MainWindow::changeCenterLineDistEven);

// }

// void MainWindow::regChangeRunModelEvent()
// {
//     removeChangeRunModelEvent();
//     connect(&communication, &Communication::changeRunModelSignal, this, &MainWindow::changeRunModelEven);
// }

// void MainWindow::removeChangeRunModelEvent()
// {
//     disconnect(&communication, &Communication::changeRunModelSignal, this, &MainWindow::changeRunModelEven);
// }

// void MainWindow::regChangeFileEvent()
// {
//     removeChangeFileEvent();
//     connect(&communication, &Communication::changeFileSignal, this, &MainWindow::changeFileEven);
// }

// void MainWindow::removeChangeFileEvent()
// {
//     disconnect(&communication, &Communication::changeFileSignal, this, &MainWindow::changeFileEven);
// }

// void MainWindow::regSendImagePixelEvent()
// {
//     removeSendImagePixelEvent();
//     connect(jobmanager.jobworkers[0].display->myGraphicsView, &MyGraphicsView::show_click_point_signals, this, &MainWindow::sendImagePixelEven1);
//     connect(jobmanager.jobworkers[1].display->myGraphicsView, &MyGraphicsView::show_click_point_signals, this, &MainWindow::sendImagePixelEven2);
// }

// void MainWindow::removeSendImagePixelEvent()
// {
//     disconnect(jobmanager.jobworkers[0].display->myGraphicsView, &MyGraphicsView::show_click_point_signals, this, &MainWindow::sendImagePixelEven1);
//     disconnect(jobmanager.jobworkers[1].display->myGraphicsView, &MyGraphicsView::show_click_point_signals, this, &MainWindow::sendImagePixelEven2);
// }

void MainWindow::reset_data()
{
    //状态重置
    jobmanager.is_busy = false;
    for(int i = 0; i < jobmanager.jobworkers.size(); i++)
    {
        jobmanager.jobworkers[i].is_busy = false;
    }
    // //获取图像时间重置
    // for(int i = 0; i < jobmanager.jobworkers.size(); i++)
    // {
    //     jobmanager.jobworkers[i].tmp_img_time_list.clear();
    // }
    //缓存输出结果重置
    for(int i = 0; i < jobmanager.jobworkers.size(); i++)
    {
        jobmanager.jobworkers[i].tmp_result_list.clear();
    }
    //仿真id重置
    //    tmp_src_img_id = 1;
    for(int i = 0; i < jobmanager.jobworkers.size(); i++)
    {
        tmp_src_img_id_list[i] = 0;
    }
    //jobover 触发次数重置
    handled_cam_number = 0;
    handled_cam_number_thrid = 0;
    handled_cam_0_result.clear();
    //jobworker result次数重置
    for(int i = 0; i < jobmanager.jobworkers.size(); i++)
    {
        jobmanager.jobworkers[i].ok_count = 0;
        jobmanager.jobworkers[i].ng_count = 0;
        jobmanager.jobworkers[i].check_count = 0;
        jobmanager.jobworkers[i].wait_for_count = 0;
    }
    jobmanager.ok_count = 0;
    jobmanager.ng_count = 0;
    //控件复原
    ui->label_result->setStyleSheet("QLabel{color:rgb(128,128,128);}");
    ui->label_result->setText("--");
    ui->label_time->setText("--------------------");
    update_count_view();
    ui->tableWidget->clear();
    ui->textEdit_log->clear();
}

void MainWindow::addStatisticsData(int cam_id)
{
    ui->tableWidget->insertRow(0);
    QDateTime currentDateTime = QDateTime::currentDateTime();
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem(currentDateTime.toString("MM-dd hh:mm:ss"))); //设置单元格内容
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem(to_string_with_precision(jobmanager.dist, 3).data())); //设置单元格内容
    std::string rt = jobmanager.result() ? "OK" : "NG";
    ui->tableWidget->setItem(0, 2, new QTableWidgetItem(rt.data())); //设置单元格内容



}

void MainWindow::update_count_view()
{
    try
    {
        for(int i = 0; i < jobmanager.jobworkers.size() ; i++)
        {
            ng_count_label_list.at(i)->setText(std::to_string(jobmanager.jobworkers[i].ng_count).data());
            ok_count_label_list.at(i)->setText(std::to_string(jobmanager.jobworkers[i].ok_count).data());
            total_count_label_list.at(i)->setText(std::to_string(jobmanager.jobworkers[i].all_count()).data());
        }
        all_ng_count_label->setText(std::to_string(jobmanager.ng_count).data());
        all_ok_count_label->setText(std::to_string(jobmanager.ok_count).data());
        all_total_count_label->setText(std::to_string(jobmanager.all_count()).data());
        ui->label_total_count->setText(std::to_string(jobmanager.all_count()).data());
        ui->label_ok_count->setText(std::to_string(jobmanager.ok_count).data());
        ui->label_ng_count->setText(std::to_string(jobmanager.ng_count).data());
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

void MainWindow::set_cam_mode()
{
    for(int i = 0; i < jobmanager.cams.size(); i++)
    {
        if(jobmanager.cams[i].m_pcMyCamera == nullptr)
        {
            continue;
        }
        if(m_work_mode_type == work_mode_type::auto_mode)
        {
            //            jobmanager.jobworkers[i].m_pcMyCamera->SetEnumValue("TriggerSelector", 1);
            _MV_CAM_TRIGGER_MODE_ m_trigger_mode = MV_TRIGGER_MODE_ON;
            jobmanager.cams[i].m_pcMyCamera->SetEnumValue("TriggerMode", m_trigger_mode);
            if(cam_type == 2)
            {
                _MV_CAM_TRIGGER_SOURCE_ m_trigger_source = MV_TRIGGER_SOURCE_LINE0;//设置line1
                jobmanager.cams[i].m_pcMyCamera->SetEnumValue("TriggerSource", m_trigger_source);
            }
            else
            {
                jobmanager.cams[i].m_pcMyCamera->SetEnumValue("TriggerSource", 2);//设置line1
            }
        }
        else if(m_work_mode_type == work_mode_type::hand_mode)
        {
            //            jobmanager.jobworkers[i].m_pcMyCamera->SetEnumValue("TriggerSelector", 1);
            _MV_CAM_TRIGGER_MODE_ m_trigger_mode = MV_TRIGGER_MODE_ON;
            jobmanager.cams[i].m_pcMyCamera->SetEnumValue("TriggerMode", m_trigger_mode);
            if(cam_type == 2)
            {
                _MV_CAM_TRIGGER_SOURCE_ m_trigger_source = MV_TRIGGER_SOURCE_SOFTWARE;//设置software
                jobmanager.cams[i].m_pcMyCamera->SetEnumValue("TriggerSource", m_trigger_source);
            }
            else
            {
                jobmanager.cams[i].m_pcMyCamera->SetEnumValue("TriggerSource", 0);//设置software
            }
        }
        else if(m_work_mode_type == work_mode_type::simulation_mode)
        {
            //            jobmanager.jobworkers[i].m_pcMyCamera->SetEnumValue("TriggerSelector", 1);
            _MV_CAM_TRIGGER_MODE_ m_trigger_mode = MV_TRIGGER_MODE_ON;
            jobmanager.cams[i].m_pcMyCamera->SetEnumValue("TriggerMode", m_trigger_mode);
            if(cam_type == 2)
            {
                _MV_CAM_TRIGGER_SOURCE_ m_trigger_source = MV_TRIGGER_SOURCE_SOFTWARE;//设置software
                jobmanager.cams[i].m_pcMyCamera->SetEnumValue("TriggerSource", m_trigger_source);
            }
            else
            {
                jobmanager.cams[i].m_pcMyCamera->SetEnumValue("TriggerSource", 0);//设置software
            }
        }
    }
}


void MainWindow::init_clock()
{
    jobworkers_start_time = std::vector<clock_t>(jobmanager.jobworkers.size(), clock());
    jobworkers_end_time = std::vector<clock_t> (jobmanager.jobworkers.size(), clock());
    jobworkers_total_use_time = std::vector<double>(jobmanager.jobworkers.size(), 0);
}

void MainWindow::set_work_mode_type(work_mode_type new_work_mode_type)
{
    m_work_mode_type = new_work_mode_type;
    for(int i = 0; i < jobmanager.jobworkers.size(); i++)
    {
        jobmanager.jobworkers[i].m_work_mode_type = new_work_mode_type;
    }
    ui->menubar->setEnabled(false);
    ui->actionnew_file->setVisible(false);
    ui->actionopen->setVisible(false);
    ui->actionsave->setVisible(false);
    ui->actionpara_setting->setVisible(false);
    ui->actioncam_setting->setVisible(false);
    ui->action_reset->setVisible(false);
    ui->action_simulation->setVisible(false);
    ui->action_last_img->setVisible(false);
    ui->action_next_img->setVisible(false);
    ui->action_check_one->setVisible(false);
    ui->action_auto_mode->setChecked(false);
    ui->action_simulation_mode->setChecked(false);
    ui->action_hand_mode->setChecked(false);
    if(m_work_mode_type == work_mode_type::auto_mode)
    {
        set_cam_mode();
        ui->action_auto_mode->setChecked(true);
        keep_run = false;
        QThread::msleep(100);
        //        run_fut = QtConcurrent::run(this, &MainWindow::auto_run_station2);
        //        std::cout << "set_work_mode_type 111" << std::endl;
        //清空之前的空线程
        //        for (auto tmp = run_fut_list.begin(); tmp != run_fut_list.end();)
        //        {
        //            if((*tmp)->isFinished())
        //            {
        //                tmp = run_fut_list.erase(tmp);
        //            }
        //            else
        //            {
        //                tmp++;
        //            }
        //        }
        //        run_fut_list.clear();
        //        std::cout << "set_work_mode_type 222" << std::endl;
        keep_run = true;
        //添加循环线程
        for(int i = 0; i < jobmanager.jobworkers.size(); i++)
        {
            auto tmp_run_fut = QtConcurrent::run(this, &MainWindow::auto_run_jobworker, i);
            run_fut_list.push_back(&tmp_run_fut);
        }
        //        std::cout << "set_work_mode_type 333" << std::endl;
        //        keep_run = false;
        //        QThread::msleep(100);
        //        keep_run = true;
        //        run_fut = QtConcurrent::run(this, &MainWindow::auto_run_jobworkers);
    }
    else if(m_work_mode_type == work_mode_type::hand_mode)
    {
        set_cam_mode();
        keep_run = false;
        for(int i = 0; i < jobmanager.jobworkers.size(); i++)
        {
            jobmanager.jobworkers[i].tmp_img_list.clear();
        }
        ui->action_hand_mode->setChecked(true);
        ui->menubar->setEnabled(true);
        ui->actionnew_file->setVisible(true);
        ui->actionopen->setVisible(true);
        ui->actionsave->setVisible(true);
        ui->actionpara_setting->setVisible(true);
        ui->actioncam_setting->setVisible(true);
        ui->action_reset->setVisible(true);
        ui->action_simulation->setVisible(false);
        ui->action_last_img->setVisible(false);
        ui->action_next_img->setVisible(false);
        ui->action_check_one->setVisible(true);
        //        std::cout << "set_work_mode_type 111" << std::endl;
        //        //清空之前的空线程
        //        keep_run = false;
        //        QThread::msleep(100);
        ////        for (auto tmp = run_fut_list.begin(); tmp != run_fut_list.end();)
        ////        {
        ////            std::cout << "(*tmp)->isFinished():" << (*tmp)->isFinished() << std::endl;
        ////            if((*tmp)->isFinished())
        ////            {
        ////                tmp = run_fut_list.erase(tmp);
        ////            }
        ////            else
        ////            {
        ////                tmp++;
        ////            }
        ////        }
        //        run_fut_list.clear();
        ////        std::cout << "set_work_mode_type 222" << std::endl;
        //        //添加循环线程
        //        keep_run = true;
        //        for (int i = 0; i < jobmanager.jobworkers.size(); i++)
        //        {
        //            auto tmp_run_fut = QtConcurrent::run(this, &MainWindow::auto_run_jobworker, i);
        //            run_fut_list.push_back(&tmp_run_fut);
        //        }
        //        std::cout << "set_work_mode_type 333" << std::endl;
        //        keep_run = false;
        //        QThread::msleep(100);
        //        keep_run = true;
        //        run_fut = QtConcurrent::run(this, &MainWindow::auto_run_jobworkers);
    }
    else
    {
        set_cam_mode();
        keep_run = false;
        for(int i = 0; i < jobmanager.jobworkers.size(); i++)
        {
            jobmanager.jobworkers[i].tmp_img_list.clear();
        }
        ui->action_simulation_mode->setChecked(true);
        ui->menubar->setEnabled(true);
        ui->actionnew_file->setVisible(true);
        ui->actionopen->setVisible(true);
        ui->actionsave->setVisible(true);
        ui->actionpara_setting->setVisible(true);
        ui->actioncam_setting->setVisible(true);
        ui->action_reset->setVisible(true);
        ui->action_simulation->setVisible(true);
        ui->action_last_img->setVisible(true);
        ui->action_next_img->setVisible(true);
        ui->action_check_one->setVisible(false);
    }
}



void MainWindow::on_actionopen_triggered()
{
    open_file();
}


void MainWindow::on_actionsave_triggered()
{
    save_file();
}


void MainWindow::on_action_save_as_triggered()
{
    save_as();
}


void MainWindow::on_actionclose_triggered()
{
    close();
}


void MainWindow::on_actionpara_setting_triggered()
{
    try
    {
        //打开设置界面
        DialogToolPara *dialog = new DialogToolPara(&jobmanager, this);
        //        DialogToolParaTest *dialog = new DialogToolParaTest(&jobmanager);
        Qt::WindowFlags flags = Qt::Dialog;
        //        flags |= Qt::WindowMinMaxButtonsHint;
        flags |= Qt::WindowCloseButtonHint;
        dialog->setWindowFlags(flags);
        dialog->setWindowFlag(Qt::WindowStaysOnTopHint, false); //


        for(int id = 0; id < jobmanager.jobworkers.size(); ++id)
        {
            jobmanager.jobworkers[id].isDebug = true;
        }
        connect(dialog, &DialogToolPara::save_file_signal, this, &MainWindow::save_file);
        dialog->exec();
        disconnect(dialog, &DialogToolPara::save_file_signal, this, &MainWindow::save_file);
        //不能删除，否则会把jobmanager也删除掉。看一下后面怎么处理好
        //    delete dialog;
        //display更新
        for(int id = 0; id < jobmanager.jobworkers.size(); ++id)
        {
            jobmanager.jobworkers[id].isDebug = false;
        }
        set_display();
        //复位
        reset_data();
    }
    catch(std::exception &e)
    {
        LOGE_HIGH(e.what());
    }
    catch(QException &e)
    {
        LOGE_HIGH(e.what());
    }
}


void MainWindow::on_actioncam_setting_triggered()
{
    //    //打开设置界面
    if(jobmanager.m_cam_image_type == cam_image_type::line_type)
    {
        //        DialogLineCam *dialog = new DialogLineCam(&jobmanager, this);
        DialogLineCam *dialog = new DialogLineCam(&jobmanager);
        Qt::WindowFlags flags = Qt::Dialog;
        //        flags |= Qt::WindowMinMaxButtonsHint;
        flags |= Qt::WindowCloseButtonHint;
        dialog->setWindowFlags(flags);
        dialog->setWindowFlag(Qt::WindowStaysOnTopHint, false); // ??????
        //        dialog->show();
        dialog->exec();
        //复位
        reset_data();
        //不能删除，否则会把jobmanager也删除掉。看一下后面怎么处理好
        //    delete dialog;
        //更新
    }
    else
    {
        DialogLineCam *dialog = new DialogLineCam(&jobmanager, this);
        Qt::WindowFlags flags = Qt::Dialog;
        //        flags |= Qt::WindowMinMaxButtonsHint;
        flags |= Qt::WindowCloseButtonHint;
        dialog->setWindowFlags(flags);
        dialog->setWindowFlag(Qt::WindowStaysOnTopHint, false); // ??????
        //        dialog->show();
        dialog->exec();
        //复位
        reset_data();
        //不能删除，否则会把jobmanager也删除掉。看一下后面怎么处理好
        //    delete dialog;
    }
}


void MainWindow::on_actionoutput_setting_triggered()
{
    //关闭socket服务器
    communication.tcp_server_dispose();
    //关闭机器人通讯
    communicationModbus.modbusDisconnect();

    communicationCOM.disconnect();
    //    //打开设置界面
    DialogOutputSet *dialog = new DialogOutputSet(&jobmanager, &communication, this);
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowCloseButtonHint;
    dialog->setWindowFlags(flags);
    dialog->setWindowFlag(Qt::WindowStaysOnTopHint, false);
    dialog->exec();
    // 保存通讯参数
    writeCommunicationPara();
    // tcp通讯初始化
    communication.tcp_server_init();

    //机器人通讯
    std::string msg;
    communicationModbus.modbusConnect(communicationModbus.plc_ip, communicationModbus.net_port, msg);

    communicationCOM.connect();

}


void MainWindow::on_actionabout_nengge_triggered()
{
}


void MainWindow::on_actionnew_file_triggered()
{
    new_file();
}




void MainWindow::on_action_simulation_triggered()
{
    //    qDebug()<<111;
    for(int i = 0 ; i < jobmanager.jobworkers.size() ; i++)
    {
        if(jobmanager.jobworkers[i].src_image_dir != "")
        {
            try
            {
                //测试
                std::vector<std::string> src_image_paths;
                get_files(jobmanager.jobworkers[i].src_image_dir, &src_image_paths);
                if(src_image_paths.size() < 1)
                {
                    continue;
                }
                std::string src_image_path = src_image_paths[tmp_src_img_id_list[i]];
                //                qDebug() << src_image_path.data();
                cv::Mat src_image;
                //                qDebug()<<333;
                src_image = cv::imread(src_image_path, -1);
                //                qDebug()<<444;
                jobmanager.jobworkers[i].img = src_image;
                //        jobmanager.jobworkers[i].display->cleanItem();
                //        jobmanager.jobworkers[i].display->setImage(MatToPixmap(src_image));
                jobmanager.jobworkers[i].update_display_img_signed(i);
                //                qDebug()<<555;
                //                QtConcurrent::run(&jobmanager, &JobManager::rundemo, i);
                //                qDebug()<<666;
                //                QThread::msleep(1);
                //        jobmanager.rundemo(i);
            }
            catch(std::exception e)
            {
                LOGE_HIGH(e.what());
            }
        }
    }
    for(int i = 0 ; i < jobmanager.jobworkers.size() ; i++)
    {
        //        QtConcurrent::run(&jobmanager, &JobManager::rundemo, i);
        jobmanager.rundemo(i);
    }
}


void MainWindow::on_action_reset_triggered()
{
    reset_data();
}


void MainWindow::on_action_next_img_triggered()
{
    //    qDebug()<<111;
    for(int i = 0 ; i < jobmanager.jobworkers.size() ; i++)
    {
        if(jobmanager.jobworkers[i].src_image_dir != "")
        {
            //测试
            std:: vector<std::string> src_image_paths;
            //            qDebug()<<222;
            get_files(jobmanager.jobworkers[i].src_image_dir, &src_image_paths);
            if(src_image_paths.size() < 1)
            {
                continue;
            }
            tmp_src_img_id_list[i] += 1;
            //            qDebug()<<333;
            if(tmp_src_img_id_list[i] > src_image_paths.size() - 1)
            {
                tmp_src_img_id_list[i] = 0;
            }
            std::string src_image_path = src_image_paths[tmp_src_img_id_list[i]];
            cv::Mat src_image;
            //            qDebug()<<444;
            src_image = cv::imread(src_image_path, -1);
            // LOGE("jobmanager->jobworkers[cam_id].rotateType:%d", jobmanager.jobworkers[i].rotateType);
            // if(jobmanager.jobworkers[i].rotateType >= 0)
            // {
            //     cv::rotate(src_image, src_image, jobmanager.jobworkers[i].rotateType);
            // }

            //            qDebug()<<555;
            jobmanager.jobworkers[i].img = src_image;
            //        jobmanager.jobworkers[i].display->cleanItem();
            //        jobmanager.jobworkers[i].display->setImage(MatToPixmap(src_image));
            jobmanager.jobworkers[i].update_display_img_signed(i);
            //    }
            //    for (int i = 0 ; i < jobmanager.jobworkers.size() ; i++ )
            //    {
            //            qDebug()<<666;
            //            QtConcurrent::run(&jobmanager, &JobManager::rundemo, i);
            //            qDebug()<<777;
            //        jobmanager.rundemo(i);
        }
    }
    for(int i = 0 ; i < jobmanager.jobworkers.size() ; i++)
    {
        // QtConcurrent::run(&jobmanager, &JobManager::rundemo, i);
        jobmanager.rundemo(i);
    }
}


void MainWindow::on_action_last_img_triggered()
{
    for(int i = 0 ; i < jobmanager.jobworkers.size() ; i++)
    {
        if(jobmanager.jobworkers[i].src_image_dir != "")
        {
            //测试
            std::vector<std::string> src_image_paths;
            get_files(jobmanager.jobworkers[i].src_image_dir, &src_image_paths);
            if(src_image_paths.size() < 1)
            {
                continue;
            }
            tmp_src_img_id_list[i] -= 1;
            if(tmp_src_img_id_list[i] < 0)
            {
                tmp_src_img_id_list[i] = src_image_paths.size() - 1;
            }
            std::string src_image_path = src_image_paths[tmp_src_img_id_list[i]];
            cv::Mat src_image;
            src_image = cv::imread(src_image_path, -1);
            jobmanager.jobworkers[i].img = src_image;
            //        jobmanager.jobworkers[i].display->cleanItem();
            //        jobmanager.jobworkers[i].display->setImage(MatToPixmap(src_image));
            jobmanager.jobworkers[i].update_display_img_signed(i);
            //    }
            //    for (int i = 0 ; i < jobmanager.jobworkers.size() ; i++ )
            //    {
            //            QtConcurrent::run(&jobmanager, &JobManager::rundemo, i);
            //        jobmanager.rundemo(i);
        }
    }
    for(int i = 0 ; i < jobmanager.jobworkers.size() ; i++)
    {
        //        QtConcurrent::run(&jobmanager, &JobManager::rundemo, i);
        jobmanager.rundemo(i);
    }
}

void MainWindow::circulate_even()
{
    while(circulate)
    {
        if(jobmanager_finished)
        {
            jobmanager_finished = false;
            on_action_simulation_triggered();
            //            for (int i = 0 ; i < jobmanager.jobworkers.size() ; i++ )
            //            {
            //                qDebug() << "circulate start time:" << clock();
            //                QtConcurrent::run(&jobmanager, &JobManager::rundemo, i);
            //            }
        }
    }
}

void MainWindow::dispose_slot()
{
}




void save_jobmanager_old(const JobManager &es, std::string file_path)
{
    std::ofstream ofs(file_path);
    boost::archive::text_oarchive oa(ofs);
    oa << es;
}

void load_jobmanager_old(JobManager &es, std::string file_path)
{
    std::ifstream ifs(file_path);
    boost::archive::text_iarchive ia(ifs);
    ia >> es;
}

bool save_jobmanager(const JobManager &es, std::string file_path)
{
    bool rt = true;
    std::ofstream os(file_path.c_str(), std::ios::binary);
    if(!os.fail())
    {
        try
        {
            boost::archive::binary_oarchive oa(os);
            oa << es;
        }
        catch(std::exception &e)
        {
            LOGE_HIGH(e.what());
            rt = false;
        }
    }
    return rt;
}

bool load_jobmanager(JobManager &es, std::string file_path)
{
    bool rt = true;
    std::ifstream is(file_path.c_str(), std::ios::binary);
    if(!is.fail())
    {
        try
        {
            boost::archive::binary_iarchive ia(is);
            ia >> es;
        }
        catch(std::exception &e)
        {
            LOGE_HIGH(e.what());
            rt = false;
        }
    }
    return rt;
}


void MainWindow::show_log_result(int work_id)
{
}

void MainWindow::get_files(std::string dir, std::vector<std::string> *file_paths)
{
    try
    {
        QDir d(dir.data());
        d.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList list = d.entryInfoList();
        for(int i = 0; i < list.size(); i++)
        {
            QFileInfo info = list.at(i);
            file_paths->push_back(dir + "/" + info.fileName().toStdString());
        }
    }
    catch(std::exception e)
    {
        LOGE_HIGH(e.what());
    }
}



void MainWindow::job_result_output(int work_id)
{
    try
    {

        //界面更新
        if(work_id == 1)
        {
            bool rt = jobmanager.result();
            jobmanager.TongJi(rt);
            update_count_view();
            if(rt)
            {
                ui->label_result->setStyleSheet("QLabel{color:rgb(0,255,0);font: 700 28pt \"Microsoft YaHei UI\";}");
                ui->label_result->setText("OK");
                QDateTime currentDateTime = QDateTime::currentDateTime();
                ui->label_time->setText(currentDateTime.toString("yyyy-MM-dd hh:mm:ss"));
            }
            else
            {
                ui->label_result->setStyleSheet("QLabel{color:rgb(255,0,0);font: 700 28pt \"Microsoft YaHei UI\";}");
                ui->label_result->setText("NG");
                QDateTime currentDateTime = QDateTime::currentDateTime();
                ui->label_time->setText(currentDateTime.toString("yyyy-MM-dd hh:mm:ss"));
            }
            addStatisticsData(work_id);

        }
        //结果输出
        if(work_id == 0)
        {
            char tmp_info[256];
            sprintf(tmp_info, "0,0,0,0,0,0,0,0,0,0,0,0,0,");
            communication.tcp_send(tmp_info);
        }
        else
        {
            char tmp_info[256];
            sprintf(tmp_info, "0,0,0,0,0,0,0,0,0,0,0,0,%f,", jobmanager.dist);
            communication.tcp_send(tmp_info);
        }

    }
    catch(cv::Exception &e)
    {
        LOGE_HIGH("%s", e.what());
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



void MainWindow::jobover_event(int work_id)
{
    try
    {
        if(jobmanager.jobworkers[work_id].isGrabbing)
        {
            jobworkers_total_use_time[work_id] = (double)(jobmanager.jobworkers[work_id].jobworker_end_time - jobmanager.jobworkers[work_id].jobworker_start_time) / CLOCKS_PER_SEC;
            QString message = QString(("jobworker" + std::to_string(work_id) + " total use time: " +
                                       std::to_string(jobworkers_total_use_time[work_id]) + " s").data()) ;
            LOGE_HIGH("%s", message.toStdString().data());
            jobmanager.jobworkers[work_id].is_busy = false;
        }
        else
        {
            //如果是第二次拍照，要计算最终距离
            if(work_id == 1)
            {
                jobmanager.getDistance();
            }

            jobworkers_total_use_time[work_id] = (double)(jobmanager.jobworkers[work_id].jobworker_end_time - jobmanager.jobworkers[work_id].jobworker_start_time) / CLOCKS_PER_SEC;
            QString message = QString(("jobworker" + std::to_string(work_id) + " total use time: " +
                                       std::to_string(jobworkers_total_use_time[work_id]) + " s").data()) ;
            LOGE_HIGH("%s", message.toStdString().data());
            {
                bool rt = jobmanager.jobworkers[work_id].result();
                jobmanager.TongJi(work_id, rt);
            }
            //over:
            jobmanager.jobworkers[work_id].is_busy = false;
            jobmanager.jobworkers[work_id].show_result();
            LOGE_HIGH("jobworker %d show result", work_id);
            //输出结果函数
            job_result_output(work_id);
        }


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

void MainWindow::jobstart_event(int work_id)
{
    //    jobworkers_start_time[work_id] = clock();
}

void MainWindow::showCenterLine(int work_id)
{
    QGraphicsLineItem *tmpLine = new QGraphicsLineItem(0, 0, 100, 100);
    jobmanager.jobworkers[work_id].display->myGraphicsView->cleanItem(tmpLine);
    delete tmpLine;


    float centerY =  jobmanager.jobworkers[work_id].img.rows / 2;
    float centerX =  jobmanager.jobworkers[work_id].img.cols / 2;
    float endY = jobmanager.jobworkers[work_id].img.rows;
    float endX = jobmanager.jobworkers[work_id].img.cols;

    QPen pn(Qt::SolidLine);
    pn.setColor(Qt::red);
    pn.setWidth(5);


    jobmanager.jobworkers[work_id].centerLineY = new QGraphicsLineItem(QLineF(QPointF(0, centerY), QPointF(endX, centerY)));
    jobmanager.jobworkers[work_id].centerLineY->setPen(pn);

    jobmanager.jobworkers[work_id].display->myGraphicsView->addItem(jobmanager.jobworkers[work_id].centerLineY, false, false);



    jobmanager.jobworkers[work_id].centerLineY2 = new QGraphicsLineItem(QLineF(QPointF(0, centerY - jobmanager.jobworkers[work_id].centerLineD)
        , QPointF(endX, centerY - jobmanager.jobworkers[work_id].centerLineD)));
    jobmanager.jobworkers[work_id].centerLineY2->setPen(pn);

    jobmanager.jobworkers[work_id].display->myGraphicsView->addItem(jobmanager.jobworkers[work_id].centerLineY2, false, false);


    jobmanager.jobworkers[work_id].centerLineY3 = new QGraphicsLineItem(QLineF(QPointF(0, centerY + jobmanager.jobworkers[work_id].centerLineD)
        , QPointF(endX, centerY + jobmanager.jobworkers[work_id].centerLineD)));
    jobmanager.jobworkers[work_id].centerLineY3->setPen(pn);

    jobmanager.jobworkers[work_id].display->myGraphicsView->addItem(jobmanager.jobworkers[work_id].centerLineY3, false, false);

    jobmanager.jobworkers[work_id].centerLineX = new QGraphicsLineItem(QLineF(QPointF(centerX, 0), QPointF(centerX, endY)));
    jobmanager.jobworkers[work_id].centerLineX->setPen(pn);

    jobmanager.jobworkers[work_id].display->myGraphicsView->addItem(jobmanager.jobworkers[work_id].centerLineX, false, false);

}

static void waitTime(int time)
{
    QThread::msleep(time);
}

void MainWindow::continueGrab(int cam_id)
{
    isContineGrabFinish [cam_id] = false;

    while(isContineGrab[cam_id])
    {
        jobmanager.cams[cam_id].m_pcMyCamera->CommandExecute("TriggerSoftware");
        //等待取像完成
        for(int id = 0; id < 50; ++id)
        {
            QCoreApplication::processEvents();
            QThread::msleep(10);
        }
        //等待运行结束
        while(jobmanager.jobworkers[cam_id].is_busy)
        {
            QThread::msleep(10);
        }

    }
    //确保处理完成
    for(int id = 0; id < 50; ++id)
    {
        QCoreApplication::processEvents();
        QThread::msleep(10);
    }
    isContineGrabFinish [cam_id] = true;

}

void MainWindow::update_display_img_even(int work_id)
{
    jobmanager.jobworkers[work_id].display->myGraphicsView->cleanItem();
    if(!jobmanager.jobworkers[work_id].img.empty())
    {
        jobmanager.jobworkers[work_id].display->myGraphicsView->setImage(MatToPixmap(jobmanager.jobworkers[work_id].img));

        //加入中心线显示
        // showCenterLine(work_id);

    }
}


void MainWindow::on_action_check_one_triggered()
{
    // for(int i = 0; i < jobmanager.jobworkers.size(); i++)
    // {
    //     if(jobmanager.jobworkers[i].m_pcMyCamera == nullptr)
    //     {
    //         LOGE_PROFESSIONAL("cam%d not init", i);
    //         //            qDebug() << ("cam" + std::to_string(i) + " not init").data();
    //         continue;
    //     }
    //     LOGE_PROFESSIONAL("cam%d TriggerSoftware", i);
    //     //        qDebug() << ("cam" + std::to_string(i) + " TriggerSoftware").data();
    //     jobmanager.jobworkers[i].m_pcMyCamera->CommandExecute("TriggerSoftware");
    // }
}


void MainWindow::on_action_simulation_path_setting_triggered()
{
    //打开设置界面
    DialogSimulationSet *dialog = new DialogSimulationSet(&jobmanager, this);
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowCloseButtonHint;
    dialog->setWindowFlags(flags);
    dialog->setWindowFlag(Qt::WindowStaysOnTopHint, false);
    dialog->exec();
    //不能删除，否则会把jobmanager也删除掉。看一下后面怎么处理好
    //    delete dialog;
    //更新
}

void MainWindow::add_log_even(QString message)
{
    QString old_message = ui->textEdit_log->toPlainText();
    //    message = message + "\n" + old_message;
    message = message + "\n"  + QString(old_message.toStdString().substr(0, 5000).data());
    ui->textEdit_log->setText(message);
}

void MainWindow::clean_log_even()
{
    ui->textEdit_log->clear();
}



void MainWindow::on_action_save_img_triggered(bool checked)
{
    if(checked)
    {
        ui->action_not_save_image->setChecked(false);
        ui->action_save_ng_image->setChecked(false);
        for(int i = 0; i < jobmanager.jobworkers.size(); i++)
        {
            jobmanager.jobworkers[i].is_save_img = checked;
            jobmanager.jobworkers[i].is_save_ng_img = !checked;
        }
    }
}

void MainWindow::qss_init()
{
    qssFile = new QFile("./qss/darkblue.css", this);
    qssFile->open(QFile::ReadOnly); //只读方式打开文件
    QString styleSheet = tr(qssFile->readAll()); //转为QString类型
    this->setStyleSheet(styleSheet); //设置样式表
    qssFile->close();
}

void MainWindow::on_action_auto_mode_triggered(bool checked)
{
    if(checked)
    {
        int nRet = QMessageBox::warning(this, "模式切换提醒",
                                        "切换成自动模型前，先确保流水线先停止下来。"
                                        "切换自动模式后，再启动检测流水线。"
                                        "否则容易出现产品误触发，导致产品误踢！！！请按标准执行。",
                                        QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel);
        switch(nRet)
        {
            case QMessageBox::StandardButton::Ok:
                break;
            case QMessageBox::StandardButton::Cancel:
                on_action_auto_mode_triggered(false);
                on_action_hand_mode_triggered(true);
                return;
        }
        jobmanager.mvs_stop_grab();
        set_work_mode_type(work_mode_type::auto_mode);
        jobmanager.mvs_start_grab();
        LOGE_HIGH("切换到自动模式");
    }
}




void MainWindow::on_action_hand_mode_triggered(bool checked)
{
    if(checked)
    {
        jobmanager.mvs_stop_grab();
        set_work_mode_type(work_mode_type::hand_mode);
        jobmanager.mvs_start_grab();
        LOGE_HIGH("切换到手动模式");
    }
}

void MainWindow::on_action_simulation_mode_triggered(bool checked)
{
    if(checked)
    {
        jobmanager.mvs_stop_grab();
        set_work_mode_type(work_mode_type::simulation_mode);
        jobmanager.mvs_start_grab();
        LOGE_HIGH("切换到仿真模式");
    }
}


void MainWindow::on_action_not_save_image_triggered(bool checked)
{
    if(checked)
    {
        ui->action_save_img->setChecked(false);
        ui->action_save_ng_image->setChecked(false);
        for(int i = 0; i < jobmanager.jobworkers.size(); i++)
        {
            jobmanager.jobworkers[i].is_save_img = !checked;
            jobmanager.jobworkers[i].is_save_ng_img = !checked;
        }
    }
}

void MainWindow::on_action_save_ng_image_triggered(bool checked)
{
    if(checked)
    {
        ui->action_not_save_image->setChecked(false);
        ui->action_save_img->setChecked(false);
        for(int i = 0; i < jobmanager.jobworkers.size(); i++)
        {
            jobmanager.jobworkers[i].is_save_img = checked;
            jobmanager.jobworkers[i].is_save_ng_img = checked;
        }
    }
}

void MainWindow::on_actionng_triggered()
{
    //    QString strLogPath = QString("start /data/save_ng_img");
    //    QProcess process(this);
    //    process.setProgram("cmd");//设置命令形式
    //    QStringList argument;
    //    argument << "/c" << strLogPath;//在cmd命令中，/c代表“执行完命令后返回”。它表示在执行完命令后，程序将返回到调用cmd的环境中。使用/c参数可以避免程序在执行过长命令时处于等待状态而呈现假死的现象。
    //    process.setArguments(argument);
    //    process.start();
    //    process.waitForStarted(); //等待程序启动
    //    process.waitForFinished();//等待程序关闭
    QString path = "/data/save_ng_img";
    QDesktopServices::openUrl(QUrl("file:" + path, QUrl::TolerantMode));
}


void MainWindow::on_pushButton_min_clicked()
{
    showMinimized();
}


void MainWindow::on_pushButton_max_clicked()
{
    if(this->windowState() == Qt::WindowNoState)
    {
        showMaximized();
    }
    else
    {
        showNormal();
        this->resize(1600, 1024);
    }
}


void MainWindow::on_pushButton_close_clicked()
{
    this->close();
}


void MainWindow::on_pushButton_start_clicked(bool checked)
{
    if(checked)
    {
        ui->label_start->setStyleSheet("QLabel { background-color: rgb(60, 68, 81); }");
        // ui->pushButton_stop->setChecked(false);
        on_action_auto_mode_triggered(true);
    }
    else
    {
        ui->label_start->setStyleSheet("QLabel { background-color: rgb(38, 56, 81); }");
        on_action_hand_mode_triggered(true);
    }
}



void MainWindow::on_pushButton_3_clicked()
{
    if(m_work_mode_type == work_mode_type::hand_mode)
    {
        //开灯
        communicationCOM.serial->write("SA0255#");
        QThread::msleep(10);

        int r = jobmanager.checkCam(0, 0);

        //关灯
        int sleepTime = jobmanager.cams[0].exposure_time / 1000;
        sleepTime += 10;
        QThread::msleep(sleepTime);
        communicationCOM.serial->write("SA0000#");
    }
    else if(m_work_mode_type == work_mode_type::simulation_mode)
    {
        on_action_next_img_triggered();
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if(m_work_mode_type == work_mode_type::hand_mode)
    {
        //开灯
        communicationCOM.serial->write("SB0255#");
        QThread::msleep(10);

        int r = jobmanager.checkCam(0, 1);

        //关灯
        int sleepTime = jobmanager.cams[0].exposure_time / 1000;
        sleepTime += 10;
        QThread::msleep(sleepTime);
        communicationCOM.serial->write("SB0000#");

    }
    else if(m_work_mode_type == work_mode_type::simulation_mode)
    {
        on_action_next_img_triggered();
    }
}


void MainWindow::on_pushButton_5_clicked()
{
    // showWindowType = ShowWindowType::ShowForm;
    isShowForm = !isShowForm;
    changeShowMode();
}


void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    cam_id = index;
    changeShowMode();
}


void MainWindow::on_pushButton_6_clicked()
{
    on_action_next_img_triggered();
}

void MainWindow::changeDisplayEven(int _cam_id)
{
    // cam_id = _cam_id;
    // changeDisplay();
    ui->comboBox->setCurrentIndex(_cam_id);
    // on_comboBox_currentIndexChanged(_cam_id);
}

void MainWindow::changeShowModeEven(bool _isShowForm)
{
    isShowForm = _isShowForm;
    changeShowMode();
}

void MainWindow::changeRunModelEven(int cam_id, bool _isAutoMode)
{
    jobmanager.jobworkers[cam_id].isAutoMode = _isAutoMode;
    if(jobmanager.jobworkers[cam_id].isAutoMode)
    {
        // LOGE("11");

        ui->label_mode->setText("自动模式");
        isContineGrab[cam_id] = false;
        while(isContineGrabFinish [cam_id] == false)
        {
            QThread::msleep(10);
        }
        // continueGrabFut.waitForFinished();
        jobmanager.jobworkers[cam_id].isGrabbing = false;
        // LOGE("22");
        enableMenubar(true);

    }
    else
    {
        ui->label_mode->setText("手动模式");
        if(jobmanager.cams[cam_id].m_pcMyCamera == nullptr)
        {
            return;
        }
        // LOGE("111");
        //先把线程关掉，确保前面没有打开。
        isContineGrab[cam_id] = false;
        while(isContineGrabFinish [cam_id] == false)
        {
            QThread::msleep(10);
        }
        // continueGrabFut.waitForFinished();//等待线程结束没有效果
        jobmanager.jobworkers[cam_id].isGrabbing = false;
        // LOGE("222");

        //启动实时取像
        isContineGrab[cam_id] = true;
        jobmanager.jobworkers[cam_id].isGrabbing = true;
        continueGrabFut = QtConcurrent::run(this, &MainWindow::continueGrab, cam_id);
        // LOGE("333");

        //屏蔽菜单栏：
        enableMenubar(false);

    }
}

void MainWindow::enableMenubar(bool enable)
{
    ui->menu_file->setEnabled(enable);
    ui->menu_help->setEnabled(enable);
    ui->menu_setting->setEnabled(enable);
    // ui->widget_16->setEnabled(enable);
    QString styleSheet;
    if(enable)
    {
        styleSheet = QString("QWidget{	background-color: rgb(60, 68, 81);}\n"
                             "QMenuBar{	background-color: rgb(60, 68, 81);}");

    }
    else
    {
        styleSheet = QString("QWidget{background-color: rgb(161, 161, 161);}\n"
                             "QMenuBar{background-color: rgb(161, 161, 161);}");

    }

    ui->widget_16->setStyleSheet(styleSheet);

}

void MainWindow::changeFileEven(std::string fileName)
{
    if(job_name.toStdString().compare(fileName) == 0)
    {
        //如果要切换的就是当前作业，就直接返回完成结果
        //结果发送
        std::string info = "SWP," + fileName + ",0\r";
        communication.tcp_send(info);
        return;
    }
    changeFile(fileName);
    changeDisplay();
    //结果发送
    std::string info = "SWP," + fileName + ",0\r";
    communication.tcp_send(info);
}



void MainWindow::sendImagePixelEven1(float x, float y, int r, int g, int b)
{
    if(!jobmanager.jobworkers[0].isAutoMode)
    {
        // //警告
        // auto r = QMessageBox::information(this, "输出定位信号。", ("确定要输出坐标:(" + std::to_string(x) + "," + std::to_string(y) + ")给机器人？").data(), QMessageBox::StandardButton::Ok,
        //                                   QMessageBox::StandardButton::Cancel);
        // if(r == QMessageBox::StandardButton::Ok)
        // {
        std::string info = "T1,0," + to_string_with_precision(x, 3) + "," + to_string_with_precision(y, 3) + ",0\r";
        communication.tcp_send(info);
        // }
        //
    }
}

void MainWindow::sendImagePixelEven2(float x, float y, int r, int g, int b)
{
    if(!jobmanager.jobworkers[1].isAutoMode)
    {
        //警告
        // auto r = QMessageBox::information(this, "输出定位信号。", ("确定要输出坐标:(" + std::to_string(x) + "," + std::to_string(y) + ")给机器人？").data(), QMessageBox::StandardButton::Ok,
        //                                   QMessageBox::StandardButton::Cancel);
        // if(r == QMessageBox::StandardButton::Ok)
        // {
        std::string info = "T2,0," + to_string_with_precision(x, 3) + "," + to_string_with_precision(y, 3) + ",0\r";
        communication.tcp_send(info);
        // }
        //
    }
}


void MainWindow::on_pushButton_7_clicked()
{
    std::string info = "SWP,0\n";
    communication.tcp_send(info);
}


void MainWindow::on_action_calibration_triggered()
{
    try
    {
        // LOGE_HIGH("00");

        //打开设置界面
        DialogRobotAcq *dialog = new DialogRobotAcq(&jobmanager, &communicationModbus, &communicationCOM, this);
        Qt::WindowFlags flags = Qt::Dialog;
        flags |= Qt::WindowCloseButtonHint;
        dialog->setWindowFlags(flags);
        dialog->setWindowFlag(Qt::WindowStaysOnTopHint, false); //
        // LOGE_HIGH("11");

        // 加载接收触发事件
        connect(&communication, &Communication::calibrationSignal, dialog, &DialogRobotAcq::calibrationCam);
        // dialog->exec();
        dialog->show();
        disconnect(&communication, &Communication::calibrationSignal, dialog, &DialogRobotAcq::calibrationCam);
        // LOGE_HIGH("22");

    }
    catch(std::exception &e)
    {
        LOGE_HIGH(e.what());
    }
    catch(QException &e)
    {
        LOGE_HIGH(e.what());
    }
}

int MainWindow::calibrationCamTest(int cam_id, float robotX, float robotY)
{
    if(jobmanager.jobworkers[cam_id].src_image_dir != "")
    {
        std:: vector<std::string> src_image_paths;
        get_files(jobmanager.jobworkers[cam_id].src_image_dir, &src_image_paths);
        if(src_image_paths.size() < 1)
        {
            return -1;
        }
        tmp_src_img_id_list[cam_id] += 1;
        if(tmp_src_img_id_list[cam_id] > src_image_paths.size() - 1)
        {
            tmp_src_img_id_list[cam_id] = 0;
        }
        std::string src_image_path = src_image_paths[tmp_src_img_id_list[cam_id]];
        cv::Mat src_image;
        src_image = cv::imread(src_image_path, -1);
        jobmanager.jobworkers[cam_id].img = src_image;
        jobmanager.jobworkers[cam_id].update_display_img_signed(cam_id);
    }
    QtConcurrent::run(&jobmanager, &JobManager::rundemo, cam_id);
    jobmanager.jobworkers[cam_id].robotCurrentCoordinates.x = robotX;
    jobmanager.jobworkers[cam_id].robotCurrentCoordinates.y = robotY;

}

void MainWindow::changeCenterLineDistEven(int cam_id, int centerLineDist)
{
    jobmanager.jobworkers[cam_id].centerLineD = centerLineDist;
    if(!jobmanager.jobworkers[cam_id].img.empty())
    {
        // showCenterLine(cam_id);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    // QPoint p = mouseEvent->pos();
    // if(ui->widget_17->geometry().contains(p))
    // {
    mouse_pos = event->globalPos();
    window_pos = this->pos();
    diff_pos = mouse_pos - window_pos;

    // }


}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    // QPoint p = mouseEvent->pos();
    // if(ui->widget_17->geometry().contains(p))
    // {
    QPoint pos = event->globalPos();
    this->move(pos - diff_pos);
    // }
}


bool clicked = false;
void MainWindow::on_pushButton_test_clicked()
{
    clicked = !clicked;
    enableMenubar(!clicked);
}



void MainWindow::on_pushButton_sim_clicked(bool checked)
{
    if(checked)
    {
        // ui->label_sim->setStyleSheet("QLabel { background-color: rgb(60, 68, 81); }");
        // ui->pushButton_stop->setChecked(false);
        on_action_simulation_mode_triggered(true);
    }
    else
    {
        // ui->label_sim->setStyleSheet("QLabel { background-color: rgb(38, 56, 81); }");
        on_action_hand_mode_triggered(true);
    }
}




void MainWindow::on_pushButton_save_calibration_data_clicked()
{
    if(communicationModbus.modbusClient->state() != QModbusDevice::ConnectedState)
    {
        LOGE("机器人未连接");
        return;
    }
    if(jobmanager.jobworkers[0].img.empty())
    {
        LOGE("标定图片为空");
        return;
    }
    //读取机器人坐标
    float x, y, z, rx, ry, rz;
    std::string msg;
    bool rt  = communicationModbus.readFloat(1, 128, QModbusDataUnit::InputRegisters, x, msg);
    if(rt)
    {
        LOGE("x:%f", x);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus.readFloat(1, 130, QModbusDataUnit::InputRegisters, y, msg);
    if(rt)
    {
        LOGE("y:%f", y);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus.readFloat(1, 132, QModbusDataUnit::InputRegisters, z, msg);
    if(rt)
    {
        LOGE("z:%f", z);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus.readFloat(1, 134, QModbusDataUnit::InputRegisters, rx, msg);
    if(rt)
    {
        LOGE("rx:%f", rx);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus.readFloat(1, 136, QModbusDataUnit::InputRegisters, ry, msg);
    if(rt)
    {
        LOGE("ry:%f", ry);
    }
    else
    {
        LOGE("读取坐标失败");
    }
    rt  = communicationModbus.readFloat(1, 138, QModbusDataUnit::InputRegisters, rz, msg);
    if(rt)
    {
        LOGE("rz:%f", rz);
    }
    else
    {
        LOGE("读取坐标失败");
    }


    //保存图片 calibration/time/2d.bmp
    QDateTime currentDateTime = QDateTime::currentDateTime();
    std::string create_time = currentDateTime.toString("yyyyMMdd_hhmmss").toStdString();
    LOGE("create_time:%s", create_time.data());

    std::string save_img_path = "calibration/" + create_time + "/2d.bmp";


    createMultipleFolders(QFileInfo(save_img_path.data()).absolutePath());

    cv::imwrite(save_img_path, jobmanager.jobworkers[0].img);


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

void MainWindow::closeEvent(QCloseEvent *event)
{
    bool is_close = false;
    if(!fouce_close)
    {
        int nRet = QMessageBox::warning(this, "作业保存提醒!!!", "请先确认作业是否已经保存."\
                                        "如果坚持要关闭程序，请按OK按键. 否则请按取消并在关闭程序前，先保存好作业.",
                                        QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel);
        switch(nRet)
        {
            case QMessageBox::StandardButton::Ok:
                is_close = true;
                break;
            case QMessageBox::StandardButton::Cancel:
                is_close = false;
                break;
        }
    }
    else
    {
        is_close = true;
    }
    if(is_close)
    {
        LOGE("111");
        //断开连接
        communication.tcp_server_dispose();
        LOGE("222");


        //关闭心跳
        isRunning = false;
        runFut.waitForFinished();
        LOGE("heartBeat thread close");

        //关闭机器人通讯
        communicationModbus.modbusDisconnect();
        LOGE("333");
        //关闭光源通讯
        communicationCOM.disconnect();


        //断开相机连接
        jobmanager.mvs_stop_grab();
        jobmanager.mvs_close_device();
        LOGE("444");


        //关闭事件
        remove_jobstart_event();
        remove_jobover_event();
        remove_update_display_img_event();
        LOGE("555");

        // this->destroy();
        // 关闭日志
        logfile::LOGEnd();
        logfile::logClose();
        LOGE("loge close");

    }
    else
    {
        event->ignore();
    }
}


void MainWindow::on_pushButton_correction_clicked()
{
    if(jobmanager.dist <= 0)
    {
        return;
    }
    double real_value = ui->doubleSpinBox_real_value->value();
    jobmanager.k = real_value / (jobmanager.dist / jobmanager.k);
}

int MainWindow::changeFileAndCheckCam(int cam_id, int worker_id, std::string fileName)
{
    //切换作业
    if(job_name.toStdString().compare(fileName) != 0)
    {
        changeFile(fileName);
    }
    //取像运行
    //开灯
    communicationCOM.serial->write("SB0255#");
    QThread::msleep(10);

    int r = jobmanager.checkCam(cam_id, worker_id);

    //关灯
    int sleepTime = jobmanager.cams[0].exposure_time / 1000;
    sleepTime += 10;
    QThread::msleep(sleepTime);
    communicationCOM.serial->write("SB0000#");


}


void MainWindow::on_pushButton_sim_2_clicked(bool checked)
{


    if(checked)
    {
        //运行线程开启
        isRunning = true;
        runFut = QtConcurrent::run(this, &MainWindow::runThread);
        LOGE("run thread start");

    }
    else
    {
        //运行线程停止
        isRunning = false;
        runFut.waitForFinished();
    }


}

