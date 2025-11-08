#ifndef CALIPERTOOLPARA_H
#define CALIPERTOOLPARA_H

#include <QWidget>

namespace Ui {
class CaliperToolPara;
}

class CaliperToolPara : public QWidget
{
    Q_OBJECT

public:
    explicit CaliperToolPara(QWidget *parent = nullptr);
    ~CaliperToolPara();

private:
    Ui::CaliperToolPara *ui;
};

#endif // CALIPERTOOLPARA_H
