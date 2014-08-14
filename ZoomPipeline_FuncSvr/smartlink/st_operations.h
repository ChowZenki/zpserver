#ifndef ST_OPERATIONS_H
#define ST_OPERATIONS_H

#include <QObject>
#include "st_message.h"
namespace ParkinglotsSvr{



	class st_operations : public QObject
	{
		Q_OBJECT
	public:
		explicit st_operations(QObject *parent = 0);

	signals:

	public slots:

	};
}

#endif // ST_OPERATIONS_H
