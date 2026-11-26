#include "formdoz.h"
#include "ui_formdoz.h"

FormDoz::FormDoz(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDoz)
{
    ui->setupUi(this);
    loadSettings();

    current_id=-1;

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    actionUpd = new QAction(this->style()->standardIcon(QStyle::SP_BrowserReload),tr("Обновить"),this);
    ui->toolButtonUpd->setDefaultAction(actionUpd);

    actionDel = new QAction(this->style()->standardIcon(QStyle::SP_TrashIcon),tr("Удалить"),this);
    ui->toolButtonDelete->setDefaultAction(actionDel);

    actionConfirm = new QAction(this->style()->standardIcon(QStyle::SP_DialogApplyButton),tr("Принять"),this);
    ui->toolButtonConfirm->setDefaultAction(actionConfirm);

    actionNew = new QAction(this->style()->standardIcon(QStyle::SP_FileIcon),tr("Новая"),this);
    ui->toolButtonNew->setDefaultAction(actionNew);

    actionSave = new QAction(this->style()->standardIcon(QStyle::SP_DialogSaveButton),tr("Сохранить в файл"),this);
    ui->toolButtonSave->setDefaultAction(actionSave);

    modelDoz = new ModelDoz(this);
    ui->tableViewDoz->setModel(modelDoz);

    modelDozData = new RestTableModel("dosage_spnd",this);
    ui->tableViewDozData->setModel(modelDozData);

    connect(ui->tableViewDoz->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updDozData(QModelIndex)));
    connect(modelDoz,SIGNAL(sigRefresh()),this,SLOT(updFinished()));

    connect(actionUpd,SIGNAL(triggered(bool)),this,SLOT(upd()));
    connect(actionDel,SIGNAL(triggered(bool)),this,SLOT(delDoz()));
    connect(actionConfirm,SIGNAL(triggered(bool)),this,SLOT(confirmDoz()));
    connect(actionNew,SIGNAL(triggered(bool)),this,SLOT(newDoz()));
    connect(actionSave,SIGNAL(triggered(bool)),this,SLOT(saveFile()));

    upd();
}

FormDoz::~FormDoz()
{
    saveSettings();
    delete ui;
}

void FormDoz::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->ui->splitter->restoreState(settings.value("doz_splitter_width").toByteArray());
}

void FormDoz::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("doz_splitter_width",ui->splitter->saveState());
}

void FormDoz::upd()
{
    modelDoz->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
}

void FormDoz::updFinished()
{
    if (modelDoz->rowCount()){
        int row = current_id>=0 ? modelDoz->findRowByKey(current_id,"id") : -1;
        if (row>=0){
            ui->tableViewDoz->selectRow(row);
        } else {
            ui->tableViewDoz->selectRow(ui->tableViewDoz->model()->rowCount()-1);
        }
    }
}

void FormDoz::updDozData(QModelIndex index)
{
    int id_doz=modelDoz->getModelData(index.row(),"id").toInt();
    current_id=id_doz;
    modelDozData->setFilter(RestFilter::rule(modelDozData->tableName(),"id_dos",RestFilter::Op::Eq,id_doz));
    modelDozData->setDefaultValue("id_dos",id_doz);
    modelDozData->select();
}

void FormDoz::delDoz()
{
    //modelDoz->removeRow(ui->tableViewDoz->currentIndex().row());
}

void FormDoz::confirmDoz()
{
    /*int row=ui->tableViewDoz->currentIndex().row();
    QString dat;
    for(int i=0; i<modelDoz->columnCount(); i++){
        if (!dat.isEmpty()){
            dat+=", ";
        }
        dat+=modelDoz->data(modelDoz->index(row,i),Qt::DisplayRole).toString();
    }
    int n=QMessageBox::question(this,tr("Принять дозировку"), tr("Принять дозировку ")+dat+tr("?"),QMessageBox::Yes| QMessageBox::No);
    if (n==QMessageBox::Yes){
        modelDoz->confirmDoz(row);
        modelDozData->select();
    }*/
}

void FormDoz::newDoz()
{
    DialogNew d;
    if (d.exec()==QDialog::Accepted){
        current_id=d.getNewId();
        upd();
        /*const int newId=d.getNewId();
        for(int i=ui->tableViewDoz->model()->rowCount()-1; i>=0; i--){
            QModelIndex ind=ui->tableViewDoz->model()->index(i,0);
            if (ui->tableViewDoz->model()->data(ind,Qt::EditRole).toInt()==newId){
                ui->tableViewDoz->selectRow(i);
            }
        }*/
    }
}

void FormDoz::saveFile()
{
    /*int row=ui->tableViewDoz->currentIndex().row();
    QString rcp=ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,3),Qt::DisplayRole).toString();
    QString dat=ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,1),Qt::EditRole).toDate().toString("dd.MM.yy");
    QString tim=ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,2),Qt::EditRole).toTime().toString("hh:mm");
    QString kvo=QString::number(ui->tableViewDoz->model()->data(ui->tableViewDoz->model()->index(row,5),Qt::EditRole).toDouble());
    DbXlsx s(ui->tableViewDozData,rcp+" "+dat+" "+tim+" "+kvo+tr(" кг"),this);
    s.saveToFile();*/
}

ModelDoz::ModelDoz(QWidget *parent) : RestRoTableModel(parent)
{

}

Qt::ItemFlags ModelDoz::flags(const QModelIndex &index) const
{
    if (index.column()==columnIndex("parti")){
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return RestRoTableModel::flags(index);
}

void ModelDoz::refresh(QDate beg, QDate end)
{
    this->setPath("api/elrtr/dosage/list/"+beg.toString("yyyy-MM-dd")+"/"+end.toString("yyyy-MM-dd"));
    select();
}

bool ModelDoz::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column() == this->columnIndex("parti")){
        QByteArray data;
        QJsonDocument doc;
        QJsonObject object
            {
                {"parti", value.toString()}
            };
        doc.setObject(object);
        QByteArray body=doc.toJson();
        int id = this->getModelData(index.row(),"id").toInt();
        bool ok = RestConnection::instance()->sendSyncRequest("api/elrtr/dosage/patch/"+QString::number(id),"PATCH",body,data);
        if (!ok){
            return false;
        }
    }
    return RestRoTableModel::setData(index,value,role);
}
