#include "BakkesModInjectorCpp.h"

#ifdef _DEBUG
#else
#pragma comment(lib, "qtmain.lib")
#pragma comment(lib, "Qt5Core.lib")
#pragma comment(lib, "Qt5Gui.lib")
#pragma comment(lib, "Qt5Network.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "qtharfbuzzng.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Qt5PlatformSupport.lib")
#pragma comment(lib, "qwindows.lib")
#pragma comment(lib, "qtpcre.lib")
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "qtfreetype.lib")
#pragma comment(lib, "qico.lib")
#include <QtWidgets/QApplication>

#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QICOPlugin)
#endif

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	BakkesModInjectorCpp w;
	w.initialize();
	w.show();
	return a.exec();
}
