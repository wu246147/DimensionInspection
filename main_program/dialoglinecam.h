#ifndef DIALOGLINECAM_H
#define DIALOGLINECAM_H

#include <QDialog>

//#include <windows.h>

#include "jobmanager.h"
#include "MvCamera.h"
#include "mainwindow.h"

#pragma execution_character_set("utf-8")

namespace Ui
{
    class DialogLineCam;
}

class DialogLineCam : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLineCam(QWidget *parent = nullptr);
    DialogLineCam(JobManager *j, QWidget *parent = nullptr);
    ~DialogLineCam();

    void mvs_stop_grab();
    void mvs_start_grab();

    ///
    /// \brief continue_run 测试定位函数
    ///
    void continueGrab();


public:
    JobManager *jobmanager = nullptr;
    bool is_loading = true;
    int cam_id = 0;
    bool running = false;
    bool isContineGrabFinish = true;
    QFuture<void> f;

private slots:


    void on_comboBox_cam_currentIndexChanged(int index);

    void on_doubleSpinBox_exposure_valueChanged(double arg1);

    void on_doubleSpinBox_gain_valueChanged(double arg1);

    void on_pushButton_check_one_clicked();

    void on_doubleSpinBox_trigger_delay_valueChanged(double arg1);

    void on_pushButton_check_one_2_clicked(bool checked);

    void on_comboBox_camRotate_currentIndexChanged(int index);

private:

    ///
    /// \brief closeEvent 关闭窗口事件
    /// \param event
    ///
    void closeEvent(QCloseEvent *event);



    Ui::DialogLineCam *ui;
};

#endif // DIALOGLINECAM_H
