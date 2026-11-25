#include "formbackfill.h"
#include "ui_formbackfill.h"

FormBackfill::FormBackfill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBackfill)
{
    ui->setupUi(this);
    ui->dateTimeEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    ui->pushButtonUpd->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pushButtonUpdStat->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pushButtonUpdStatTime->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

    ui->comboBoxCex->setModel(RelModels::instance()->getModel("bunk_cex"));
    colVal curCex;
    curCex.val=1;
    ui->comboBoxCex->setCurrentData(curCex);

    modelLoadBunk = new RestTableModel("bunk_comp",this);
    ui->tableViewLoad->setModel(modelLoadBunk);
    ui->comboBoxCex->setEditable(false);

    modelStatBunk = new RestRoTableModel(this);

    modelBunk = new QSortFilterProxyModel(this);
    modelBunk->setSourceModel(modelStatBunk);
    modelBunk->setFilterKeyColumn(0);
    modelBunk->setFilterRole(Qt::EditRole);
    modelBunk->setFilterFixedString("0");

    modelPart = new QSortFilterProxyModel(this);
    modelPart->setSourceModel(modelStatBunk);
    modelPart->setFilterKeyColumn(0);
    modelPart->setFilterRole(Qt::EditRole);
    modelPart->setFilterFixedString("1");

    ui->tableViewBunk->setModel(modelBunk);
    ui->tableViewPart->setModel(modelPart);

    connect(ui->pushButtonUpdStatTime,SIGNAL(clicked(bool)),this,SLOT(updStatTime()));
    connect(ui->pushButtonUpdStat,SIGNAL(clicked(bool)),this,SLOT(updStat()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(updLoad()));
    connect(modelLoadBunk,SIGNAL(sigUpd()),this,SLOT(updStat()));
    connect(ui->comboBoxCex,SIGNAL(currentIndexChanged(int)),this,SLOT(updLoad()));
    connect(modelStatBunk,SIGNAL(sigRefresh()),this,SLOT(updStatFinished()));

    updLoad();
}

FormBackfill::~FormBackfill()
{
    delete ui;
}

void FormBackfill::updStat()
{
    int id_cex=ui->comboBoxCex->getCurrentData().val.toInt();
    QUrlQuery query;
    query.addQueryItem("dtm",ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd HH:mm:ss"));
    modelStatBunk->setPath("api/elrtr/dosage/comp/bunk/stat/"+QString::number(id_cex)+"?"+query.toString());
    modelStatBunk->select();
}

void FormBackfill::updStatFinished()
{
    ui->tableViewBunk->setColumnHidden(0,true);
    ui->tableViewPart->setColumnHidden(0,true);
    ui->tableViewPart->setColumnHidden(1,true);
    ui->tableViewBunk->resizeToContents();
    ui->tableViewPart->resizeToContents();
}

void FormBackfill::updStatTime()
{
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    updStat();
}

void FormBackfill::updLoad()
{
    if (sender()==ui->pushButtonUpd){
        QVector<RestTableModel*> arrMod;
        arrMod.push_back(modelLoadBunk);
        RelModels::instance()->updateRels(arrMod);
    }

    int id_cex=ui->comboBoxCex->getCurrentData().val.toInt();
    RestFilter filter(RestFilter::Group::And);
    filter.addRule(modelLoadBunk->tableName(),"id_cex",RestFilter::Op::Eq,id_cex);
    filter.add(RestFilter::ruleBetween(modelLoadBunk->tableName(),"dat",ui->dateEditBeg->date().toString("yyyy-MM-dd"),ui->dateEditEnd->date().toString("yyyy-MM-dd")));

    QTime tm = QTime::currentTime();
    modelLoadBunk->setFilter(filter);
    modelLoadBunk->setDefaultValue("id_cex",id_cex);
    modelLoadBunk->setDefaultValue("dat",QDate::currentDate());
    modelLoadBunk->setDefaultValue("tm",QTime(tm.hour(),tm.minute()));
    modelLoadBunk->select();

    updStat();
}

