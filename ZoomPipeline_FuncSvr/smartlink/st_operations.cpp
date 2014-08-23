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
	QString st_operations::hex2ascii(const quint8 ptr[], int sz)
	{
		QString str;
		const char asc[] = "0123456789ABCDEF";
		for (int i=0;i<sz;++i)
		{
			str += asc[(ptr[i] >> 4) & 0x0F];
			str += asc[(ptr[i] ) & 0x0F];
		}
		return str;
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

	bool    st_operations::insert_device_table (int nItems, QVector<QString> & vec_dev_names,QVector<QString> & vec_dev_nos,QVector<QString> & vec_dev_ids, quint32 macid)
	{
		bool res = true;
		QSqlDatabase & db = *m_pDb;
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);
			for (int i=0;i<nItems && res == true; ++i)
			{
				QString sql = "select deviceid from sensorlist where deviceid = ?;";
				query.prepare(sql);
				query.addBindValue(vec_dev_ids[i]);
				if (true==query.exec())
				{
					if (query.next()) //need update
					{
						sql = "update sensorlist set devicename =  ? , sensorlist.no = ?  , macid = ? where deviceid = ?;";
						query.prepare(sql);
						query.addBindValue(vec_dev_names[i]);
						query.addBindValue(vec_dev_nos[i]);
						query.addBindValue(macid);
						query.addBindValue(vec_dev_ids[i]);
					}
					else //need insert
					{
						sql = "insert into sensorlist (devicename,sensorlist.no, deviceid ,macid) values (?,?,?,?);";
						query.prepare(sql);
						query.addBindValue(vec_dev_names[i]);
						query.addBindValue(vec_dev_nos[i]);
						query.addBindValue(vec_dev_ids[i]);
						query.addBindValue(macid);
					}
					if (false==query.exec())
					{
						qDebug()<<tr("Database Access Error :")+query.lastError().text()+"\n";
						res = false;
					}
				}
				else
				{
					qDebug()<<tr("Database Access Error :")+query.lastError().text()+"\n";
					res = false;
				}

			}// end for items


		}
		else
		{
			qDebug()<<tr("Database is not ready.");
			res = false;
		}
		return res;
	}
	bool    st_operations::insert_mac_table(quint32 macID, const QString & macSerial,const stMsg_SendMacInfoReq_internal & info)
	{
		bool res = true;
		QSqlDatabase & db = *m_pDb;
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);
			QString sql = "select macid from maclist where macid = ? and serialnum = ?;";
			query.prepare(sql);
			query.addBindValue(macID);
			query.addBindValue(macSerial);
			if (true==query.exec())
			{
				if (query.next()) //need update
				{
					QSqlQuery queryUpdate(db);

					sql = "update maclist set name =  ? , info = ?,  firmwareversion = ? ";
					if (info.tail_data.IEEEAdd_Flag==1)
						sql += ", ieeeadd = ? ";
					if (info.tail_data.PANID_Flag==1)
						sql += ", panid = ? ";
					if (info.tail_data.EPANID_Flag==1)
						sql += ", epanid = ? ";
					sql += ", sensornum = ? , relaynum = ? , ansensornum = ? , anrelaynum = ? ";
					sql += "where macid = ? and serialnum = ?;";
					queryUpdate.prepare(sql);
					queryUpdate.addBindValue(QString(info.HostName));
					queryUpdate.addBindValue(QString(info.HostInfo));
					queryUpdate.addBindValue(info.FirmwareVersion);
					if (info.tail_data.IEEEAdd_Flag==1)
						queryUpdate.addBindValue(hex2ascii(info.tail_data.IEEEAdd,8));
					if (info.tail_data.PANID_Flag==1)
						queryUpdate.addBindValue(hex2ascii(info.tail_data.PANID,2));
					if (info.tail_data.EPANID_Flag==1)
						queryUpdate.addBindValue(hex2ascii(info.tail_data.EPANID,8));
					queryUpdate.addBindValue(info.tail_data.SensorNum);
					queryUpdate.addBindValue(info.tail_data.RelayNum);
					queryUpdate.addBindValue(info.tail_data.ANSensorNum);
					queryUpdate.addBindValue(info.tail_data.ANRelayNum);
					queryUpdate.addBindValue(macID);
					queryUpdate.addBindValue(macSerial);
					if (false==queryUpdate.exec())
					{
						qDebug()<<tr("Database Access Error :")+queryUpdate.lastError().text()+"\n";
						res = false;
					}
				}
				else //no such mac
				{
					qDebug()<<tr("Database Access Error : No such macid and serialnum: %1:%2").arg(macID).arg(macSerial);
					res = false;
				}

			}
			else
			{
				qDebug()<<tr("Database Access Error :")+query.lastError().text()+"\n";
				res = false;
			}
		}
		else
		{
			qDebug()<<tr("Database is not ready.");
			res = false;
		}
		return res;
	}

}
