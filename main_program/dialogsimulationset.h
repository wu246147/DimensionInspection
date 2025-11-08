#ifndef DIALOGSIMULATIONSET_H
#define DIALOGSIMULATIONSET_H

#include <QDialog>
#include "jobmanager.h"
#include <QFileDialog>

namespace Ui
{
    class DialogSimulationSet;
}

class DialogSimulationSet : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSimulationSet(QWidget *parent = nullptr);
    DialogSimulationSet(JobManager *j, QWidget *parent = nullptr);

    ~DialogSimulationSet();


public:
    JobManager *jobmanager = nullptr;
    bool is_loading = false;
    int cam_id = 0;


private slots:
    void on_comboBox_cam_currentIndexChanged(int index);

    void on_lineEdit_simulation_path_textChanged(const QString &arg1);

    void on_pushButton_open_dir_clicked();

    void on_lineEdit_simulation_path_editingFinished();

private:
    Ui::DialogSimulationSet *ui;
};

#endif // DIALOGSIMULATIONSET_H
