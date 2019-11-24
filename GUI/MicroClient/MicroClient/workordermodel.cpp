#include "workordermodel.h"

WorkOrderModel::WorkOrderModel(QObject *parent)
{
}

void WorkOrderModel::addWorkOrderItem(const WorkOrderItem &workOrderItem)
{

    bool update = false;

    // Check if this work order is already present
    for (int i = 0; i < workOrderItems.length(); i++) {

        if(workOrderItem.workId() == workOrderItems.at(i).workId()) {

            // If so update item
            if(workOrderItem.workState() != workOrderItems.at(i).workState()) {

                QString newState = workOrderItem.workState();

                workOrderItems[i] = workOrderItem;

                emit dataChanged(index(i), index(i));

                update = true;
                return;
            }
        }
    }

    if(!update) {

        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        workOrderItems << workOrderItem;
        endInsertRows();
    }
}

int WorkOrderModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return workOrderItems.count();
}

QVariant WorkOrderModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= workOrderItems.count())
        return QVariant();

    const WorkOrderItem &workOrderItem = workOrderItems[index.row()];
    if (role == WorkIdRole)
        return workOrderItem.workId();
    if (role == StateRole)
        return workOrderItem.workState();
    return QVariant();
}

QHash<int, QByteArray> WorkOrderModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[WorkIdRole] = "workId";
    roles[StateRole]  = "workState";
    return roles;
}

WorkOrderItem::WorkOrderItem(const int &workId, const QString &workState)
{
    m_workId = workId;
    m_state  = workState;
}

int WorkOrderItem::workId() const
{
    return m_workId;
}

QString WorkOrderItem::workState() const
{
    return m_state;
}

void WorkOrderItem::setWorkState(const QString &state)
{
    m_state = state;
}
