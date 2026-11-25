#ifndef FORMBACKFILL_H
#define FORMBACKFILL_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include "rest/resttablemodel.h"
#include "rest/restrotablemodel.h"
#include "formbackfill/formbackfill.h"

namespace Ui {
class FormBackfill;
}

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

private slots:
    void updStat();
    void updStatFinished();
    void updStatTime();
    void updLoad();
};

#endif // FORMBACKFILL_H
