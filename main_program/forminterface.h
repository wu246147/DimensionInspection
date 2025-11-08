#ifndef FORMINTERFACE_H
#define FORMINTERFACE_H

#include <QWidget>
#include <QtConcurrent>
#include "jobmanager.h"
#include "SharePara.h"

namespace Ui
{
    class FormInterface;
}

class FormInterface : public QWidget
{
    Q_OBJECT

public:
    explicit FormInterface(QWidget *parent = nullptr);
    explicit FormInterface(JobManager *j, QWidget *parent = nullptr);

    ~FormInterface();



    JobManager *jobmanager;

    QGridLayout *display_grid_layout = nullptr;

    int cam_id = 0;

    std::vector<int> tmp_src_img_id_list;

    void init();

    void setDisplay();

    void changeDisplay();

    void get_files(std::string dir, std::vector<std::string> *file_paths);

    void updateDisplay();

private slots:
    void on_pushButton_changeDisplay_clicked();

    void on_pushButton_sim_clicked();

    void on_pushButton_updateDisplay_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::FormInterface *ui;
};

#endif // FORMINTERFACE_H
