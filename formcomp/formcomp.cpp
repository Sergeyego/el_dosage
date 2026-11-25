#include "formcomp.h"
#include "ui_formcomp.h"

FormComp::FormComp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormComp)
{
    ui->setupUi(this);
    loadSettings();

    modelGost = new RestTableModel("comp_gost",this);
    ui->tableViewGost->setModel(modelGost);

    modelComp = new RestTableModel("matr",this);
    ui->tableViewComp->setModel(modelComp);

    mapper = new RestMapper(ui->tableViewComp,this);

    mapper->addMapping(ui->lineEditNam,"nam");
    mapper->addMapping(ui->comboBoxType,"id_type");
    mapper->addMapping(ui->lineEditKoef,"loss");
    mapper->addMapping(ui->lineEditOrder,"sort_order");
    mapper->addEmptyLock(ui->groupBox);
    mapper->addLock(ui->pushButtonUpd);

    ui->horizontalLayoutMap->insertWidget(0,mapper);

    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updData(int)));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(modelComp,SIGNAL(sigUpd()),RelModels::instance()->getModel("matr"),SLOT(refresh()));

    upd();
}

FormComp::~FormComp()
{
    saveSettings();
    delete ui;
}

void FormComp::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->ui->splitter->restoreState(settings.value("formcomp_splitter_width").toByteArray());
}

void FormComp::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("formcomp_splitter_width",ui->splitter->saveState());
}

void FormComp::upd()
{
    if (sender()==ui->pushButtonUpd){
        QVector<RestTableModel*> arrMod;
        arrMod.push_back(modelGost);
        arrMod.push_back(modelComp);
        RelModels::instance()->updateRels(arrMod);
    }
    modelComp->select();
}

void FormComp::updData(int index)
{
    if (index<0){
        modelGost->setDefaultValue("id_comp",-1);
        modelGost->clear();
        return;
    }
    int id_comp = mapper->modelData(index,"id").toInt();
    modelGost->setFilter(RestFilter::rule(modelGost->tableName(),"id_comp",RestFilter::Op::Eq,id_comp));
    modelGost->setDefaultValue("id_comp",id_comp);
    modelGost->select();
}
