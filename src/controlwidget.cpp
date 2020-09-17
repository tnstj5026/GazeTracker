#include "controlwidget.h"
#include "gazetracker.h"
#include <iostream>

#include <QtGui>
#include <QKeyEvent>
#include <QtOpenGL>

ControlWidget::ControlWidget(int& gazeTime, int& fakeLastTime, int& subFakeLastTime, int& subFakefeedUpdate, int& startTime, int& gazeProtectCount,
	int& gazeProtectNum, int& caliStartTime, int& incrKey, int& incrDist, int& incrSX, int& incrSY, bool& onCalibration, bool& onGazeProtect, bool& onFeedback, bool& onFakeFeed, bool& onSubFake,
	bool& onGazePoint, bool& onCaps, bool& onShift, bool& onEnter, bool& onBackspace, bool& feedState, bool& fakeState, bool& subFakeState,
	bool& subFakeInitFlag, bool& onTuneKeyInfo, char*& userText)
	: controlDwellTime(gazeTime), controlFakeLastTime(fakeLastTime), controlSubFakeLastTime(subFakeLastTime), controlSubFakeUpdate(subFakefeedUpdate),
	controlTrackerStartTime(startTime), controlGazeProtectCount(gazeProtectCount), controlGazeProtectNum(gazeProtectNum), controlCaliStartTime(caliStartTime),
	controlIncrKey(incrKey), controlIncrDist(incrDist), controlIncrSX(incrSX), controlIncrSY(incrSY), controlCalibration(onCalibration), controlOnGazeProtect(onGazeProtect),
	controlOnFeedback(onFeedback), controlOnFakeFeed(onFakeFeed), controlOnSubFake(onSubFake), controlOnGazePoint(onGazePoint), controlOnCaps(onCaps), controlOnShift(onShift),
	controlOnEnter(onEnter), controlOnBackspace(onBackspace), controlFeedState(feedState), controlFakeState(fakeState), controlSubFakeState(subFakeState), controlSubFakeInitFlag(subFakeInitFlag),
	controlOnTuneKeyInfo(onTuneKeyInfo), controlUserText(userText) {
	createGazeFakeGroupBox();
	createNoInputFakeGroupBox();
	createGazeProtectGroupBox();
	createCalibrationGroupBox();
	createOptionalKeyGroupBox();
	createKeyboardGroupBox();

	QGridLayout* layout = new QGridLayout;
	gazeTracker = new QPushButton("Gaze Tracker On", this);
	clear = new QPushButton("Clear", this);
	calibration = new QPushButton("Calibration", this);
	layout->addWidget(gazeFakeGroupBox, 0, 0, 2, 2);
	layout->addWidget(noInputFakeGroupBox, 2, 0, 2, 2);
	layout->addWidget(keyboardGroupBox, 4, 0, 2, 2);
	layout->addWidget(gazeProtectGroupBox, 6, 0, 1, 2);
	layout->addWidget(calibrationGroupBox, 7, 0, 1, 2);
	layout->addWidget(optionalKeyGroupBox, 8, 0, 1, 2);
	layout->addWidget(gazeTracker, 9, 0, 1, 1);
	layout->addWidget(clear, 9, 1, 1, 1);
	layout->addWidget(calibration, 10, 0, 1, 2);

	setLayout(layout);

	abstractInit();

	connect(gazeTracker, SIGNAL(clicked()), this, SLOT(gazeTrackerClicked()));
	connect(clear, SIGNAL(clicked()), this, SLOT(clearClicked()));
	connect(calibration, SIGNAL(clicked()), this, SLOT(calibrationClicked()));

	FILE* stream = fopen("./Data/data.txt", "wt");
	fputs("----------------------------------\n", stream);
	fputs("| The results of each experiment |\n", stream);
	fputs("----------------------------------\n", stream);
	fclose(stream);
	expCount = 1;
}

ControlWidget::~ControlWidget() {
}

void ControlWidget::createGazeFakeGroupBox() {
	gazeFakeGroupBox = new QGroupBox(tr("Gaze Feedback and Fake Feedback"));
	gazeFeedback = new QCheckBox("Gaze Feedback", this);
	fakeFeedback = new QCheckBox("Fake Feedback", this);
	fakeRenewalTimeLabel = new QLabel(tr("Active Time:"));
	fakeRenewalTime = new QDoubleSpinBox(this);
	dwellTimeLabel = new QLabel(tr("Dwell Time:"));
	dwellTime = new QDoubleSpinBox(this);

	dwellTime->setDecimals(1);
	dwellTime->setSingleStep(0.1);
	fakeRenewalTime->setDecimals(1);
	fakeRenewalTime->setSingleStep(0.1);
	
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(gazeFeedback, 0, 0, 1, 3);
	layout->addWidget(fakeFeedback, 0, 3, 1, 3);
	layout->addWidget(dwellTimeLabel, 1, 0, 1, 1);
	layout->addWidget(dwellTime, 1, 1, 1, 1);
	layout->addWidget(fakeRenewalTimeLabel, 1, 3, 1, 1);
	layout->addWidget(fakeRenewalTime, 1, 4, 1, 1);
	
	gazeFakeGroupBox->setLayout(layout);

	connect(gazeFeedback, SIGNAL(clicked()), this, SLOT(gazeFeedbackClicked()));
	connect(fakeFeedback, SIGNAL(clicked()), this, SLOT(fakeFeedbackClicked()));
	connect(fakeRenewalTime, SIGNAL(valueChanged(double)), this, SLOT(fakeRenewalTimeChanged(double)));
	connect(dwellTime, SIGNAL(valueChanged(double)), this, SLOT(dwellTimeChanged(double)));
}

void ControlWidget::createNoInputFakeGroupBox() {
	noInputFakeGroupBox = new QGroupBox(tr("No Input Fake Feedback"));	
	subFakeFeedback = new QCheckBox("Fake Feedback", this);
	subFakeUpdateLabel = new QLabel(tr("Update Time:"));
	subFakeUpdate = new QDoubleSpinBox(this);
	subFakeRenewalTimeLabel = new QLabel(tr("Active Time:"));
	subFakeRenewalTime = new QDoubleSpinBox(this);

	subFakeUpdate->setDecimals(1);
	subFakeUpdate->setSingleStep(0.1);
	subFakeRenewalTime->setDecimals(1);
	subFakeRenewalTime->setSingleStep(0.1);
	
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(subFakeFeedback, 0, 0, 1, 3);
	layout->addWidget(subFakeUpdateLabel, 1, 0, 1, 1);
	layout->addWidget(subFakeUpdate, 1, 1, 1, 1);
	layout->addWidget(subFakeRenewalTimeLabel, 1, 3, 1, 1);
	layout->addWidget(subFakeRenewalTime, 1, 4, 1, 1);
	
	noInputFakeGroupBox->setLayout(layout);

	connect(subFakeFeedback, SIGNAL(clicked()), this, SLOT(subFakeFeedbackClicked()));
	connect(subFakeUpdate, SIGNAL(valueChanged(double)), this, SLOT(subFakeUpdateChanged(double)));
	connect(subFakeRenewalTime, SIGNAL(valueChanged(double)), this, SLOT(subFakeRenewalTimeChanged(double)));
}

void ControlWidget::createKeyboardGroupBox() {
	keyboardGroupBox = new QGroupBox(tr("Keyboard"));
	keySizeLabel = new QLabel(tr("Size:"));
	keySize = new QSpinBox(this);
	keyDistLabel = new QLabel(tr("Distance:"));
	keyDist = new QSpinBox(this);
	posXLabel = new QLabel(tr("X's Pos:"));
	posX = new QSpinBox(this);
	posYLabel = new QLabel(tr("Y's Pos:"));
	posY = new QSpinBox(this);

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(keySizeLabel, 0, 0, 1, 1);
	layout->addWidget(keySize, 0, 1, 1, 1);
	layout->addWidget(keyDistLabel, 0, 2, 1, 1);
	layout->addWidget(keyDist, 0, 3, 1, 1);
	layout->addWidget(posXLabel, 1, 0, 1, 1);
	layout->addWidget(posX, 1, 1, 1, 1);
	layout->addWidget(posYLabel, 1, 2, 1, 1);
	layout->addWidget(posY, 1, 3, 1, 1);

	keyboardGroupBox->setLayout(layout);

	connect(keySize, SIGNAL(valueChanged(int)), this, SLOT(keySizeChanged(int)));
	connect(keyDist, SIGNAL(valueChanged(int)), this, SLOT(keyDistChanged(int)));
	connect(posX, SIGNAL(valueChanged(int)), this, SLOT(posXChanged(int)));
	connect(posY, SIGNAL(valueChanged(int)), this, SLOT(posYChanged(int)));
}

void ControlWidget::createGazeProtectGroupBox() {
	gazeProtectGroupBox = new QGroupBox(tr("Gaze Protection"));
	gazeProtect = new QCheckBox("Gaze Protection", this);
	gazeProtectNumLabel = new QLabel(tr("Protection #:"));
	gazeProtectNum = new QSpinBox(this);

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(gazeProtect, 0, 0, 1, 3);
	layout->addWidget(gazeProtectNumLabel, 0, 3, 1, 1);
	layout->addWidget(gazeProtectNum, 0, 4, 1, 1);

	gazeProtectGroupBox->setLayout(layout);

	connect(gazeProtect, SIGNAL(clicked()), this, SLOT(gazeProtectClicked()));
	connect(gazeProtectNum, SIGNAL(valueChanged(int)), this, SLOT(gazeProtectNumChanged(int)));
}

void ControlWidget::createCalibrationGroupBox() {
	calibrationGroupBox = new QGroupBox(tr("Calibration"));
	gazePoint = new QCheckBox("Gaze Point", this);
	caliXLabel = new QLabel(tr("X:"));
	caliX = new QSpinBox(this);
	caliYLabel = new QLabel(tr("Y:"));
	caliY = new QSpinBox(this);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(gazePoint);
	layout->addWidget(caliXLabel);
	layout->addWidget(caliX);
	layout->addWidget(caliYLabel);
	layout->addWidget(caliY);

	calibrationGroupBox->setLayout(layout);

	connect(gazePoint, SIGNAL(clicked()), this, SLOT(gazePointClicked()));
	connect(caliX, SIGNAL(valueChanged(int)), this, SLOT(caliXChanged(int)));
	connect(caliY, SIGNAL(valueChanged(int)), this, SLOT(caliYChanged(int)));
}

void ControlWidget::createOptionalKeyGroupBox() {
	optionalKeyGroupBox = new QGroupBox(tr("Optional Key"));
	enter = new QCheckBox("Enter", this);
	backspace = new QCheckBox("BackSpace", this);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(enter);
	layout->addWidget(backspace);

	optionalKeyGroupBox->setLayout(layout);

	connect(enter, SIGNAL(clicked()), this, SLOT(enterClicked()));
	connect(backspace, SIGNAL(clicked()), this, SLOT(backspaceClicked()));
}

void ControlWidget::abstractInit() {
	gazeProtectNum->setRange(0, 10);
	gazeProtectNum->valueChanged(0);
	gazeProtectNum->setValue(0);

	subFakeUpdate->setRange(0, 10);
	subFakeUpdate->valueChanged(2);
	subFakeUpdate->setValue(2);

	dwellTime->setRange(0, 10);
	dwellTime->valueChanged(2);
	dwellTime->setValue(2);

	fakeRenewalTime->setRange(0, 10);
	fakeRenewalTime->valueChanged(2);
	fakeRenewalTime->setValue(2);

	subFakeRenewalTime->setRange(0, 10);
	subFakeRenewalTime->valueChanged(2);
	subFakeRenewalTime->setValue(2);

	caliX->setRange(-800, 800);
	caliY->setRange(-800, 800);
	caliX->valueChanged(95);
	caliY->valueChanged(135);
	caliX->setValue(95);
	caliY->setValue(135);

	keySize->setRange(0, 100);
	keyDist->setRange(0, 10);
	posX->setRange(0, 200);
	posY->setRange(0, 200);

	gazeFeedback->setCheckState(Qt::Unchecked);
	fakeFeedback->setCheckState(Qt::Unchecked);
	subFakeFeedback->setCheckState(Qt::Unchecked);
	gazePoint->setCheckState(Qt::Unchecked);
	gazeProtect->setCheckState(Qt::Unchecked);

	controlOnFeedback = false;
	controlOnFakeFeed = false;
	controlOnSubFake = false;
	controlOnGazePoint = false;
	controlOnGazeProtect = false;
	controlFeedState = false;
	controlFakeState = false;
	controlSubFakeState = false;
	controlSubFakeInitFlag = true;
	controlOnCaps = false;
	controlOnShift = false;
	controlOnEnter = false;
	controlOnBackspace = false;
}

void ControlWidget::gazeFeedbackClicked() {
	if (onTracker) {
		if (gazeFeedback->isChecked()) controlOnFeedback = true;
		else controlOnFeedback = false;
	}
	else {
		gazeFeedback->setCheckState(Qt::Unchecked);
	}	
}

void ControlWidget::fakeFeedbackClicked() {
	if (onTracker) {
		if (fakeFeedback->isChecked()) controlOnFakeFeed = true;
		else controlOnFakeFeed = false;
	}
	else {
		fakeFeedback->setCheckState(Qt::Unchecked);
	}	
}

void ControlWidget::subFakeFeedbackClicked() {
	if (onTracker) {
		if (subFakeFeedback->isChecked()) {
			controlOnSubFake = true;

			gazeProtectNum->setValue(0);
			gazeProtectNum->valueChanged(0);
		}
		else {
			gazeProtectNum->valueChanged(0);
			gazeProtectNum->setValue(0);
			
			controlOnGazeProtect = false;
			gazeProtect->setCheckState(Qt::Unchecked);

			controlOnSubFake = false;
		}
	}
	else {
		subFakeFeedback->setCheckState(Qt::Unchecked);
	}
}

void ControlWidget::gazeProtectClicked() {
	if (controlOnSubFake) {
		if (gazeProtect->isChecked()) {
			controlOnGazeProtect = true;			
		}
		else {
			gazeProtectNum->setValue(0);
			gazeProtectNum->valueChanged(0);

			controlGazeProtectCount = 0;
			controlOnGazeProtect = false;
		}
	}
	else {
		gazeProtect->setCheckState(Qt::Unchecked);
	}
}

void ControlWidget::gazeTrackerClicked() {
	if (onTracker) {
		gazetracker_destroy();		
		gazeTracker->setText("Gaze Tracker On");
		
		abstractInit();
		onTracker = false;
	}
	else{
		gazetracker_init();		
		gazeTracker->setText("Gaze Tracker Off");
		controlTrackerStartTime = clock();

		onTracker = true;
		abstractInit();
	}
}

void ControlWidget::gazePointClicked() {
	if (onTracker) {
		if (gazePoint->isChecked()) controlOnGazePoint = true;
		else controlOnGazePoint = false;
	}
	else {
		gazePoint->setCheckState(Qt::Unchecked);
	}
}

void ControlWidget::clearClicked() {
	controlSubFakeState = false;
	controlFeedState = false;
	controlFakeState = false;
	onTracker = false;

	QMessageBox msgBox;
	msgBox.setWindowFlags(Qt::WindowTitleHint | Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint);
	msgBox.setStyleSheet("QLabel{min-width: 225px; min-height: 50px;}");
	msgBox.setWindowTitle(tr("Save"));
	msgBox.setText("Do you want to save your input data?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
	msgBox.setDefaultButton(QMessageBox::Save);
	msgBox.setEscapeButton(QMessageBox::Discard);

	int ret = msgBox.exec();
	if (ret == QMessageBox::Save) {
		FILE* stream = fopen("./Data/data.txt", "at");
		fprintf(stream, "The results of %d experiment:\n", expCount);
		fputs(controlUserText, stream);
		fputs("\n\n\n", stream);
		fclose(stream);
		expCount += 1;
	}
	else if (QMessageBox::Discard) {
	}
	
	controlUserText = "";
	controlSubFakeInitFlag = true;
	controlGazeProtectCount = 0;
	if (gazeTracker->text() == "Gaze Tracker Off") onTracker = true;
}

void ControlWidget::enterClicked() {
	if (enter->isChecked()) controlOnEnter = true;
	else controlOnEnter = false;
}

void ControlWidget::calibrationClicked() {
	if (!onTracker) gazeTrackerClicked();
	controlCalibration = true;
	controlCaliStartTime = clock();
}

void ControlWidget::backspaceClicked() {
	if (backspace->isChecked()) controlOnBackspace = true;
	else controlOnBackspace = false;
}

void ControlWidget::keySizeChanged(int i) {
	controlIncrKey = i;
	controlOnTuneKeyInfo = true;
}

void ControlWidget::keyDistChanged(int i) {
	controlIncrDist = i;
	controlOnTuneKeyInfo = true;
}

void ControlWidget::posXChanged(int i) {
	controlIncrSX = i;
	controlOnTuneKeyInfo = true;
}

void ControlWidget::posYChanged(int i) {
	controlIncrSY = i;
	controlOnTuneKeyInfo = true;
}

void ControlWidget::caliXChanged(int i) {
	if (onTracker) {
		calibrationX = i;
	}
	else {
		caliX->setValue(95);
	}
}

void ControlWidget::caliYChanged(int i) {
	if (onTracker) {
		calibrationY = i;
	}
	else {
		caliY->setValue(135);
	}
}

void ControlWidget::fakeRenewalTimeChanged(double i) {
	if (onTracker) {
		controlFakeLastTime = i * 10;
	}
	else {
		fakeRenewalTime->setValue(2);
	}
}

void ControlWidget::subFakeUpdateChanged(double i) {
	if (onTracker) {
		controlSubFakeUpdate = i * 10;
	}
	else {
		subFakeUpdate->setValue(2);
	}
}

void ControlWidget::subFakeRenewalTimeChanged(double i) {
	if (onTracker) {
		controlSubFakeLastTime = i * 10;
	}
	else {
		subFakeRenewalTime->setValue(2);
	}
}

void ControlWidget::dwellTimeChanged(double i) {
	if (onTracker) {
		controlDwellTime = i * 10;
	}
	else {
		dwellTime->setValue(2);
	}
}

void ControlWidget::gazeProtectNumChanged(int i) {
	if (controlOnSubFake) {
		controlGazeProtectCount = 0;
		controlGazeProtectNum = i;
	}
	else {
		gazeProtectNum->setValue(0);
	}
}

double* ControlWidget::getgPoint() {
	return gPoint;
}

bool* ControlWidget::getOnTracker() {
	return &onTracker;
}