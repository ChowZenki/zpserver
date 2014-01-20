#include "databaseresource.h"
#include <QThread>
#include <QMutexLocker>
#include <QSqlError>
DatabaseResource::DatabaseResource(QObject *parent) :
    QObject(parent)
{
}
//!Get an database connection belong to current thread.
//!if database does not exist, it will be added using dbtype
QSqlDatabase & DatabaseResource::databse(const QString & strDBName)
{
    QMutexLocker locker(&m_mutex_reg);
    if (false==QSqlDatabase::contains(strDBName))
    {
        QString msg = tr(" Connection name ")+strDBName+ tr(" does not exist.");
        emit evt_Message(msg);
        return QSqlDatabase();
    }
    return  QSqlDatabase::database(strDBName);
}

//!Remove Database
void DatabaseResource::remove_connection(const QString & strDBName)
{
    QMutexLocker locker(&m_mutex_reg);
    if (true==QSqlDatabase::contains(strDBName))
    {
        QSqlDatabase db = QSqlDatabase::database(strDBName);
        if (db.isOpen()==true)
            db.close();
        QSqlDatabase::removeDatabase(strDBName);
        QString msg = tr(" Connection removed ")+strDBName+ tr(" .");
        emit evt_Message(msg);

    }
    else
    {
        QString msg = tr(" Connection name ")+strDBName+ tr(" does not exist.");
        emit evt_Message(msg);
    }
}
bool DatabaseResource::confirmConnection(
        const QString & connName,
        const QString & type,
        const QString & HostAddr,
        int port,
        const QString & dbName,
        const QString & User,
        const QString & Pass,
        const QString & ExtraOptions
        )
{
    QMutexLocker locker(&m_mutex_reg);
    if (true==QSqlDatabase::contains(strDBName))
    {
        QSqlDatabase db = QSqlDatabase::database(strDBName);
        if (db.isOpen()==true)
            return true;
        QString msg = tr(" Connection ")+strDBName+ tr(" has not not opened.");
        emit evt_Message(msg);

        QSqlDatabase::removeDatabase(strDBName);
        db = QSqlDatabase::addDatabase(type,connName);
        db.setHostName(HostAddr);
        db.setPort(port);
        db.setDatabaseName(dbName);
        db.setUserName(User);
        db.setPassword(Pass);
        db.setConnectOptions(ExtraOptions);
        if (db.open()==true)
            return true;
        QString msg = tr(" Connection  ")+strDBName+ tr(" Can;t be opened. MSG=");
        msg += db.lastError().text();
        emit evt_Message(msg);

        return false;
    }

    db = QSqlDatabase::addDatabase(type,connName);
    db.setHostName(HostAddr);
    db.setPort(port);
    db.setDatabaseName(dbName);
    db.setUserName(User);
    db.setPassword(Pass);
    db.setConnectOptions(ExtraOptions);
    if (db.open()==true)
        return true;
    QString msg = tr(" Connection  ")+strDBName+ tr(" Can;t be opened. MSG=");
    msg += db.lastError().text();
    emit evt_Message(msg);
    return false;

}
