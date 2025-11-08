#include "dialogsimulationset.h"
#include "ui_dialogsimulationset.h"

DialogSimulationSet::DialogSimulationSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSimulationSet)
{
    ui->setupUi(this);
}

DialogSimulationSet::DialogSimulationSet(JobManager *j, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogSimulationSet)
{
    ui->setupUi(this);
    jobmanager = j;


    for(int i = 0; i < jobmanager->jobworkers.size(); i++)
    {
        ui->comboBox_cam->addItem(("jobworker" + std::to_string(i + 1)).data());
    }

    ui->comboBox_cam->setCurrentIndex(0);
}
DialogSimulationSet::~DialogSimulationSet()
{
    delete ui;
}

void DialogSimulationSet::on_comboBox_cam_currentIndexChanged(int index)
{
    cam_id = index;
    is_loading = true;

    ui->lineEdit_simulation_path->setText(jobmanager->jobworkers[cam_id].src_image_dir.data());
    is_loading = false;
}


void DialogSimulationSet::on_lineEdit_simulation_path_textChanged(const QString &arg1)
{

    if(is_loading)
    {
        return;
    }

    jobmanager->jobworkers[cam_id].src_image_dir = ui->lineEdit_simulation_path->text().toStdString();
}


void DialogSimulationSet::on_pushButton_open_dir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Simulation Image Directory"),
                  QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(NULL == dir)
    {
        return;
    }
    ui->lineEdit_simulation_path->setText(dir);
}


void DialogSimulationSet::on_lineEdit_simulation_path_editingFinished()
{
//    if(is_loading)
//    {
//        return;
//    }

//    jobmanager->jobworkers[cam_id].src_image_dir = ui->lineEdit_simulation_path->text().toStdString();

}

