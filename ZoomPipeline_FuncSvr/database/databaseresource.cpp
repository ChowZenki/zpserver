#include "databaseresource.h"
#include <QThread>
#include <QMutexLocker>
#include <QSqlError>
namespace ZPDatabase{
DatabaseResource::DatabaseResource(QObject *parent) :
    QObject(parent)
{
}
//!Get an database connection belong to current thread.
//!if database does not exist, it will be added using dbtype
QSqlDatabase  DatabaseResource::databse(const QString & strDBName)
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
void DatabaseResource::remove_connections()
{
    QMap<QString,tagConnectionPara> sets = currentDatabaseConnections();
    foreach (QString name, sets.keys())
        this->remove_connection(name);
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
    m_dbNames.remove(strDBName) ;

}
bool DatabaseResource::addConnection(
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
    tagConnectionPara para;
    para.connName = connName;
    para.type = type;
    para.HostAddr = HostAddr;
    para.port = port;
    para.dbName = dbName;
    para.User = User;
    para.Pass = Pass;
    para.ExtraOptions = ExtraOptions;

    if (true==QSqlDatabase::contains(connName))
    {
        QSqlDatabase db = QSqlDatabase::database(connName);
        if (db.isOpen()==true)
            db.close();
        QSqlDatabase::removeDatabase(connName);
        QString msg = tr(" Connection removed ")+connName+ tr(" .");
        emit evt_Message(msg);
    }

    m_dbNames[connName] = para;
    QSqlDatabase db = QSqlDatabase::addDatabase(type,connName);
    db.setHostName(HostAddr);
    db.setPort(port);
    db.setDatabaseName(dbName);
    db.setUserName(User);
    db.setPassword(Pass);
    db.setConnectOptions(ExtraOptions);
    if (db.open()==true)
        return true;
    QString msg = tr(" Connection  ")+connName+ tr(" Can't be opened. MSG=");
    msg += db.lastError().text();
    emit evt_Message(msg);
    QSqlDatabase::removeDatabase(connName);
    m_dbNames.remove(connName) ;
    return false;
}
bool DatabaseResource::confirmConnection (const QString & connName)
{
    QMutexLocker locker(&m_mutex_reg);
    if (false==m_dbNames.contains(connName))
    {
        QString msg = tr(" Connection ")+connName+ tr(" has not been added.");
        emit evt_Message(msg);
        return false;
    }
    const tagConnectionPara & para = m_dbNames[connName];
    if (true==QSqlDatabase::contains(connName)  )
    {
        QSqlDatabase db = QSqlDatabase::database(connName);
        if (db.isOpen()==true)
            return true;
        QString msg = tr(" Connection ")+connName+ tr(" has not been opened.");
        emit evt_Message(msg);
        db = QSqlDatabase::addDatabase(para.type,para.connName);
        db.setHostName(para.HostAddr);
        db.setPort(para.port);
        db.setDatabaseName(para.dbName);
        db.setUserName(para.User);
        db.setPassword(para.Pass);
        db.setConnectOptions(para.ExtraOptions);
        if (db.open()==true)
            return true;
        QSqlDatabase::removeDatabase(connName);
        msg = tr(" Connection  ")+connName+ tr(" Can't be opened. MSG=");
        msg += db.lastError().text();
        emit evt_Message(msg);
        return false;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(para.type,para.connName);
    db.setHostName(para.HostAddr);
    db.setPort(para.port);
    db.setDatabaseName(para.dbName);
    db.setUserName(para.User);
    db.setPassword(para.Pass);
    db.setConnectOptions(para.ExtraOptions);
    if (db.open()==true)
        return true;
    QString msg = tr(" Connection  ")+connName+ tr(" Can't be opened. MSG=");
    msg += db.lastError().text();
    emit evt_Message(msg);
    QSqlDatabase::removeDatabase(connName);
    return false;
}
};
