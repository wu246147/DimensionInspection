#include "forminterface.h"
#include "ui_forminterface.h"

FormInterface::FormInterface(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormInterface)
{
    ui->setupUi(this);
}

FormInterface::FormInterface(JobManager *j, QWidget *parent):
    QWidget(parent),
    ui(new Ui::FormInterface)
{
    try
    {
        ui->setupUi(this);
        jobmanager = j;
        init();
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


FormInterface::~FormInterface()
{
    delete ui;
}

void FormInterface::init()
{
    // ui->widget_2->setVisible(false);
    tmp_src_img_id_list = std::vector<int>(jobmanager->jobworkers.size(), 0);
    setDisplay();
}

void FormInterface::setDisplay()
{
    try
    {
        for(int i = 0; i < jobmanager->jobworkers.size() ; i++)
        {
            if(jobmanager->jobworkers[i].display != nullptr)
            {
                delete jobmanager->jobworkers[i].display;
                jobmanager->jobworkers[i].display = nullptr;
            }
        }
        if(this->display_grid_layout != nullptr)
        {
            delete  this->display_grid_layout;
            this->display_grid_layout = nullptr;
        }
        this->display_grid_layout = new QGridLayout(this);
        if(jobmanager->jobworkers[cam_id].display == nullptr)
        {
            jobmanager->jobworkers[cam_id].display = new myWidgetView();
            jobmanager->jobworkers[cam_id].display->myGraphicsView->setStyleSheet("background-color:rgb(161, 161, 161);border:1px solid black;");
        }
        this->display_grid_layout->addWidget(this->jobmanager->jobworkers[cam_id].display);
        ui->widget->setLayout(this->display_grid_layout);
        ui->widget->layout()->activate();
    }
    catch(std::exception &e)
    {
        LOGE_HIGH(e.what());
    }
}


void FormInterface::changeDisplay()
{
    try
    {
        // 删除布局中所有的控件
        while(this->display_grid_layout->count())
        {
            QWidget *p = this->display_grid_layout->itemAt(0)->widget();
            p->setParent(NULL);
            this->display_grid_layout->removeWidget(p);
        }
        //添加窗口
        if(jobmanager->jobworkers[cam_id].display == nullptr)
        {
            jobmanager->jobworkers[cam_id].display = new myWidgetView();
            jobmanager->jobworkers[cam_id].display->myGraphicsView->setStyleSheet("background-color:rgb(161, 161, 161);border:1px solid black;");
        }
        this->display_grid_layout->addWidget(this->jobmanager->jobworkers[cam_id].display);
    }
    catch(std::exception &e)
    {
        LOGE_HIGH(e.what());
    }
}

void FormInterface::get_files(std::string dir, std::vector<std::string> *file_paths)
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

void FormInterface::on_pushButton_changeDisplay_clicked()
{
    cam_id += 1;
    if(cam_id >= jobmanager->cam_num)
    {
        cam_id = 0;
    }
    changeDisplay();
    //updateDisplay
    updateDisplay();
}


void FormInterface::on_pushButton_sim_clicked()
{
    if(jobmanager->jobworkers[cam_id].src_image_dir != "")
    {
        //测试
        std:: vector<std::string> src_image_paths;
        //            qDebug()<<222;
        get_files(jobmanager->jobworkers[cam_id].src_image_dir, &src_image_paths);
        if(src_image_paths.size() < 1)
        {
            return;
        }
        tmp_src_img_id_list[cam_id] += 1;
        //            qDebug()<<333;
        if(tmp_src_img_id_list[cam_id] > src_image_paths.size() - 1)
        {
            tmp_src_img_id_list[cam_id] = 0;
        }
        std::string src_image_path = src_image_paths[tmp_src_img_id_list[cam_id]];
        cv::Mat src_image;
        //            qDebug()<<444;
        src_image = cv::imread(src_image_path, 1);
        //            qDebug()<<555;
        jobmanager->jobworkers[cam_id].img = src_image;
        jobmanager->jobworkers[cam_id].update_display_img_signed(cam_id);
    }
    QtConcurrent::run(jobmanager, &JobManager::rundemo, cam_id);
}


void FormInterface::updateDisplay()
{
    jobmanager->jobworkers[cam_id].update_display_img_signed(cam_id);
    jobmanager->jobworkers[cam_id].show_result();
}

void FormInterface::on_pushButton_updateDisplay_clicked()
{
    updateDisplay();
}


void FormInterface::on_comboBox_currentIndexChanged(int index)
{
    showWindowType = ShowWindowType(index);
}

