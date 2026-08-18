#include "formedtresttbl.h"
#include "ui_formedtresttbl.h"

FormEdtRestTbl::FormEdtRestTbl(QString title, QString tableName, QString relName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormEdtRestTbl)
{
    ui->setupUi(this);
    ui->labelTitle->setText(title);
    this->setWindowTitle(title);
    model = new RestTableModel(tableName,this);
    ui->tableView->setModel(model);
    if (!relName.isEmpty()){
        connect(model,SIGNAL(sigUpd()),RelModels::instance()->getModel(relName),SLOT(refresh()));
    }
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updTbl()));
    model->select();
}

FormEdtRestTbl::~FormEdtRestTbl()
{
    delete ui;
}

void FormEdtRestTbl::updTbl()
{
    QVector<RestTableModel*> arrModel;
    arrModel.push_back(model);
    RelModels::instance()->updateRels(arrModel);
    model->select();
}
