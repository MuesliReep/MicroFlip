#ifndef LOGITEMCONTROLLER_H
#define LOGITEMCONTROLLER_H

#include <QObject>

class logItemController : public QObject {

    Q_OBJECT

    Q_PROPERTY(QString logTime   READ LogTime   WRITE setLogTime   NOTIFY logTimeChanged   )
    Q_PROPERTY(int     workID    READ WorkID    WRITE setWorkID    NOTIFY workIDChanged    )
    Q_PROPERTY(QString className READ ClassName WRITE setClassName NOTIFY classNameChanged )
    Q_PROPERTY(QString log       READ Log       WRITE setLog       NOTIFY logChanged       )
    Q_PROPERTY(int     severity  READ Severity  WRITE setSeverity  NOTIFY severityChanged  )

public:
    explicit logItemController(int workID, QString className, QString log,
                               int severity, QObject *parent = nullptr);

    QString  LogTime          ();
    int      WorkID           ();
    QString  ClassName        ();
    QString  Log              ();
    int      Severity         ();

private:
    QString  logTime          {};
    int      workID           {};
    QString  className        {};
    QString  log              {};
    int      severity         {};

signals:
    void     logTimeChanged   ();
    void     workIDChanged    ();
    void     classNameChanged ();
    void     logChanged       ();
    void     severityChanged  ();

public slots:
    void     setLogTime       (QString logTime   );
    void     setWorkID        (int     workID    );
    void     setClassName     (QString className );
    void     setLog           (QString log       );
    void     setSeverity      (int     severity  );
};

#endif // LOGITEMCONTROLLER_H
