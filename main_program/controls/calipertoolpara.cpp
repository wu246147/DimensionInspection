#include "calipertoolpara.h"
#include "ui_calipertoolpara.h"

CaliperToolPara::CaliperToolPara(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CaliperToolPara)
{
    ui->setupUi(this);
}

CaliperToolPara::~CaliperToolPara()
{
    delete ui;
}
