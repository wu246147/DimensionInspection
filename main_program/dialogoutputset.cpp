#include "dialogoutputset.h"
#include "ui_dialogoutputset.h"
#include "qDebug"

DialogOutputSet::DialogOutputSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOutputSet)
{
    ui->setupUi(this);
}


DialogOutputSet::DialogOutputSet(JobManager *j, Communication *c, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogOutputSet)
{
    ui->setupUi(this);
    jobmanager = j;
    communication = c;
    is_loading = true;
    //    ui->spinBoxb
    ui->spinBox_station_1_delay->setValue(jobmanager->station_1_delay);
    ui->spinBox_station_2_delay->setValue(jobmanager->station_2_delay);
    ui->spinBox_output_delay->setValue(jobmanager->output_delay);
    //    for (int i = 0; i < jobmanager->jobworkers.size(); i++)
    for(int i = 0; i < 1; i++)
    {
        ui->comboBox_station_1_output_cam->addItem(("cam" + std::to_string(i + 1)).data());
    }
    ui->comboBox_station_1_output_cam->setCurrentIndex(jobmanager->station_1_output_cam);
    for(int i = 0; i < 3; i++)
    {
        ui->comboBox_station_1_output_line->addItem(("line" + std::to_string(i)).data());
    }
    ui->comboBox_station_1_output_line->setCurrentIndex(jobmanager->station_1_output_line);
    ui->checkBox_station_1_enable->setChecked(jobmanager->station_1_output_enable);
    ui->lineEdit_ip->setText(communication->plc_ip.data());
    ui->spinBox_port->setValue(communication->net_port);
    ui->lineEdit_ip_2->setText(jobmanager->jobworkers[1].plc_ip.data());
    ui->spinBox_port_2->setValue(jobmanager->jobworkers[1].net_port);
    is_loading = false;
    ui->spinBox_station_1_delay->setVisible(false);
    ui->label_station_1_delay->setVisible(false);
    ui->label_station_1_delay_value->setVisible(false);
    ui->spinBox_station_2_delay->setVisible(false);
    ui->label_station_2_delay->setVisible(false);
    ui->label_station_2_delay_value->setVisible(false);
    ui->pushButton_station_1_test->setVisible(false);
    ui->pushButton_station_2_test->setVisible(false);
    ui->checkBox_station_1_enable->setVisible(false);
    ui->label_station_1_output_cam->setVisible(false);
    ui->comboBox_station_1_output_cam->setVisible(false);
    ui->label_station_1_output_line->setVisible(false);
    ui->comboBox_station_1_output_line->setVisible(false);
    ui->label_output_delay->setVisible(false);
    ui->label_output_delay_value->setVisible(false);
    ui->spinBox_output_delay->setVisible(false);
    // ui->label_ip->setVisible(false);
    // ui->lineEdit_ip->setVisible(false);
    // ui->label_port->setVisible(false);
    // ui->spinBox_port->setVisible(false);
    ui->label_ip_2->setVisible(false);
    ui->lineEdit_ip_2->setVisible(false);
    ui->label_ip_3->setVisible(false);
    ui->spinBox_port_2->setVisible(false);
}

DialogOutputSet::~DialogOutputSet()
{
    delete ui;
}

void DialogOutputSet::on_spinBox_station_1_delay_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->station_1_delay = arg1;
}

void DialogOutputSet::on_spinBox_station_2_delay_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->station_2_delay = arg1;
}

void DialogOutputSet::on_spinBox_output_delay_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->output_delay = arg1;
}

void DialogOutputSet::on_pushButton_station_1_test_clicked()
{
    QFuture<void> fut_thread = QtConcurrent::run(jobmanager, &JobManager::station_1_output);
}

void DialogOutputSet::on_pushButton_station_2_test_clicked()
{
    QFuture<void> fut_thread = QtConcurrent::run(jobmanager, &JobManager::station_2_output);
}

void DialogOutputSet::on_checkBox_station_1_enable_clicked(bool checked)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->station_1_output_enable = checked;
}

void DialogOutputSet::on_comboBox_station_1_output_cam_currentIndexChanged(int index)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->station_1_output_cam = index;
}

void DialogOutputSet::on_comboBox_station_1_output_line_currentIndexChanged(int index)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->station_1_output_line = index;
}

void DialogOutputSet::on_lineEdit_ip_textChanged(const QString &arg1)
{
    if(is_loading)
    {
        return;
    }
    communication->plc_ip = arg1.toStdString();
    // qDebug() << "IP address changed to:" << arg1;
}


void DialogOutputSet::on_spinBox_port_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    communication->net_port = arg1;
}








void DialogOutputSet::on_lineEdit_ip_2_textChanged(const QString &arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[1].plc_ip = arg1.toStdString();
}


void DialogOutputSet::on_spinBox_port_2_valueChanged(int arg1)
{
    if(is_loading)
    {
        return;
    }
    jobmanager->jobworkers[1].net_port = arg1;
}

