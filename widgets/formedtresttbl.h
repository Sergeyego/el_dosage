#ifndef FORMEDTRESTTBL_H
#define FORMEDTRESTTBL_H

#include <QWidget>
#include "rest/resttablemodel.h"
#include "rest/relmodels.h"

namespace Ui {
class FormEdtRestTbl;
}

class FormEdtRestTbl : public QWidget
{
    Q_OBJECT

public:
    explicit FormEdtRestTbl(QString title, QString tableName, QString relName = QString(), QWidget *parent = nullptr);
    ~FormEdtRestTbl();

private:
    Ui::FormEdtRestTbl *ui;
    RestTableModel *model;

private slots:
    void updTbl();
};

#endif // FORMEDTRESTTBL_H
