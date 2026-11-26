#include "tablemodel.h"

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{
    decimal = 1;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()
        || index.row() < 0 || index.row() >= p_d.size()
        || index.column() < 0 || index.column() >= p_header.size()) {
        return QVariant();
    }

    const QVariant origData = p_d.at(index.row()).at(index.column());

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    const int type = origData.type();
#else
    const int type = origData.typeId();
#endif

    if (role == Qt::DisplayRole) {
        if (type == QMetaType::Double) {
            if (origData.isNull()) return QString();
            const double d = origData.toDouble();
            if (d == 0.0 || qIsNaN(d)) return QString();
            return QLocale().toString(d, 'f', mdecimal.value(index.column(), decimal));
        }
        if (type == QMetaType::QDate) {
            return origData.isNull()
                       ? QString()
                       : origData.toDate().toString("dd.MM.yyyy");
        }
    } else if (role == Qt::TextAlignmentRole) {
        const bool numeric = (type == QMetaType::Int
                              || type == QMetaType::Double
                              || type == QMetaType::LongLong);
        return int((numeric ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter);
    }

    if (role == Qt::EditRole || role == Qt::DisplayRole) {
        return origData;
    }
    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole) return false;
    if (index.row() < 0 || index.row() >= p_d.size()) return false;
    if (index.column() < 0 || index.column() >= p_header.size()) return false;

    p_d[index.row()][index.column()] = value;
    emit dataChanged(index, index);
    return true;
}

bool TableModel::removeRow(int row, const QModelIndex &/*parent*/)
{
    if (row < 0 || row >= p_d.size()) return false;

    beginRemoveRows(QModelIndex(), row, row);
    p_d.remove(row);
    endRemoveRows();
    return true;
}

int TableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return p_d.size();
}

int TableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return p_header.size();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole
        && section >= 0 && section < p_header.size()) {
        return p_header.at(section);
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void TableModel::setModelData(const QVector<QVector<QVariant>> &data, const QStringList &hdata)
{
    const bool headerSame = hdata.isEmpty() || hdata == p_header;
    if (p_d == data && headerSame) {
        // Ничего не поменялось — не дёргаем view зря
        return;
    }

    beginResetModel();
    p_d = data;
    if (!hdata.isEmpty()) {
        p_header = hdata;
    }
    endResetModel();
    emit sigUpd();
}

void TableModel::setHeader(const QStringList &hdata)
{
    if (hdata == p_header) return;

    if (hdata.size() != p_header.size()) {
        beginResetModel();
        p_header = hdata;
        endResetModel();
        return;
    }

    p_header = hdata;
    if (!p_header.isEmpty()) {
        emit headerDataChanged(Qt::Horizontal, 0, p_header.size() - 1);
    }
}

QStringList TableModel::getHeader() const
{
    return p_header;
}

void TableModel::clear()
{
    if (p_d.isEmpty()) return;
    beginResetModel();
    p_d.clear();
    endResetModel();
}

void TableModel::setDecimal(int dec)
{
    if (decimal == dec) return;
    decimal = dec;
    if (rowCount() > 0 && columnCount() > 0) {
        emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
    }
}

void TableModel::setDecimalForColumn(int section, int dec)
{
    if (section < 0 || section >= columnCount()) return;
    if (mdecimal.value(section, decimal) == dec) return;

    mdecimal.insert(section, dec);
    if (rowCount() > 0) {
        emit dataChanged(index(0, section), index(rowCount() - 1, section));
    }
}
