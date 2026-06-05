#ifndef FORMRCP_H
#define FORMRCP_H

#include <QWidget>
#include "rest/resttablemodel.h"
#include "rest/restmapper.h"

namespace Ui {
class FormRcp;
}

class FormRcp : public QWidget
{
    Q_OBJECT

public:
    explicit FormRcp(QWidget *parent = nullptr);
    ~FormRcp();

private:
    Ui::FormRcp *ui;
    RestTableModel *modelRcp;
    RestMapper *mapper;

private slots:
    void upd();
    void markTextChanged();
};

#endif // FORMRCP_H
