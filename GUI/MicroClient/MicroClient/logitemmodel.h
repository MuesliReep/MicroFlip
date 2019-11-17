#ifndef LOGITEMMODEL_H
#define LOGITEMMODEL_H

#include <QAbstractListModel>

class LogItem {

public:
    LogItem(const int &workId,
            const QString &className,
            const QString &log,
            const int &severity);

    int     workId     () const;
    QString className  () const;
    QString log        () const;
    int     severity   () const;
    QString reportTime () const;

private:

    int     m_workId;
    QString m_className;
    QString m_log;
    int     m_severity;
    QString m_reportTime;
};

class LogItemModel : public QAbstractListModel {

    Q_OBJECT

public:
    enum LogItemRoles {
        WorkIdRole = Qt::UserRole + 1,
        ClassNameRole,
        LogRole,
        SeverityRole,
        ReportTimeRole
    };

    LogItemModel(QObject *parent = nullptr);

    void addLogItem(const LogItem &logItem);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private:
    QList<LogItem> logItems;

protected:
    QHash<int, QByteArray> roleNames() const;
};

#endif // LOGITEMMODEL_H
