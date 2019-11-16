#include "logitemmodel.h"

LogItemModel::LogItemModel(QObject *parent) : QAbstractListModel(parent)
{

}

void LogItemModel::addLogItem(const LogItem &logItem)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    logItems << logItem;
    endInsertRows();
}

int LogItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return logItems.count();
}

QVariant LogItemModel::data(const QModelIndex &index, int role) const
{

    if (index.row() < 0 || index.row() >= logItems.count())
        return QVariant();

    const LogItem &logItem = logItems[index.row()];
    if (role == WorkIdRole)
        return logItem.workId();
    else if (role == ClassNameRole)
        return logItem.className();
    else if (role == LogRole)
        return logItem.log();
    else if (role == SeverityRole)
        return logItem.severity();
    return QVariant();
}

QHash<int, QByteArray> LogItemModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[WorkIdRole]    = "workId";
    roles[ClassNameRole] = "className";
    roles[LogRole]       = "log";
    roles[SeverityRole]  = "severity";
    return roles;
}

int LogItem::workId() const
{
    return m_workId;
}

QString LogItem::className() const
{
    return m_className;
}

QString LogItem::log() const
{
    return m_log;
}

int LogItem::severity() const
{
    return m_severity;
}
