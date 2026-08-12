#ifndef MODELRCPCONF_H
#define MODELRCPCONF_H

#include <QAbstractTableModel>
#include <QObject>
#include <QQueue>
#include <QFile>
#include <QFileDialog>
#include <QHttpPart>
#include <QSettings>
#include <QDesktopServices>
#include <QMimeDatabase>
#include <QFileIconProvider>
#include <QTemporaryFile>
#include "rest/restconnection.h"

class ModelRcpConf : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ModelRcpConf(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    void refresh(int id_el);
    QString getFileName(int index);
    QIcon getIconByFileName(const QString &fileName) const;

private:
    QQueue<QUrl> queue;
    bool isProcessing;
    QVector<QVector<QVariant>> _data;
    int current_id_el;
    QStringList header;

public slots:
    void upload();
    void download(QString filename);
    void remove(QString filename);

private slots:
    void processNextRequest();
    void onResult();
    void onUploadFinished();
    void onDownloadFinished();

signals:
    void refreshFinished();
};

#endif // MODELRCPCONF_H
