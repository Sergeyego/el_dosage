#ifndef FORMCOMP_H
#define FORMCOMP_H

#include <QWidget>
#include "rest/resttablemodel.h"
#include "rest/restmapper.h"

namespace Ui {
class FormComp;
}

class FormComp : public QWidget
{
    Q_OBJECT

public:
    explicit FormComp(QWidget *parent = nullptr);
    ~FormComp();

private:
    Ui::FormComp *ui;
    RestTableModel *modelComp;
    RestTableModel *modelGost;
    RestMapper *mapper;
    void loadSettings();
    void saveSettings();

private slots:
    void upd();
    void updData(int index);
};

#endif // FORMCOMP_H
