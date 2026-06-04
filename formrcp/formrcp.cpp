#include "formrcp.h"
#include "ui_formrcp.h"

FormRcp::FormRcp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormRcp)
{
    ui->setupUi(this);
}

FormRcp::~FormRcp()
{
    delete ui;
}
