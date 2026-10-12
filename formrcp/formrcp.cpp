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

    ui->comboBoxDiam->setModel(RelModels::instance()->getModel("el_diam"));
    colVal cDim;
    cDim.val=6;
    ui->comboBoxDiam->setCurrentData(cDim);
    ui->comboBoxDiam->setEditable(false);

    ui->comboBoxLel->setModel(RelModels::instance()->getModel("el_long"));
    ui->comboBoxGlass->setModel(RelModels::instance()->getModel("glass"));
    ui->comboBoxProv->setModel(RelModels::instance()->getModel("provol"));

    modelMode = new RestRoTableModel(this);

    modelCalc = new RestRoTableModel(this);
    ui->tableViewCalc->setModel(modelCalc);

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
    mapper->addEmptyLock(ui->comboBoxDiam);
    mapper->addEmptyLock(ui->pushButtonReCalc);
    mapper->addEmptyLock(ui->lineEditKfmp);
    mapper->addEmptyLock(ui->comboBoxLel);
    mapper->addEmptyLock(ui->lineEditFil);
    mapper->addEmptyLock(ui->comboBoxGlass);
    mapper->addEmptyLock(ui->comboBoxProv);

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
    connect(modelMode,SIGNAL(sigRefresh()),this,SLOT(onUpdModeFinisfhed()));
    connect(ui->comboBoxDiam,SIGNAL(currentIndexChanged(int)),this,SLOT(updMode()));
    connect(ui->pushButtonReCalc,SIGNAL(clicked(bool)),this,SLOT(updCacl()));
    connect(modelRcpData,SIGNAL(sigUpd()),this,SLOT(updCacl()));
    connect(ui->pushButtonXlsx,SIGNAL(clicked(bool)),this,SLOT(createXlsx()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));

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
    if (sender()==ui->pushButtonUpd){
        RelModels::instance()->updateAllRels();
    }

    colVal id_el=ui->comboBoxMarkOnly->getCurrentData();

    RestFilter filter;

    RestFilter idElFilter = RestFilter::rule(modelRcp->tableName(),"id_el",RestFilter::Op::Eq,id_el.val.toInt());

    if (id_el.val.toInt()>0){
        filter=idElFilter;
    }
    QJsonArray levs;
    if (ui->checkBoxOld->isChecked()){
        levs.append(0);
    }
    if (ui->checkBoxOk->isChecked()){
        levs.append(1);
    }
    if (ui->checkBoxTest->isChecked()){
        levs.append(2);
    }
    if (ui->checkBoxBad->isChecked()){
        levs.append(3);
    }
    if (levs.size()){
        RestFilter levFilter = RestFilter::ruleIn(modelRcp->tableName(),"lev",levs);
        if (id_el.val.toInt()>0){
            filter=RestFilter(RestFilter::Group::And);
            filter.add(idElFilter);
            filter.add(levFilter);
        } else {
            filter = levFilter;
        }

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
    modelRcpData->setFilter(RestFilter::rule(modelRcpData->tableName(),"id_rcp",RestFilter::Op::Eq,id_rcp));
    modelRcpData->setDefaultValue("id_rcp",id_rcp);
    modelRcpData->select();
    modelRcpConf->refresh(id_rcp);

    updMode();
}

void FormRcp::updMode()
{
    int id_el=mapper->modelData(mapper->currentIndex(),"id_el").toInt();
    int id_diam=ui->comboBoxDiam->getCurrentData().val.toInt();

    modelMode->setPath("api/elrtr/dosage/mode/"+QString::number(id_el)+"/"+QString::number(id_diam));
    modelMode->select();
}

QUrlQuery FormRcp::getRcpQuery()
{
    QUrlQuery query;
    query.addQueryItem("provol",ui->comboBoxProv->currentText());
    query.addQueryItem("glass",ui->comboBoxGlass->currentText());
    query.addQueryItem("lel",ui->comboBoxLel->currentText());
    query.addQueryItem("kfmp",ui->lineEditKfmp->text());
    query.addQueryItem("marka",ui->comboBoxMark->currentText());
    query.addQueryItem("diam",ui->comboBoxDiam->currentText());
    query.addQueryItem("dfil",ui->lineEditFil->text());
    return query;
}

void FormRcp::updCacl()
{
    QVariant id_rcp = mapper->modelData(mapper->currentIndex(),"id");
    if (id_rcp.isNull()){
        modelCalc->clear();
    } else {
        modelCalc->setPath("api/elrtr/dosage/calc/"+id_rcp.toString()+"?"+getRcpQuery().toString());
        modelCalc->select();
    }
}

void FormRcp::createXlsx()
{
    QVariant id_rcp = mapper->modelData(mapper->currentIndex(),"id");
    if (!id_rcp.isNull()){
        QUrl url(RestConnection::instance()->getUrl()+"/api/elrtr/dosage/xlsx/"+id_rcp.toString()+"?"+getRcpQuery().toString());
        QNetworkReply *reply = RestConnection::instance()->sendGet(url);
        connect(reply,SIGNAL(finished()),this,SLOT(onCreateXlsxFinished()));
    }
}

void FormRcp::onCreateXlsxFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply){
        return;
    }
    if (reply->error()!=QNetworkReply::NoError){
        QMessageBox::critical(this,tr("Ошибка"),reply->errorString()+"\n"+reply->readAll(),QMessageBox::Cancel);
    } else {
        TempFileManager::instance().createFile(reply->readAll(),"xlsx",true);
    }
    reply->deleteLater();
}

void FormRcp::onUpdModeFinisfhed()
{
    ui->lineEditKfmp->setText(modelMode->getModelData(0,"kfmp").toString());

    colVal el_long;
    el_long.val=modelMode->getModelData(0,"id_long").toInt();
    ui->comboBoxLel->setCurrentData(el_long);

    ui->lineEditFil->setText(modelMode->getModelData(0,"dfil").toString());

    colVal id_glyb;
    id_glyb.val=modelMode->getModelData(0,"id_glyb").toInt();
    ui->comboBoxGlass->setCurrentData(id_glyb);

    colVal id_prov;
    id_prov.val=modelMode->getModelData(0,"id_prov").toInt();
    ui->comboBoxProv->setCurrentData(id_prov);

    updCacl();
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
    id_copy=modelRcp->getModelData(mapper->currentIndex(),"id").toInt();
    ui->pushButtonPaste->setEnabled(id_copy > 0);
}

void FormRcp::paste()
{
    if (id_copy <= 0) {
        QMessageBox::warning(this, tr("Вставка"),tr("Сначала нажмите «Взять состав» на нужной рецептуре."));
        return;
    }
    QByteArray body, data;
    int id_rcp = mapper->modelData(mapper->currentIndex(),"id").toInt();
    RestConnection::instance()->sendSyncRequest("api/elrtr/dosage/paste/"+QString::number(id_copy)+"/"+QString::number(id_rcp),"POST",body,data);
    modelRcpData->select();
    updCacl();
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
