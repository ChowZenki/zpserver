#include "st_operations.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
namespace ParkinglotsSvr{
	st_operations::st_operations(QSqlDatabase * db,QObject *parent) :
		QObject(parent)
	  ,m_pDb(db)
	{
	}

	quint8 st_operations::regisit_host(QString serialnum, quint32 * host_id )
	{
		int DoneCode = 2;
		QSqlDatabase & db = *m_pDb;
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);

			QString sql = "select serialnum,macid,ifregisted from maclist where serialnum = ?;";
			query.prepare(sql);
			query.addBindValue(serialnum);

			if (true==query.exec())
			{
				if (query.next())
				{
					bool bOk = false;
					int ncurrid = query.value(1).toInt(&bOk);
					int nregisdited =  query.value(2).toInt();
					if (bOk==true)
					{
						if (ncurrid>=0x0010000 && ncurrid <=0x0FFFFFFF)
						{
							* host_id = ncurrid;
							DoneCode = nregisdited==0?0:1;
							if (nregisdited==0)
							{
								QSqlQuery queryUpdate(db);
								sql = "update maclist set ifregisted = 1 where  macid = ?;";
								queryUpdate.prepare(sql);
								queryUpdate.addBindValue(ncurrid);
								if (false==queryUpdate.exec())
								{
									DoneCode = 2;
									qDebug()<<tr("Database Access Error :")+queryUpdate.lastError().text()+"\n";
								}
							}

						}
						else
							qDebug()<<tr("ID is not valid : %1").arg(ncurrid)+"\n";

					}
					else
						qDebug()<<tr("ID is not valid : %1").arg(query.value(1).toString())+"\n";
				}
				else
					qDebug()<<tr("Serial is not valid : %1").arg(serialnum)+"\n";
			}
			else
				qDebug()<<tr("Database Access Error :")+query.lastError().text()+"\n";

		}
		else
			qDebug()<<tr("Database is not ready.");
		return DoneCode;
	}

	quint8  st_operations::login_host(QString serialnum, quint32 host_id )
	{
		int DoneCode = 3;
		QSqlDatabase & db = *m_pDb;
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);

			QString sql = "select macid,serialnum from maclist where macid = ? and serialnum = ?;";
			query.prepare(sql);
			query.addBindValue(host_id);
			query.addBindValue(serialnum);

			if (true==query.exec())
			{
				if (query.next())
				{
					bool bOk = false;
					int ncurrid = query.value(0).toInt(&bOk);
					if (bOk==true)
					{
						if (ncurrid>=0x0010000 && ncurrid <=0x0FFFFFFF)
							DoneCode = 0;
						else
							qDebug()<<tr("ID is not valid : %1").arg(ncurrid)+"\n";

					}
					else
						qDebug()<<tr("ID is not valid : %1").arg(query.value(0).toString())+"\n";
				}
				else
					qDebug()<<tr("Serial/ID is not valid : %1,%2").arg(serialnum).arg(query.value(0).toString())+"\n";
			}
			else
				qDebug()<<tr("Database Access Error :")+query.lastError().text()+"\n";

		}
		else
			qDebug()<<tr("Database is not ready.");
		return DoneCode;
	}

	bool    st_operations::insert_device_table (int nItems, QVector<QString> & vec_dev_names,QVector<QString> & vec_dev_nos,QVector<QString> & vec_dev_ids)
	{
		return false;
	}

}
