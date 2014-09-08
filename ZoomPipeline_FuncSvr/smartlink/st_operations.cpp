#include "st_operations.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QDateTime>
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
									qCritical()<<tr("Database Access Error :")+queryUpdate.lastError().text();
								}
							}

						}
						else
							qCritical()<<tr("ID is not valid : %1").arg(ncurrid);

					}
					else
						qCritical()<<tr("ID is not valid : %1").arg(query.value(1).toString());
				}
				else
					qCritical()<<tr("Serial is not valid : %1").arg(serialnum);
			}
			else
			{
				qCritical()<<tr("Database Access Error :")+query.lastError().text();
				db.close();
			}

		}
		else
			qCritical()<<tr("Database is not ready.");
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
							qCritical()<<tr("ID is not valid : %1").arg(ncurrid);

					}
					else
						qCritical()<<tr("ID is not valid : %1").arg(query.value(0).toString());
				}
				else
					qCritical()<<tr("Serial/ID is not valid : %1,%2").arg(serialnum).arg(query.value(0).toString());
			}
			else
			{
				qCritical()<<tr("Database Access Error :")+query.lastError().text();
				db.close();
			}

		}
		else
			qCritical()<<tr("Database is not ready.");
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
						sql = "insert into sensorlist (devicename,sensorlist.no, deviceid ,macid,createtime ) values (?,?,?,?,?);";
						query.prepare(sql);
						query.addBindValue(vec_dev_names[i]);
						query.addBindValue(vec_dev_nos[i]);
						query.addBindValue(vec_dev_ids[i]);
						query.addBindValue(macid);
						query.addBindValue(QDateTime::currentDateTimeUtc());
					}
					if (false==query.exec())
					{
						qCritical()<<tr("Database Access Error :")+query.lastError().text();
						res = false;
						db.close();
					}
				}
				else
				{
					qCritical()<<tr("Database Access Error :")+query.lastError().text();
					res = false;
					db.close();
				}

			}// end for items


		}
		else
		{
			qCritical()<<tr("Database is not ready.");
			res = false;
		}
		return res;
	}
	bool    st_operations::insert_mac_table(quint32 macID, QString  macSerial,const stMsg_SendMacInfoReq_internal & info)
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
						qCritical()<<tr("Database Access Error :")+queryUpdate.lastError().text();
						res = false;
					}
				}
				else //no such mac
				{
					qCritical()<<tr("Database Access Error : No such macid and serialnum: %1:%2").arg(macID).arg(macSerial);
					res = false;
				}

			}
			else
			{
				qCritical()<<tr("Database Access Error :")+query.lastError().text();
				res = false;
				db.close();
			}
		}
		else
		{
			qCritical()<<tr("Database is not ready.");
			res = false;
		}
		return res;
	}
	quint8 st_operations::add_new_device(quint32 macid,const quint8 deviceID[/*24*/])
	{
		bool res = true;
		QSqlDatabase & db = *m_pDb;
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);
			QString sql = "select deviceid from sensorlist where deviceid = ?;";
			query.prepare(sql);
			QString devID = hex2ascii(deviceID,24);
			query.addBindValue(devID);

			if (true==query.exec())
			{
				if (query.next()) //need update
				{
					//Already has this ID
					qWarning()<<tr("Already exists a Device :") + devID;
					sql = "update sensorlist set  macid = ? where deviceid = ?;";
					query.prepare(sql);
					query.addBindValue(macid);
					query.addBindValue(devID);
				}
				else //need insert
				{
					sql = "insert into sensorlist (deviceid ,macid, createtime) values (?,?,?);";
					query.prepare(sql);
					query.addBindValue(devID);
					query.addBindValue(macid);
					query.addBindValue(QDateTime::currentDateTimeUtc());
				}
				if (false==query.exec())
				{
					qCritical()<<tr("Database Access Error :")+query.lastError().text();
					res = false;
					db.close();
				}
			}
			else
			{
				qCritical()<<tr("Database Access Error :")+query.lastError().text();
				res = false;
				db.close();
			}
		}
		else
		{
			qCritical()<<tr("Database is not ready.");
			res = false;
		}
		return res==true?0:1;
		return 0;
	}

	quint8 st_operations::del_old_device(quint32 /*macid*/,const quint8 deviceID[/*24*/])
	{
		bool res = true;
		QSqlDatabase & db = *m_pDb;
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);
			QString sql = "delete from sensorlist where deviceid = ?;";
			query.prepare(sql);
			QString devID = hex2ascii(deviceID,24);
			query.addBindValue(devID);
			if (false==query.exec())
			{
				qCritical()<<tr("Database Access Error :")+query.lastError().text();
				res = false;
				db.close();
			}
		}
		else
		{
			qCritical()<<tr("Database is not ready.");
			res = false;
		}
		return res==true?0:1;
	}

	quint8 st_operations::update_DAL_event(quint32 /*macid*/, const QByteArray &array_DAL)
	{
		quint8 res = 0;
		const quint8 * ptr_rawdata = (const quint8 *)array_DAL.constData();
		const stEvent_DeviceEvent * pEvent = (const stEvent_DeviceEvent *) ptr_rawdata;
		const quint8 * pDalLayer = ptr_rawdata + sizeof(stEvent_DeviceEvent);
		int nTotalDalLen = array_DAL.size() -  sizeof(stEvent_DeviceEvent);
		//0xCBFE0001 is the sensor
		if (pEvent->DeviceID[0] == 0x01 && pEvent->DeviceID[1] == 0x00 &&pEvent->DeviceID[2] == 0xFE &&pEvent->DeviceID[3] == 0xCB)
		{
			switch (pEvent->DALEventID)
			{
			case 0x00:
				res = dal_sensor_0x00(pEvent,pDalLayer,nTotalDalLen);
				break;
			case 0x01:
				res = dal_sensor_0x01(pEvent,pDalLayer,nTotalDalLen);
				break;
			case 0x02:
				res = dal_sensor_0x02(pEvent,pDalLayer,nTotalDalLen);
				break;
			default:
				qCritical()<<tr("DAL EventID Error (Event ID %1 is not valid.").arg(pEvent->DALEventID);
				break;

			}
		}
		//0xCBFE0101 is the sensor
		else if (pEvent->DeviceID[0] == 0x01 && pEvent->DeviceID[1] == 0x01 &&pEvent->DeviceID[2] == 0xFE &&pEvent->DeviceID[3] == 0xCB)
		{
			if (pEvent->DALEventID == 0x00)
			{
				//Heartbeating
				QSqlDatabase & db = *m_pDb;
				if (db.isValid()==true && db.isOpen()==true )
				{
					QSqlQuery query(db);
					QString sql = "update sensorlist set lastacttime = ? where deviceid = ?;";
					query.prepare(sql);
					QString devID = hex2ascii(pEvent->DeviceID,24);
					query.addBindValue(QDateTime::currentDateTimeUtc());
					query.addBindValue(devID);
					if (false==query.exec())
					{
						qCritical()<<tr("Database Access Error :")+query.lastError().text();
						res = 1;
						db.close();
					}
				}
				else
				{
					qCritical()<<tr("Database is not ready.");
					res = 1;
				}
			}
			else
				qCritical()<<tr("DAL EventID Error (Event ID %1 is not valid.").arg(pEvent->DALEventID);

		}
		else
		{
			res = 1;
			qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr("Send a unsupported Device Type.");
		}

		return res;
	}

	quint8 st_operations::update_DAL_exception(quint32 /*macid*/, const QByteArray & array_DAL)
	{
		quint8 res = 0;
		const quint8 * ptr_rawdata = (const quint8 *)array_DAL.constData();
		const stEvent_DeviceException * pEvent = (const stEvent_DeviceException *) ptr_rawdata;
		//const quint8 * pDalLayer = ptr_rawdata + sizeof(stEvent_DeviceException);
		//int nTotalDalLen = array_DAL.size() -  sizeof(stEvent_DeviceException);
		//0xCBFE0001 is the sensor
		if (pEvent->DeviceID[0] == 0x01 && pEvent->DeviceID[1] == 0x00 &&pEvent->DeviceID[2] == 0xFE &&pEvent->DeviceID[3] == 0xCB)
		{
			int nStatus = 0;
			if (pEvent->ExceptionID==1)
				nStatus = 2;
			else if (pEvent->ExceptionID==0)
				nStatus = 3;
			else
				nStatus = 3;
			QSqlDatabase & db = *m_pDb;
			if (db.isValid()==true && db.isOpen()==true )
			{
				QSqlQuery query(db);
				QString sql = "update sensorlist set status = ? , lastacttime = ? where deviceid = ?;";
				query.prepare(sql);

				query.addBindValue(nStatus);
				query.addBindValue(QDateTime::currentDateTimeUtc());
				QString devID = hex2ascii(pEvent->DeviceID,24);
				query.addBindValue(devID);
				if (false==query.exec())
				{
					qCritical()<<tr("Database Access Error :")+query.lastError().text();
					res = 1;
					db.close();
				}
			}
			else
			{
				qCritical()<<tr("Database is not ready.");
				res = 1;
			}
		}
		//0xCBFE0101 is the sensor
		else if (pEvent->DeviceID[0] == 0x01 && pEvent->DeviceID[1] == 0x01 &&pEvent->DeviceID[2] == 0xFE &&pEvent->DeviceID[3] == 0xCB)
		{
			int nStatus = 0;
			if (pEvent->ExceptionID==1)
				nStatus = 2;
			else if (pEvent->ExceptionID==0)
				nStatus = 3;
			else
				nStatus = 3;
			QSqlDatabase & db = *m_pDb;
			if (db.isValid()==true && db.isOpen()==true )
			{
				QSqlQuery query(db);
				QString sql = "update sensorlist set status = ? , lastacttime = ? where deviceid = ?;";
				query.prepare(sql);

				query.addBindValue(nStatus);
				query.addBindValue(QDateTime::currentDateTimeUtc());
				QString devID = hex2ascii(pEvent->DeviceID,24);
				query.addBindValue(devID);
				if (false==query.exec())
				{
					qCritical()<<tr("Database Access Error :")+query.lastError().text();
					res = 1;
					db.close();
				}
			}
			else
			{
				qCritical()<<tr("Database is not ready.");
				res = 1;
			}
		}
		else
		{
			res = 1;
			qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr("Send a unsupported Device Type.");
		}

		return res;
	}


	quint8 st_operations::dal_sensor_0x00(const stEvent_DeviceEvent * pEvent,const quint8 * pDalLayer, int nTotalDalLen)
	{
		quint8 res = 0;
		if (pEvent->ParamNum>=1)
		{
			if (nTotalDalLen >=2)
			{
				quint8 dtp = pDalLayer[0];
				if (dtp==ParkinglotsSvr::DAL_TYPE_BOOL)
				{
					quint8 value = pDalLayer[1];
					QSqlDatabase & db = *m_pDb;
					if (db.isValid()==true && db.isOpen()==true )
					{
						//Update sensor list
						QSqlQuery query(db);
						QString sql = "update sensorlist set occupied = ?, lastacttime = ? where deviceid = ?;";
						query.prepare(sql);
						QString devID = hex2ascii(pEvent->DeviceID,24);
						query.addBindValue(value);
						query.addBindValue(QDateTime::currentDateTimeUtc());
						query.addBindValue(devID);
						if (false==query.exec())
						{
							qCritical()<<tr("Database Access Error :")+query.lastError().text();
							res = 1;
							db.close();
						}
						//update sensor event
						sql = "insert into sensorevent (deviceid, eventid, eventparamid, eventparamtype, eventparamvalue, eventtime) values (?, ?, ?, ?, ?, ?);";
						query.prepare(sql);
						query.addBindValue(devID);
						query.addBindValue(pEvent->DALEventID);
						query.addBindValue(0);
						query.addBindValue((int)ParkinglotsSvr::DAL_TYPE_BOOL);
						QString strVal = QString("%1").arg(value);
						query.addBindValue(strVal);
						query.addBindValue(QDateTime::currentDateTimeUtc());
						if (false==query.exec())
						{
							qCritical()<<tr("Database Access Error :")+query.lastError().text();
							res = 1;
							db.close();
						}
					}
					else
					{
						qCritical()<<tr("Database is not ready.");
						res = 1;
					}
				}
				else
				{
					res = 1;
					qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Send a wrong DAL Para (%1) Value (unexpected type %2).").arg(pEvent->DALEventID).arg(dtp);
				}
			}
			else
			{
				res = 1;
				qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Send a wrong DAL Para (%1) Value (Para too short, need 2 bytes, got %2 byte(s)).").arg(pEvent->DALEventID).arg(nTotalDalLen);
			}

		}
		else
		{
			res = 1;
			qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Send a wrong DAL Para (%1) Value (Para not enough, need 1 para).").arg(pEvent->DALEventID);
		}
		return res;
	}
	quint8 st_operations::dal_sensor_0x01(const stEvent_DeviceEvent * pEvent,const quint8 * pDalLayer, int nTotalDalLen)
	{
		quint8 res = 0;
		if (pEvent->ParamNum>=1)
		{
			if (nTotalDalLen >=2)
			{
				quint8 dtp = pDalLayer[0];
				if (dtp==ParkinglotsSvr::DAL_TYPE_UINT8)
				{
					quint8 value = pDalLayer[1];
					QSqlDatabase & db = *m_pDb;
					if (db.isValid()==true && db.isOpen()==true )
					{
						QSqlQuery query(db);
						QString sql = "update sensorlist set batteryvoltage = ?, lastacttime = ? where deviceid = ?;";
						query.prepare(sql);
						QString devID = hex2ascii(pEvent->DeviceID,24);
						query.addBindValue(value);
						query.addBindValue(QDateTime::currentDateTimeUtc());
						query.addBindValue(devID);
						if (false==query.exec())
						{
							qCritical()<<tr("Database Access Error :")+query.lastError().text();
							res = 1;
							db.close();
						}
						//update sensor event
						sql = "insert into sensorevent (deviceid, eventid, eventparamid, eventparamtype, eventparamvalue, eventtime) values (?, ?, ?, ?, ?, ?);";
						query.prepare(sql);
						query.addBindValue(devID);
						query.addBindValue(pEvent->DALEventID);
						query.addBindValue(0);
						query.addBindValue((int)ParkinglotsSvr::DAL_TYPE_UINT8);
						QString strVal = QString("%1").arg(value);
						query.addBindValue(strVal);
						query.addBindValue(QDateTime::currentDateTimeUtc());
						if (false==query.exec())
						{
							qCritical()<<tr("Database Access Error :")+query.lastError().text();
							res = 1;
							db.close();
						}
					}
					else
					{
						qCritical()<<tr("Database is not ready.");
						res = 1;
					}
				}
				else
				{
					res = 1;
					qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Send a wrong DAL Para (%1) Value (unexpected type %2).").arg(pEvent->DALEventID).arg(dtp);
				}
			}
			else
			{
				res = 1;
				qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Send a wrong DAL Para (%1) Value (Para too short, need 2 bytes, got %2 byte(s)).").arg(pEvent->DALEventID).arg(nTotalDalLen);
			}

		}
		else
		{
			res = 1;
			qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr( "Send a wrong DAL Para (%1) Value (Para not enough, need 1 para).").arg(pEvent->DALEventID);
		}
		return res;
	}
	quint8 st_operations::dal_sensor_0x02(const stEvent_DeviceEvent * pEvent,const quint8 * pDalLayer, int nTotalDalLen)
	{
		quint8 res = 0;
		if (pEvent->ParamNum>=9)
		{
			int nSwim = 0;
			quint8 dtp = 0;

			//status
			quint8 para_dal_status = 0;
			if (nSwim >= nTotalDalLen)
			{
				qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Need more data.");
				return 1;
			}
			dtp = pDalLayer[nSwim++];
			if (dtp==ParkinglotsSvr::DAL_TYPE_BOOL)
			{
				if (nSwim >= nTotalDalLen)
				{
					qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Need more data.");
					return 1;
				}
				para_dal_status = pDalLayer[nSwim++];
			}
			else
			{
				qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Send a wrong DAL Para (%1) Value (unexpected type %2).").arg(pEvent->DALEventID).arg(dtp);
				return 1;
			}

			//Mag A
			qint16 para_dal_cmaga[3];
			for (int i=0;i<3;++i)
			{
				if (nSwim >= nTotalDalLen)
				{
					qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Need more data.");
					return 1;
				}
				dtp = pDalLayer[nSwim++];
				if (dtp==ParkinglotsSvr::DAL_TYPE_INT16)
				{
					if (nSwim + 1 >= nTotalDalLen)
					{
						qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Need more data.");
						return 1;
					}
					const qint16 * ptr16 = (const qint16 *)(pDalLayer + nSwim);
					para_dal_cmaga[i] = * ptr16;
					nSwim += 2;
				}
				else
				{
					qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Send a wrong DAL Para (%1) Value (unexpected type %2).").arg(pEvent->DALEventID).arg(dtp);
					return 1;
				}
			}

			//Mag B
			float para_dal_bmaga[3];
			for (int i=0;i<3;++i)
			{
				if (nSwim >= nTotalDalLen)
				{
					qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Need more data.");
					return 1;
				}
				dtp = pDalLayer[nSwim++];
				if (dtp==ParkinglotsSvr::DAL_TYPE_SINGLEFLOAT)
				{
					if (nSwim + 3 >= nTotalDalLen)
					{
						qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Need more data.");
						return 1;
					}
					const float * ptrfloat = (const float *)(pDalLayer + nSwim);
					para_dal_bmaga[i] = * ptrfloat;
					nSwim += 4;
				}
				else
				{
					qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Send a wrong DAL Para (%1) Value (unexpected type %2).").arg(pEvent->DALEventID).arg(dtp);
					return 1;
				}
			}

			//Temp
			qint8 para_dal_tempr = 0;
			if (nSwim >= nTotalDalLen)
			{
				qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Need more data.");
				return 1;
			}
			dtp = pDalLayer[nSwim++];
			if (dtp==ParkinglotsSvr::DAL_TYPE_INT8)
			{
				if (nSwim >= nTotalDalLen)
				{
					qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Need more data.");
					return 1;
				}
				para_dal_tempr = pDalLayer[nSwim++];
			}
			else
			{
				qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Send a wrong DAL Para (%1) Value (unexpected type %2).").arg(pEvent->DALEventID).arg(dtp);
				return 1;
			}

			//Vol
			quint8 para_dal_vol = 0;
			if (nSwim >= nTotalDalLen)
			{
				qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Need more data.");
				return 1;
			}
			dtp = pDalLayer[nSwim++];
			if (dtp==ParkinglotsSvr::DAL_TYPE_UINT8)
			{
				if (nSwim >= nTotalDalLen)
				{
					qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Need more data.");
					return 1;
				}
				para_dal_vol = pDalLayer[nSwim++];
			}
			else
			{
				qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr(" Send a wrong DAL Para (%1) Value (unexpected type %2).").arg(pEvent->DALEventID).arg(dtp);
				return 1;
			}
			QSqlDatabase & db = *m_pDb;
			if (db.isValid()==true && db.isOpen()==true )
			{
				QSqlQuery query(db);
				QString sql = "update sensorlist set occupied = ?, cmx = ? , cmy = ? , cmz = ? , bmx = ?, bmy = ? , bmz = ? ,temperature = ?, batteryvoltage = ?, lastacttime = ?  where deviceid = ?;";
				query.prepare(sql);
				QString devID = hex2ascii(pEvent->DeviceID,24);
				query.addBindValue(para_dal_status);
				query.addBindValue(para_dal_cmaga[0]);
				query.addBindValue(para_dal_cmaga[1]);
				query.addBindValue(para_dal_cmaga[2]);
				query.addBindValue(para_dal_bmaga[0]);
				query.addBindValue(para_dal_bmaga[1]);
				query.addBindValue(para_dal_bmaga[2]);
				query.addBindValue(para_dal_tempr);
				query.addBindValue(para_dal_vol);
				query.addBindValue(QDateTime::currentDateTimeUtc());
				query.addBindValue(devID);
				if (false==query.exec())
				{
					qCritical()<<tr("Database Access Error :")+query.lastError().text();
					res = 1;
					db.close();
				}
				//update sensor event
				sql = "insert into sensorevent (deviceid, eventid, eventparamid, eventparamtype, eventparamvalue, eventtime) values (?, ?, ?, ?, ?, ?);";
				query.prepare(sql);
				query.addBindValue(devID);
				query.addBindValue(pEvent->DALEventID);
				query.addBindValue(9);
				query.addBindValue((int)ParkinglotsSvr::DAL_TYPE_STRING);
				QString strVal = QString("Occupied=%1, cm = {%2,%3,%4}, bm = {%5, %6, %7}, tempr = %8, bat = %9")
						.arg(para_dal_status)
						.arg(para_dal_cmaga[0])
						.arg(para_dal_cmaga[1])
						.arg(para_dal_cmaga[2])
						.arg(para_dal_bmaga[0])
						.arg(para_dal_bmaga[1])
						.arg(para_dal_bmaga[2])
						.arg(para_dal_tempr)
						.arg(para_dal_vol);
				query.addBindValue(strVal);
				query.addBindValue(QDateTime::currentDateTimeUtc());
				if (false==query.exec())
				{
					qCritical()<<tr("Database Access Error :")+query.lastError().text();
					res = 1;
					db.close();
				}
			}
			else
			{
				qCritical()<<tr("Database is not ready.");
				res = 1;
			}
		}
		else
		{
			res = 1;
			qWarning()<< tr("Device ") << hex2ascii(pEvent->DeviceID,24) << tr( "Send a wrong DAL Para (%1) Value (Para not enough, need 1 para).").arg(pEvent->DALEventID);
		}

		return res;
	}
	bool   st_operations::deleteOldEventTableRecords(qint32 evtTableLastDays)
	{
		bool res = true;
		QSqlDatabase & db = *m_pDb;
		if (db.isValid()==true && db.isOpen()==true )
		{
			QSqlQuery query(db);
			QString sql = QString("delete from sensorevent where eventtime <= adddate(now(),INTERVAL -%1 DAY);").arg(evtTableLastDays);
			if (false==query.exec(sql))
			{
				qCritical()<<tr("Database Access Error :")+query.lastError().text();
				res = false;
				db.close();
			}
		}
		else
		{
			qCritical()<<tr("Database is not ready.");
			res = false;
		}
		return res;
	}
}
