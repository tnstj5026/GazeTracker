#define _CONTROLWIDGET_H_
#ifdef _CONTROLWIDGET_H_
#pragma once

#include <ctime>

#include <QWidget>
#include <QMessageBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QLabel>

class ControlWidget : public QWidget {
	Q_OBJECT
public:
	ControlWidget(int& gazeTime, int& fakeLastTime, int& subFakeLastTime, int& subFakefeedUpdate, int& startTime, int& gazeProtectCount, int& gazeProtectNum,
		int& caliStartTime, int& incrKey, int& incrDist, int& incrSX, int& incrSY, bool& onCalibration, bool& onGazeProtect, bool& onFeedback, bool& onFakeFeed,
		bool& onSubFake, bool& onGazePoint, bool& onCpas, bool& onShift, bool& onEnter, bool& onBackspace, bool& feedState, bool& fakeState, bool& subFakeState,
		bool& subFakeInitFlag, bool& onTuneKeyInfo, char*& userText);
	~ControlWidget();

	double* getgPoint();
	bool* getOnTracker();

public slots:
	void gazeFeedbackClicked();
	void fakeFeedbackClicked();
	void subFakeFeedbackClicked();
	void gazeProtectClicked();
	void gazeTrackerClicked();
	void gazePointClicked();
	void clearClicked();
	void enterClicked();
	void calibrationClicked();
	void backspaceClicked();
	void keySizeChanged(int i);
	void keyDistChanged(int i);
	void posXChanged(int i);
	void posYChanged(int i);
	void caliXChanged(int i);
	void caliYChanged(int i);
	void fakeRenewalTimeChanged(double i);
	void subFakeUpdateChanged(double i);	
	void subFakeRenewalTimeChanged(double i);
	void dwellTimeChanged(double i);
	void gazeProtectNumChanged(int i);

private:
	void createGazeFakeGroupBox();
	void createNoInputFakeGroupBox();
	void createGazeProtectGroupBox();
	void createCalibrationGroupBox();
	void createOptionalKeyGroupBox();
	void createKeyboardGroupBox();
	void abstractInit();

	QCheckBox *gazeFeedback, *fakeFeedback, *subFakeFeedback, *gazeProtect, *gazePoint, *enter, *backspace;
	QLabel *fakeRenewalTimeLabel, *subFakeUpdateLabel, *gazeProtectNumLabel, *subFakeRenewalTimeLabel, *dwellTimeLabel;
	QLabel *caliXLabel, *caliYLabel, *keySizeLabel, *keyDistLabel, *posXLabel, *posYLabel;
	QDoubleSpinBox *fakeRenewalTime, *subFakeUpdate, *dwellTime, *subFakeRenewalTime;
	QSpinBox *gazeProtectNum, *caliX, *caliY, *keySize, *keyDist, *posX, *posY;
	QPushButton *gazeTracker, *clear, *calibration;
	QGroupBox *gazeFakeGroupBox, *noInputFakeGroupBox, *gazeProtectGroupBox, *calibrationGroupBox, *keyboardGroupBox, *optionalKeyGroupBox;

	int &controlDwellTime, &controlFakeLastTime, &controlSubFakeLastTime, &controlSubFakeUpdate, &controlTrackerStartTime,
		&controlCaliStartTime, &controlGazeProtectCount, &controlGazeProtectNum, &controlIncrKey, &controlIncrDist, &controlIncrSX, &controlIncrSY, expCount;
	bool &controlOnFeedback, &controlOnFakeFeed, &controlOnSubFake, &controlOnGazePoint, &controlOnGazeProtect, &controlOnCaps,
		&controlOnShift, &controlOnEnter, &controlOnBackspace, &controlFeedState, &controlFakeState, &controlSubFakeState,
		&controlSubFakeInitFlag, &controlCalibration, &controlOnTuneKeyInfo;
	char* &controlUserText;
};
#endif