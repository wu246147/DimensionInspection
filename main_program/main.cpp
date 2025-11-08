#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>
#include <QTextCodec>
#include <QDebug>
#include "algorithm/qtstreambuf.h"
#include "superdog.h"

// //兼容版sleep
// #include <QCoreApplication>
// #ifdef Q_OS_WIN32
//     #include <window.h>
//     #define sleep(sec) Sleep(sec*1000)
//     #define msleep(sec) Sleep(sec)
// #else
//     #include <unistd.h>
//     #define msleep(msec) usleep(msec*1000)
// #endif
// //#include "sys/timeb.h"

#include <QThread>
#include <mylog.h>
#include "SharePara.h"

#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#define DEBUG 1

int main(int argc, char *argv[])
{
    // debug时可以临时屏蔽，但是release必须打开
    if(DEBUG == 0)
    {
        SuperDog dog;
    }

    QApplication a(argc, argv);
    //log初始化
    QDir qdir;
    QString logfile_dir = QString(QDir::currentPath() + "/logfile");
    //    std::cout << "logfile_dir :" << logfile_dir.toStdString().data() << std::endl;
    if(!qdir.exists(logfile_dir))
    {
        qdir.mkdir(logfile_dir);
    }
    //    std::string current_time = logfile::getCurrentSystemTime();
    //    current_time = current_time.replace(10, 13, "");
    //    QString logfile_path = (logfile_dir.toStdString() + "/" + current_time + ".log").data();
    //    logfile::LOGINIT(logfile_path.toStdString());
    logfile::LOGINIT(logfile_dir.toStdString());
    if(argc > 1)
    {
        //        std::cout << argv[1] << std::endl;
        //        std::cout << argv[2] << std::endl;
        //        std::cout << std::string(argv[1]).compare("professional") << std::endl;
        if(std::string(argv[1]).compare("normal") == 0)
        {
            logfile::set_rank(logfile::log_rank::normal);
            std::cout << "set_rank as " << "normal" << std::endl;
        }
        else if(std::string(argv[1]).compare("high") == 0)
        {
            logfile::set_rank(logfile::log_rank::high);
            std::cout << "set_rank as " << "high" << std::endl;
        }
        else if(std::string(argv[1]).compare("professional") == 0)
        {
            logfile::set_rank(logfile::log_rank::professional);
            std::cout << "set_rank as " << "professional" << std::endl;
        }
        else
        {
            logfile::set_rank(logfile::log_rank::normal);
            std::cout << "there is not type as " << argv[1] << std::endl;
        }
        if(std::string(argv[2]).compare("true") == 0)
        {
            logfile::set_is_write(true);
            std::cout << "set_is_write as " << "true" << std::endl;
        }
        else if(std::string(argv[2]).compare("false") == 0)
        {
            logfile::set_is_write(false);
            std::cout << "set_is_write as " << "false" << std::endl;
        }
        else
        {
            logfile::set_is_write(false);
            std::cout << "there is not type as " << argv[2] << std::endl;
        }
    }
    else
    {
        logfile::set_rank(logfile::log_rank::high);
        logfile::set_is_write(false);
    }
    QPixmap pixmap("./img/logo.jpg");
    pixmap = pixmap.scaled(QSize(800, 400));
    QSplashScreen splash(pixmap);
    splash.show();
    QThread::msleep(10);
    a.processEvents();
    MainWindow w;
    splash.finish(&w);
    //log显示
    std::shared_ptr<qtStreamBuf>buffer;
    buffer = std::make_shared<qtStreamBuf>(&w);
    new(&std::cout) std::ostream(buffer.get());
    LOGE_HIGH("----------------------------------------------");
    LOGE_HIGH("程序打开");
    w.setWindowFlags(Qt::FramelessWindowHint);
    // if(argc > 1)
    // {
    //     if(std::string(argv[3]).compare("true") == 0)
    //     {
    //         showWindowType = ShowWindowType::ShowMain;
    //     }
    //     else
    //     {
    //         showWindowType = ShowWindowType::ShowForm;
    //     }
    // }
    // else
    // {
    //     showWindowType = ShowWindowType::ShowForm;
    // }
    // if(showWindowType == ShowWindowType::ShowMain)
    // {
    //     w.isShowForm = false;
    // }
    // else if(showWindowType == ShowWindowType::ShowForm)
    // {
    //     w.isShowForm = true;
    // }
    w.show();
    return a.exec();
}
