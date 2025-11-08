#include "cam.h"

Cam::Cam()
{
}

Cam::Cam(int _cam_id)
{
    cam_id = _cam_id;
}

Cam::Cam(const Cam &_cam)
{
    this->cam_id = _cam.cam_id;
    this-> src_image_dir = _cam.src_image_dir;
    this-> trigger_delay = _cam.trigger_delay;
    this-> exposure_time  = _cam.exposure_time;
    this-> gain  = _cam.gain;
}

Cam &Cam::operator =(const Cam &_cam)
{
    //相机id
    this->cam_id = _cam.cam_id;
    this-> src_image_dir = _cam.src_image_dir;
    this-> trigger_delay = _cam.trigger_delay;
    this-> exposure_time  = _cam.exposure_time;
    this-> gain  = _cam.gain;
    return *this;
}

Cam::~Cam()
{
    if(m_pcMyCamera != nullptr)
    {
        delete m_pcMyCamera;
    }
}

/************************************************************************
 *  @fn     IsBayer8PixelFormat()
 *  @brief  判断是否是Bayer8格式
 *  @param  enType                [IN]            像素格式
 *  @return 是，返回true；否，返回false
 ************************************************************************/
bool IsBayer8PixelFormat(MvGvspPixelType enType)
{
    switch(enType)
    {
        case PixelType_Gvsp_BayerGR8:
        case PixelType_Gvsp_BayerRG8:
        case PixelType_Gvsp_BayerGB8:
        case PixelType_Gvsp_BayerBG8:
            return true;
        default:
            return false;
    }
}

/************************************************************************
 *  @fn     IsColorPixelFormat()
 *  @brief  判断是否是彩色格式
 *  @param  enType                [IN]            像素格式
 *  @return 是，返回true；否，返回false
 ************************************************************************/
bool IsColorPixelFormat(MvGvspPixelType enType)
{
    switch(enType)
    {
        case PixelType_Gvsp_RGB8_Packed:
        case PixelType_Gvsp_BGR8_Packed:
        case PixelType_Gvsp_RGBA8_Packed:
        case PixelType_Gvsp_BGRA8_Packed:
        case PixelType_Gvsp_YUV422_Packed:
        case PixelType_Gvsp_YUV422_YUYV_Packed:
        case PixelType_Gvsp_BayerGB10:
        case PixelType_Gvsp_BayerGB10_Packed:
        case PixelType_Gvsp_BayerBG10:
        case PixelType_Gvsp_BayerBG10_Packed:
        case PixelType_Gvsp_BayerRG10:
        case PixelType_Gvsp_BayerRG10_Packed:
        case PixelType_Gvsp_BayerGR10:
        case PixelType_Gvsp_BayerGR10_Packed:
        case PixelType_Gvsp_BayerGB12:
        case PixelType_Gvsp_BayerGB12_Packed:
        case PixelType_Gvsp_BayerBG12:
        case PixelType_Gvsp_BayerBG12_Packed:
        case PixelType_Gvsp_BayerRG12:
        case PixelType_Gvsp_BayerRG12_Packed:
        case PixelType_Gvsp_BayerGR12:
        case PixelType_Gvsp_BayerGR12_Packed:
            return true;
        default:
            return false;
    }
}

/************************************************************************
 *  @fn     IsMonoPixelFormat()
 *  @brief  判断是否是黑白格式
 *  @param  enType                [IN]            像素格式
 *  @return 是，返回true；否，返回false
 ************************************************************************/
bool IsMonoPixelFormat(MvGvspPixelType enType)
{
    switch(enType)
    {
        case PixelType_Gvsp_Mono8:
        case PixelType_Gvsp_Mono10:
        case PixelType_Gvsp_Mono10_Packed:
        case PixelType_Gvsp_Mono12:
        case PixelType_Gvsp_Mono12_Packed:
            return true;
        default:
            return false;
    }
}


void Cam::acqfifo_complete()
{
    MV_FRAME_OUT stImageInfo = {0};
    unsigned char *pTemp = NULL;
    unsigned char *pImageBuf = NULL;
    unsigned int nImageBufSize = 0;
    int nRet = MV_OK;
    try
    {
        while(b_thread_state)
        {
            if(!b_start_grabbing)
            {
                QThread::msleep(1);
                continue;
            }
            nRet = m_pcMyCamera->GetImageBuffer(&stImageInfo, 1000);
            cv::Mat tmp_img;
            cv::Mat save_img;
            if(nRet == MV_OK)
            {
                std::cout << "cam " << cam_id << " acq finish";
                LOGE_HIGH("cam %d worker %d acq finish", cam_id, worker_id);
                //            qDebug()<<333;
                if(IsBayer8PixelFormat(stImageInfo.stFrameInfo.enPixelType))
                {
                    //                    qDebug() << "origin image";
                    tmp_img = cv::Mat(stImageInfo.stFrameInfo.nHeight, stImageInfo.stFrameInfo.nWidth, CV_8UC1, stImageInfo.pBufAddr);
                    cv::Mat tmp_img2;
                    switch(stImageInfo.stFrameInfo.enPixelType)
                    {
                        case PixelType_Gvsp_BayerGR8:
                            cv::cvtColor(tmp_img, tmp_img2, cv::COLOR_BayerGR2BGR);
                        case PixelType_Gvsp_BayerRG8:
                            cv::cvtColor(tmp_img, tmp_img2, cv::COLOR_BayerRG2BGR);
                        case PixelType_Gvsp_BayerGB8:
                            cv::cvtColor(tmp_img, tmp_img2, cv::COLOR_BayerGB2BGR);
                        case PixelType_Gvsp_BayerBG8:
                            cv::cvtColor(tmp_img, tmp_img2, cv::COLOR_BayerBG2BGR);
                    }
                    tmp_img = tmp_img2;
                    if(MV_OK != nRet)
                    {
                        break;
                    }
                }
                else if(IsColorPixelFormat(stImageInfo.stFrameInfo.enPixelType))      //Color
                {
                    if(PixelType_Gvsp_BGR8_Packed == stImageInfo.stFrameInfo.enPixelType)
                    {
                        pTemp = stImageInfo.pBufAddr;
                    }
                    else
                    {
                        if(NULL == pImageBuf || nImageBufSize < (stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight * 3))
                        {
                            if(pImageBuf)
                            {
                                free(pImageBuf);
                                pImageBuf = NULL;
                            }
                            pImageBuf = (unsigned char *)malloc(stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight * 3);
                            if(pImageBuf == NULL)
                            {
                                break;
                            }
                            nImageBufSize = stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight * 3;
                        }
                        MV_CC_PIXEL_CONVERT_PARAM stPixelConvertParam;
                        memset(&stPixelConvertParam, 0, sizeof(MV_CC_PIXEL_CONVERT_PARAM));
                        stPixelConvertParam.pSrcData = stImageInfo.pBufAddr;
                        stPixelConvertParam.nWidth = stImageInfo.stFrameInfo.nWidth;
                        stPixelConvertParam.nHeight = stImageInfo.stFrameInfo.nHeight;
                        stPixelConvertParam.enSrcPixelType = stImageInfo.stFrameInfo.enPixelType;
                        stPixelConvertParam.nSrcDataLen = stImageInfo.stFrameInfo.nFrameLen;
                        stPixelConvertParam.nDstBufferSize = nImageBufSize;
                        stPixelConvertParam.pDstBuffer = pImageBuf;
                        stPixelConvertParam.enDstPixelType = PixelType_Gvsp_BGR8_Packed;
                        nRet = m_pcMyCamera->ConvertPixelType(&stPixelConvertParam);
                        if(MV_OK != nRet)
                        {
                            break;
                        }
                        pTemp = pImageBuf;
                    }
                    //                    qDebug() << "color image";
                    tmp_img = cv::Mat(stImageInfo.stFrameInfo.nHeight, stImageInfo.stFrameInfo.nWidth, CV_8UC3, pTemp);
                    if(MV_OK != nRet)
                    {
                        break;
                    }
                }
                else if(IsMonoPixelFormat(stImageInfo.stFrameInfo.enPixelType))      //Mono
                {
                    if(PixelType_Gvsp_Mono8 == stImageInfo.stFrameInfo.enPixelType)
                    {
                        pTemp = stImageInfo.pBufAddr;
                    }
                    else
                    {
                        if(NULL == pImageBuf || nImageBufSize < (stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight))
                        {
                            if(pImageBuf)
                            {
                                free(pImageBuf);
                                pImageBuf = NULL;
                            }
                            pImageBuf = (unsigned char *)malloc(stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight);
                            if(pImageBuf == NULL)
                            {
                                break;
                            }
                            nImageBufSize = stImageInfo.stFrameInfo.nWidth * stImageInfo.stFrameInfo.nHeight;
                        }
                        MV_CC_PIXEL_CONVERT_PARAM stPixelConvertParam;
                        memset(&stPixelConvertParam, 0, sizeof(MV_CC_PIXEL_CONVERT_PARAM));
                        stPixelConvertParam.pSrcData = stImageInfo.pBufAddr;
                        stPixelConvertParam.nWidth = stImageInfo.stFrameInfo.nWidth;
                        stPixelConvertParam.nHeight = stImageInfo.stFrameInfo.nHeight;
                        stPixelConvertParam.enSrcPixelType = stImageInfo.stFrameInfo.enPixelType;
                        stPixelConvertParam.nSrcDataLen = stImageInfo.stFrameInfo.nFrameLen;
                        stPixelConvertParam.nDstBufferSize = nImageBufSize;
                        stPixelConvertParam.pDstBuffer = pImageBuf;
                        stPixelConvertParam.enDstPixelType = PixelType_Gvsp_Mono8;
                        nRet = m_pcMyCamera->ConvertPixelType(&stPixelConvertParam);
                        if(MV_OK != nRet)
                        {
                            break;
                        }
                        pTemp = pImageBuf;
                    }
                    //                    qDebug() << "gray image";
                    tmp_img = cv::Mat(stImageInfo.stFrameInfo.nHeight, stImageInfo.stFrameInfo.nWidth, CV_8UC1, pTemp);
                    if(MV_OK != nRet)
                    {
                        break;
                    }
                }
                img = tmp_img.clone();
                m_pcMyCamera->FreeImageBuffer(&stImageInfo);
            }
            else
            {
                QThread::msleep(1);
            }
            if(nRet == MV_OK)
            {
                acq_count += 1;
                //发出采集完成信号
                emit finish_acq(cam_id, worker_id);
            }
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
