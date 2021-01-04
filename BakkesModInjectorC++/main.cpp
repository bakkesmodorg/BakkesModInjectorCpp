#include "BakkesModInjectorCpp.h"

#undef _DEBUG

#ifdef _DEBUG
#else
#include <QtWidgets/QStyleFactory>
#include <QtCore/QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QICOPlugin)
Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin)
#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "qtmain.lib")
#pragma comment(lib, "Qt5Core.lib")
#pragma comment(lib, "Qt5Gui.lib")
#pragma comment(lib, "Qt5Network.lib")
#pragma comment(lib, "Qt5Widgets.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "qtharfbuzz.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "winmm.lib")
//#pragma comment(lib, "libpng.lib")
//#pragma comment(lib, "Qt5PlatformSupport.lib")

//#pragma comment(lib, "qtpcre.lib")
//#pragma comment(lib, "libeay32.lib")
//#pragma comment(lib, "ssleay32.lib")

#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "qtfreetype.lib")
#pragma comment(lib, "qtfreetyped.lib")
#include <QtWidgets/QApplication>
#pragma comment(lib, "Qt5FontDatabaseSupport.lib")
#pragma comment(lib, "Qt5EventDispatcherSupport.lib")
#pragma comment(lib, "Qt5Widgets.lib")
#include <QtCore/QtPlugin>
//Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)

#pragma comment(lib, "qwindows.lib")
#pragma comment(lib, "qwindowsvistastyle.lib")
#pragma comment(lib, "qico.lib")
#pragma comment(lib, "libssl_static.lib")
#pragma comment(lib, "libcrypto_static.lib")
#endif
#include <QtCore/QLoggingCategory>
#include <QtCore/qglobal.h>
void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	switch (type)
	{
		case QtDebugMsg:
			LOG_LINE(INFO, "QTDebug: " << msg.toStdString());
			break;
		case QtWarningMsg:
			LOG_LINE(INFO, "QTWarning: " << msg.toStdString());
			//fprintf(stderr, "Warning: %s\n", msg);
			break;
		case QtCriticalMsg:
			LOG_LINE(INFO, "QTCritical: " << msg.toStdString());
			//fprintf(stderr, "Critical: %s\n", msg);
			break;
		case QtFatalMsg:
			LOG_LINE(INFO, "QTFatal: " << msg.toStdString());
			//fprintf(stderr, "Fatal: %s\n", msg);
			//abort();
			break;
	}
}

int main(int argc, char *argv[])
{
	std::wstring TempPath;
	wchar_t wcharPath[MAX_PATH];
	if (GetTempPathW(MAX_PATH, wcharPath))
		TempPath = wcharPath;
	AixLog::Log::init<AixLog::SinkFile>(AixLog::Severity::trace, WindowsUtils::WStringToString(TempPath) + "\\injectorlog.log");
	LOG_LINE(INFO, "Initialized logger for " << BAKKESMODINJECTOR_VERSION);

	if (argc >= 2 && std::string(argv[1]) == "qt")
	{
		qInstallMessageHandler(myMessageOutput);
		QLoggingCategory::setFilterRules("*.debug=true");
	}

	QApplication a(argc, argv);
	LOG_LINE(INFO, "Initialized QApplication")
	BakkesModInjectorCpp w;
	LOG_LINE(INFO, "Constructed BakkesModInjectorCpp")
	a.setStyle(QStyleFactory::create("windowsvista"));
	LOG_LINE(INFO, "Set style")
	w.initialize();
	LOG_LINE(INFO, "Initialized injector")
	w.show();
	LOG_LINE(INFO, "Shown injector")
	return a.exec();
}
