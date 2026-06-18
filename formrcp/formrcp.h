#ifndef FORMRCP_H
#define FORMRCP_H

#include <QWidget>
#include <QSettings>
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
    int id_copy;
    Ui::FormRcp *ui;
    RestTableModel *modelRcp;
    RestTableModel *modelRcpData;
    RestMapper *mapper;
    void loadSettings();
    void saveSettings();

private slots:
    void upd();
    void markTextChanged();
    void updData(int index);
    void calcSum();
    void copy();
    void paste();
};

#endif // FORMRCP_H
