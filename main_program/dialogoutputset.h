#ifndef DIALOGOUTPUTSET_H
#define DIALOGOUTPUTSET_H

#include <QDialog>
#include <QThread>
#include <QtConcurrent>

#include "jobmanager.h"
#include "tool/communication.h"
#include "mainwindow.h"

namespace Ui
{
    class DialogOutputSet;
}

class DialogOutputSet : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOutputSet(QWidget *parent = nullptr);
    DialogOutputSet(JobManager *jobManager, Communication *c, QWidget *parent = nullptr);

    ~DialogOutputSet();


public:
    JobManager *jobmanager = nullptr;
    Communication *communication = nullptr;
    bool is_loading = false;


private slots:
    void on_spinBox_station_1_delay_valueChanged(int arg1);

    void on_spinBox_station_2_delay_valueChanged(int arg1);

    void on_spinBox_output_delay_valueChanged(int arg1);

    void on_pushButton_station_1_test_clicked();

    void on_pushButton_station_2_test_clicked();

    void on_checkBox_station_1_enable_clicked(bool checked);

    void on_comboBox_station_1_output_cam_currentIndexChanged(int index);

    void on_comboBox_station_1_output_line_currentIndexChanged(int index);

    void on_lineEdit_ip_textChanged(const QString &arg1);

    void on_spinBox_port_valueChanged(int arg1);



    void on_lineEdit_ip_2_textChanged(const QString &arg1);

    void on_spinBox_port_2_valueChanged(int arg1);

private:
    Ui::DialogOutputSet *ui;
};

#endif // DIALOGOUTPUTSET_H
