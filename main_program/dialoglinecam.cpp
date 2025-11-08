#include "dialoglinecam.h"
#include "ui_dialoglinecam.h"

DialogLineCam::DialogLineCam(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLineCam)
{
    ui->setupUi(this);
}

DialogLineCam::DialogLineCam(JobManager *j, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogLineCam)
{
    ui->setupUi(this);
    jobmanager = j;
    //控件赋值
    for(int i = 0; i < jobmanager->cams.size(); i++)
    {
        ui->comboBox_cam->addItem(("cam" + std::to_string(i + 1)).data());
    }
    ui->comboBox_cam->setCurrentIndex(0);
}

DialogLineCam::~DialogLineCam()
{
    // running = false;

    // f.waitForFinished();
    // jobmanager->jobworkers[cam_id].isFocusing = false;
    delete ui;
}

void DialogLineCam::mvs_stop_grab()
{
    for(int i = 0; i < jobmanager->cams.size() ; i++)
    {
        if(false == jobmanager->cams[i].b_open_device ||
                false == jobmanager->cams[i].b_start_grabbing ||
                NULL == jobmanager->cams[i].m_pcMyCamera)
        {
            return;
        }
        jobmanager->cams[i].b_thread_state = false;
        if(jobmanager->cams[i].h_grab_thread)
        {
//            WaitForSingleObject(jobmanager->cams[i].h_grab_thread, INFINITE);
//            CloseHandle(jobmanager->cams[i].h_grab_thread);
            jobmanager->cams[i].fut.waitForFinished();
            jobmanager->cams[i].h_grab_thread = NULL;
        }
        int nRet = jobmanager->cams[i].m_pcMyCamera->StopGrabbing();
        if(MV_OK != nRet)
        {
//        ShowErrorMsg(TEXT("Stop grabbing fail"), nRet);
            return;
        }
        jobmanager->cams[i].b_start_grabbing = false;
    }
}

void DialogLineCam::mvs_start_grab()
{
    for(int i = 0; i < jobmanager->cams.size() ; i++)
    {
        if(false == jobmanager->cams[i].b_open_device ||
                true == jobmanager->cams[i].b_start_grabbing ||
                NULL == jobmanager->cams[i].m_pcMyCamera)
        {
            return;
        }
        jobmanager->cams[i].b_thread_state = true;
//        unsigned int nThreadID = 0;
//        jobmanager.jobworkers[i].h_grab_thread = (void *)_beginthreadex( NULL, 0, grab_thread, &jobmanager.jobworkers[i], 0, NULL );
//        if (NULL == jobmanager.jobworkers[i].h_grab_thread)
//        {
//            jobmanager.jobworkers[i]. b_thread_state = true;
////            ShowErrorMsg(TEXT("Create thread fail"), 0);
//            return;
//        }
        jobmanager->cams[i].fut = QtConcurrent::run(&jobmanager->cams[i], &Cam::acqfifo_complete);
        int nRet = jobmanager->cams[i]. m_pcMyCamera->StartGrabbing();
        if(MV_OK != nRet)
        {
            jobmanager->cams[i].b_thread_state = false;
//            ShowErrorMsg(TEXT("Start grabbing fail"), nRet);
            return;
        }
        jobmanager->cams[i].b_start_grabbing = true;
    }
}

void DialogLineCam::continueGrab()
{
    isContineGrabFinish = false;

    while(running)
    {
        jobmanager->cams[cam_id].m_pcMyCamera->CommandExecute("TriggerSoftware");
        //等待取像完成
        for(int id = 0; id < 50; ++id)
        {
            QCoreApplication::processEvents();
            QThread::msleep(10);
        }
        while(jobmanager->jobworkers[cam_id].is_busy)
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
    isContineGrabFinish = true;
}


void DialogLineCam::on_comboBox_cam_currentIndexChanged(int index)
{
    //确保对焦没影响
    if(ui->pushButton_check_one_2->isChecked())
    {
        ui->pushButton_check_one_2->setChecked(false);

        on_pushButton_check_one_2_clicked(false);
    }
    cam_id = index;
    // if(jobmanager->cams[cam_id].m_pcMyCamera == nullptr)
    // {
    //     return;
    // }
    is_loading = true;
    //参数获取
    //曝光
    MVCC_FLOATVALUE stFloatValue = {0};
//    jobmanager->cams[cam_id].m_pcMyCamera->GetFloatValue("ExposureTime", &stFloatValue);
//    ui->doubleSpinBox_exposure->setValue(stFloatValue.fCurValue);
    ui->doubleSpinBox_exposure->setValue(jobmanager->cams[cam_id].exposure_time);
    //增益
//    jobmanager->cams[cam_id].m_pcMyCamera->GetFloatValue("Gain", &stFloatValue);
//    ui->doubleSpinBox_gain->setValue(stFloatValue.fCurValue);
    ui->doubleSpinBox_gain->setValue(jobmanager->cams[cam_id].gain);
    //触发延迟
//    jobmanager->cams[cam_id].m_pcMyCamera->GetFloatValue("TriggerDelay", &stFloatValue);
    ui->doubleSpinBox_trigger_delay->setValue(jobmanager->cams[cam_id].trigger_delay);
    // LOGE("jobmanager->jobworkers[cam_id].rotateType:%d", jobmanager->jobworkers[cam_id].rotateType);
    ui->comboBox_camRotate->setCurrentIndex(jobmanager->jobworkers[cam_id].rotateType + 1);
    //参数显示
    is_loading = false;
}


void DialogLineCam::on_doubleSpinBox_exposure_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    if(jobmanager->cams[cam_id].m_pcMyCamera == nullptr)
    {
        return;
    }
    //参数修改
    jobmanager->cams[cam_id].exposure_time = arg1;
    jobmanager->cams[cam_id].m_pcMyCamera->SetFloatValue("ExposureTime", (float)arg1);
    //相机参数保存
//    mvs_stop_grab();
//    jobmanager->cams[cam_id].m_pcMyCamera->CommandExecute("UserSetSave");
//    mvs_start_grab();
}


void DialogLineCam::on_doubleSpinBox_gain_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    if(jobmanager->cams[cam_id].m_pcMyCamera == nullptr)
    {
        return;
    }
    //参数修改
    jobmanager->cams[cam_id].gain = arg1;
    jobmanager->cams[cam_id].m_pcMyCamera->SetFloatValue("Gain", (float)arg1);
    //相机参数保存
//    mvs_stop_grab();
//    jobmanager->cams[cam_id].m_pcMyCamera->CommandExecute("UserSetSave");
//    mvs_start_grab();
}

void DialogLineCam::on_pushButton_check_one_clicked()
{
    //相机取像
    if(jobmanager->cams[cam_id].m_pcMyCamera == nullptr)
    {
        return;
    }
    jobmanager->cams[cam_id].m_pcMyCamera->CommandExecute("TriggerSoftware");
}


void DialogLineCam::on_doubleSpinBox_trigger_delay_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    if(jobmanager->cams[cam_id].m_pcMyCamera == nullptr)
    {
        return;
    }
    //参数修改
    jobmanager->cams[cam_id].trigger_delay = arg1;
    jobmanager->cams[cam_id].m_pcMyCamera->SetFloatValue("TriggerDelay", (float)arg1);
    //相机参数保存
//    mvs_stop_grab();
//    jobmanager->cams[cam_id].m_pcMyCamera->CommandExecute("UserSetSave");
//    mvs_start_grab();
}

void DialogLineCam::on_pushButton_check_one_2_clicked(bool checked)
{
    if(checked)
    {
        if(jobmanager->cams[cam_id].m_pcMyCamera == nullptr)
        {
            ui->pushButton_check_one_2->setChecked(false);
            return;
        }

        ui->pushButton_check_one_2->setText("停止对焦");
        running = true;
        jobmanager->jobworkers[cam_id].isFocusing = true;
        f = QtConcurrent::run(this, &DialogLineCam::continueGrab);
        // continue_run();
    }
    else
    {
        ui->pushButton_check_one_2->setText("自动对焦");
        running = false;
        while(isContineGrabFinish == false)
        {
            QThread::msleep(10);
        }
        // f.waitForFinished();
        jobmanager->jobworkers[cam_id].isFocusing = false;


    }
}


void DialogLineCam::on_comboBox_camRotate_currentIndexChanged(int index)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].rotateType = index - 1 ;

    // LOGE("jobmanager->jobworkers[cam_id].rotateType:%d", jobmanager->jobworkers[cam_id].rotateType);

}

void DialogLineCam::closeEvent(QCloseEvent *event)
{
    running = false;
    f.waitForFinished();
    jobmanager->jobworkers[cam_id].isFocusing = false;

}

