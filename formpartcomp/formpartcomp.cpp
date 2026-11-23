#include "formpartcomp.h"
#include "ui_formpartcomp.h"

FormPartComp::FormPartComp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormPartComp)
{
    ui->setupUi(this);

    ui->dateEditBeg->setDate(QDate(QDate::currentDate().year(),1,1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    modelPart = new RestRoTableModel(this);
    ui->tableViewPart->setModel(modelPart);

    upd();
}

FormPartComp::~FormPartComp()
{
    delete ui;
}

void FormPartComp::upd()
{
    modelPart->setPath("api/elrtr/dosage/comp/parti/list/"+ui->dateEditBeg->date().toString("yyyy-MM-dd")+"/"+ui->dateEditEnd->date().toString("yyyy-MM-dd"));
    modelPart->select();
}
