#define _KEYBOARDWIDGET_H_
#ifdef _KEYBOARDWIDGET_H_
#pragma once

#include <QGLWidget>
#include "controlwidget.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <stack>

#define BTN_SIZE_STD 45
#define BTN_SIZE_1 65
#define BTN_SIZE_2 85
#define BTN_SIZE_3 118
#define BTN_SIZE_4 138
#define BTN_SIZE_5 164
#define BTN_SIZE_6 98
#define BTN_SIZE_7 65
#define KEYPAD_SX 35
#define KEYPAD_SY 140
#define BTN_X_GAP 8
#define BTN_Y_GAP 8

using namespace std;

typedef QPoint RowColumn;

class KeyboardWidget : public QGLWidget {
	Q_OBJECT
public:
	KeyboardWidget();
	~KeyboardWidget();

	int &getDwellTime();
	int &getFakeLastTime();
	int &getSubFakeLastTime();
	int &getSubFakeUpdate();
	int &getStartTime();
	int &getGazeProtectCount();
	int &getGazeProtectNum();
	int &getCaliStartTime();
	int &getIncrKey();
	int &getIncrDist();
	int &getIncrSX();
	int &getIncrSY();
	bool &getOnCalibration();
	bool &getOnGazeProtect();
	bool &getOnFeedback();
	bool &getOnFakeFeed();
	bool &getOnSubFake();
	bool &getOnGazePoint();
	bool &getOnCaps();
	bool &getOnShift();
	bool &getOnEnter();
	bool &getOnBackspace();
	bool &getOnTuneKeyInfo();
	bool &getFeedState();
	bool &getFakeState();
	bool &getSubFakeState();
	bool &getSubFakeInitFlag();
	char* &getUserText();
	void setCWidgetVars(ControlWidget *param);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	void mousePressEvent(QMouseEvent *event);
	GLint LoadPNGtoTexture(char *filename);

private:
	void initKeypadInfo();
	void modifyKeypadInfo();
	void btnsInfo();
	void setKeyImageTextures();

	RowColumn getRowColumn(int x, int y);

	void classfyingKeys(RowColumn param);
	void addNewInputText(char* inputText);
	void drawUserText(double x, double y, double z, int fontSize, QString txt);
	void gazeInput();
	void gazePoint(double gazeX, double gazeY, double radius, double color[3]);
	void rendering(GLint imagePath, GLfloat color[3], int width[2], int height[2]);
	void renderingMuti(RowColumn* param, GLfloat color[3], int startIndex, int endIndex);
	void calculateProbability();

	void feedback();
	void fakeFeedback();
	void subFakeFeedback();
	void subFakeActCheck();
	void findFakeIndex(RowColumn* param, int fakeNum, int excl);
	void findSubFakeIndex(RowColumn* param, int fakeNum, int excl);
	bool Isdissatisfied(int nomineeRow, int nomineeColumn, int chosenRow, int chosenColumn);

private:
	ControlWidget *CWidget;

	float transform[4][4];
	int viewport_width[2];
	int viewport_height[2];

	char* userText = "";
	char starText[125] = "";
	char stackText[125] = "";

	double* gX = NULL;
	double* gY = NULL;
	bool* keyboardOnTracker = NULL;

	int btnScale[8] = { BTN_SIZE_STD, BTN_SIZE_1, BTN_SIZE_2, BTN_SIZE_3, BTN_SIZE_4, BTN_SIZE_5, BTN_SIZE_6, BTN_SIZE_7 };
	int keyPadSX = KEYPAD_SX;
	int keyPadSY = KEYPAD_SY;
	int btnXGap = BTN_X_GAP;
	int btnYGap = BTN_Y_GAP;
	int incrDist = 0;
	int incrKey = 0;
	int incrSX = 0;
	int incrSY = 0;

	int** keyName;
	int keyMaxNum[5];
	struct KEYPAD{
		int x[2];
		int y[2];
		int btnSize;
		int ratio;
	};
	KEYPAD** keyBtns;
	int ratioNum;

	GLint*** keyBtnTextures;

	RowColumn gazeRowColumn[3];
	RowColumn currRowColumn;
	RowColumn prevRowColumn = RowColumn(-1, -1);
	RowColumn* chosenIndex;
	RowColumn* subFakeIndex;
	
	GLfloat std_color[3] = {1, 1, 1};
	GLfloat red_color[3] = {1, 0.5, 0.5};
	GLfloat blue_color[3][3] = { { 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 } };
	double gazeColor[3] = { 0.852, 0.792, 0.445 };

	int startOfFakeLast;
	int startOfDwellTime;
	int startOfSubFakeUpdate;
	int	startOfSubFakeLast;
	int startOfCali;
	int dwellTime;
	int fakeLastTime;
	int subFakeLastTime;
	int subFakeUpdate;
	int gazeProtectNum;
	int gazeProtectCount = 0;
	bool probability = false;
	bool feedState = false;
	bool fakeState = false;
	bool subFakeState = false;
	bool subFakeInitFlag = true;
	bool onFeedback = false;
	bool onFakeFeed = false;
	bool onSubFake = false;
	bool onGazePoint = false;
	bool onGazeProtect = false;
	bool onCaps = false;
	bool onShift = false;
	bool onEnter = false;
	bool onBackspace = false;
	bool onCalibration = false;
	bool onReady = true;
	bool onInit = false;
	bool onTuneKeyInfo = false;	
};
#endif