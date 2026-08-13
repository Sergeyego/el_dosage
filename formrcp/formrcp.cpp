#include "formrcp.h"
#include "ui_formrcp.h"

FormRcp::FormRcp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormRcp)
{
    ui->setupUi(this);
    id_copy=0;

    ui->comboBoxMarkOnly->setModel(RelModels::instance()->getModel("mark"));
    colVal cMark;
    cMark.val=-1;
    ui->comboBoxMarkOnly->setCurrentData(cMark);

    modelRcp = new RestTableModel("rcp_nam",this);
    modelRcp->setPath("api/elrtr/dosage/rcp");
    ui->tableViewRcp->setModel(modelRcp);

    modelRcpData = new RestTableModel("rcp_cont",this);
    ui->tableViewData->setModel(modelRcpData);

    mapper = new RestMapper(ui->tableViewRcp,this);
    mapper->addMapping(ui->comboBoxLev,"lev");
    mapper->addMapping(ui->lineEditNam,"nam");
    mapper->addMapping(ui->comboBoxMark,"id_el");

    mapper->addLock(ui->comboBoxMarkOnly);
    mapper->addLock(ui->checkBoxBad);
    mapper->addLock(ui->checkBoxOk);
    mapper->addLock(ui->checkBoxOld);
    mapper->addLock(ui->checkBoxTest);
    mapper->addEmptyLock(ui->tableViewData);
    mapper->addEmptyLock(ui->pushButtonCopy);
    mapper->addEmptyLock(ui->pushButtonPaste);
    mapper->addEmptyLock(ui->tableViewConf);
    mapper->addEmptyLock(ui->pushButtonXlsx);
    mapper->addEmptyLock(ui->pushButtonAddConf);

    ui->horizontalLayoutMapper->insertWidget(0,mapper);

    modelRcpConf = new ModelRcpConf(this);
    ui->tableViewConf->setModel(modelRcpConf);
    ui->tableViewConf->setColumnHidden(3,true);
    ui->tableViewConf->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->comboBoxMarkOnly,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->comboBoxMarkOnly->lineEdit(),SIGNAL(textChanged(QString)),this,SLOT(markTextChanged()));
    connect(ui->checkBoxOld,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->checkBoxOk,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->checkBoxTest,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->checkBoxBad,SIGNAL(clicked(bool)),this,SLOT(upd()));

    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(updData(int)));
    connect(modelRcpData,SIGNAL(sigRefresh()),this,SLOT(calcSum()));
    connect(modelRcpData,SIGNAL(sigUpd()),this,SLOT(calcSum()));
    connect(ui->pushButtonCopy,SIGNAL(clicked(bool)),this,SLOT(copy()));
    connect(ui->pushButtonPaste,SIGNAL(clicked(bool)),this,SLOT(paste()));
    connect(ui->pushButtonAddConf,SIGNAL(clicked(bool)),modelRcpConf,SLOT(upload()));
    connect(ui->tableViewConf,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(downloadRcpConf(QModelIndex)));
    connect(modelRcpConf,SIGNAL(refreshFinished()),ui->tableViewConf,SLOT(resizeColumnsToContents()));
    connect(ui->tableViewConf,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onRcpConfContextMenuRequested(QPoint)));

    loadSettings();

    upd();
}

FormRcp::~FormRcp()
{
    saveSettings();
    delete ui;
}

void FormRcp::upd()
{
    //qDebug()<<"upd!";
    QString filter="";
    colVal id_el=ui->comboBoxMarkOnly->getCurrentData();
    if (id_el.val.toInt()>0){
        filter+=modelRcp->tableName()+".id_el = "+id_el.val.toString();
    }
    QSet <int> levs;
    if (ui->checkBoxOld->isChecked()){
        levs.insert(0);
    }
    if (ui->checkBoxOk->isChecked()){
        levs.insert(1);
    }
    if (ui->checkBoxTest->isChecked()){
        levs.insert(2);
    }
    if (ui->checkBoxBad->isChecked()){
        levs.insert(3);
    }
    if (levs.size()){
        if (!filter.isEmpty()){
            filter+=" and ";
        }
        QString flev;
        for (const int lev : levs){
            if (!flev.isEmpty()){
                flev+=", ";
            }
            flev+=QString::number(lev);
        }
        filter+=modelRcp->tableName()+".lev in ("+flev+")";
    }
    modelRcp->setFilter(filter);
    modelRcp->select();
}

void FormRcp::downloadRcpConf(QModelIndex index)
{
    modelRcpConf->download(modelRcpConf->getFileName(index.row()));
}

void FormRcp::onRcpConfContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->tableViewConf->indexAt(pos);
    QMenu contextMenu(this);

    QAction *showAction = contextMenu.addAction("Показать");
    contextMenu.addSeparator();
    QAction *addAction = contextMenu.addAction("Добавить");
    contextMenu.addSeparator();
    QAction *refreshAction = contextMenu.addAction("Обновить");
    contextMenu.addSeparator();
    QAction *deleteAction = contextMenu.addAction("Удалить");

    if (!index.isValid()) {
        showAction->setEnabled(false);
        deleteAction->setEnabled(false);
    }

    QPoint globalPos = ui->tableViewConf->viewport()->mapToGlobal(pos);
    QAction *selectedAction = contextMenu.exec(globalPos);

    if (selectedAction == showAction) {
        modelRcpConf->download(modelRcpConf->getFileName(index.row()));
    } if (selectedAction == addAction) {
        modelRcpConf->upload();
    } else if (selectedAction == deleteAction) {
        modelRcpConf->remove(modelRcpConf->getFileName(index.row()));
    } else if (selectedAction == refreshAction) {

    }
}

void FormRcp::markTextChanged()
{
    int currentPos=ui->comboBoxMarkOnly->getCurrentData().val.toInt();
    //qDebug()<<"textCh"<<currentPos;
    if (ui->comboBoxMarkOnly->currentText().isEmpty() && currentPos>=0){
        colVal cMark;
        cMark.val=-1;
        ui->comboBoxMarkOnly->setCurrentData(cMark);
    }
}

void FormRcp::updData(int index)
{
    int id_rcp=mapper->modelData(index,"id").isNull() ? -1 : mapper->modelData(index,"id").toInt();
    modelRcpData->setFilter(modelRcpData->tableName()+".id_rcp="+QString::number(id_rcp));
    modelRcpData->setDefaultValue("id_rcp",id_rcp);
    modelRcpData->select();
    modelRcpConf->refresh(id_rcp);
}

void FormRcp::calcSum()
{
    double sum=0.0;
    for (int i=0; i<modelRcpData->rowCount(); i++){
        sum+=modelRcpData->getModelData(i,"kvo").toDouble();
    }
    ui->lineEditItogo->setText(QLocale().toString(sum,'f',2));
}

void FormRcp::copy()
{
    ui->pushButtonPaste->setEnabled(true);
    id_copy=modelRcp->getModelData(mapper->currentIndex(),"id").toInt();
}

void FormRcp::paste()
{
    QByteArray body, data;
    int id_rcp = mapper->modelData(mapper->currentIndex(),"id").toInt();
    RestConnection::instance()->sendSyncRequest("api/elrtr/dosage/paste/"+QString::number(id_copy)+"/"+QString::number(id_rcp),"POST",body,data);
    modelRcpData->select();
}

void FormRcp::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("rcp_splitter_state").toByteArray());
    ui->checkBoxOld->setChecked(settings.value("rcp_ckeck_old").toBool());
    ui->checkBoxOk->setChecked(settings.value("rcp_ckeck_ok").toBool());
    ui->checkBoxTest->setChecked(settings.value("rcp_ckeck_test").toBool());
    ui->checkBoxBad->setChecked(settings.value("rcp_ckeck_bad").toBool());
}

void FormRcp::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("rcp_splitter_state", ui->splitter->saveState());
    settings.setValue("rcp_ckeck_old",ui->checkBoxOld->isChecked());
    settings.setValue("rcp_ckeck_ok",ui->checkBoxOk->isChecked());
    settings.setValue("rcp_ckeck_test",ui->checkBoxTest->isChecked());
    settings.setValue("rcp_ckeck_bad",ui->checkBoxBad->isChecked());
}
