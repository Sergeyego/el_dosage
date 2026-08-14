#ifndef FORMRCP_H
#define FORMRCP_H

#include <QWidget>
#include <QSettings>
#include "rest/resttablemodel.h"
#include "rest/restmapper.h"
#include "rest/restrotablemodel.h"
#include "models/modelrcpconf.h"

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
    int id_copy;
    Ui::FormRcp *ui;
    RestTableModel *modelRcp;
    RestTableModel *modelRcpData;
    RestRoTableModel *modelMode;
    RestRoTableModel *modelCalc;
    RestMapper *mapper;
    ModelRcpConf *modelRcpConf;
    void loadSettings();
    void saveSettings();

private slots:
    void upd();
    void downloadRcpConf(QModelIndex index);
    void onRcpConfContextMenuRequested(const QPoint &pos);
    void markTextChanged();
    void updData(int index);
    void updMode();
    void updCacl();
    void onUpdModeFinifhed();
    void calcSum();
    void copy();
    void paste();
};

#endif // FORMRCP_H
