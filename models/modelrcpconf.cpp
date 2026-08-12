#include "modelrcpconf.h"

ModelRcpConf::ModelRcpConf(QObject *parent)
    : QAbstractTableModel{parent}
{
    isProcessing=false;
    current_id_el=-1;
    header<<tr("Название")<<tr("Дата изменения")<<tr("Размер")<<tr("md5");
}

QVariant ModelRcpConf::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()){
        return QVariant();
    }
    if (role==Qt::EditRole){
        return _data[index.row()][index.column()];
    }
    if (role==Qt::DisplayRole){
        QVariant data=_data[index.row()][index.column()];
        if (index.column()==0){
            data=data.toString().section('/', -1);
        } else if (index.column()==1){
            data=data.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
        }
        return data;
    }
    if (role==Qt::TextAlignmentRole){
        return (index.column()==2) ? int(Qt::AlignRight | Qt::AlignVCenter) : int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    if (role==Qt::DecorationRole && index.column()==0){
        QString fname=(_data[index.row()][0]).toString();
        return getIconByFileName(fname);
    }
    return QVariant();
}

QVariant ModelRcpConf::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal && (role==Qt::DisplayRole || role==Qt::EditRole)){
        return header.at(section);
    } else {
        return QAbstractTableModel::headerData(section,orientation,role);
    }
}

int ModelRcpConf::rowCount(const QModelIndex &/*parent*/) const
{
    return _data.size();
}

int ModelRcpConf::columnCount(const QModelIndex &/*parent*/) const
{
    return 4;
}

void ModelRcpConf::refresh(int id_el)
{
    QUrl url = QUrl(RestConnection::instance()->getUrl()+"/api/elrtr/dosage/s3/files/list/"+QString::number(id_el));
    queue.enqueue(url);
    if (!isProcessing) {
        processNextRequest();
    }
}

QString ModelRcpConf::getFileName(int index)
{
    return this->data(this->index(index,0),Qt::EditRole).toString();
}

QIcon ModelRcpConf::getIconByFileName(const QString &fileName) const
{
    QString suffix = fileName;
    if (suffix.contains('.')) {
        suffix = suffix.section('.', -1); // Берем всё, что после последней точки
    }
    suffix = suffix.toLower().trimmed(); // Приводим к нижнему регистру

    if (suffix.isEmpty()) {
        suffix = "txt"; // Дефолтное расширение, если ничего не передано
    }

    enum FileCategory {
        Unknown,
        Doc,
        Xls,
        Pdf,
        Jpg,
        Png
    };

    static const QHash<QString, FileCategory> extensionMap = {
        {"doc", Doc},  {"docx", Doc}, {"rtf", Doc},
        {"xls", Xls},  {"xlsx", Xls}, {"csv",  Xls},
        {"pdf", Pdf},
        {"jpg", Jpg},  {"gpeg", Jpg},
        {"png", Png}
    };

    FileCategory category = extensionMap.value(suffix, Unknown);

    switch (category) {
    case Doc:
        return QIcon(":images/icons/doc.png");

    case Xls:
        return QIcon(":images/icons/xls.png");

    case Pdf:
        return QIcon(":images/icons/pdf.png");

    case Jpg:
        return QIcon(":images/icons/jpg.png");

    case Png:
        return QIcon(":images/icons/png.png");

    case Unknown:
    default:
        return QIcon(":images/icons/file.png");
    }
}

void ModelRcpConf::upload()
{
    if (current_id_el<=0){
        return;
    }
    QSettings settings("szsm", QApplication::applicationName());
    QDir dir(settings.value("savePath",QDir::homePath()).toString());
    QString filePath=QFileDialog::getOpenFileName(NULL, QString::fromUtf8("Открыть файл"),dir.path());
    QFile *file = new QFile(filePath, this);
    if (!file->open(QIODevice::ReadOnly)) {
        file->deleteLater();
        return;
    }

    // Создаем multipart/form-data запрос (совместимый с multer на бэкенде)
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    QHttpPart filePart;

    QFileInfo fileInfo(filePath);
    settings.setValue("savePath",fileInfo.path());
    QString rawFileName = fileInfo.fileName();

    // Кодируем имя файла для передачи в HTTP-заголовке (процентное кодирование UTF-8)
    QByteArray encodedFileName = QUrl::toPercentEncoding(rawFileName);

    // Формируем заголовок по стандарту RFC 5987
    // Параметр filename используется как запасной (обычно латиница),
    // а filename* указывает серверу, что данные идут в UTF-8
    QString dispositionValue = QString("form-data; name=\"file\"; filename=\"%1\"; filename*=UTF-8''%2").arg(rawFileName, QString::fromUtf8(encodedFileName));

    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, dispositionValue);
    filePart.setBodyDevice(file);
    file->setParent(multiPart); // Файл удалится вместе с multiPart

    multiPart->append(filePart);

    QUrl url = QUrl(RestConnection::instance()->getUrl()+"/api/elrtr/dosage/s3/files/upload/"+QString::number(current_id_el));
    QNetworkReply *reply = RestConnection::instance()->sendRequest(url,"POST",multiPart);
    multiPart->setParent(reply);
    connect(reply,SIGNAL(finished()),this,SLOT(onUploadFinished()));
}

void ModelRcpConf::download(QString filename)
{
    QByteArray encodedFileName = QUrl::toPercentEncoding(filename);

    QUrl url = QUrl(RestConnection::instance()->getUrl()+"/api/elrtr/dosage/s3/files/get/"+encodedFileName);
    QString pureFileName = filename.section('/', -1);
    QDir dir(QDir::tempPath());
    QString fullSavePath = dir.filePath(pureFileName);

    QNetworkReply *reply = RestConnection::instance()->sendGet(url);
    reply->setProperty("savePath", fullSavePath);
    connect(reply,SIGNAL(finished()),this,SLOT(onDownloadFinished()));
}

void ModelRcpConf::remove(QString filename)
{
    QString pureFileName = filename.section('/', -1);
    int n=QMessageBox::question(NULL,QString::fromUtf8("Подтвердите удаление"),QString::fromUtf8("Подтветждаете удаление ")+pureFileName+QString::fromUtf8("?"),QMessageBox::Yes| QMessageBox::No);
    if (n==QMessageBox::Yes) {
        QByteArray encodedFileName = QUrl::toPercentEncoding(filename);
        QUrl url = QUrl(RestConnection::instance()->getUrl()+"/api/elrtr/dosage/s3/files/delete/"+encodedFileName);
        QByteArray body;
        QNetworkReply *reply = RestConnection::instance()->sendRequest(url,"DELETE",body);
        connect(reply,SIGNAL(finished()),this,SLOT(onUploadFinished()));
    }
}

void ModelRcpConf::processNextRequest()
{
    if (queue.isEmpty()) {
        isProcessing = false;
        return;
    }
    isProcessing = true;
    QUrl url = queue.dequeue();
    QNetworkReply *reply = RestConnection::instance()->sendGet(url);
    connect(reply,SIGNAL(finished()),this,SLOT(onResult()));
}

void ModelRcpConf::onResult()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply){
        return;
    }
    QString lastSegment = QFileInfo(reply->url().path()).fileName();
    current_id_el=lastSegment.toInt();
    if (reply->error()!=QNetworkReply::NoError){
        QMessageBox::critical(nullptr,tr("Ошибка"),reply->errorString()+"\n"+reply->readAll(),QMessageBox::Cancel);
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        const QJsonArray arr=doc.object().value("files").toArray();
        beginResetModel();
        _data.clear();
        for (const QJsonValue &value : arr) {
            QVector<QVariant>row;
            QDateTime dt = QDateTime::fromString(value.toObject().value("lastModified").toString(),Qt::ISODate).toLocalTime();
            row.push_back(value.toObject().value("name").toVariant());
            row.push_back(dt);
            row.push_back(value.toObject().value("size").toVariant());
            row.push_back(value.toObject().value("etag").toVariant());
            _data.push_back(row);
        }
        endResetModel();
        emit refreshFinished();
    }
    reply->deleteLater();
    processNextRequest();
}

void ModelRcpConf::onUploadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply){
        return;
    }
    if (reply->error()!=QNetworkReply::NoError){
        QMessageBox::critical(nullptr,tr("Ошибка"),reply->errorString()+"\n"+reply->readAll(),QMessageBox::Cancel);
    } else {
        refresh(current_id_el);
    }
    reply->deleteLater();
}

void ModelRcpConf::onDownloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply){
        return;
    }
    if (reply->error()!=QNetworkReply::NoError){
        QMessageBox::critical(nullptr,tr("Ошибка"),reply->errorString()+"\n"+reply->readAll(),QMessageBox::Cancel);
    } else {
        QString savePath = reply->property("savePath").toString();
        QFile localFile(savePath);
        if (localFile.open(QIODevice::WriteOnly)) {
            localFile.write(reply->readAll());
            localFile.close();
            QFileInfo fileInfo(savePath);
            QDesktopServices::openUrl((QUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()))));
        } else {
            QMessageBox::critical(nullptr,tr("Ошибка"),tr("Не удалось записать файл: ")+"\n"+savePath,QMessageBox::Cancel);
        }
    }
    reply->deleteLater();
}
