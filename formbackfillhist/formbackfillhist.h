#ifndef FORMBACKFILLHIST_H
#define FORMBACKFILLHIST_H

#include <QWidget>
#include "rest/restrotablemodel.h"

namespace Ui {
class FormBackfillHist;
}

class FormBackfillHist : public QWidget
{
    Q_OBJECT

public:
    explicit FormBackfillHist(QWidget *parent = nullptr);
    ~FormBackfillHist();

private:
    Ui::FormBackfillHist *ui;
    RestRoTableModel *modelHist;
    int current_id;
    RestRelModel *modelComp;

private slots:
    void updHist();
    void updCompFinished();
};

#endif // FORMBACKFILLHIST_H
