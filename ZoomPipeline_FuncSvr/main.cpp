#include "zpmainframe.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

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

	//!the main program arg formats:
	/*!  ZoomPipeline_FuncSvr [[options] <config file name> [options]]
	  *  When start with no commandline arguments, the server will enter a dialog-controlled UI model.
	  *  If the commandline args has been specified, this server will enter an aut-config and start model.
	  *  Cmdline formats:
	  *  --ui (default) 	the program will start with an UI
	  *  --noui				the program will start without an UI
	  *  config file has a same format with UI-Saved ini file. this file name should be surrounded with ""
	  *  if there are spaces in filename.
	*/
	//Command Line Args, support batch auto-config and auto start.
	if (argc>1)
	{
		bool bHasUI = false;
		QString configfile;
		for (int i=1;i < argc;++i)
		{
			QString strArg = argv[i];
			if (-1!=strArg.indexOf("--ui"))
				bHasUI = true;
			else if (-1!=strArg.indexOf("--noui"))
				;
			else
			{
				configfile = strArg;
			}
		}
		w.LoadSettingsAndForkServer(configfile);
	}

	w.show();
	int pp = app.exec();
	return pp;
}
