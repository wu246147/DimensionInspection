#include "dialogtoolpara.h"
#include "ui_dialogtoolpara.h"

DialogToolPara::DialogToolPara(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogToolPara)
{
    ui->setupUi(this);
}

DialogToolPara::DialogToolPara(JobManager *j, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogToolPara)
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

DialogToolPara::~DialogToolPara()
{
    if(display_grid_layout != nullptr)
    {
        delete display_grid_layout;
        display_grid_layout = nullptr;
    }
    if(myRectItem_location != nullptr)
    {
        delete myRectItem_location;
        myRectItem_location = nullptr;
    }
    if(myRectItem_center != nullptr)
    {
        delete myRectItem_center;
        myRectItem_center = nullptr;
    }
    if(myRectItem_corner1 != nullptr)
    {
        delete myRectItem_corner1;
        myRectItem_corner1 = nullptr;
    }
    if(myRectItem_corner2 != nullptr)
    {
        delete myRectItem_corner2;
        myRectItem_corner2 = nullptr;
    }
    if(myRectItem_corner3 != nullptr)
    {
        delete myRectItem_corner3;
        myRectItem_corner3 = nullptr;
    }
    if(myRectItem_corner4 != nullptr)
    {
        delete myRectItem_corner4;
        myRectItem_corner4 = nullptr;
    }

    if(myRectItem_caliperTool1 != nullptr)
    {
        delete myRectItem_caliperTool1;
        myRectItem_caliperTool1 = nullptr;
    }
    if(myRectItem_caliperTool2 != nullptr)
    {
        delete myRectItem_caliperTool2;
        myRectItem_caliperTool2 = nullptr;
    }
}


void DialogToolPara::init()
{
    try
    {
        //控件赋值
        ui->splitter->setStretchFactor(0, 1);
        ui->splitter->setStretchFactor(1, 10);
        //选用相机std::cout<<e.what()<<std::endl;
        for(int i = 0; i < jobmanager->jobworkers.size(); i++)
        {
            ui->comboBox_cam->addItem(("cam" + std::to_string(i + 1)).data());
        }
        ui->comboBox_cam->setCurrentIndex(0);
    }
    catch(std::exception &e)
    {
        LOGE_HIGH(e.what());
    }
}


void DialogToolPara::set_display()
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

void DialogToolPara::update_train_roi()
{
    try
    {
        if(this->myRectItem_center == nullptr)
        {
            return;
        }
        // //定位
        // {
        //     //只保存搜索区域参数，训练区域只有点击训练后，才会保存
        //     //框参数保存
        //     QPointF center_point;
        //     qreal w, h, angle;
        //     //同步train_roi
        //     this->myRectItem_location->getRectPara(&center_point, &angle, &w, &h);
        //     jobmanager->jobworkers[cam_id].model_box.x = center_point.x() - w / 2;
        //     jobmanager->jobworkers[cam_id].model_box.y = center_point.y() - h / 2;
        //     jobmanager->jobworkers[cam_id].model_box.width = w;
        //     jobmanager->jobworkers[cam_id].model_box.height = h;
        // }
        // {
        //     //只保存搜索区域参数，训练区域只有点击训练后，才会保存
        //     //框参数保存
        //     QPointF center_point;
        //     qreal w, h, angle;
        //     //同步train_roi
        //     this->myRectItem_center->getRectPara(&center_point, &angle, &w, &h);
        //     jobmanager->jobworkers[cam_id].center_box.x = center_point.x() - w / 2;
        //     jobmanager->jobworkers[cam_id].center_box.y = center_point.y() - h / 2;
        //     jobmanager->jobworkers[cam_id].center_box.width = w;
        //     jobmanager->jobworkers[cam_id].center_box.height = h;
        // }
        // {
        //     //只保存搜索区域参数，训练区域只有点击训练后，才会保存
        //     //框参数保存
        //     QPointF center_point;
        //     qreal w, h, angle;
        //     //同步train_roi
        //     this->myRectItem_corner1->getRectPara(&center_point, &angle, &w, &h);
        //     jobmanager->jobworkers[cam_id].boxes[0].x = center_point.x() - w / 2;
        //     jobmanager->jobworkers[cam_id].boxes[0].y = center_point.y() - h / 2;
        //     jobmanager->jobworkers[cam_id].boxes[0].width = w;
        //     jobmanager->jobworkers[cam_id].boxes[0].height = h;
        // }
        // {
        //     //只保存搜索区域参数，训练区域只有点击训练后，才会保存
        //     //框参数保存
        //     QPointF center_point;
        //     qreal w, h, angle;
        //     //同步train_roi
        //     this->myRectItem_corner2->getRectPara(&center_point, &angle, &w, &h);
        //     jobmanager->jobworkers[cam_id].boxes[1].x = center_point.x() - w / 2;
        //     jobmanager->jobworkers[cam_id].boxes[1].y = center_point.y() - h / 2;
        //     jobmanager->jobworkers[cam_id].boxes[1].width = w;
        //     jobmanager->jobworkers[cam_id].boxes[1].height = h;
        // }
        // {
        //     //只保存搜索区域参数，训练区域只有点击训练后，才会保存
        //     //框参数保存
        //     QPointF center_point;
        //     qreal w, h, angle;
        //     //同步train_roi
        //     this->myRectItem_corner3->getRectPara(&center_point, &angle, &w, &h);
        //     jobmanager->jobworkers[cam_id].boxes[2].x = center_point.x() - w / 2;
        //     jobmanager->jobworkers[cam_id].boxes[2].y = center_point.y() - h / 2;
        //     jobmanager->jobworkers[cam_id].boxes[2].width = w;
        //     jobmanager->jobworkers[cam_id].boxes[2].height = h;
        // }
        // {
        //     //只保存搜索区域参数，训练区域只有点击训练后，才会保存
        //     //框参数保存
        //     QPointF center_point;
        //     qreal w, h, angle;
        //     //同步train_roi
        //     this->myRectItem_corner4->getRectPara(&center_point, &angle, &w, &h);
        //     jobmanager->jobworkers[cam_id].boxes[3].x = center_point.x() - w / 2;
        //     jobmanager->jobworkers[cam_id].boxes[3].y = center_point.y() - h / 2;
        //     jobmanager->jobworkers[cam_id].boxes[3].width = w;
        //     jobmanager->jobworkers[cam_id].boxes[3].height = h;
        // }

        {

            //只保存搜索区域参数，训练区域只有点击训练后，才会保存
            //框参数保存
            QPointF center_point;
            qreal w, h, angle;
            //同步train_roi
            this->myRectItem_center->getRectPara(&center_point, &angle, &w, &h);
            jobmanager->jobworkers[cam_id].shapeMatchROI.center.x = center_point.x();
            jobmanager->jobworkers[cam_id].shapeMatchROI.center.y = center_point.y();
            jobmanager->jobworkers[cam_id].shapeMatchROI.size.width = w;
            jobmanager->jobworkers[cam_id].shapeMatchROI.size.height = h;
            jobmanager->jobworkers[cam_id].shapeMatchROI.angle = angle;


        }
        // if(jobmanager->jobworkers[cam_id].is_train)
        // {
        //     if(jobmanager->jobworkers[cam_id].locationShapeResult)
        //     {
        //         {
        //             //只保存搜索区域参数，训练区域只有点击训练后，才会保存
        //             //框参数保存
        //             QPointF center_point;
        //             qreal w, h, angle;
        //             //同步train_roi
        //             this->myRectItem_caliperTool1->getRectPara(&center_point, &angle, &w, &h);
        //             jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].center.x = center_point.x();
        //             jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].center.y = center_point.y();
        //             jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].size.width = w;
        //             jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].size.height = h;
        //             jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].angle = angle;


        //         }
        //         {
        //             //只保存搜索区域参数，训练区域只有点击训练后，才会保存
        //             //框参数保存
        //             QPointF center_point;
        //             qreal w, h, angle;
        //             //同步train_roi
        //             this->myRectItem_caliperTool2->getRectPara(&center_point, &angle, &w, &h);
        //             jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].center.x = center_point.x();
        //             jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].center.y = center_point.y();
        //             jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].size.width = w;
        //             jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].size.height = h;
        //             jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].angle = angle;
        //         }

        //         jobmanager->jobworkers[cam_id].updataCaliperRoiFromTransformCaliperRoi();

        //     }
        // }


    }
    catch(std::exception &e)
    {
        LOGE_HIGH(e.what());
    }
}

void DialogToolPara::update_run_roi()
{
    try
    {
        // if(this->myRectItem_run == nullptr)
        // {
        //     return;
        // }
        // //只保存搜索区域参数，训练区域只有点击训练后，才会保存
        // //框参数保存
        // QPointF center_point;
        // qreal w, h, angle;
        // //同步src_roi
        // this->myRectItem_run->getRectPara(&center_point, &angle, &w, &h);
        // jobmanager->jobworkers[cam_id].location_tool.src_roi_transform_center_x = center_point.x();
        // jobmanager->jobworkers[cam_id].location_tool.src_roi_transform_center_y = center_point.y();
        // jobmanager->jobworkers[cam_id].location_tool.src_roi_transform_phi = -angle / 180 * M_PI;
        // jobmanager->jobworkers[cam_id].location_tool.src_roi_transform_w = w;
        // jobmanager->jobworkers[cam_id].location_tool.src_roi_transform_h = h;
        // jobmanager->jobworkers[cam_id].location_tool.update_src_ori_para();
        // // if(myRectItem_run != nullptr)
        // // {
        // //     delete myRectItem_run;
        // //     myRectItem_run = nullptr;
        // // }

        if(this->myRectItem_caliperLine == nullptr)
        {
            return;
        }

        QPointF startPoint, endPoint;
        //同步train_roi
        this->myRectItem_caliperLine->getPoint(startPoint, endPoint);

        jobmanager->jobworkers[cam_id].CaliperROIList[0] = cv::Point2f(startPoint.x(), startPoint.y());
        jobmanager->jobworkers[cam_id].CaliperROIList[1] = cv::Point2f(endPoint.x(), endPoint.y());

        // if(myRectItem_caliperLine != nullptr)
        // {
        //     delete myRectItem_caliperLine;
        //     myRectItem_caliperLine = nullptr;
        // }


    }
    catch(std::exception &e)
    {
        LOGE(e.what());
    }
}

void DialogToolPara::showCenterLine(int work_id)
{
    // LOGE("111");
    QGraphicsLineItem *tmpLine = new QGraphicsLineItem(0, 0, 100, 100);
    jobmanager->jobworkers[work_id].display->myGraphicsView->cleanItem(tmpLine);
    delete tmpLine;

    // LOGE("222");

    float centerY =  show_img.rows / 2;
    float centerX =  show_img.cols / 2;
    float endY = show_img.rows;
    float endX = show_img.cols;

    QPen pn(Qt::SolidLine);
    pn.setColor(Qt::red);
    pn.setWidth(5);

    jobmanager->jobworkers[work_id].centerLineY = new QGraphicsLineItem(QLineF(QPointF(0, centerY), QPointF(endX, centerY)));
    jobmanager->jobworkers[work_id].centerLineY->setPen(pn);

    jobmanager->jobworkers[work_id].display->myGraphicsView->addItem(jobmanager->jobworkers[work_id].centerLineY, false, false);



    jobmanager->jobworkers[work_id].centerLineY2 = new QGraphicsLineItem(QLineF(QPointF(0, centerY - jobmanager->jobworkers[work_id].centerLineD)
        , QPointF(endX, centerY - jobmanager->jobworkers[work_id].centerLineD)));
    jobmanager->jobworkers[work_id].centerLineY2->setPen(pn);

    jobmanager->jobworkers[work_id].display->myGraphicsView->addItem(jobmanager->jobworkers[work_id].centerLineY2, false, false);


    jobmanager->jobworkers[work_id].centerLineY3 = new QGraphicsLineItem(QLineF(QPointF(0, centerY + jobmanager->jobworkers[work_id].centerLineD)
        , QPointF(endX, centerY + jobmanager->jobworkers[work_id].centerLineD)));
    jobmanager->jobworkers[work_id].centerLineY3->setPen(pn);

    jobmanager->jobworkers[work_id].display->myGraphicsView->addItem(jobmanager->jobworkers[work_id].centerLineY3, false, false);

    jobmanager->jobworkers[work_id].centerLineX = new QGraphicsLineItem(QLineF(QPointF(centerX, 0), QPointF(centerX, endY)));
    jobmanager->jobworkers[work_id].centerLineX->setPen(pn);

    jobmanager->jobworkers[work_id].display->myGraphicsView->addItem(jobmanager->jobworkers[work_id].centerLineX, false, false);


}


void DialogToolPara::show_display()
{
    jobmanager->jobworkers[cam_id].display->myGraphicsView->cleanItem();
    if(exist_show_img)
    {
        jobmanager->jobworkers[cam_id].display->myGraphicsView->setImage(MatToPixmap(show_img));

        {
            myRectItem_caliperLine = new MyGraphicLineItem(QPointF(jobmanager->jobworkers[cam_id]. CaliperROIList[0].x, jobmanager->jobworkers[cam_id]. CaliperROIList[0].y),
                QPointF(jobmanager->jobworkers[cam_id]. CaliperROIList[1].x, jobmanager->jobworkers[cam_id]. CaliperROIList[1].y));
            this->myRectItem_caliperLine->setToolTip("caliper_line");
            this->myRectItem_caliperLine->SetColor(Qt::darkGreen);
            jobmanager->jobworkers[cam_id].display->myGraphicsView->addItem(myRectItem_caliperLine, true);

        }
    }


    // if(work_mode == 1)
    // {
    //     if(exist_show_img)
    //     {
    //         jobmanager->jobworkers[cam_id].display->myGraphicsView->setImage(MatToPixmap(show_img));
    //         if(cam_id == 1)
    //         {
    //             /// 定位训练框
    //             {
    //                 QRectF tool_rect(jobmanager->jobworkers[cam_id].shapeMatchROI.center.x -  jobmanager->jobworkers[cam_id].shapeMatchROI.size.width / 2,
    //                                  jobmanager->jobworkers[cam_id].shapeMatchROI.center.y -  jobmanager->jobworkers[cam_id].shapeMatchROI.size.height / 2,
    //                                  jobmanager->jobworkers[cam_id].shapeMatchROI.size.width,  jobmanager->jobworkers[cam_id].shapeMatchROI.size.height);
    //                 this->myRectItem_center = new MyGraphicRectItem(tool_rect,  jobmanager->jobworkers[cam_id].shapeMatchROI.angle);
    //                 this->myRectItem_center->setToolTip("shapeMatch_roi");
    //                 this->myRectItem_center->SetColor(Qt::blue);
    //                 this->myRectItem_center->setRotatable(true);
    //                 jobmanager->jobworkers[cam_id].display->myGraphicsView->addItem(myRectItem_center, true);
    //             }

    //             ///卡尺
    //             if(jobmanager->jobworkers[cam_id].is_train)
    //             {
    //                 jobmanager->jobworkers[cam_id].runShapeMatch(jobmanager->jobworkers[cam_id].train_image);
    //                 if(jobmanager->jobworkers[cam_id].locationShapeResult)
    //                 {
    //                     //更新一下映射的卡尺框位置
    //                     jobmanager->jobworkers[cam_id].updataTransformCaliperRoiFromCaliperRoi();


    //                     {
    //                         QRectF tool_rect(jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].center.x - jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].size.width / 2,
    //                                          jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].center.y - jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].size.height / 2,
    //                                          jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].size.width, jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].size.height);
    //                         this->myRectItem_caliperTool1 = new MyGraphicRectItem(tool_rect, jobmanager->jobworkers[cam_id].CaliperTransformROIList[0].angle);
    //                         this->myRectItem_caliperTool1->setToolTip("caliper_roi1");
    //                         this->myRectItem_caliperTool1->SetColor(Qt::green);
    //                         this->myRectItem_caliperTool1->setRotatable(true);
    //                         jobmanager->jobworkers[cam_id].display->myGraphicsView->addItem(myRectItem_caliperTool1, true);
    //                     }
    //                     {
    //                         QRectF tool_rect(jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].center.x - jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].size.width / 2,
    //                                          jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].center.y - jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].size.height / 2,
    //                                          jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].size.width, jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].size.height);
    //                         this->myRectItem_caliperTool2 = new MyGraphicRectItem(tool_rect, jobmanager->jobworkers[cam_id].CaliperTransformROIList[1].angle);
    //                         this->myRectItem_caliperTool2->setToolTip("caliper_roi2");
    //                         this->myRectItem_caliperTool2->SetColor(Qt::green);
    //                         this->myRectItem_caliperTool2->setRotatable(true);
    //                         jobmanager->jobworkers[cam_id].display->myGraphicsView->addItem(myRectItem_caliperTool2, true);
    //                     }
    //                 }
    //             }

    //         }
    //     }
    // }
    // else if(work_mode == 0)
    // {
    //     if(exist_show_img)
    //     {
    //         jobmanager->jobworkers[cam_id].display->myGraphicsView->setImage(MatToPixmap(show_img));
    //     }
    // }


    if(exist_show_img)
    {
        //添加中心线
        // showCenterLine(cam_id);
    }
}

void DialogToolPara::on_comboBox_cam_currentIndexChanged(int index)
{
    try
    {
        //        update_code_tool_roi();
        cam_id = index;
        //        std::cout << "cam_id:" << cam_id << std::endl;
        //画面部署，显示
        set_display();
        //        std::cout << "set_display" << std::endl;
        if(ui->comboBox_type->currentIndex() == 0)
        {
            on_comboBox_type_currentIndexChanged(0);
        }
        else
        {
            ui->comboBox_type->setCurrentIndex(0);
        }
    }
    catch(std::exception &e)
    {
        LOGE_HIGH(e.what());
    }
}



void DialogToolPara::closeEvent(QCloseEvent *)
{
    // update_run_roi();
    int nRet = QMessageBox::warning(this, "作业保存提醒!!!", "作业参数修改后，记得点击保存作业，对修改后的作业进行保存.", QMessageBox::StandardButton::Ok);
}
void DialogToolPara::on_pushButton_run_clicked()
{
    update_run_roi();
    //显示等待画面
    int center_x = int(this->frameGeometry().x() + this->frameGeometry().width() / 2);
    int center_y = int(this->frameGeometry().y()) + this->frameGeometry().height() / 2;
    QPixmap pixmap("./img/loading.gif");
    QSplashScreen splash(pixmap);
    splash.setGeometry(center_x - 130, center_y - 130, 260, 260);
    splash.setWindowFlag(Qt::WindowStaysOnTopHint, true); // 取消置顶
    splash.show();
    //中间这里还是要以线程等待形式才行
    QCoreApplication::processEvents();
    jobmanager->rundemo(cam_id);
    splash.hide();
    splash.close();
}

void DialogToolPara::on_pushButton_set_train_image_clicked()
{
    if(jobmanager->jobworkers[cam_id].img.empty())
    {
        return;
    }
    jobmanager->jobworkers[cam_id].set_train_img(jobmanager->jobworkers[cam_id].img);
    get_show_img();
    show_display();
}

void DialogToolPara::on_pushButton_train_clicked()
{
    if(jobmanager->jobworkers[cam_id].train_image.empty())
    {
        QMessageBox::warning(this, "训练图片为空.", "训练图片为空，请先设置好训练图片.", QMessageBox::StandardButton::Ok);
        return;
    }
    update_train_roi();
    if(cam_id == 0)
    {
        jobmanager->jobworkers[cam_id].is_train = true;
        //显示图像处理效果
        jobmanager->jobworkers[cam_id].show_chips_contour();
        QMessageBox::warning(this, "训练成功.", "训练成功.", QMessageBox::StandardButton::Ok);
    }
    else
    {
        // jobmanager->jobworkers[cam_id].is_train = true;
        jobmanager->jobworkers[cam_id].train_location();
        if(jobmanager->jobworkers[cam_id].is_train)
        {
            //显示定位效果
            jobmanager->jobworkers[cam_id].show_train_model();

            //重新定位一下，刷新一下卡尺ROI
            jobmanager->jobworkers[cam_id].runLocation(jobmanager->jobworkers[cam_id].train_image);
            show_display();
            QMessageBox::warning(this, "训练成功.", "训练成功.", QMessageBox::StandardButton::Ok);
        }
        else
        {
            QMessageBox::warning(this, "训练失败.", "训练失败.", QMessageBox::StandardButton::Ok);
        }
    }

}

void DialogToolPara::updata_control_value()
{
    // LOGE("000");

    ui->groupBox->setVisible(false);
    ui->groupBox_run->setVisible(false);
    ui->groupBox_train->setVisible(false);
    if(work_mode == 0)
    {
        ui->groupBox_run->setVisible(true);
    }
    else
    {
        ui->groupBox_train->setVisible(true);

        ui->groupBox_center_location->setVisible(false);
        ui->groupBox_caliper_location1->setVisible(false);
        ui->groupBox_caliper_location2->setVisible(false);

        if(cam_id == 0)
        {
            ui->groupBox_center_location->setVisible(true);

        }
        else
        {
            ui->groupBox_caliper_location1->setVisible(true);
            ui->groupBox_caliper_location2->setVisible(true);

        }

    }
    // LOGE("001");

    //参数显示控制
    is_loading = true;


    ui->spinBox_location_caliper_roi_w->setValue(jobmanager->jobworkers[cam_id].CaliperROIW);
    ui->spinBox_location_caliper_roi_h->setValue(jobmanager->jobworkers[cam_id].CaliperROIH);
    ui->spinBox_location_caliper_roi_count->setValue(jobmanager->jobworkers[cam_id].CaliperROIC);

    ui->comboBox_location_caliper1_select->setCurrentIndex(jobmanager->jobworkers[cam_id].caliperSelectPosition);
    ui->comboBox_location_caliper1_transition->setCurrentIndex(jobmanager->jobworkers[cam_id].caliperPolarity);
    ui->spinBox_location_caliper1_thre->setValue(jobmanager->jobworkers[cam_id].caliperThre);

    ui->doubleSpinBox_distance_thre_min->setValue(jobmanager->distance_thre_min);
    ui->doubleSpinBox_distance_thre_max->setValue(jobmanager->distance_thre_max);

    ui->lineEdit_simulation_path->setText(jobmanager->jobworkers[cam_id].src_image_dir.data());

    is_loading = false;

    // LOGE("111");
    show_simulation_table();
    // LOGE("222");

}


void DialogToolPara::clear_simulation_table()
{
    try
    {
        ui->tableWidget_simualtion_filePath->clearContents();

        int row = ui->tableWidget_simualtion_filePath->rowCount();
        int column = ui->tableWidget_simualtion_filePath->columnCount();
        for(int i = 0; i < row ; ++i)
        {
            for(int j = 0; j < column; ++j)
            {
                QTableWidgetItem *item = ui->tableWidget_simualtion_filePath->item(i, j);
                if(item)
                {
                    delete item;
                }
            }
        }
        int n = ui->tableWidget_simualtion_filePath->rowCount();//总行数
        // qDebug() << "total count:" << n ;
        for(int i = 0; i < n; i++)
        {
            // qDebug() << "remove:" << i ;
            // qDebug() << "tableWidget_order row count:" << ui->tableWidget_order->rowCount();
            ui->tableWidget_simualtion_filePath->removeRow(0);
            // qDebug() << "remove:" << i << "finish";
        }

    }
    catch(...)
    {
    }
}

void DialogToolPara::show_simulation_table()
{
    // LOGE("1111");

    is_loading = true;
    clear_simulation_table();
    is_loading = false;
    // LOGE("2222");

    select_simulation_img_id = -1;
    std::vector<std::string> src_image_paths;
    get_files(jobmanager->jobworkers[cam_id].src_image_dir, &src_image_paths);
    if(src_image_paths.size() < 1)
    {
        return;
    }
    // LOGE("3333");
    // LOGE("src_image_paths size:%d", src_image_paths.size());

    for(int id  = 0; id < src_image_paths.size(); ++id)
    {
        int index = ui->tableWidget_simualtion_filePath->rowCount();//行数
        ui->tableWidget_simualtion_filePath->insertRow(index);//在最后插入一行

        QStringList fileNameList = QString(src_image_paths[id].data()).split("/");
        QTableWidgetItem *item =  new QTableWidgetItem(fileNameList[fileNameList.size() - 1]);
        item->setToolTip(src_image_paths[id].data());

        ui->tableWidget_simualtion_filePath->setItem(index, 0, item); //设置单元格内容

        // LOGE("src_image_path:%s", src_image_paths[id].data());

    }
    // LOGE("4444");

}




void DialogToolPara::on_comboBox_type_currentIndexChanged(int index)
{
    try
    {
        // LOGE("00");

        work_mode = index;
        //display显示
        // LOGE("11");

        get_show_img();
        // LOGE("22");

        show_display();
        // LOGE("33");

        updata_control_value();
    }
    catch(std::exception &e)
    {
        LOGE_HIGH(e.what());
    }
}


void DialogToolPara::get_show_img()
{
    exist_show_img = false;
    if(work_mode == 1)
    {
        if(!jobmanager->jobworkers[cam_id].train_image.empty())
        {
            exist_show_img = true;
            show_img = jobmanager->jobworkers[cam_id].train_image;
        }
    }
    else if(work_mode == 0)
    {
        if(!jobmanager->jobworkers[cam_id].img.empty())
        {
            exist_show_img = true;
            show_img = jobmanager->jobworkers[cam_id].img;
        }
    }
}

void DialogToolPara::change_roi_even(QGraphicsItem *item)
{
    //roi显示更新
    update_run_roi();
}



void DialogToolPara::on_comboBox_location_caliper1_transition_currentIndexChanged(int index)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].caliperPolarity = index;
}


void DialogToolPara::on_comboBox_location_caliper1_select_currentIndexChanged(int index)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].caliperSelectPosition = index;

}


void DialogToolPara::on_spinBox_location_caliper1_thre_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].caliperThre = arg1;

}


void DialogToolPara::on_comboBox_location_caliper2_transition_currentIndexChanged(int index)
{
    if(is_loading)
    {
        return;
    }
    // jobmanager->jobworkers[cam_id].caliperPolarity[1] = index;

}


void DialogToolPara::on_comboBox_location_caliper2_select_currentIndexChanged(int index)
{
    if(is_loading)
    {
        return;
    }
    // jobmanager->jobworkers[cam_id].caliperSelectPosition[1] = index;

}


void DialogToolPara::on_spinBox_location_caliper2_thre_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    // jobmanager->jobworkers[cam_id].caliperThre[1] = arg1;

}


void DialogToolPara::on_pushButton_save_clicked()
{
    save_file_signal();
}


void DialogToolPara::on_spinBox_canny_thre_min_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].circleThre = arg1;
}


void DialogToolPara::on_spinBox_canny_thre_max_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].chipThre = arg1;
}


void DialogToolPara::on_spinBox_chip_area_min_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].chipAreaMin = arg1;
}


void DialogToolPara::on_spinBox_chip_area_max_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].chipAreaMax = arg1;
}


void DialogToolPara::on_pushButton_getStandardPoint_clicked()
{
    //判断是否已经有训练图片及是否已经训练
    if(!jobmanager->jobworkers[cam_id].is_train || jobmanager->jobworkers[cam_id].train_image.empty())
    {
        QMessageBox::warning(this, "获取失败.", "训练图片为空或工具还没训练成功.", QMessageBox::StandardButton::Ok);

        return;
    }
    //获取训练图片的定位结果
    jobmanager->jobworkers[cam_id].runLocation(jobmanager->jobworkers[cam_id].train_image);
    jobmanager->jobworkers[cam_id].show_result();

    if(!jobmanager->jobworkers[cam_id].locationResult)
    {
        QMessageBox::warning(this, "定位失败.", "定位失败.", QMessageBox::StandardButton::Ok);
        return;
    }
    //将定位结果设置为标准位置
    jobmanager->jobworkers[cam_id].standardPoint = jobmanager->jobworkers[cam_id].locationPositionTransform;
    jobmanager->jobworkers[cam_id].standardAngle = jobmanager->jobworkers[cam_id].locationAngleOutPut;

    //界面更新
    is_loading = true;
    ui->doubleSpinBox_standardPointX->setValue(jobmanager->jobworkers[cam_id].standardPoint.x);
    ui->doubleSpinBox_standardPointY->setValue(jobmanager->jobworkers[cam_id].standardPoint.y);
    ui->doubleSpinBox_standardAngle->setValue(jobmanager->jobworkers[cam_id].standardAngle);

    is_loading = false;

}


void DialogToolPara::on_comboBox_resultType_currentIndexChanged(int index)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].resultType = index;
}


void DialogToolPara::on_doubleSpinBox_xb_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].xb = arg1;
}


void DialogToolPara::on_doubleSpinBox_yb_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].yb = arg1;
}


void DialogToolPara::on_doubleSpinBox_rb_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].rb = arg1;
}


void DialogToolPara::on_doubleSpinBox_xa_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].xa = arg1;
}


void DialogToolPara::on_doubleSpinBox_ya_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].ya = arg1;
}


void DialogToolPara::on_doubleSpinBox_ra_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].ra = arg1;
}


void DialogToolPara::on_spinBox_centerLineD_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].centerLineD = arg1;
    showCenterLine(cam_id);
}


void DialogToolPara::on_comboBox_location_Politary_currentIndexChanged(int index)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].lightChannel = index;
}


void DialogToolPara::on_doubleSpinBox_distance_thre_min_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->distance_thre_min = arg1;
}


void DialogToolPara::on_doubleSpinBox_distance_thre_max_valueChanged(double arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->distance_thre_max = arg1;
}


void DialogToolPara::on_lineEdit_simulation_path_textChanged(const QString &arg1)
{
    if(is_loading)
    {
        return;
    }

    jobmanager->jobworkers[cam_id].src_image_dir = ui->lineEdit_simulation_path->text().toStdString();


    //列表显示仿真图地址
    show_simulation_table();
}


void DialogToolPara::on_pushButton_open_dir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Simulation Image Directory"),
                  QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(NULL == dir)
    {
        return;
    }
    ui->lineEdit_simulation_path->setText(dir);
}


void DialogToolPara::on_tableWidget_simualtion_filePath_itemSelectionChanged()
{
    if(is_loading)
    {
        return;
    }

    LOGE("cam_id:%d", cam_id);

    LOGE("ui->tableWidget_simualtion_filePath->currentRow:%d", ui->tableWidget_simualtion_filePath->currentRow());

    select_simulation_img_id = ui->tableWidget_simualtion_filePath->currentRow();

    if(select_simulation_img_id >= 0)
    {

        std::string src_image_path = jobmanager->jobworkers[cam_id].src_image_dir + "/" + ui->tableWidget_simualtion_filePath->item(select_simulation_img_id, 0)->text().toStdString();
        //切换图片
        cv::Mat src_image;
        src_image = cv::imread(src_image_path, -1);
        jobmanager->jobworkers[cam_id].img = cv::Mat();
        jobmanager->jobworkers[cam_id].img = src_image;
        //显示图片
        get_show_img();
        show_display();



    }
    else
    {

    }
}


void DialogToolPara::on_pushButton_run_last_clicked()
{
    if(ui->tableWidget_simualtion_filePath->rowCount() <= 0)
    {
        return;
    }
    if(select_simulation_img_id == 0)
    {
        return;
    }
    select_simulation_img_id -= 1;

    // int row = ui->tableWidget_simualtion_filePath->rowCount();

    //切换图片
    ui->tableWidget_simualtion_filePath->setCurrentCell(select_simulation_img_id, 0);

    //运行模型
    on_pushButton_run_clicked();
}


void DialogToolPara::on_pushButton_run_next_clicked()
{
    if(ui->tableWidget_simualtion_filePath->rowCount() <= 0)
    {
        return;
    }
    int row = ui->tableWidget_simualtion_filePath->rowCount();

    if(select_simulation_img_id == row - 1)
    {
        return;
    }
    select_simulation_img_id += 1;


    //切换图片
    ui->tableWidget_simualtion_filePath->setCurrentCell(select_simulation_img_id, 0);

    //运行模型
    on_pushButton_run_clicked();
}


void DialogToolPara::on_spinBox_location_caliper_roi_count_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].CaliperROIC = arg1;
}


void DialogToolPara::on_spinBox_location_caliper_roi_h_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].CaliperROIH = arg1;

}


void DialogToolPara::on_spinBox_location_caliper_roi_w_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[cam_id].CaliperROIW = arg1;

}

