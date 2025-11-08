#ifndef DIALOGROBOTACQ_H
#define DIALOGROBOTACQ_H

#include <QDialog>
#include "jobmanager.h"
#include "tool/projecttool.h"
#include "tool/communicationmodbus.h"
#include "tool/communicationcom.h"

namespace Ui
{
    class DialogRobotAcq;
}

class DialogRobotAcq : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRobotAcq(QWidget *parent = nullptr);
    ~DialogRobotAcq();
    DialogRobotAcq(JobManager *j, CommunicationModbus *c, COM *com, QWidget *parent = nullptr);

    JobManager *jobmanager = nullptr;
    //机器人通讯管理对象
    CommunicationModbus *communicationModbus = nullptr;
    //光源通讯管理对象
    COM *communicationCOM = nullptr;

    bool is_loading = true;
    int cam_id = 0;

    std::vector<float>centerPoints;

    std::vector<float>movePoints;

    std::vector<float>robotPoints;


private slots:
    void on_pushButton_acq_clicked();

    void on_pushButton_read_coordinates_clicked();

    void on_pushButton_move_and_acq_clicked();

    void on_pushButton_save_data_clicked();

private:
    Ui::DialogRobotAcq *ui;
};

#endif // DIALOGROBOTACQ_H
