#ifndef ST_CLIENT_TABLE_H
#define ST_CLIENT_TABLE_H

#include <QObject>
#include <QList>
#include <QMutex>
#include <QMap>
#include "../network/zp_net_threadpool.h"
#include "../pipeline/zp_pipeline.h"
#include "./st_message.h"
#include "../database/databaseresource.h"
namespace SmartLink{
	class st_clientNode_baseTrans;
	class st_client_table : public QObject
	{
		Q_OBJECT
	public:
		explicit st_client_table(ZPNetwork::zp_net_Engine * pool, ZPTaskEngine::zp_pipeline * taskeng, ZPDatabase::DatabaseResource *pDb, QObject *parent = 0);
		~st_client_table();

		bool regisitClientUUID(st_clientNode_baseTrans *);
		st_clientNode_baseTrans * clientNodeFromUUID(quint32);
		st_clientNode_baseTrans * clientNodeFromSocket(QObject *);

		//Heart beating and healthy
		void KickDeadClients();
		int heartBeatingThrd();
		void setHeartBeatingThrd(int h) ;

		//Database and disk resources
		QString Database_UserAcct();
		void setDatabase_UserAcct(const QString & s);
		QString Database_Event();
		void setDatabase_Event(const QString & s);
		QString largeFileFolder();
		void setLargeFileFolder(const QString & s);

		ZPDatabase::DatabaseResource * dbRes();
	protected:
		//This list hold dead nodes that still in task queue,avoiding crash
		QList<st_clientNode_baseTrans *> m_nodeToBeDel;

		//Very important hashes. will be improved for cross-server transfer
		QMutex m_hash_mutex;
		QMap<quint32,st_clientNode_baseTrans *> m_hash_uuid2node;
		QMap<QObject *,st_clientNode_baseTrans *> m_hash_sock2node;

		//Concurrent Network frame work
		ZPNetwork::zp_net_Engine * m_pThreadPool;
		//The piple-line
		ZPTaskEngine::zp_pipeline * m_pTaskEngine;
		//The database pool
		ZPDatabase::DatabaseResource * m_pDatabaseRes;

		//The max seconds before dead client be kicked out
		int m_nHeartBeatingDeadThrd;
		//Database Resource Names used by nodes
		QString m_strDBName_useraccount;
		QString m_strDBName_event;
		QString m_largeFileFolder;


	signals:
		void evt_Message (QObject * psource,const QString &);

	public slots:
		//this event indicates new client connected.
		void on_evt_NewClientConnected(QObject * /*clientHandle*/);
		//this event indicates new client encrypted.
		void on_evt_ClientEncrypted(QObject * /*clientHandle*/);
		//this event indicates a client disconnected.
		void on_evt_ClientDisconnected(QObject * /*clientHandle*/);
		//some data arrival
		void on_evt_Data_recieved(QObject *  /*clientHandle*/,const QByteArray & /*datablock*/ );
		//a block of data has been successfuly sent
		void on_evt_Data_transferred(QObject *   /*clientHandle*/,qint64 /*bytes sent*/);

	};
}
#endif // ST_CLIENT_TABLE_H
