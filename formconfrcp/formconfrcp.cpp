#include "formconfrcp.h"
#include "ui_formconfrcp.h"

FormConfRcp::FormConfRcp(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormConfRcp)
{
    ui->setupUi(this);

    modelRcpConf = new ModelRcpConf(this);
    ui->tableViewRcpConf->setModel(modelRcpConf);

    RestRelModel *markModel = RelModels::instance()->getModel("mark");
    ui->tableViewMark->setModel(markModel);
    ui->tableViewMark->setColumnHidden(0,true);
    ui->tableViewMark->resizeToContents();

    connect(markModel,SIGNAL(refreshFinished(QString)),ui->tableViewMark,SLOT(resizeToContents()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),markModel,SLOT(refresh()));
    connect(ui->tableViewMark->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updCont(QModelIndex)));
    connect(modelRcpConf,SIGNAL(refreshFinished()),ui->tableViewRcpConf,SLOT(resizeToContents()));
    connect(ui->pushButtonUpload,SIGNAL(clicked(bool)),modelRcpConf,SLOT(upload()));
    connect(ui->tableViewRcpConf,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(download(QModelIndex)));
    connect(ui->pushButtonRemove,SIGNAL(clicked(bool)),this,SLOT(removeRcp()));
}

FormConfRcp::~FormConfRcp()
{
    delete ui;
}

void FormConfRcp::updCont(QModelIndex index)
{
    int id_el = ui->tableViewMark->model()->data(ui->tableViewMark->model()->index(index.row(),0),Qt::EditRole).toInt();
    QString marka = ui->tableViewMark->model()->data(ui->tableViewMark->model()->index(index.row(),1),Qt::EditRole).toString();
    ui->labelMark->setText(tr("Утвержденные рецептуры для ")+marka+tr(":"));
    modelRcpConf->refresh(id_el);
}

void FormConfRcp::download(QModelIndex index)
{
    modelRcpConf->download(modelRcpConf->getFileName(index.row()));
}

void FormConfRcp::removeRcp()
{
    QModelIndex index = ui->tableViewRcpConf->currentIndex();
    modelRcpConf->remove(modelRcpConf->getFileName(index.row()));
}