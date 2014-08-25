#ifndef ST_LOGGER_H
#define ST_LOGGER_H

#include <QObject>
#include <QDateTime>
#include <QDir>
#include <QByteArray>
#include <QCoreApplication>
namespace STMsgLogger{
	class st_logger : public QObject
	{
		Q_OBJECT
	public:
		explicit st_logger(QObject *parent = 0);
		void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
		int maxFileSize() const;
		int logLevel() const;
	protected:
		bool CreateNewLogFile(QCoreApplication * app);
		QFile * m_pLogFile;
		bool m_bUseLogFile;
		QString m_currLogFileName;
		int m_nLogLevel;
		int m_nMaxFileSize;
	signals:

	public slots:
		void setMaxFileSize(int nSize);
		void setLogLevel(int);

	};
}
#endif // ST_LOGGER_H
