#include "zp_pltaskbase.h"
namespace ZPTaskEngine{
	zp_plTaskBase::zp_plTaskBase(QObject *parent) :
		QObject(parent)
	{
		refCount = 0;
	}

}
