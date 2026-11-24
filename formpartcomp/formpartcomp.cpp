#include "formpartcomp.h"
#include "ui_formpartcomp.h"

FormPartComp::FormPartComp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormPartComp)
{
    ui->setupUi(this);
    loadSettings();

    current_id_part=-1;

    ui->dateEditBeg->setDate(QDate(QDate::currentDate().year(),1,1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    modelChemCert = new RestTableModel("comp_chem",this);
    modelChemCert->setPath("api/elrtr/dosage/comp/chem");
    ui->tableViewChemCert->setModel(modelChemCert);

    modelChem = new RestTableModel("comp_test",this);
    modelChem->setPath("api/elrtr/dosage/comp/chemtest");
    ui->tableViewChem->setModel(modelChem);

    modelPart = new RestRoTableModel(this);
    ui->tableViewPart->setModel(modelPart);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updData(QModelIndex)));
    connect(modelPart,SIGNAL(sigRefresh()),this,SLOT(updFinished()));

    upd();
}

FormPartComp::~FormPartComp()
{
    saveSettings();
    delete ui;
}

void FormPartComp::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->ui->splitter->restoreState(settings.value("formpartcomp_splitter_width").toByteArray());
}

void FormPartComp::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("formpartcomp_splitter_width",ui->splitter->saveState());
}

void FormPartComp::upd()
{
    if (sender()==ui->pushButtonUpd){
        QVector<RestTableModel*> arrMod;
        arrMod.push_back(modelChemCert);
        arrMod.push_back(modelChem);
        RelModels::instance()->updateRels(arrMod);
    }
    modelPart->setPath("api/elrtr/dosage/comp/parti/list/"+ui->dateEditBeg->date().toString("yyyy-MM-dd")+"/"+ui->dateEditEnd->date().toString("yyyy-MM-dd"));
    modelPart->select();
}

void FormPartComp::updFinished()
{
    bool ok = modelPart->rowCount();
    if (ok){
        int row = modelPart->findRowByKey(current_id_part,"id");
        if (row>=0){
            ui->tableViewPart->selectRow(row);
        } else {
            ui->tableViewPart->selectRow(modelPart->rowCount()-1);
        }
    } else {
        modelChemCert->setDefaultValue("id_part",-1);
        modelChemCert->clear();

        modelChem->setDefaultValue("id_part",-1);
        modelChem->clear();
    }
    ui->tableViewChemCert->setEnabled(ok);
    ui->tableViewChem->setEnabled(ok);
}

void FormPartComp::updData(QModelIndex index)
{
    if (index.row()<1){
        modelChemCert->setDefaultValue("id_part",-1);
        modelChemCert->clear();

        modelChem->setDefaultValue("id_part",-1);
        modelChem->clear();

        return;
    }

    int id_part = modelPart->getModelData(index.row(),"id").toInt();
    current_id_part=id_part;

    modelChemCert->setFilter(RestFilter::rule(modelChemCert->tableName(),"id_part",RestFilter::Op::Eq,id_part));
    modelChemCert->setDefaultValue("id_part",id_part);
    modelChemCert->select();

    modelChem->setFilter(RestFilter::rule(modelChem->tableName(),"id_part",RestFilter::Op::Eq,id_part));
    modelChem->setDefaultValue("id_part",id_part);
    modelChem->select();
}
