#ifndef DATABASERESOURCE_H
#define DATABASERESOURCE_H

#include <QObject>
#include <QMap>
#include <QSqlDatabase>
#include <QString>
#include <QMutex>
#include <QThread>
//!this class provide an database reource,
//!In different thread, workers can get existing db connections
//! immediately without re-creation operations.
namespace ZPDatabase{
	class DatabaseResource : public QThread
	{
		Q_OBJECT
	public:
		struct tagConnectionPara{
			QString connName;
			QString type;
			QString  HostAddr;
			int port;
			QString  dbName;
			QString  User;
			QString  Pass;
			QString  ExtraOptions;
			QString  testSQL;
			bool status;
			QString lastError;

		} ;

	public:
		explicit DatabaseResource(QObject *parent = 0);

		//!Get an database connection belong to current thread.
		//!if database does not exist, it will be added using dbtype
		QSqlDatabase  databse(const QString & strDBName);

		//!add connection connName, return true if ok.
		bool addConnection(
				const QString & connName,
				const QString & type,
				const QString & HostAddr,
				int port,
				const QString & dbName,
				const QString & User,
				const QString & Pass,
				const QString & ExtraOptions,
				const QString & testSQL
				);
		void remove_connection(const QString & strDBName);
		void remove_connections();
		bool confirmConnection (const QString & connName);

		QMap <QString,tagConnectionPara> currentDatabaseConnections(){return m_dbNames;}

		void run();

		void TerminateMe(){bTerm = true;}

	protected:
		bool bTerm;
		QMutex m_mutex_reg;
		QMap <QString,tagConnectionPara> m_dbNames;
	signals:
		void evt_Message(const QString &);
	public slots:

	};
};
#endif // DATABASERESOURCE_H
