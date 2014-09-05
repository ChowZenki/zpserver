#include "zpmainframe.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include "logger/st_logger.h"

STMsgLogger::st_logger g_logger;

void stMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	g_logger.MessageOutput(type,context,msg);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	//Install message handler
	qInstallMessageHandler(stMessageOutput);

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
	w.setLogger(&g_logger);
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
