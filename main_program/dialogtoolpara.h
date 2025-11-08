#ifndef DIALOGTOOLPARA_H
#define DIALOGTOOLPARA_H

#include <QDialog>
#include "jobmanager.h"
#include "jobworker.h"
#include <QGridLayout>
#include <QSplashScreen>
#include "qwaiting.h"
#include <QMessageBox>
#include "mainwindow.h"


namespace Ui
{
    class DialogToolPara;
}




void draw_roi(int event, int x, int y, int flags, void *param);


class DialogToolPara : public QDialog
{
    Q_OBJECT

public:
    explicit DialogToolPara(QWidget *parent = nullptr);
    DialogToolPara(JobManager *j, QWidget *parent = nullptr);

    ~DialogToolPara();

    ///
    /// \brief set_display
    ///
    void set_display();

    ///
    /// \brief init
    ///
    void init();

    ///
    /// \brief update_train_roi 更新训练roi
    ///
    void update_train_roi();

    ///
    /// \brief update_run_roi 更新运行roi
    ///
    void update_run_roi();

public:
    JobManager *jobmanager = nullptr;

    bool is_loading = true;

    MyGraphicRectItem *myRectItem_location = nullptr ;

    MyGraphicRectItem *myRectItem_center = nullptr ;

    MyGraphicRectItem *myRectItem_corner1 = nullptr ;
    MyGraphicRectItem *myRectItem_corner2 = nullptr ;
    MyGraphicRectItem *myRectItem_corner3 = nullptr ;
    MyGraphicRectItem *myRectItem_corner4 = nullptr ;


    MyGraphicRectItem *myRectItem_caliperTool1 = nullptr ;
    MyGraphicRectItem *myRectItem_caliperTool2 = nullptr ;

    MyGraphicLineItem *myRectItem_caliperLine = nullptr ;


    QGridLayout *display_grid_layout = nullptr;

    int cam_id = 0;

    int work_mode = 0;//工作模式0为运行，1为训练

    bool exist_show_img = false;

    cv::Mat show_img;

    int select_simulation_img_id = -1; //仿真图片id


    void showCenterLine(int work_id);

    ///
    /// \brief show_display
    ///
    void show_display();

    ///
    /// \brief get_show_img
    ///
    void get_show_img();


    void updata_control_value();

    ///
    /// \brief clear_simulation_table
    ///
    void clear_simulation_table();

    ///
    /// \brief show_simulation_table
    ///
    void show_simulation_table();

private slots:

    void on_comboBox_cam_currentIndexChanged(int index);

    void on_comboBox_type_currentIndexChanged(int index);

    void on_pushButton_set_train_image_clicked();

    void on_pushButton_train_clicked();

    void on_pushButton_run_clicked();

    void change_roi_even(QGraphicsItem *item);


    void on_comboBox_location_caliper1_transition_currentIndexChanged(int index);

    void on_comboBox_location_caliper1_select_currentIndexChanged(int index);

    void on_spinBox_location_caliper1_thre_valueChanged(int arg1);

    void on_comboBox_location_caliper2_transition_currentIndexChanged(int index);

    void on_comboBox_location_caliper2_select_currentIndexChanged(int index);

    void on_spinBox_location_caliper2_thre_valueChanged(int arg1);

    void on_pushButton_save_clicked();

    void on_spinBox_canny_thre_min_valueChanged(int arg1);

    void on_spinBox_canny_thre_max_valueChanged(int arg1);

    void on_spinBox_chip_area_min_valueChanged(int arg1);

    void on_spinBox_chip_area_max_valueChanged(int arg1);

    void on_pushButton_getStandardPoint_clicked();

    void on_comboBox_resultType_currentIndexChanged(int index);

    void on_doubleSpinBox_xb_valueChanged(double arg1);

    void on_doubleSpinBox_yb_valueChanged(double arg1);

    void on_doubleSpinBox_rb_valueChanged(double arg1);

    void on_doubleSpinBox_xa_valueChanged(double arg1);

    void on_doubleSpinBox_ya_valueChanged(double arg1);

    void on_doubleSpinBox_ra_valueChanged(double arg1);

    void on_spinBox_centerLineD_valueChanged(int arg1);

    void on_comboBox_location_Politary_currentIndexChanged(int index);

    void on_doubleSpinBox_distance_thre_min_valueChanged(double arg1);

    void on_doubleSpinBox_distance_thre_max_valueChanged(double arg1);

    void on_lineEdit_simulation_path_textChanged(const QString &arg1);

    void on_pushButton_open_dir_clicked();

    void on_tableWidget_simualtion_filePath_itemSelectionChanged();

    void on_pushButton_run_last_clicked();

    void on_pushButton_run_next_clicked();

    void on_spinBox_location_caliper_roi_count_valueChanged(int arg1);

    void on_spinBox_location_caliper_roi_h_valueChanged(int arg1);

    void on_spinBox_location_caliper_roi_w_valueChanged(int arg1);

signals:
    void save_file_signal();


private:
    void closeEvent(QCloseEvent *event);

private:
    Ui::DialogToolPara *ui;
};

#endif // DIALOGTOOLPARA_H
