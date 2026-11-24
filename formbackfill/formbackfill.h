#ifndef FORMBACKFILL_H
#define FORMBACKFILL_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include "rest/resttablemodel.h"
#include "rest/restrotablemodel.h"
#include "formbackfill/formbackfill.h"
#include "progressreportdialog/progressreportdialog.h"

namespace Ui {
class FormBackfill;
}

/*class ModelStatBunk: public ModelRo
{
public:
    ModelStatBunk(QObject *parent = nullptr);
    void refresh(QDateTime datetime, int id_cex);
};

class ModelLoadBunk : public DbTableModel
{
public:
    ModelLoadBunk(QObject *parent = nullptr);
    void refresh(QDate beg, QDate end, int id_cex);
};*/

class FormBackfill : public QWidget
{
    Q_OBJECT

public:
    explicit FormBackfill(QWidget *parent = nullptr);
    ~FormBackfill();

private:
    Ui::FormBackfill *ui;
    RestRoTableModel *modelStatBunk;
    RestTableModel *modelLoadBunk;
    QSortFilterProxyModel *modelBunk;
    QSortFilterProxyModel *modelPart;
    ProgressReportDialog *prd;

private slots:
    void updStat();
    void updStatTime();
    void updLoad();
    void save();
    void updPart();
};

#endif // FORMBACKFILL_H
