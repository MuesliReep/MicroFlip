#ifndef LOGITEM_H
#define LOGITEM_H

#include <QObject>

class logItem : public QObject
{
    Q_OBJECT
public:
    explicit logItem(QObject *parent = nullptr);

signals:

public slots:
};

#endif // LOGITEM_H
