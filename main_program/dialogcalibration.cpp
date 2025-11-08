#include "dialogcalibration.h"
#include "ui_dialogcalibration.h"

DialogCalibration::DialogCalibration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCalibration)
{
    ui->setupUi(this);
}

DialogCalibration::DialogCalibration(JobManager *j, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogCalibration)

{
    ui->setupUi(this);
    jobmanager = j;

    // LOGE_HIGH("111");

    //控件赋值
    for(int i = 0; i < jobmanager->cams.size(); i++)
    {
        ui->comboBox_cam->addItem(("cam" + std::to_string(i + 1)).data());
    }
    ui->comboBox_cam->setCurrentIndex(0);
    // LOGE_HIGH("222");

    //都设置成不可修改
    ui->doubleSpinBox_H00->setReadOnly(true);
    ui->doubleSpinBox_H01->setReadOnly(true);
    ui->doubleSpinBox_H02->setReadOnly(true);
    ui->doubleSpinBox_H10->setReadOnly(true);
    ui->doubleSpinBox_H11->setReadOnly(true);
    ui->doubleSpinBox_H12->setReadOnly(true);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->doubleSpinBox_camX->setReadOnly(true);
    ui->doubleSpinBox_camY->setReadOnly(true);
    ui->doubleSpinBox_robotX->setReadOnly(true);
    ui->doubleSpinBox_robotY->setReadOnly(true);
    // LOGE_HIGH("333");

}

DialogCalibration::~DialogCalibration()
{
    delete ui;
}

int DialogCalibration::calibrationCam(int cam_id, float robotX, float robotY)
{
    //修改相机id
    ui->comboBox_cam->setCurrentIndex(cam_id);
    //等待相机结果
    QThread::msleep(500);

    //更新界面结果
    if(jobmanager->jobworkers[cam_id].locationResult)
    {
        ui->doubleSpinBox_camX->setValue(jobmanager->jobworkers[cam_id].locationPosition.x);
        ui->doubleSpinBox_camY->setValue(jobmanager->jobworkers[cam_id].locationPosition.y);
        ui->doubleSpinBox_robotX->setValue(jobmanager->jobworkers[cam_id].robotCurrentCoordinates.x);
        ui->doubleSpinBox_robotY->setValue(jobmanager->jobworkers[cam_id].robotCurrentCoordinates.y);

    }
    else
    {
        int nRet = QMessageBox::warning(this, "定位失败!!!", "定位失败.", QMessageBox::StandardButton::Ok);

    }

    return 0;
}

void DialogCalibration::on_comboBox_cam_currentIndexChanged(int index)
{
    // LOGE_HIGH("0000");
    if(cam_id == index)
    {
        return;
    }
    // LOGE_HIGH("1111");

    cam_id = index;
    //清空数组及界面刷新
    camPoints.clear();
    robotPoints.clear();
    // LOGE_HIGH("2222");

    ui->tableWidget->clear();
    //旋转矩阵刷新
    showTransformH();
    // LOGE_HIGH("3333");

}

void DialogCalibration::addStatisticsData()
{
    ui->tableWidget->clear();

    for(int id = 0; id < camPoints.size(); ++id)
    {
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0, 0, new QTableWidgetItem(std::to_string(robotPoints[id].x).data())); //设置单元格内容
        ui->tableWidget->setItem(0, 1, new QTableWidgetItem(std::to_string(robotPoints[id].y).data())); //设置单元格内容
        ui->tableWidget->setItem(0, 2, new QTableWidgetItem(std::to_string(camPoints[id].x).data())); //设置单元格内容
        ui->tableWidget->setItem(0, 3, new QTableWidgetItem(std::to_string(camPoints[id].y).data()));
    }
}

void DialogCalibration::on_pushButton_record_clicked()
{
    //添加到数组及界面刷新
    if(jobmanager->jobworkers[cam_id].locationResult)
    {
        camPoints.push_back(cv::Point2f(jobmanager->jobworkers[cam_id].locationPosition.x, jobmanager->jobworkers[cam_id].locationPosition.y));
        robotPoints.push_back(jobmanager->jobworkers[cam_id].robotCurrentCoordinates);

        addStatisticsData();

    }
    else
    {
        int nRet = QMessageBox::warning(this, "定位失败!!!", "定位失败.", QMessageBox::StandardButton::Ok);

    }

}


void DialogCalibration::on_pushButton_clear_clicked()
{
    //清空数组及界面刷新
    camPoints.clear();
    robotPoints.clear();

    ui->tableWidget->clear();
}


void DialogCalibration::showTransformH()
{
    ui->doubleSpinBox_H00->setValue(jobmanager->jobworkers[cam_id].transformH.at<double>(0, 0));
    ui->doubleSpinBox_H01->setValue(jobmanager->jobworkers[cam_id].transformH.at<double>(0, 1));
    ui->doubleSpinBox_H02->setValue(jobmanager->jobworkers[cam_id].transformH.at<double>(0, 2));
    ui->doubleSpinBox_H10->setValue(jobmanager->jobworkers[cam_id].transformH.at<double>(1, 0));
    ui->doubleSpinBox_H11->setValue(jobmanager->jobworkers[cam_id].transformH.at<double>(1, 1));
    ui->doubleSpinBox_H12->setValue(jobmanager->jobworkers[cam_id].transformH.at<double>(1, 2));
}

void DialogCalibration::on_pushButton_calibration_clicked()
{
    //标定及界面刷新
    if(camPoints.size() >= 5)
    {
        cv::Mat H;
        int rt = ImgAlg::getTransformFromPoints(camPoints, robotPoints, H);
        if(rt == 0)
        {
            jobmanager->jobworkers[cam_id].transformH = H;
            showTransformH();
        }
        else
        {
            QMessageBox::warning(this, "标定失败!!!", "标定失败.", QMessageBox::StandardButton::Ok);
        }
    }
    else
    {
        QMessageBox::warning(this, "标定点位不足!!!", "标定点位不足.", QMessageBox::StandardButton::Ok);
    }
}


void DialogCalibration::on_pushButton_calibration_check_clicked()
{
    //标定及界面刷新
    if(camPoints.size() >= 5)
    {
        double diff = 0;

        for(int id = 0; id < camPoints.size(); ++id)
        {
            cv::Point2f robotPoint = robotPoints[id];
            cv::Point2f camPoint = camPoints[id];

            cv::Point2f transformPoint;

            ImgAlg::transformPoint(jobmanager->jobworkers[cam_id].transformH, camPoint, transformPoint);

            diff += std::abs(robotPoint.x - transformPoint.x) + std::abs(robotPoint.y - transformPoint.y);

        }
        diff /= camPoints.size();

        QMessageBox::warning(this, "标定验证", ("标定点映射误差为：" + std::to_string(diff)).data(), QMessageBox::StandardButton::Ok);

    }
    else
    {
        QMessageBox::warning(this, "标定点位不足!!!", "标定点位不足.", QMessageBox::StandardButton::Ok);
    }
}

