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
    QSqlDatabase & databse(const QString & strDBName);

    //!confire connection connName, return true if connection can be used.
    bool confirmConnection(
            const QString & connName,
            const QString & type,
            const QString & HostAddr,
            int port,
            const QString & dbName,
            const QString & User,
            const QString & Pass,
            const QString & ExtraOptions
            );
    void remove_connection(const QString & strDBName);
protected:
    QMutex m_mutex_reg;

signals:
    void evt_Message(const QString &);
public slots:

};

#endif // DATABASERESOURCE_H
