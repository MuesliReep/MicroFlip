#ifndef WORKORDERMODEL_H
#define WORKORDERMODEL_H

#include <QAbstractListModel>

class WorkOrderItem {

public:
    WorkOrderItem(const int &workId, const QString &workState);

    int     workId() const;
    QString workState () const;

    void setWorkState(const QString &state);

private:
    int     m_workId;
    QString m_state;
};

class WorkOrderModel : public QAbstractListModel {

    Q_OBJECT

public:
    enum WorkOrderRoles {
        WorkIdRole = Qt::UserRole + 1,
        StateRole
    };

    WorkOrderModel(QObject *parent = nullptr);

    void addWorkOrderItem(const WorkOrderItem &workOrderItem);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private:
    QList<WorkOrderItem> workOrderItems;

protected:
    QHash<int, QByteArray> roleNames() const;
};

#endif // WORKORDERMODEL_H
