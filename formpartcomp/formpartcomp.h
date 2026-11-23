#ifndef FORMPARTCOMP_H
#define FORMPARTCOMP_H

#include <QWidget>
#include "rest/restrotablemodel.h"

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

private slots:
    void upd();
};

#endif // FORMPARTCOMP_H
