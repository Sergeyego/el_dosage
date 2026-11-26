#ifndef FORMDOZ_H
#define FORMDOZ_H

#include <QWidget>
#include <QAction>
#include <QSettings>
#include "rest/resttablemodel.h"
#include "rest/restrotablemodel.h"
#include "dialognew/dialognew.h"

namespace Ui {
class FormDoz;
}

class ModelDoz : public RestRoTableModel
{
    Q_OBJECT
public:
    explicit ModelDoz(QWidget *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void refresh(QDate beg, QDate end);
protected:
    bool setData(const QModelIndex &index, const QVariant &value, int role);
};

class FormDoz : public QWidget
{
    Q_OBJECT
public:
    explicit FormDoz(QWidget *parent = nullptr);
    ~FormDoz();

private:
    Ui::FormDoz *ui;
    QAction *actionUpd;
    QAction *actionDel;
    QAction *actionConfirm;
    QAction *actionSave;
    QAction *actionNew;
    ModelDoz *modelDoz;
    RestTableModel *modelDozData;
    void loadSettings();
    void saveSettings();
    int current_id;

private slots:
    void upd();
    void updFinished();
    void updDozData(QModelIndex index);
    void delDoz();
    void confirmDoz();
    void newDoz();
    void saveFile();
};

#endif // FORMDOZ_H
