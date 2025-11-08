#ifndef DIALOGCALIBRATION_H
#define DIALOGCALIBRATION_H

#include <QDialog>

#include "jobmanager.h"


namespace Ui
{
    class DialogCalibration;
}

class DialogCalibration : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCalibration(QWidget *parent = nullptr);
    DialogCalibration(JobManager *j, QWidget *parent = nullptr);

    ~DialogCalibration();


public :
    JobManager *jobmanager = nullptr;
    bool is_loading = true;
    int cam_id = -1;

    std::vector<cv::Point2f>camPoints;
    std::vector<cv::Point2f>robotPoints;


    void showTransformH();

    void addStatisticsData();

public slots:
    int calibrationCam(int cam_id, float robotX, float robotY);


private slots:
    void on_comboBox_cam_currentIndexChanged(int index);

    void on_pushButton_record_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_calibration_clicked();

    void on_pushButton_calibration_check_clicked();

private:
    Ui::DialogCalibration *ui;
};

#endif // DIALOGCALIBRATION_H
