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

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	BakkesModInjectorCpp w;
	a.setStyle(QStyleFactory::create("windowsvista"));
	w.initialize();
	w.show();
	return a.exec();
}
