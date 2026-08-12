#ifndef FORMCONFRCP_H
#define FORMCONFRCP_H

#include <QWidget>
#include <QQueue>
#include <QModelIndex>
#include "models/modelrcpconf.h"

namespace Ui {
class FormConfRcp;
}

class FormConfRcp : public QWidget
{
    Q_OBJECT

public:
    explicit FormConfRcp(QWidget *parent = nullptr);
    ~FormConfRcp();

private:
    Ui::FormConfRcp *ui;
    ModelRcpConf *modelRcpConf;

private slots:
    void updCont(QModelIndex index);
    void download(QModelIndex index);
    void removeRcp();
};

#endif // FORMCONFRCP_H
