#ifndef DATABASERESOURCE_H
#define DATABASERESOURCE_H

#include <QObject>
#include <QMap>
#include <QSqlDatabase>
#include <QString>
#include <QMutex>
//!this class provide an database reource,
//!In different thread, workers can get existing db connections
//! immediately without re-creation operations.
class DatabaseResource : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseResource(QObject *parent = 0);

    //!Get an database connection belong to current thread.
    //!if database does not exist, it will be added using dbtype
    QSqlDatabase & databse(const QString & strDBName, const QString & dbtype);

    //!Remove Database
    void remove_db(const QString & strDBName);

    //!Prepare for db connection operations.
    //!in multi-thread programs, db add, remove, conn should be protected with mutex,
    void PrepareDbReg();
    void FinishDbReg();

protected:
    QMap<QString, QSqlDatabase> m_map_databses;
    QMutex m_mutex_reg;

signals:

public slots:

};

#endif // DATABASERESOURCE_H
