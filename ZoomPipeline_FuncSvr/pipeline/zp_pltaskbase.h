#ifndef ZP_PLTASKBASE_H
#define ZP_PLTASKBASE_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
namespace ZPTaskEngine{
class zp_plTaskBase : public QObject
{
    Q_OBJECT
public:
    explicit zp_plTaskBase(QObject *parent = 0);

    virtual int run() = 0;
    int addRef()
    {
        QMutexLocker locker(&m_mutex_ref);
        refCount++;

        return refCount;
    }
    int delRef()
    {
        QMutexLocker locker(&m_mutex_ref);
        refCount--;

        return refCount;
    }

    int ref()
    {
        QMutexLocker locker(&m_mutex_ref);
        return refCount;
    }

private:
     int refCount;
     QMutex m_mutex_ref;
signals:

public slots:

};
}
#endif // ZP_PLTASKBASE_H
