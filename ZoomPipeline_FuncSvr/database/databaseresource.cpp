#include "databaseresource.h"
#include <QThread>
DatabaseResource::DatabaseResource(QObject *parent) :
    QObject(parent)
{
}
//!Get an database connection belong to current thread.
//!if database does not exist, it will be added using dbtype
QSqlDatabase & DatabaseResource::databse(const QString & strDBName, const QString & dbtype)
{
    Qt::HANDLE handleThread =  QThread::currentThreadId();
    QString internal_name = QString("%1_%2").arg(strDBName).arg((quint64)(handleThread));
    if (false==m_map_databses.contains(internal_name))
        m_map_databses[internal_name] = QSqlDatabase::addDatabase(dbtype,internal_name);
    return  m_map_databses[internal_name];
}

//!Remove Database
void DatabaseResource::remove_db(const QString & strDBName)
{
    Qt::HANDLE handleThread =  QThread::currentThreadId();
    QString internal_name = QString("%1_%2").arg(strDBName).arg((quint64)(handleThread));
    if (true==m_map_databses.contains(internal_name))
    {
        QSqlDatabase::removeDatabase(internal_name);
        m_map_databses.remove(internal_name);
    }
}

//!Prepare for db connection operations.
//!in multi-thread programs, db add, remove, conn should be protected with mutex,
void DatabaseResource::PrepareDbReg()
{
    m_mutex_reg.lock();
}

void DatabaseResource::FinishDbReg()
{
    m_mutex_reg.unlock();
}
