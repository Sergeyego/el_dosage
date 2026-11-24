#ifndef FORMPARTCOMP_H
#define FORMPARTCOMP_H

#include <QWidget>
#include "rest/restrotablemodel.h"
#include "rest/resttablemodel.h"

namespace Ui {
class FormPartComp;
}

class FormPartComp : public QWidget
{
    Q_OBJECT

public:
    explicit FormPartComp(QWidget *parent = nullptr);
    ~FormPartComp();

private:
    Ui::FormPartComp *ui;
    RestRoTableModel *modelPart;
    RestTableModel *modelChemCert;
    RestTableModel *modelChem;
    void loadSettings();
    void saveSettings();
    int current_id_part;

private slots:
    void upd();
    void updFinished();
    void updData(QModelIndex index);
};

#endif // FORMPARTCOMP_H
