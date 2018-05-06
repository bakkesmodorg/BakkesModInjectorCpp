#include "BakkesModInjectorCpp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	BakkesModInjectorCpp w;
	w.show();
	return a.exec();
}
