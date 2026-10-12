#include "formedtresttbl.h"
#include "ui_formedtresttbl.h"

FormEdtRestTbl::FormEdtRestTbl(QString title, QString tableName, QString relName, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormEdtRestTbl)
{
    ui->setupUi(this);
    ui->labelTitle->setText(title);
    this->setWindowTitle(title);
    edtModel = new RestTableModel(tableName,this);
    ui->tableView->setModel(edtModel);
    if (!relName.isEmpty()){
        connect(edtModel,SIGNAL(sigUpd()),RelModels::instance()->getModel(relName),SLOT(refresh()));
    }
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updTbl()));
}

RestTableModel *FormEdtRestTbl::model()
{
    return edtModel;
}

FormEdtRestTbl::~FormEdtRestTbl()
{
    delete ui;
}

void FormEdtRestTbl::updTbl()
{
    QVector<RestTableModel*> arrModel;
    arrModel.push_back(edtModel);
    RelModels::instance()->updateRels(arrModel);
    edtModel->select();
}
