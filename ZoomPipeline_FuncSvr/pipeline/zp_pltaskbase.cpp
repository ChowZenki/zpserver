#include "zp_pltaskbase.h"
namespace ZPTaskEngine{
	zp_plTaskBase::zp_plTaskBase(QObject *parent) :
		QObject(parent)
	{
		refCount = 0;
	}
	int zp_plTaskBase::addRef()
	{
		QMutexLocker locker(&m_mutex_ref);
		refCount++;

		return refCount;
	}
	int zp_plTaskBase::delRef()
	{
		QMutexLocker locker(&m_mutex_ref);
		refCount--;

		return refCount;
	}

	int zp_plTaskBase::ref()
	{
		QMutexLocker locker(&m_mutex_ref);
		return refCount;
	}
}
