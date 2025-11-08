#include "jobworker.h"
#include "QDeBug"


QImage MatToQImage(const cv::Mat &cvImage)
{
    std::vector<uchar> imgBuf;
    imencode(".bmp", cvImage, imgBuf);
    QByteArray baImg((char *)imgBuf.data(), static_cast<int>(imgBuf.size()));
    QImage image;
    image.loadFromData(baImg, "BMP");
    return image;
}
QPixmap MatToPixmap(const cv::Mat &cvImage)
{
    return QPixmap::fromImage(MatToQImage(cvImage));
}


JobWorker::JobWorker(JobWorker *parent)
{
    this->cam_id = parent->cam_id;
    this->src_image_dir = parent->src_image_dir;

    //取象触发延迟时间
    this-> trigger_delay = parent->trigger_delay;
    //相机曝光时间
    this-> exposure_time =  parent->exposure_time;
    //相机增益
    this-> gain =  parent->gain;

    ///初定位参数
    this-> circleThre =  parent->circleThre;
    this-> chipThre =  parent->chipThre;
    this-> chipAreaMin =  parent->chipAreaMin;
    this-> chipAreaMax =  parent->chipAreaMax;
    this-> lightChannel =  parent->lightChannel;


    //尺寸检测范围
    this-> caliper_distances_limit =  parent->caliper_distances_limit;

    // this-> is_train = false;
    //记录训练图片
    this-> train_image =  parent->train_image;

    //记录训练模板框
    this-> model_box =  parent->model_box;
    // cv::Mat train_image_cut;
    //记录训练边缘框
    this-> boxes =  parent->boxes;
    //记录训练中间框
    this-> center_box =  parent->center_box;

    ///卡尺参数
    //记录ROI

    this-> CaliperROIList =  parent->CaliperROIList;

    this->CaliperTransformROIList =  parent->CaliperTransformROIList;


    this->CaliperROIW =  parent->CaliperROIW;   //卡尺极值，0为黑到白、1为白到黑
    this->CaliperROIH =  parent->CaliperROIH;   //卡尺极值，0为黑到白、1为白到黑
    this->CaliperROIC =  parent->CaliperROIC;   //卡尺极值，0为黑到白、1为白到黑


    this->caliperPolarity =  parent->caliperPolarity;   //卡尺极值，0为黑到白、1为白到黑
    this->caliperSelectPosition =  parent->caliperSelectPosition;//卡尺找边模式，0为第一边、1为最后一边
    this->caliperThre =  parent->caliperThre;//卡尺找边阈值

    /// 定位参数

    this->ModelID =  parent->ModelID;
    this->shapeMatchROI =  parent->shapeMatchROI;
    //检测参数
    this-> AngleStart =  parent->AngleStart;
    this-> AngleExtent =  parent->AngleExtent;
    this-> AngleStep =  parent->AngleStep;
    this-> Optimization =  parent->Optimization;
    this-> Metric =  parent->Metric;
    this-> LowThreshold =  parent->LowThreshold;
    this-> HighThreshold =  parent->HighThreshold;
    this-> MinContrast =  parent->MinContrast;
    this-> MinComponentSize =  parent->MinComponentSize;
    this-> Pregeneration =  parent->Pregeneration;
    this-> QuantizedOrientations =  parent->QuantizedOrientations;
    this-> MaxOverlap =  parent->MaxOverlap;
    this-> SubPixel =  parent->SubPixel;
    this-> NumLevels =  parent->NumLevels;
    this-> Greediness =  parent->Greediness;
    this-> ExpandBorder =  parent->ExpandBorder;
    this-> MoreAggressiveCandidateStrategy =  parent->MoreAggressiveCandidateStrategy;
    this-> maxMatches =  parent->maxMatches;
    this-> MinScore =  parent->MinScore;

    this-> transformH =  parent->transformH;
    this->  standardRotateCenter =  parent->standardRotateCenter;
    this->  standardPoint =  parent->standardPoint;
    this->  standardAngle =  parent->standardAngle;


    ///输出结果
    this-> resultType =  parent->resultType;
}

JobWorker::JobWorker(const JobWorker &jobworker)
{
    this->cam_id = jobworker.cam_id;
    this->src_image_dir = jobworker.src_image_dir;

    //取象触发延迟时间
    this-> trigger_delay = jobworker.trigger_delay;
    //相机曝光时间
    this-> exposure_time = jobworker.exposure_time;
    //相机增益
    this-> gain = jobworker.gain;

    ///初定位参数
    this-> circleThre = jobworker.circleThre;
    this-> chipThre = jobworker.chipThre;
    this-> chipAreaMin = jobworker.chipAreaMin;
    this-> chipAreaMax = jobworker.chipAreaMax;
    this-> lightChannel = jobworker.lightChannel;


    //尺寸检测范围
    this-> caliper_distances_limit = jobworker.caliper_distances_limit;

    // this-> is_train = false;
    //记录训练图片
    this-> train_image = jobworker.train_image;

    //记录训练模板框
    this-> model_box = jobworker.model_box;
    // cv::Mat train_image_cut;
    //记录训练边缘框
    this-> boxes = jobworker.boxes;
    //记录训练中间框
    this-> center_box = jobworker.center_box;

    ///卡尺参数
    //记录ROI

    this-> CaliperROIList = jobworker.CaliperROIList;

    this->CaliperTransformROIList = jobworker.CaliperTransformROIList;

    this->CaliperROIW = jobworker.CaliperROIW;   //
    this->CaliperROIH = jobworker.CaliperROIH;   //
    this->CaliperROIC = jobworker.CaliperROIC;   //

    this->caliperPolarity = jobworker.caliperPolarity;   //卡尺极值，0为黑到白、1为白到黑
    this->caliperSelectPosition = jobworker.caliperSelectPosition;//卡尺找边模式，0为第一边、1为最后一边
    this->caliperThre = jobworker.caliperThre;//卡尺找边阈值

    /// 定位参数

    this->ModelID = jobworker.ModelID;
    this->shapeMatchROI = jobworker.shapeMatchROI;
    //检测参数
    this-> AngleStart = jobworker.AngleStart;
    this-> AngleExtent = jobworker.AngleExtent;
    this-> AngleStep = jobworker.AngleStep;
    this-> Optimization = jobworker.Optimization;
    this-> Metric = jobworker.Metric;
    this-> LowThreshold = jobworker.LowThreshold;
    this-> HighThreshold = jobworker.HighThreshold;
    this-> MinContrast = jobworker.MinContrast;
    this-> MinComponentSize = jobworker.MinComponentSize;
    this-> Pregeneration = jobworker.Pregeneration;
    this-> QuantizedOrientations = jobworker.QuantizedOrientations;
    this-> MaxOverlap = jobworker.MaxOverlap;
    this-> SubPixel = jobworker.SubPixel;
    this-> NumLevels = jobworker.NumLevels;
    this-> Greediness = jobworker.Greediness;
    this-> ExpandBorder = jobworker.ExpandBorder;
    this-> MoreAggressiveCandidateStrategy = jobworker.MoreAggressiveCandidateStrategy;
    this-> maxMatches = jobworker.maxMatches;
    this-> MinScore = jobworker.MinScore;

    this-> transformH = jobworker.transformH;
    this->  standardRotateCenter = jobworker.standardRotateCenter;
    this->  standardPoint = jobworker.standardPoint;
    this->  standardAngle = jobworker.standardAngle;


    ///输出结果
    this-> resultType = jobworker.resultType;
}

JobWorker &JobWorker::operator =(const JobWorker &jobworker)
{
    this->cam_id = jobworker.cam_id;
    this->src_image_dir = jobworker.src_image_dir;

    //取象触发延迟时间
    this-> trigger_delay = jobworker.trigger_delay;
    //相机曝光时间
    this-> exposure_time = jobworker.exposure_time;
    //相机增益
    this-> gain = jobworker.gain;

    ///初定位参数
    this-> circleThre = jobworker.circleThre;
    this-> chipThre = jobworker.chipThre;
    this-> chipAreaMin = jobworker.chipAreaMin;
    this-> chipAreaMax = jobworker.chipAreaMax;
    this-> lightChannel = jobworker.lightChannel;


    //尺寸检测范围
    this-> caliper_distances_limit = jobworker.caliper_distances_limit;

    // this-> is_train = false;
    //记录训练图片
    this-> train_image = jobworker.train_image;

    //记录训练模板框
    this-> model_box = jobworker.model_box;
    // cv::Mat train_image_cut;
    //记录训练边缘框
    this-> boxes = jobworker.boxes;
    //记录训练中间框
    this-> center_box = jobworker.center_box;

    ///卡尺参数
    //记录ROI

    this-> CaliperROIList = jobworker.CaliperROIList;

    this->CaliperTransformROIList = jobworker.CaliperTransformROIList;

    this->caliperPolarity = jobworker.caliperPolarity;   //卡尺极值，0为黑到白、1为白到黑
    this->caliperSelectPosition = jobworker.caliperSelectPosition;//卡尺找边模式，0为第一边、1为最后一边
    this->caliperThre = jobworker.caliperThre;//卡尺找边阈值

    this->CaliperROIW = jobworker.CaliperROIW;   //卡尺极值，0为黑到白、1为白到黑
    this->CaliperROIH = jobworker.CaliperROIH;   //卡尺极值，0为黑到白、1为白到黑
    this->CaliperROIC = jobworker.CaliperROIC;   //卡尺极值，0为黑到白、1为白到黑

    /// 定位参数

    this->ModelID = jobworker.ModelID;
    this->shapeMatchROI = jobworker.shapeMatchROI;
    //检测参数
    this-> AngleStart = jobworker.AngleStart;
    this-> AngleExtent = jobworker.AngleExtent;
    this-> AngleStep = jobworker.AngleStep;
    this-> Optimization = jobworker.Optimization;
    this-> Metric = jobworker.Metric;
    this-> LowThreshold = jobworker.LowThreshold;
    this-> HighThreshold = jobworker.HighThreshold;
    this-> MinContrast = jobworker.MinContrast;
    this-> MinComponentSize = jobworker.MinComponentSize;
    this-> Pregeneration = jobworker.Pregeneration;
    this-> QuantizedOrientations = jobworker.QuantizedOrientations;
    this-> MaxOverlap = jobworker.MaxOverlap;
    this-> SubPixel = jobworker.SubPixel;
    this-> NumLevels = jobworker.NumLevels;
    this-> Greediness = jobworker.Greediness;
    this-> ExpandBorder = jobworker.ExpandBorder;
    this-> MoreAggressiveCandidateStrategy = jobworker.MoreAggressiveCandidateStrategy;
    this-> maxMatches = jobworker.maxMatches;
    this-> MinScore = jobworker.MinScore;

    this-> transformH = jobworker.transformH;
    this->  standardRotateCenter = jobworker.standardRotateCenter;
    this->  standardPoint = jobworker.standardPoint;
    this->  standardAngle = jobworker.standardAngle;


    ///输出结果
    this-> resultType = jobworker.resultType;



    return *this;
}

JobWorker::JobWorker(int &id)
{
    cam_id = id;
    //定位工具初始化
    //找边工具初始化
}

void JobWorker::init()
{
    if(is_train)
    {
        if(cam_id == 0)
        {
            is_train = true;
        }
        else
        {
            train_location();
        }

    }
}

void JobWorker::acqfifo_complete(cv::Mat tmp_img)
{
    check_count += 1;
    if(is_busy == false)
    {

        img = tmp_img.clone();

        if(rotateType >= 0)
        {
            cv::rotate(img, img, rotateType);
        }
        // img = tmp_img;
        emit update_display_img_signed(cam_id);
        is_busy = true;
        run();
    }
    else
    {
        LOGE_HIGH("loss one frame");
    }
}

void JobWorker::set_train_img(cv::Mat _img)
{
    train_image = _img.clone();
}



cv::Mat JobWorker::get_transform_mat(float detect_origin_x, float detect_origin_y, float diff_origin_phi)
{
    cv::Mat mtx_rotate = cv::Mat::eye(3, 3, CV_32FC1); //旋转角度,并移到目标位置
    mtx_rotate.at<float>(0, 0) = std::cos(diff_origin_phi);
    mtx_rotate.at<float>(0, 1) = -std::sin(diff_origin_phi);
    mtx_rotate.at<float>(0, 2) = detect_origin_x;
    mtx_rotate.at<float>(1, 0) = std::sin(diff_origin_phi);
    mtx_rotate.at<float>(1, 1) = std::cos(diff_origin_phi);
    mtx_rotate.at<float>(1, 2) = detect_origin_y;
    return mtx_rotate;
}


// Load center template and calculate bounding box
void getCenterBox(cv::Rect& box)
{
    const std::string center_path = "./model/Center.png";
    cv::Mat center_image = cv::imread(center_path, cv::IMREAD_GRAYSCALE);
    std::vector<std::vector<cv::Point >> contours_1;
    cv::findContours(center_image, contours_1, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    box = cv::boundingRect(contours_1[0]);
}

// Load four corner templates and calculate their bounding boxes
void getFourBoxes(std::vector<cv::Rect>& boxes)
{
    const std::string paths[] =
    {
        "./model/Corner_1.png",
        "./model/Corner_2.png",
        "./model/Corner_3.png",
        "./model/Corner_4.png"
    };
    std::vector<cv::Mat> templates;
    std::vector<std::vector<std::vector<cv::Point >>> all_contours;
    // Load and process all corner templates
    for(const auto& path : paths)
    {
        cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
        std::vector<std::vector<cv::Point >> contours;
        cv::findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        all_contours.push_back(contours);
    }
    // Extract bounding boxes
    boxes =
    {
        cv::boundingRect(all_contours[0][0]),
        cv::boundingRect(all_contours[1][0]),
        cv::boundingRect(all_contours[2][0]),
        cv::boundingRect(all_contours[3][0])
    };
}

void JobWorker::updataTransformCaliperRoiFromCaliperRoi()
{
    for(int id = 0; id < CaliperTransformROIList.size(); ++id)
    {
        // ImgAlg::transformROI(locationTransformH, -shapeMatchAngle, CaliperROIList[id], CaliperTransformROIList[id]);
    }
}

void JobWorker::updataCaliperRoiFromTransformCaliperRoi()
{
    for(int id = 0; id < CaliperTransformROIList.size(); ++id)
    {
        // std::cout << "locationTransformH:" << cv::format(locationTransformH, cv::Formatter::FMT_NUMPY) << std::endl;
        cv::Mat locationTransformH33 = cv::Mat::eye(cv::Size(3, 3), CV_64F);

        locationTransformH33.at<double>(0, 0) = locationTransformH.at<double>(0, 0);
        locationTransformH33.at<double>(0, 1) = locationTransformH.at<double>(0, 1);
        locationTransformH33.at<double>(0, 2) = locationTransformH.at<double>(0, 2);
        locationTransformH33.at<double>(1, 0) = locationTransformH.at<double>(1, 0);
        locationTransformH33.at<double>(1, 1) = locationTransformH.at<double>(1, 1);
        locationTransformH33.at<double>(1, 2) = locationTransformH.at<double>(1, 2);

        cv::Mat locationTransformHInv33 = locationTransformH33.inv();
        // std::cout << "locationTransformHInv33:" << cv::format(locationTransformHInv33, cv::Formatter::FMT_NUMPY) << std::endl;
        cv::Mat locationTransformHInv = locationTransformHInv33(cv::Rect(0, 0, 3, 2));
        // std::cout << "locationTransformHInv:" << cv::format(locationTransformHInv, cv::Formatter::FMT_NUMPY) << std::endl;

        // ImgAlg::transformROI(locationTransformHInv, shapeMatchAngle, CaliperTransformROIList[id], CaliperROIList[id]);

    }
}

void JobWorker::runShapeMatch(cv::Mat img)
{
    locationShapeResult = true;
    cv::Mat img_gray;
    if(img.channels() == 3)
    {
        cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        img_gray = img;
    }
    // LOGE_HIGH("1111");

    std::vector<float> Row;
    std::vector<float> Column;
    std::vector<float> Angle;
    std::vector<float> Score;
    int NumLevels = 0;
    int rt = ImgAlg::FindShapeModel(img_gray, ModelID,
                                    AngleStart,
                                    AngleExtent,
                                    MinScore,
                                    MaxOverlap,
                                    SubPixel,
                                    NumLevels,
                                    Greediness,
                                    ExpandBorder,
                                    MoreAggressiveCandidateStrategy,
                                    Row,
                                    Column,
                                    Angle,
                                    Score);

    // LOGE_HIGH("2222");

    if(rt == 0 && Row.size() > 0)
    {
        // LOGE_HIGH("3333");

        //计算映射矩阵
        ImgAlg::getTransform(Column[0], Row[0], -Angle[0], cv::Point2f(0, 0), locationTransformH);
        locationShapeResult = true;
        locationPosition.x = Column[0];
        locationPosition.y = Row[0];
        shapeMatchAngle = Angle[0];

        //计算结果世界坐标、相对坐标、输出结果
        ImgAlg::transformPoint(transformH, cv::Point2f(locationPosition), locationPositionTransform);
        locationPositionRelative = standardPoint - locationPositionTransform;
        if(resultType == 0)
        {
            locationPositionOutput = cv::Point2f(locationPosition);
        }
        else if(resultType == 1)
        {
            locationPositionOutput = cv::Point2f(locationPositionTransform);
        }
        else
        {
            locationPositionOutput = cv::Point2f(locationPositionRelative);
        }
        //添加补偿值
        locationPositionOutput.x = locationPositionOutput.x * xa + xb;
        locationPositionOutput.y = locationPositionOutput.y * ya + yb;

    }
    else
    {
        locationShapeResult = false;
        locationResult = false;
        return;
    }
}

void JobWorker::runCaliperTool(cv::Mat img)
{
    locationCaliperResult = true;
    // // 卡尺区域调整
    // updataTransformCaliperRoiFromCaliperRoi();
    cv::Mat img_gray;
    if(img.channels() == 3)
    {
        cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        img_gray = img;
    }
    // LOGE_HIGH("1111");
    int rt = ImgAlg::CaliperFindLine(img_gray, CaliperROIList, CaliperROIH, CaliperROIW, CaliperROIC, caliperPolarity, caliperSelectPosition,
                                     caliperThre, fitline, caliperLocationResult, caliperLocationOriginPoints, isDebug);

    // LOGE_HIGH("2222");
    // LOGE_HIGH("rt:%d", rt);

    if(rt != 0)
    {
        locationCaliperResult = false;
        // locationResult = false;
        return;
    }

}

void JobWorker::runCoarsePositioning(cv::Mat img)
{
    // cv::Mat img_gray;
    // if(img.channels() == 3)
    // {
    //     cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    // }
    // else
    // {
    //     img_gray = img;
    // }
    // // locationResult = raivas::wafer_align::roughAlign(img_gray, locationPosition, locationAngle, edgeType);

    // std::vector<std::vector<cv::Point >> circle_contours, chips_contours;

    // raivas::wafer_align::get_circle_chips_contour(img_gray, circleThre, chipThre, chipAreaMin, chipAreaMax, lightChannel, &circle_contours, &chips_contours);
    // locationResult = raivas::wafer_align::get_angle_position(img_gray, circle_contours, chips_contours, edgeType, 0.8,  &locationAngle, &locationPosition);

    // locationAngleOutPut = locationAngle;

    // //加个范围判断
    // // if(locationPosition.x>img_gray.cols * 0.55 || locationPosition.x < img_gray.cols * 0.45 ||
    // //         locationPosition.y > img_gray.rows * 0.55 || locationPosition.y < img_gray.rows * 0.45)

    // if(locationPosition.x>img_gray.cols * 0.7 || locationPosition.x < img_gray.cols * 0.3 ||
    //         locationPosition.y > img_gray.rows * 0.7 || locationPosition.y < img_gray.rows * 0.3)
    // {
    //     locationResult = false;
    // }


    // ImgAlg::transformPoint(transformH, cv::Point2f(locationPosition), locationPositionTransform);

    // locationPositionRelative = standardPoint - locationPositionTransform;
    // if(resultType == 0)
    // {
    //     locationPositionOutput = cv::Point2f(locationPosition);
    // }
    // else if(resultType == 1)
    // {
    //     locationPositionOutput = cv::Point2f(locationPositionTransform);
    // }
    // else
    // {
    //     locationPositionOutput = cv::Point2f(locationPositionRelative);
    // }

    // //添加补偿值
    // locationAngleOutPut = locationAngleOutPut * ra + rb;
    // locationPositionOutput.x = locationPositionOutput.x * xa + xb;
    // locationPositionOutput.y = locationPositionOutput.y * ya + yb;

}

void JobWorker::runLocation(cv::Mat img)
{
    try
    {
        locationResult = true;
        locationAngle = 0;
        shapeMatchAngle = 0;
        locationAngleOutPut = 0;
        locationPosition = cv::Point(0, 0);
        locationPositionTransform = cv::Point2f(0, 0);
        locationPositionRelative = cv::Point2f(0, 0);
        locationPositionOutput = cv::Point2f(0, 0);

        // if(cam_id == 0)
        // {
        //     runCoarsePositioning(img);
        // }
        // else
        // {
        //     locationShapeResult = true;
        //     locationCaliperResult = true;

        //     if(!is_train)

        //     {
        //         locationShapeResult = false;
        //         locationCaliperResult = false;
        //         locationResult = false;
        //         return;
        //     }

        //     //运行
        //     //定位检测
        //     // LOGE_HIGH("111");
        //     runShapeMatch(img);
        //     //卡尺角度检测
        //     if(locationShapeResult)
        //     {
        //         // LOGE_HIGH("222");
        //         runCaliperTool(img);

        //     }

        // }

        LOGE_HIGH("locationPosition:(%f,%f)", (float)locationPosition.x, (float)locationPosition.y);
        LOGE_HIGH("locationPositionTransform:(%f,%f)", (float)locationPositionTransform.x, (float)locationPositionTransform.y);
        LOGE_HIGH("locationPositionRelative:(%f,%f)", (float)locationPositionRelative.x, (float)locationPositionRelative.y);
        LOGE_HIGH("locationPositionOutput:(%f,%f)", (float)locationPositionOutput.x, (float)locationPositionOutput.y);


    }
    catch(cv::Exception &e)
    {
        locationResult = false;
        LOGE_HIGH(e.what());
    }
    catch(std::exception &e)
    {
        locationResult = false;
        LOGE_HIGH(e.what());
    }
    catch(QException &e)
    {
        locationResult = false;
        LOGE_HIGH(e.what());
    }
    catch(...)
    {
        locationResult = false;
        LOGE_HIGH("other error.");
    }
}

void JobWorker::train_location()
{
    is_train = false;

    if(train_image.empty())
    {
        return;
    }
    cv::Mat gray;
    if(train_image.channels() == 3)
    {
        cv::cvtColor(train_image, gray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        gray = train_image;
    }
    //模型训练
    cv::Mat Temp;
    cv::Mat mask;
    int NumLevels = 0;
    ImgAlg::transformImgFromRotateRect(shapeMatchROI, gray, Temp, 1);
    int rt = ImgAlg::CreateShapeModel(Temp, mask, NumLevels, AngleStart,
                                      AngleExtent,
                                      AngleStep,
                                      Optimization,
                                      Metric,
                                      LowThreshold,
                                      HighThreshold,
                                      MinContrast,
                                      MinComponentSize,
                                      Pregeneration,
                                      QuantizedOrientations,
                                      &ModelID);

    if(rt == 0)
    {
        is_train = true;
    }

}

void JobWorker::show_train_model()
{
    cv::Mat gray;

    if(train_image.channels() == 3)
    {
        cv::cvtColor(train_image, gray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        gray = train_image;
    }
    //模型训练
    cv::Mat Temp;
    cv::Mat mask;
    int NumLevels = 0;
    ImgAlg::transformImgFromRotateRect(shapeMatchROI, gray, Temp, 1);


    cv::Mat imgShow;
    cv::cvtColor(Temp, imgShow, cv::COLOR_GRAY2BGR);

    // cv::polylines(imgShow, contours, true, cv::Scalar(0, 0, 255), 1);

    std::vector<cv::Point> contours;
    int rt = 0;
    NumLevels = 0;
    int modelWidth;
    int modelHeight;
    rt = ImgAlg::GetShapeModelContours(ModelID, contours, NumLevels, modelWidth, modelHeight);

    for(int id = 0; id < contours.size() ; ++id)
    {
        cv::circle(imgShow, contours[id], 1, cv::Scalar(0, 0, 255), 1);
    }
    cv::namedWindow("imgShow", cv::WINDOW_NORMAL);
    cv::imshow("imgShow", imgShow);
    cv::waitKey(0);



}

void JobWorker::show_chips_contour()
{
    if(train_image.empty())
    {
        return;
    }
    // cv::Mat gray;
    // cv::Mat imageRGB;
    // if(train_image.channels() == 3)
    // {
    //     cv::cvtColor(train_image, gray, cv::COLOR_BGR2GRAY);
    //     imageRGB = train_image.clone();
    // }
    // else
    // {
    //     gray = train_image;
    //     cv::cvtColor(train_image, imageRGB, cv::COLOR_GRAY2RGB);

    // }


    // std::vector<std::vector<cv::Point >> circle_contours, chips_contours;

    // raivas::wafer_align::get_circle_chips_contour(gray, circleThre, chipThre, chipAreaMin, chipAreaMax, lightChannel, &circle_contours, &chips_contours);

    // cv::drawContours(imageRGB, circle_contours, -1, cv::Scalar(255, 0, 0), 2);
    // cv::drawContours(imageRGB, chips_contours, -1, cv::Scalar(0, 0, 255), 2);
    // cv::namedWindow("image", cv::WINDOW_NORMAL);
    // cv::imshow("image", imageRGB);
    // cv::waitKey(0);
}

void JobWorker::run()
{
    if(img.empty())
    {
        is_busy = false;
        return;
    }
    emit jobworker_start_signed(cam_id);
    try
    {
        jobworker_start_time = clock();
        // if(isFocusing)
        // {
        //     cv::Mat img_gray;
        //     if(img.channels() == 3)
        //     {
        //         cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
        //     }
        //     else
        //     {
        //         img_gray = img;
        //     }
        //     raivas::imgprocess::get_clarity_quantization_value(img_gray, &clarity_quantization_value);
        //     LOGE_HIGH("clarity_quantization_value:%f", clarity_quantization_value);
        // }
        // else
        if(isGrabbing)
        {

        }
        else
        {
            // runLocation(img);
            runCaliperTool(img);
            if(is_save_img)
            {
                if(!is_save_ng_img)
                {
                    save_img();
                }
                else if(is_save_ng_img && !result())
                {
                    save_ng_img();
                }
            }
        }
        jobworker_end_time = clock();
        //    qDebug() << "jobworker" << std::to_string(cam_id).data() << "finish run time:" << jobworker_end_time;
        jobworker_total_use_time = (double)(jobworker_end_time - jobworker_start_time) / CLOCKS_PER_SEC;
        //    qDebug() << "jobworker" << std::to_string(cam_id).data() << "total run time:" << jobworker_total_use_time<<" s";
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
    is_busy = false;
    emit jobworker_end_signed(cam_id);
}

void JobWorker::show_shape()
{
    if(img.empty())
    {
        return;
    }
    display->myGraphicsView->cleanItem();
    //显示结果
    display->myGraphicsView->setImage(MatToPixmap(img));
    display->myGraphicsView->addItem(train_roi, true);
    display->myGraphicsView->show();
}

void JobWorker::show_train_result()
{
    try
    {
        LOGE_HIGH("111")
        QGraphicsLineItem *tmp_line = new QGraphicsLineItem(QLine(QPoint(1, 2), QPoint(2, 3)));
        QGraphicsEllipseItem *tmp_ellip = new QGraphicsEllipseItem();
        display->myGraphicsView->cleanItem(tmp_line);
        display->myGraphicsView->cleanItem(tmp_ellip);
        delete tmp_line;
        delete tmp_ellip;
        LOGE_HIGH("222")
        std::vector<QGraphicsItem *> item_list;
        // LOGE_HIGH("444")
        for(int i = 0; i < item_list.size(); i++)
        {
            display->myGraphicsView->addItem(item_list[i], false);
        }
        // LOGE_HIGH("555")
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

void JobWorker::show_caliper_roi()
{
    cv::Mat img_gray;
    if(img.channels() == 3)
    {
        cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        img_gray = img;
    }
    int rt = ImgAlg::CaliperFindLine(img_gray, CaliperROIList, CaliperROIW, CaliperROIH, CaliperROIC, caliperPolarity, caliperSelectPosition,
                                     caliperThre, fitline, caliperLocationResult, caliperLocationOriginPoints, true);

}



void JobWorker::show_result()
{
    try
    {
        QGraphicsRectItem *tmp_rect = new QGraphicsRectItem(0, 0, 100, 100);
        QGraphicsTextItem *tmp_string = new QGraphicsTextItem("str");
        QGraphicsLineItem *tmp_my_line = new QGraphicsLineItem();
        QGraphicsEllipseItem *tmp_ellip = new QGraphicsEllipseItem();
        display->myGraphicsView->cleanItem(tmp_rect);
        display->myGraphicsView->cleanItem(tmp_string);
        display->myGraphicsView->cleanItem(tmp_my_line);
        display->myGraphicsView->cleanItem(tmp_ellip);
        delete tmp_rect;
        delete tmp_string;
        delete tmp_my_line;
        delete tmp_ellip;
        std::vector<QGraphicsItem *> item_list;


        if(isGrabbing)
        {

        }
        else
        {
            if(locationCaliperResult)
            {

                int line_wide = 5;
                int radio = 10;

                for(int id = 0; id < caliperLocationResult.size(); ++id)
                {
                    if(caliperLocationResult[id])
                    {
                        QGraphicsEllipseItem *result_item = new QGraphicsEllipseItem(caliperLocationOriginPoints[id].x - radio / 2,
                            caliperLocationOriginPoints[id].y - radio / 2, radio, radio);

                        QPen pn(Qt::SolidLine);
                        pn.setColor(Qt::green);
                        pn.setWidth(line_wide);
                        result_item->setPen(pn);
                        item_list.push_back(result_item);
                    }
                }
                //求出直线上的两个点
                double k_line = fitline[1] / fitline[0];
                QPointF p1(0, k_line * (0 - fitline[2]) + fitline[3]);
                QPointF p2(img.cols - 1, k_line * (img.cols - 1 - fitline[2]) + fitline[3]);

                QGraphicsLineItem *result_item2 = new QGraphicsLineItem(QLineF(p1, p2));
                QPen pn(Qt::SolidLine);
                pn.setColor(Qt::blue);
                pn.setWidth(line_wide);
                result_item2->setPen(pn);
                item_list.push_back(result_item2);

                //结果显示

            }
            if(!result())
            {
                QGraphicsTextItem *result_string  = new QGraphicsTextItem("NG");
                result_string->setDefaultTextColor(Qt::red);
                QFont font;
                font.setPointSize(qMin(img.cols, img.rows) / 20);
                result_string->setFont(font);
                result_string->setTextWidth(-1);
                result_string->setPos(int(display->myGraphicsView->width() * 8 / 10),
                                      int(display->myGraphicsView->height() * 8.5 / 10));
                item_list.push_back(result_string);
            }
            else
            {
                QGraphicsTextItem *result_string  = new QGraphicsTextItem("OK");
                result_string->setDefaultTextColor(Qt::green);
                QFont font;
                font.setPointSize(qMin(img.cols, img.rows) / 20);
                result_string->setFont(font);
                result_string->setTextWidth(-1);
                result_string->setPos(int(display->myGraphicsView->width() * 8 / 10),
                                      int(display->myGraphicsView->height() * 8.5 / 10));
                item_list.push_back(result_string);
            }
        }

        for(int i = 0; i < item_list.size(); i++)
        {
            display->myGraphicsView->addItem(item_list[i], false, false);
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

void JobWorker::dispose()
{
    if(display != nullptr)
    {
        delete display;
    }
    if(train_roi != nullptr)
    {
        delete train_roi;
    }
}

bool JobWorker::result()
{
    bool rt = true;
    if(!locationCaliperResult)
    {
        rt = false;
    }
    return rt;
}

void JobWorker::save_img()
{
    std::string save_img_dir = QDir::currentPath().toStdString() + "/save_img";
    // std::string save_img_dir = "/data/save_img";
    std::string jobworker_save_img_dir = save_img_dir + "/jobworker" + std::to_string(cam_id);
    //        string save_img_path = jobworker_save_img_dir+"/" + std::to_string(time(nullptr))+".bmp";
    //        timeb t;
    QDateTime _t = QDateTime::currentDateTime();
    //        ftime(&t);
    //        string save_img_path = jobworker_save_img_dir+"/" + std::to_string(t.time *1000+ t.millitm)+".bmp";
    std::string save_img_path = jobworker_save_img_dir + "/" + std::to_string(_t.toMSecsSinceEpoch()) + ".bmp";
    QDir save_img_dir_dir(save_img_dir.data());
    // if(!save_img_dir_dir.exists())
    // {
    //     save_img_dir_dir.mkdir(save_img_dir.data());
    // }
    // QDir jobworker_save_img_dir_dir(jobworker_save_img_dir.data());
    // if(!jobworker_save_img_dir_dir.exists())
    // {
    //     jobworker_save_img_dir_dir.mkdir(jobworker_save_img_dir.data());
    // }
    createMultipleFolders(jobworker_save_img_dir.data());

    cv::imwrite(save_img_path, img);
}

void JobWorker::save_ng_img()
{
    std::string save_img_dir = QDir::currentPath().toStdString() + "/save_ng_img";
    // std::string save_img_dir = "/data/save_ng_img";
    std::string jobworker_save_img_dir = save_img_dir + "/jobworker" + std::to_string(cam_id);
    // QDir save_img_dir_dir(save_img_dir.data());
    // if(!save_img_dir_dir.exists())
    // {
    //     save_img_dir_dir.mkdir(save_img_dir.data());
    // }
    // QDir jobworker_save_img_dir_dir(jobworker_save_img_dir.data());
    // if(!jobworker_save_img_dir_dir.exists())
    // {
    //     jobworker_save_img_dir_dir.mkdir(jobworker_save_img_dir.data());
    // }
    createMultipleFolders(jobworker_save_img_dir.data());

    //清除多余图片
    std::vector<std::string> src_image_paths;
    QDir d(jobworker_save_img_dir.data());
    d.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = d.entryInfoList();
    for(int i = 0; i < list.size(); i++)
    {
        QFileInfo info = list.at(i);
        src_image_paths.push_back(jobworker_save_img_dir + "/" + info.fileName().toStdString());
    }
    if(src_image_paths.size() > save_ng_img_limit_count - 1)
    {
        for(int i = 0; i < src_image_paths.size() - save_ng_img_limit_count + 1; i++)
        {
            std::string img_path = src_image_paths[i];
            bool ok = QFile::remove(img_path.data());
        }
    }
    // 保存图片
    QDateTime _t = QDateTime::currentDateTime();
    std::string save_img_path = jobworker_save_img_dir + "/" + std::to_string(_t.toMSecsSinceEpoch()) + ".bmp";
    cv::imwrite(save_img_path, img);
}


QString createMultipleFolders(const QString path)
{
    QDir dir(path);
    if(path == "" || dir.exists(path))
    {
        return path;
    }
    QString parentDir = createMultipleFolders(path.mid(0, path.lastIndexOf('/')));
    QString dirName = path.mid(path.lastIndexOf('/') + 1);
    QDir parentPath(parentDir);
    if(!dirName.isEmpty())
    {
        parentPath.mkpath(dirName);
    }
    return parentDir + "/" + dirName;
}

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    int nn = n ;
    std::ostringstream out;
    out.setf(std::ios::fixed);
    out << std::setprecision(nn) << a_value;
    return out.str();
}

//void JobWorker::save_img(Yolo::BoxArray boxs)
//{

//    std::string save_img_dir = "/data/save_data";
//    auto time_now = std::chrono::system_clock::now();
//    auto tt = std::chrono::system_clock::to_time_t(time_now);
//    struct tm *ptm = std::localtime(&tt);
//    char date[60] = { 0 };
//    sprintf(date, "/%d-%02d-%02d",
//            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday);

//    std::string jobworker_save_img_dir = save_img_dir+date+"/images"+"/jobworker"+std::to_string(cam_id);

//    std::string jobworker_save_label_dir = save_img_dir+date+"/labels"+"/jobworker"+std::to_string(cam_id);

//    QDateTime _t = QDateTime::currentDateTime();

//    std::string save_img_path = jobworker_save_img_dir+"/" + std::to_string(_t.toMSecsSinceEpoch())+".bmp";
//    std::string save_label_path = jobworker_save_label_dir+ "/" + std::to_string(_t.toMSecsSinceEpoch())+".txt";

////    QDir save_img_dir_dir(save_img_dir.data());
////    if(!save_img_dir_dir.exists())
////    {
////        save_img_dir_dir.mkdir(save_img_dir.data());
////    }
////    QDir jobworker_save_img_dir_dir(jobworker_save_img_dir.data());
////    if(!jobworker_save_img_dir_dir.exists())
////    {
////        jobworker_save_img_dir_dir.mkdir(jobworker_save_img_dir.data());
////    }
////    QDir jobworker_save_label_dir_dir(jobworker_save_label_dir.data());
////    if(!jobworker_save_label_dir_dir.exists())
////    {
////        jobworker_save_label_dir_dir.mkdir(jobworker_save_label_dir.data());
////    }
//    createMultipleFolders(jobworker_save_img_dir.data());
//    createMultipleFolders(jobworker_save_label_dir.data());

//    //记录label
//    if(boxs.size()>0)
//    {
//        QFile file(save_label_path.data());
//        if(file.open(QFile::WriteOnly | QFile::Truncate))
//        {
//            for(auto& obj : boxs)
//            {
//                char date[60] = { 0 };
//                float x_center_normal = (obj.left + obj.right)/2/img.size().width;
//                float y_center_normal = (obj.top + obj.bottom)/2/img.size().height;
//                float w_normal = (obj.right - obj.left)/img.size().width;
//                float h_normal = (obj.bottom - obj.top)/img.size().height;

//                std::string x_center_normal_str = to_string_with_precision(x_center_normal,6);
//                std::string y_center_normal_str = to_string_with_precision(y_center_normal,6);
//                std::string w_normal_str = to_string_with_precision(w_normal,6);
//                std::string h_normal_str = to_string_with_precision(h_normal,6);
//                std::string conf_str = to_string_with_precision(obj.confidence,3);

//                sprintf(date, "%d %s %s %s %s %s\n" ,obj.class_label,
//                        x_center_normal_str.data(),y_center_normal_str.data(),w_normal_str.data(),h_normal_str.data(),conf_str.data());
//                file.write(date);

//            }

//            file.close();
//        }
//    }

//    //保存图片
//    cv::imwrite(save_img_path,img);

//}

// void JobWorker::acqfifo_complete()
// {
//     MV_FRAME_OUT stImageInfo = {0};
//     unsigned char *pTemp = NULL;
//     unsigned char *pImageBuf = NULL;
//     unsigned int nImageBufSize = 0;
//     int nRet = MV_OK;
//     try
//     {
//         while(b_thread_state)
//         {
//             if(!b_start_grabbing)
//             {
//                 QThread::msleep(1);
//                 continue;
//             }
//             nRet = m_pcMyCamera->GetImageBuffer(&stImageInfo, 1000);
//             cv::Mat tmp_img;
//             cv::Mat save_img;
//             if(nRet == MV_OK)
//             {
//                 if(m_work_mode_type == work_mode_type::auto_mode)
//                 {
//                     //记录获取图像时间
//                     tmp_img_time_list_mutex.lock();
//                     tmp_img_time_list.push_back(std::chrono::system_clock::now());
//                     tmp_img_time_list_mutex.unlock();
//                 }
//                 //                std::cout << "cam " << cam_id << " acq finish";
//                 LOGE_HIGH("cam %d acq finish", cam_id);
//                 //            qDebug()<<333;
//                 if(IsBayer8PixelFormat(stImageInfo.stFrameInfo.enPixelType))
//                 {
//                     //                    qDebug() << "origin image";
//                     tmp_img = cv::Mat(stImageInfo.stFrameInfo.nHeight, stImageInfo.stFrameInfo.nWidth, CV_8UC1, stImageInfo.pBufAddr);
//                     cv::Mat tmp_img2;
//                     switch(stImageInfo.stFrameInfo.enPixelType)
//                     {
//                         case PixelType_Gvsp_BayerGR8:
//                             cv::cvtColor(tmp_img, tmp_img2, cv::COLOR_BayerGR2BGR);
//                         case PixelType_Gvsp_BayerRG8:
//                             cv::cvtColor(tmp_img, tmp_img2, cv::COLOR_BayerRG2BGR);
//                         case PixelType_Gvsp_BayerGB8:
//                             cv::cvtColor(tmp_img, tmp_img2, cv::COLOR_BayerGB2BGR);
//                         case PixelType_Gvsp_BayerBG8:
//                             cv::cvtColor(tmp_img, tmp_img2, cv::COLOR_BayerBG2BGR);
//                     }
//                     tmp_img = tmp_img2;
//                     if(MV_OK != nRet)
//                     {
//                         break;
//                     }
//                 }
//                 else if(IsColorPixelFormat(stImageInfo.stFrameInfo.enPixelType))      //Color
//                 {
//                     if(PixelType_Gvsp_BGR8_Packed == stImageInfo.stFrameInfo.enPixelType)
//                     {
//                         pTemp = stImageInfo.pBufAddr;
//                     }
//                     else
//                     {
//                         if(NULL == pImageBuf || nImageBufSize < (stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight * 3))
//                         {
//                             if(pImageBuf)
//                             {
//                                 free(pImageBuf);
//                                 pImageBuf = NULL;
//                             }
//                             pImageBuf = (unsigned char *)malloc(stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight * 3);
//                             if(pImageBuf == NULL)
//                             {
//                                 break;
//                             }
//                             nImageBufSize = stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight * 3;
//                         }
//                         MV_CC_PIXEL_CONVERT_PARAM stPixelConvertParam;
//                         memset(&stPixelConvertParam, 0, sizeof(MV_CC_PIXEL_CONVERT_PARAM));
//                         stPixelConvertParam.pSrcData = stImageInfo.pBufAddr;
//                         stPixelConvertParam.nWidth = stImageInfo.stFrameInfo.nWidth;
//                         stPixelConvertParam.nHeight = stImageInfo.stFrameInfo.nHeight;
//                         stPixelConvertParam.enSrcPixelType = stImageInfo.stFrameInfo.enPixelType;
//                         stPixelConvertParam.nSrcDataLen = stImageInfo.stFrameInfo.nFrameLen;
//                         stPixelConvertParam.nDstBufferSize = nImageBufSize;
//                         stPixelConvertParam.pDstBuffer = pImageBuf;
//                         stPixelConvertParam.enDstPixelType = PixelType_Gvsp_BGR8_Packed;
//                         nRet = m_pcMyCamera->ConvertPixelType(&stPixelConvertParam);
//                         if(MV_OK != nRet)
//                         {
//                             break;
//                         }
//                         pTemp = pImageBuf;
//                     }
//                     //                    qDebug() << "color image";
//                     tmp_img = cv::Mat(stImageInfo.stFrameInfo.nHeight, stImageInfo.stFrameInfo.nWidth, CV_8UC3, pTemp);
//                     if(MV_OK != nRet)
//                     {
//                         break;
//                     }
//                 }
//                 else if(IsMonoPixelFormat(stImageInfo.stFrameInfo.enPixelType))      //Mono
//                 {
//                     if(PixelType_Gvsp_Mono8 == stImageInfo.stFrameInfo.enPixelType)
//                     {
//                         pTemp = stImageInfo.pBufAddr;
//                     }
//                     else
//                     {
//                         if(NULL == pImageBuf || nImageBufSize < (stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight))
//                         {
//                             if(pImageBuf)
//                             {
//                                 free(pImageBuf);
//                                 pImageBuf = NULL;
//                             }
//                             pImageBuf = (unsigned char *)malloc(stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight);
//                             if(pImageBuf == NULL)
//                             {
//                                 break;
//                             }
//                             nImageBufSize = stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight;
//                         }
//                         MV_CC_PIXEL_CONVERT_PARAM stPixelConvertParam;
//                         memset(&stPixelConvertParam, 0, sizeof(MV_CC_PIXEL_CONVERT_PARAM));
//                         stPixelConvertParam.pSrcData = stImageInfo.pBufAddr;
//                         stPixelConvertParam.nWidth = stImageInfo.stFrameInfo.nWidth;
//                         stPixelConvertParam.nHeight = stImageInfo.stFrameInfo.nHeight;
//                         stPixelConvertParam.enSrcPixelType = stImageInfo.stFrameInfo.enPixelType;
//                         stPixelConvertParam.nSrcDataLen = stImageInfo.stFrameInfo.nFrameLen;
//                         stPixelConvertParam.nDstBufferSize = nImageBufSize;
//                         stPixelConvertParam.pDstBuffer = pImageBuf;
//                         stPixelConvertParam.enDstPixelType = PixelType_Gvsp_Mono8;
//                         nRet = m_pcMyCamera->ConvertPixelType(&stPixelConvertParam);
//                         if(MV_OK != nRet)
//                         {
//                             break;
//                         }
//                         pTemp = pImageBuf;
//                     }
//                     //                    qDebug() << "gray image";
//                     tmp_img = cv::Mat(stImageInfo.stFrameInfo.nHeight, stImageInfo.stFrameInfo.nWidth, CV_8UC1, pTemp);
//                     if(MV_OK != nRet)
//                     {
//                         break;
//                     }
//                 }
//                 //判断是否自动状态
//                 //                if(m_work_mode_type == work_mode_type::auto_mode || m_work_mode_type == work_mode_type::hand_mode)
//                 if(m_work_mode_type == work_mode_type::auto_mode)
//                 {
//                     //如果是自动模式，则先把图片缓存，等到所有图片都缓存好，再进行推理
//                     save_img = tmp_img.clone();
//                 }
//                 m_pcMyCamera->FreeImageBuffer(&stImageInfo);
//             }
//             else
//             {
//                 QThread::msleep(1);
//             }
//             if(nRet == MV_OK)
//             {
//                 check_count += 1;
//                 //判断是否自动状态
//                 //                if(m_work_mode_type == work_mode_type::auto_mode || m_work_mode_type == work_mode_type::hand_mode)
//                 if(m_work_mode_type == work_mode_type::auto_mode)
//                 {
//                     //                    char date[60] = { 0 };
//                     //                    sprintf(date, "save_img %d", cam_id);
//                     //                    cv::namedWindow(date, cv::WINDOW_NORMAL);
//                     //                    cv::resizeWindow(date, 2048, 1440) ;
//                     //                    cv::imshow(date, save_img);
//                     LOGE_PROFESSIONAL("cam %d add one frame to list", cam_id);
//                     //如果是自动模式，并且是2号工位的相机，则先把图片缓存，等到所有图片都缓存好，再进行推理
//                     wait_for_count += 1;
//                     tmp_img_list_mutex.lock();
//                     tmp_img_list.push_back(save_img);
//                     tmp_img_list_mutex.unlock();
//                     QThread::msleep(1);
//                     continue;
//                 }
//                 if(is_busy == false)
//                 {
//                     //                img = tmp_img.clone();
//                     img = tmp_img;
//                     emit update_display_img_signed(cam_id);
//                     is_busy = true;
//                     run();
//                 }
//                 else
//                 {
//                     LOGE_HIGH("loss one frame");
//                     //                    qDebug() << "loss one frame";
//                 }
//             }
//         }
//     }
//     catch(cv::Exception &e)
//     {
//         LOGE_HIGH(e.what());
//     }
//     catch(std::exception &e)
//     {
//         LOGE_HIGH(e.what());
//     }
//     catch(QException &e)
//     {
//         LOGE_HIGH(e.what());
//     }
//     catch(...)
//     {
//         LOGE_HIGH("other error.");
//     }
// }

void JobWorker::tcp_server_init()
{
    if(m_tcpServer == nullptr)
    {
        m_tcpServer = new QTcpServer(this);
    }
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(tcp_socker_connected()));
    QString Ip = QString(plc_ip.data());
    quint16 port = net_port;
    QHostAddress addr(Ip);
    m_tcpServer->listen(addr, port);
}

void JobWorker::tcp_server_dispose()
{
    if(m_tcpServer != nullptr)
    {
        disconnect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(tcp_socker_connected()));
        if(m_tcpServer->isListening())
        {
            m_tcpServer->close();
        }
        delete  m_tcpServer;
        m_tcpServer = nullptr;
    }
}

void JobWorker::tcp_socker_connected()
{
    m_tcpSocket = m_tcpServer->nextPendingConnection();   //创建 socket
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(tcp_socker_disconnected()));
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(tcp_ready_read()));
    std::cout << "connect success!!!" << std::endl;
}

void JobWorker::tcp_socker_disconnected()
{
    std::cout << "disconnect success!!!" << std::endl;
}

void JobWorker::tcp_ready_read()
{
    try
    {
        QString info = m_tcpSocket->readAll();
        // std::cout << "receive tcp:" << info.toStdString() << std::endl;
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}


void JobWorker::reset_count()
{
    //    is_busy = false;
    //    ok_count = 0;
    //    ng_count = 0;
    //    check_count = 0;
    //    wait_for_count = 0;
}

int JobWorker::all_count()
{
    return ok_count + ng_count;
}

void JobWorker::get_channel_img()
{
    // if(img.channels() > 1)
    // {
    //     cv::extractChannel(img, channel_img, input_channel - 1);
    // }
    // else
    // {
    //     channel_img = img;
    // }
}


// unsigned int grab_thread(void *pUser)
// {
//     if(pUser)
//     {
//         JobWorker *pCam = (JobWorker *)pUser;
//         qDebug() << "grab_thread";
//         pCam->acqfifo_complete();
//         return 0;
//     }
//     return -1;
// }
