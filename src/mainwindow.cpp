#include "mainwindow.h"

MainWindow::MainWindow(int x, int y, int l, int w){
	setWindowTitle(tr("X-MAS Gaze Test Tool - QT5.7"));
	
	CentralWidget = new QWidget;
	setCentralWidget(CentralWidget);

	QGridLayout* layout = new QGridLayout;
	
	KWidget = new KeyboardWidget;
	CWidget = new ControlWidget(KWidget->getDwellTime(), KWidget->getFakeLastTime(), KWidget->getSubFakeLastTime(), KWidget->getSubFakeUpdate(),
		KWidget->getStartTime(), KWidget->getGazeProtectCount(), KWidget->getGazeProtectNum(), KWidget->getCaliStartTime(), KWidget->getIncrKey(),
		KWidget->getIncrDist(), KWidget->getIncrSX(), KWidget->getIncrSY(), KWidget->getOnCalibration(), KWidget->getOnGazeProtect(), KWidget->getOnFeedback(),
		KWidget->getOnFakeFeed(), KWidget->getOnSubFake(), KWidget->getOnGazePoint(), KWidget->getOnCaps(), KWidget->getOnShift(), KWidget->getOnEnter(),
		KWidget->getOnBackspace(), KWidget->getFeedState(), KWidget->getFakeState(), KWidget->getSubFakeState(), KWidget->getSubFakeInitFlag(),
		KWidget->getOnTuneKeyInfo(), KWidget->getUserText());
	KWidget->setCWidgetVars(CWidget);

	layout->addWidget(KWidget, 0, 0, 1, 5);
	layout->addWidget(CWidget, 0, 5, 1, 1);
	
	CentralWidget->setLayout(layout);
	
	this->setGeometry(x, y, l, w);
}

MainWindow::~MainWindow(){
}