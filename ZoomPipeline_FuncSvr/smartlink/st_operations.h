#ifndef ST_OPERATIONS_H
#define ST_OPERATIONS_H

#include <QObject>
#include <QVector>
#include <QSqlDatabase>
#include "st_message.h"
namespace ParkinglotsSvr{



	/**
	 * @brief this class holds all database opertations .
	 *
	 */
	class st_operations : public QObject
	{
		Q_OBJECT
	public:
		explicit st_operations(QSqlDatabase * db, QObject *parent = 0);
	protected:
		QSqlDatabase * m_pDb;
		QString hex2ascii(const quint8 ptr[], int sz);
	public:
		bool   deleteOldEventTableRecords(qint32 evtTableLastDays);
		quint8 regisit_host(QString serialnum, quint32 * host_id );
		quint8 login_host(QString serialnum, quint32 host_id );
		bool   insert_device_table (int nItems, QVector<QString> & vec_dev_names,QVector<QString> & vec_dev_nos,QVector<QString> & vec_dev_ids, quint32 macid);
		bool   insert_mac_table(quint32 macID, QString macSerial,const stMsg_SendMacInfoReq_internal & info);
		quint8 add_new_device(quint32 macid,const quint8 deviceID[/*24*/]);
		quint8 confirm_device(quint32 macid,const quint8 deviceID[/*24*/]);
		quint8 del_old_device(quint32 macid,const quint8 deviceID[/*24*/]);
		quint8 update_DAL_event(quint32 macid, const QByteArray & array_DAL);
		quint8 update_DAL_exception(quint32 macid, const QByteArray & array_DAL);

		bool log_to_macevent(quint32 macevt_sourceid,
											quint32 macevt_destinid,
											qint32  macevt_msgtype,
											qint32  macevt_donecode,
											QString macevt_ip,
											QString macevt_comment
											);
	protected:
		quint8 dal_sensor_0x00(const stEvent_DeviceEvent * pEvent,const quint8 * ptrDal, int nMaxLen);
		quint8 dal_sensor_0x01(const stEvent_DeviceEvent * pEvent,const quint8 * ptrDal, int nMaxLen);
		quint8 dal_sensor_0x02(const stEvent_DeviceEvent * pEvent,const quint8 * ptrDal, int nMaxLen);
		void RecalcSensorStatics(quint32 macid);
	};
}

#endif // ST_OPERATIONS_H
