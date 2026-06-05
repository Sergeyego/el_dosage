#include "formrcp.h"
#include "ui_formrcp.h"

FormRcp::FormRcp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormRcp)
{
    ui->setupUi(this);

    ui->comboBoxMarkOnly->setModel(RelModels::instance()->getModel("mark"));
    colVal cMark;
    cMark.val=-1;
    ui->comboBoxMarkOnly->setCurrentData(cMark);

    modelRcp = new RestTableModel("rcp_nam",this);
    modelRcp->setPath("api/elrtr/dosage/rcp");
    ui->tableViewRcp->setModel(modelRcp);

    mapper = new RestMapper(ui->tableViewRcp,this);
    mapper->addMapping(ui->comboBoxLev,"lev");
    mapper->addMapping(ui->lineEditNam,"nam");
    mapper->addMapping(ui->comboBoxMark,"id_el");

    ui->horizontalLayoutMapper->insertWidget(0,mapper);

    connect(ui->comboBoxMarkOnly,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->comboBoxMarkOnly->lineEdit(),SIGNAL(textChanged(QString)),this,SLOT(markTextChanged()));

    upd();
}

FormRcp::~FormRcp()
{
    delete ui;
}

void FormRcp::upd()
{
    QString filter="";
    colVal id_el=ui->comboBoxMarkOnly->getCurrentData();
    if (id_el.val.toInt()>0){
        filter+=modelRcp->tableName()+".id_el = "+id_el.val.toString();
    }
    /*bool b0=ui->checkBoxOld->isChecked();
    bool b1=ui->checkBoxOk->isChecked();
    bool b2=ui->checkBoxTest->isChecked();
    bool b3=ui->checkBoxBad->isChecked();
    if (b0||b1||b2||b3){
        if (!filter.isEmpty()){
            filter+=" and ";
        }
        filter+=modelRcp->tableName()+".lev in (";
        filter+=")";
    }*/
    modelRcp->setFilter(filter);
    modelRcp->select();
}

void FormRcp::markTextChanged()
{
    if (ui->comboBoxMarkOnly->currentText().isEmpty()){
        colVal cMark;
        cMark.val=-1;
        ui->comboBoxMarkOnly->setCurrentData(cMark);
    }
}
