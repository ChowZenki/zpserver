#include "zpmainframe.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDateTime>
#include <QDir>
#include <QByteArray>
namespace STMsgLogger{
	QFile * g_pLogFile = 0;
	bool bUseLogFile = true;
	QString logFilePrefix;

	bool CreateNewLogFile(QCoreApplication * app)
	{
		bool res = false;
		QDateTime dtmCur = QDateTime::currentDateTime();
		STMsgLogger::logFilePrefix = app->applicationDirPath() + "/Log/" + dtmCur.toString("yyyy-MM-dd") + "/";
		QDir dir;
		dir.mkpath(STMsgLogger::logFilePrefix);
		STMsgLogger::logFilePrefix += dtmCur.toString("yyyy_MM_dd_HH_mm_ss_");
		STMsgLogger::logFilePrefix += app->applicationName() + QString("(%1).txt").arg(app->applicationPid());
		if (g_pLogFile)
		{
			if (g_pLogFile->isOpen()==true)
				g_pLogFile->close();
			g_pLogFile->deleteLater();
			g_pLogFile = 0;
		}
		g_pLogFile = new QFile (STMsgLogger::logFilePrefix);
		if (g_pLogFile)
		{
			if (g_pLogFile->open(QIODevice::WriteOnly)==false)
			{
				g_pLogFile->deleteLater();
				g_pLogFile = 0;
			}
			else
				res = true;
		}
		return res;
	}
	void stMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
	{
		if (g_pLogFile==0)
		{
			if (bUseLogFile==true)
				bUseLogFile = CreateNewLogFile(QCoreApplication::instance());
			if (g_pLogFile==0)
				return;
		}

		QDateTime dtmCur = QDateTime::currentDateTime().toUTC();

		QString strMsg = dtmCur.toString("yyyy-MM-dd HH:mm:ss.zzz");
		QString strMsgHeader = dtmCur.toString("yyyy-MM-dd HH:mm:ss.zzz");
		strMsg += "(UTC)>";
		strMsgHeader += "(UTC)>";
		switch (type) {
		case QtDebugMsg:
			strMsg += QString("Debug   :");
			break;
		case QtWarningMsg:
			strMsg += QString("Warning :");
			break;
		case QtCriticalMsg:
			strMsg += QString("Critical:");
			break;
		case QtFatalMsg:
			strMsg += QString("Fatal   :");
			break;
		default:
			strMsg += QString("Unknown :");
			break;
		}
		strMsg.append(msg);
		strMsg.append("\n");
		strMsgHeader += QString("         From {%1:%2,%3}\n").arg(QString(context.file)).arg(QString(context.line)).arg(QString(context.function));
		strMsg.append(strMsgHeader);

		QTextStream stream(g_pLogFile);
		stream << strMsg;
		stream.flush();
		if (g_pLogFile->pos()>=256*1024*1024)
			bUseLogFile = CreateNewLogFile(QCoreApplication::instance());
	}

}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	//Install message handler
	 qInstallMessageHandler(STMsgLogger::stMessageOutput);

	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name(),
					  QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&qtTranslator);

	QTranslator appTranslator;
	QString strTransLocalFile =
			QCoreApplication::applicationDirPath()+"/" +
			QCoreApplication::applicationName()+"_"+
			QLocale::system().name()+".qm";
	appTranslator.load(strTransLocalFile );
	app.installTranslator(&appTranslator);


	ZPMainFrame w;
	w.show();

	//!the main program arg formats:
	/*!  ZoomPipeline_FuncSvr [<--autostart> [config file name] ]
	  *  When start with no commandline arguments, the server will enter a dialog-controlled UI model.
	  *  If the commandline args has been specified, this server will enter an aut-config and start model.
	  *  Cmdline formats:
	  *  --autostart  	specify the cluster address to join.
	  *  config file has a same format with UI-Saved ini file. this file name should be surrounded with ""
	  *  if there are spaces in filename.
	*/
	//Command Line Args, support batch auto-config and auto start.
	if (argc>1)
	{
		QString configfile;
		for (int i=1;i < argc;++i)
		{
			QString strArg = argv[i];
			if (-1!=strArg.indexOf("--autostart"))
			{
				if (++i < argc)
					configfile = argv[i];
				w.LoadSettingsAndForkServer(configfile);
				break;
			}
		}
	}


	int pp = app.exec();
	return pp;
}
