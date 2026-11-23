#ifndef RESTROTABLEMODEL_H
#define RESTROTABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QApplication>
#include <QMessageBox>
#include "rest/resttablemodel.h"
#include "rest/resttypes.h"
#include "rest/resterror.h"

class RestRoTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RestRoTableModel(QObject *parent = nullptr);
    ~RestRoTableModel();
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    QString formatVal(const QVariant &val, int column) const;
    colInfo columnInfo(int col) const;
    QMetaType::Type columnType(int col) const;
    void setPath(QString p);
    void setModelData(const QJsonObject &data);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role=Qt::EditRole);
    QString title() const;
    QString path() const;
    QVariant getModelData(int row, QString col) const;
    int findRowByKey(QVariant key, QString col) const;
    int columnIndex(QString nam) const;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

public slots:
    void select();
    void selectSync();
    void clear();

private slots:
    void processNextRequest();
    void onResult();

protected:
    QVector<QVector<cellData>> modelData;

private:
    QMap<QString,colInfo> colMap;
    QString _path;
    QString _title;
    QStringList _columns;
    QQueue<QUrl> queue;
    bool isProcessing;

signals :
    void sigRefresh();
    void sigStartRefresh();
};

#endif // RESTROTABLEMODEL_H
