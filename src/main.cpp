#include <QApplication>
#include <QDesktopWidget>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
	app.setAttribute(Qt::AA_UseDesktopOpenGL);

	MainWindow window(50, 100, 1200, 550);
	window.show();

	return app.exec();
}