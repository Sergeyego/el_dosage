#include "formbackfillhist.h"
#include "ui_formbackfillhist.h"

FormBackfillHist::FormBackfillHist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBackfillHist)
{
    ui->setupUi(this);
    ui->dateTimeEdit->setDate(QDate::currentDate().addDays(-365));
    ui->dateTimeEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);

    current_id=-1;

    ui->comboBoxCex->setModel(RelModels::instance()->getModel("bunk_cex"));
    colVal curCex;
    curCex.val=1;
    ui->comboBoxCex->setCurrentData(curCex);

    modelComp=RelModels::instance()->getModel("matr");
    ui->tableViewComp->setModel(modelComp);
    ui->tableViewComp->setColumnHidden(0,true);
    ui->tableViewComp->setColumnWidth(1,270);

    modelHist = new RestRoTableModel(this);
    ui->tableViewHist->setModel(modelHist);

    connect(ui->tableViewComp->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updHist()));
    connect(ui->dateTimeEdit,SIGNAL(dateTimeChanged(QDateTime)),this,SLOT(updHist()));
    connect(ui->comboBoxCex,SIGNAL(currentIndexChanged(int)),this,SLOT(updHist()));
    connect(modelComp,SIGNAL(refreshFinished(QString)),this,SLOT(updCompFinished()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),modelComp,SLOT(refresh()));

    updCompFinished();
}

FormBackfillHist::~FormBackfillHist()
{
    delete ui;
}

void FormBackfillHist::updHist()
{
    QModelIndex indComp=ui->tableViewComp->model()->index(ui->tableViewComp->currentIndex().row(),0);
    if (indComp.row()<0){
        modelHist->clear();
        return;
    }
    int id_comp=ui->tableViewComp->model()->data(indComp,Qt::EditRole).toInt();
    int id_cex=ui->comboBoxCex->getCurrentData().val.toInt();
    current_id=id_comp;
    QUrlQuery query;
    query.addQueryItem("dtm",ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd HH:mm:ss"));
    modelHist->setPath("api/elrtr/dosage/comp/bunk/history/"+QString::number(id_cex)+"/"+QString::number(id_comp)+"?"+query.toString());
    modelHist->select();
}

void FormBackfillHist::updCompFinished()
{
    if (ui->tableViewComp->model()->rowCount()){
        int row = current_id>=0 ? modelComp->findRowByKey(current_id) : -1;
        if (row>=0){
            ui->tableViewComp->selectRow(row);
        } else {
            ui->tableViewComp->selectRow(0);
        }
    }
}
