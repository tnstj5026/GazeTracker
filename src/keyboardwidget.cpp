#include "keyboardwidget.h"
#include <iostream>
#include <fstream>
#include <string.h>

#include <QtGui>
#include <QKeyEvent>
#include <QtOpenGL>

using namespace std;

extern int decodePNG(std::vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);

#define MAXCHARNUM 30
#define FAKENUM 9
#define MILISEC 100

KeyboardWidget::KeyboardWidget() {
	chosenIndex = new RowColumn[FAKENUM + 1];
	subFakeIndex = new RowColumn[FAKENUM + 2];
	srand(time(NULL));
	
	initKeypadInfo();
	btnsInfo();
}

KeyboardWidget::~KeyboardWidget() {
	delete[] keyBtns;

	for (int i = 0; i < 5; i++)	{
		delete[] keyBtnTextures[i];
		delete[] keyName[i];
	}

	delete[] keyBtnTextures;
	delete[] keyName;

	delete[] chosenIndex;
	delete[] subFakeIndex;
}

void KeyboardWidget::initializeGL(){
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glShadeModel(GL_SMOOTH);

	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *)this->transform);
	glPopMatrix();

	setKeyImageTextures();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void KeyboardWidget::paintGL(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (onTuneKeyInfo) {
		// renew information of keypad
		modifyKeypadInfo();

		onTuneKeyInfo = false;
	}
	
	// draw key-image textures corresponding to buttons
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < keyMaxNum[i]; j++) {
			rendering(keyBtnTextures[i][j][0], std_color, keyBtns[i][j].x, keyBtns[i][j].y);
		}
	}

	// let userTest be shown in GL screen
	if (strcmp(starText, "")) {
		drawUserText(280, 100, 0, 20, starText);
	}

	// make no input fake feedback visible
	if (onSubFake && subFakeState) {
		subFakeFeedback();
	}

	// make feedback visible
	if (onFeedback && feedState) {
		feedback();
	}

	// make fake feedback visible
	if (onFakeFeed && fakeState) {
		fakeFeedback();
	}

	if (*keyboardOnTracker) {
		// process the task related to text and feedback
		gazeInput();

		// let the gaze point be shown in screen
		if (onGazePoint) {
			gazePoint(*gX, *gY, 5.0, gazeColor);
		}
	}

	glFlush();
	update();
}

void KeyboardWidget::resizeGL(int width, int height){
	viewport_width[0] = 0;
	viewport_height[0] = 0;
	viewport_width[1] = width;
	viewport_height[1] = height;

	glViewport(0, 0, viewport_width[1], viewport_height[1]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewport_width[1], viewport_height[1], 0, 0.0, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void KeyboardWidget::mousePressEvent(QMouseEvent *event){
	if ((event->x() >= keyPadSX && event->x() <= (keyPadSX + (btnScale[0] + btnXGap) * 13 + (btnScale[1] * 2 + btnXGap)))
		&& (event->y() >= keyPadSY && event->y() <= (keyPadSY + (btnScale[0] + btnYGap) * 4 + btnScale[0]))) {
		RowColumn keyboardIndex = getRowColumn(event->x(), event->y());
		
		if (keyboardIndex.y() != -1) {
			classfyingKeys(keyboardIndex);
		}
	}
}

GLint KeyboardWidget::LoadPNGtoTexture(char *filename){
	std::vector<unsigned char> buffer, image;
	std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

	// get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	// read contents of the file into the vector
	if (size > 0) {
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else buffer.clear();

	unsigned long w, h;
	int error = decodePNG(image, w, h, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size());

	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// build our texture mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return texture;
}

void KeyboardWidget::initKeypadInfo(){
	// The number of components contained in keyName matrix's each rows
	keyMaxNum[0] = 15;
	keyMaxNum[1] = 15;
	keyMaxNum[2] = 13;
	keyMaxNum[3] = 13;
	keyMaxNum[4] = 12;

	// allocate keyBtns in memory dynamically for saving the coordinates of the each buttons
	keyBtns = new KEYPAD*[5];
	for (int i = 0; i < 5; i++) {
		keyBtns[i] = new KEYPAD[keyMaxNum[i]];
	}

	// initiate struct KEYPAD
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < keyMaxNum[i]; j++) {
			keyBtns[i][j].btnSize = btnScale[0];
			keyBtns[i][j].x[0] = 0;
			keyBtns[i][j].x[1] = 0;
			keyBtns[i][j].y[0] = 0;
			keyBtns[i][j].y[1] = 0;
		}
	}

	// button settins, except for the type 0 button
	keyBtns[0][0].btnSize = btnScale[1];	// Esc
	keyBtns[0][14].btnSize = btnScale[1];	// Back_Space
	keyBtns[1][0].btnSize = btnScale[2];	// Tab
	keyBtns[2][0].btnSize = btnScale[3];	// Caps
	keyBtns[2][12].btnSize = btnScale[3];	// Enter
	keyBtns[3][0].btnSize = btnScale[4];	// Shift (L)
	keyBtns[3][12].btnSize = btnScale[6];	// Shift (R)
	keyBtns[4][0].btnSize = btnScale[2];	// Ctrl
	keyBtns[4][3].btnSize = btnScale[7];	// Chineses Character
	keyBtns[4][4].btnSize = btnScale[5];	// Space
	keyBtns[4][5].btnSize = btnScale[7];	// Korean / English
	
	// set the coordinates of the each buttons which will be layouted
	for (int i = 0; i < 5; i++) {
		int _sx = keyPadSX;
		int _sy = keyPadSY + (btnScale[0] * i) + (btnYGap * i);

		for (int j = 0; j < keyMaxNum[i]; j++) {
			keyBtns[i][j].x[0] = _sx; // set s_x, s_y coordinate
			keyBtns[i][j].y[0] = _sy;

			keyBtns[i][j].y[1] = _sy + btnScale[0]; // set end y_coordinate
			keyBtns[i][j].x[1] = _sx + keyBtns[i][j].btnSize; // set end x_coordinate
			
			_sx += keyBtns[i][j].btnSize + btnXGap; // adding gap lenth for next x_coordinate
		}
	}

	// allocate GLint** in memory dynamically
	keyBtnTextures = new GLint**[5];
	for (int i = 0; i < 5; i++) {
		keyBtnTextures[i] = new GLint*[keyMaxNum[i]];
	}
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < keyMaxNum[i]; j++) {
			keyBtnTextures[i][j] = new GLint[1];
		}
	}

	// initiate variable accum
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < keyMaxNum[i]; j++) {
			keyBtns[i][j].ratio = 0;
		}
	}
}

void KeyboardWidget::modifyKeypadInfo() {
	// chage values
	btnScale[0] = BTN_SIZE_STD + incrKey;
	btnScale[1] = BTN_SIZE_1 + incrKey;
	btnScale[2] = BTN_SIZE_2 + incrKey;
	btnScale[3] = BTN_SIZE_3 + incrKey * 2 + incrDist;
	btnScale[4] = BTN_SIZE_4 + incrKey * 2 + incrDist;
	btnScale[5] = BTN_SIZE_5 + incrKey * 2 + incrDist;
	btnScale[6] = BTN_SIZE_6 + incrKey * 2 + incrDist;
	btnScale[7] = BTN_SIZE_7 + incrKey * 2 + incrDist;
	btnXGap = BTN_X_GAP + incrDist;
	btnYGap = BTN_Y_GAP + incrDist;
	keyPadSX = KEYPAD_SX + incrSX;
	keyPadSY = KEYPAD_SY + incrSY;

	// initiate struct KEYPAD
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < keyMaxNum[i]; j++) {
			keyBtns[i][j].btnSize = btnScale[0];
			keyBtns[i][j].x[0] = 0;
			keyBtns[i][j].x[1] = 0;
			keyBtns[i][j].y[0] = 0;
			keyBtns[i][j].y[1] = 0;
		}
	}

	// button settins, except for the type 0 button
	keyBtns[0][0].btnSize = btnScale[1];	// Esc
	keyBtns[0][14].btnSize = btnScale[1];	// Back_Space
	keyBtns[1][0].btnSize = btnScale[2];	// Tab
	keyBtns[2][0].btnSize = btnScale[3];	// Caps
	keyBtns[2][12].btnSize = btnScale[3];	// Enter
	keyBtns[3][0].btnSize = btnScale[4];	// Shift (L)
	keyBtns[3][12].btnSize = btnScale[6];	// Shift (R)
	keyBtns[4][0].btnSize = btnScale[2];	// Ctrl
	keyBtns[4][3].btnSize = btnScale[7];	// Chineses Character
	keyBtns[4][4].btnSize = btnScale[5];	// Space
	keyBtns[4][5].btnSize = btnScale[7];	// Korean / English

											// set the coordinates of the each buttons which will be layouted
	for (int i = 0; i < 5; i++) {
		int _sx = keyPadSX;
		int _sy = keyPadSY + (btnScale[0] * i) + (btnYGap * i);

		for (int j = 0; j < keyMaxNum[i]; j++) {
			keyBtns[i][j].x[0] = _sx; // set s_x, s_y coordinate
			keyBtns[i][j].y[0] = _sy;

			keyBtns[i][j].y[1] = _sy + btnScale[0]; // set end y_coordinate
			keyBtns[i][j].x[1] = _sx + keyBtns[i][j].btnSize; // set end x_coordinate

			_sx += keyBtns[i][j].btnSize + btnXGap; // adding gap lenth for next x_coordinate
		}
	}
}

void KeyboardWidget::btnsInfo() {
	keyName = new int*[5];
	for (int i = 0; i < 5; i++) {
		keyName[i] = new int[keyMaxNum[i]];
	}
		
	keyName[0][0] = Qt::Key_Escape;
	keyName[0][1] = Qt::Key_Apostrophe;
	keyName[0][2] = Qt::Key_1;
	keyName[0][3] = Qt::Key_2;
	keyName[0][4] = Qt::Key_3;
	keyName[0][5] = Qt::Key_4;
	keyName[0][6] = Qt::Key_5;
	keyName[0][7] = Qt::Key_6;
	keyName[0][8] = Qt::Key_7;
	keyName[0][9] = Qt::Key_8;
	keyName[0][10] = Qt::Key_9;
	keyName[0][11] = Qt::Key_0;
	keyName[0][12] = Qt::Key_hyphen;
	keyName[0][13] = Qt::Key_Equal;
	keyName[0][14] = Qt::Key_Backspace;
	
	keyName[1][0] = Qt::Key_Tab;
	keyName[1][1] = Qt::Key_Q;
	keyName[1][2] = Qt::Key_W;
	keyName[1][3] = Qt::Key_E;
	keyName[1][4] = Qt::Key_R;
	keyName[1][5] = Qt::Key_T;
	keyName[1][6] = Qt::Key_Y;
	keyName[1][7] = Qt::Key_U;
	keyName[1][8] = Qt::Key_I;
	keyName[1][9] = Qt::Key_O;
	keyName[1][10] = Qt::Key_P;
	keyName[1][11] = Qt::Key_BracketLeft;
	keyName[1][12] = Qt::Key_BracketRight;
	keyName[1][13] = Qt::Key_Backslash;
	keyName[1][14] = Qt::Key_Delete;
	
	keyName[2][0] = Qt::Key_CapsLock;
	keyName[2][1] = Qt::Key_A;
	keyName[2][2] = Qt::Key_S;
	keyName[2][3] = Qt::Key_D;
	keyName[2][4] = Qt::Key_F;
	keyName[2][5] = Qt::Key_G;
	keyName[2][6] = Qt::Key_H;
	keyName[2][7] = Qt::Key_J;
	keyName[2][8] = Qt::Key_K;
	keyName[2][9] = Qt::Key_L;
	keyName[2][10] = Qt::Key_Semicolon;
	keyName[2][11] = Qt::Key_QuoteLeft;
	keyName[2][12] = Qt::Key_Enter;
	
	keyName[3][0] = Qt::Key_Shift;
	keyName[3][1] = Qt::Key_Z;
	keyName[3][2] = Qt::Key_X;
	keyName[3][3] = Qt::Key_C;
	keyName[3][4] = Qt::Key_V;
	keyName[3][5] = Qt::Key_B;
	keyName[3][6] = Qt::Key_N;
	keyName[3][7] = Qt::Key_M;
	keyName[3][8] = Qt::Key_Comma;
	keyName[3][9] = Qt::Key_Period;
	keyName[3][10] = Qt::Key_Slash;
	keyName[3][11] = Qt::Key_Up;
	keyName[3][12] = Qt::Key_Shift;
	
	keyName[4][0] = Qt::Key_Control;
	keyName[4][1] = Qt::Key_Meta;
	keyName[4][2] = Qt::Key_Alt;
	keyName[4][3] = Qt::Key_ZoomIn;
	keyName[4][4] = Qt::Key_Space;
	keyName[4][5] = Qt::Key_ZoomIn;
	keyName[4][6] = Qt::Key_Alt;
	keyName[4][7] = Qt::Key_Menu;
	keyName[4][8] = Qt::Key_Left;
	keyName[4][9] = Qt::Key_Down;
	keyName[4][10] = Qt::Key_Right;
	keyName[4][11] = Qt::Key_Help;
}

void KeyboardWidget::setKeyImageTextures(){
	char _f[50];
	
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < keyMaxNum[i]; j++) {
			sprintf(_f, "%s%d%s%d%s", "./img/keytype/key_", i, "_", j, ".png");
			keyBtnTextures[i][j][0] = LoadPNGtoTexture(_f);
		}
	}
}

RowColumn KeyboardWidget::getRowColumn(int x, int y){
	int row, column = -1;

	// make sure that y_coordinate is not located on the blind spot
	if ((y - keyPadSY) % (btnScale[0] + btnYGap) - btnScale[0] <= 0) {
		row = (y - keyPadSY) / (btnScale[0] + btnYGap);
	}
	else {
		row = -1;
	}

	// determine which button was pressed and what column of that button was
	if (row != -1) {
		for (int i = 0; i < keyMaxNum[row]; i++) {
			if (x >= keyBtns[row][i].x[0] && x <= keyBtns[row][i].x[1]) {
				column = i;
				break;
			}
		}
	}

	return RowColumn(row, column);
}

void KeyboardWidget::classfyingKeys(RowColumn param){
	char* inputText;

	if (keyName[param.x()][param.y()] >= Qt::Key_A && keyName[param.x()][param.y()] <= Qt::Key_Z) {
		QString temp = QKeySequence(keyName[param.x()][param.y()]).toString();
		QByteArray byteTemp = temp.toLocal8Bit();
		inputText = byteTemp.data();
		if (!(onCaps ^ onShift) && (inputText[0] >= 'A' && inputText[0] <= 'z')) inputText[0] += 'a' - 'A';
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Apostrophe) {
		if (!(onCaps ^ onShift)) inputText = "`";
		else inputText = "~";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_1) {
		if (!(onCaps ^ onShift)) inputText = "1";
		else inputText = "!";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_2) {
		if (!(onCaps ^ onShift)) inputText = "2";
		else inputText = "@";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_3) {
		if (!(onCaps ^ onShift)) inputText = "3";
		else inputText = "#";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_4) {
		if (!(onCaps ^ onShift)) inputText = "4";
		else inputText = "$";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_5) {
		if (!(onCaps ^ onShift)) inputText = "5";
		else inputText = "%";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_6) {
		if (!(onCaps ^ onShift)) inputText = "6";
		else inputText = "^";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_7) {
		if (!(onCaps ^ onShift)) inputText = "7";
		else inputText = "&";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_8) {
		if (!(onCaps ^ onShift)) inputText = "8";
		else inputText = "*";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_9) {
		if (!(onCaps ^ onShift)) inputText = "9";
		else inputText = "(";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_0) {
		if (!(onCaps ^ onShift)) inputText = "0";
		else inputText = ")";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_hyphen) {
		if (!(onCaps ^ onShift)) inputText = "-";
		else inputText = "_";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Equal) {
		if (!(onCaps ^ onShift)) inputText = "=";
		else inputText = "+";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_BracketLeft) {
		if (!(onCaps ^ onShift)) inputText = "[";
		else inputText = "{";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_BracketRight) {
		if (!(onCaps ^ onShift)) inputText = "]";
		else inputText = "}";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Backslash) {
		if (!(onCaps ^ onShift)) inputText = "\\";
		else inputText = "|";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Semicolon) {
		if (!(onCaps ^ onShift)) inputText = ";";
		else inputText = ":";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_QuoteLeft) {
		if (!(onCaps ^ onShift)) inputText = "\'";
		else inputText = "\"";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Comma) {
		if (!(onCaps ^ onShift)) inputText = ",";
		else inputText = "<";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Period) {
		if (!(onCaps ^ onShift)) inputText = ".";
		else inputText = ">";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Slash) {
		if (!(onCaps ^ onShift)) inputText = "/";
		else inputText = "?";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Backspace) {
		if (onBackspace) userText[strlen(userText) - 1] = '\0';
		onShift = false;
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_CapsLock) {
		if (onCaps) onCaps = false;
		else onCaps = true;
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Shift) {
		if (!onShift) onShift = true;
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Enter) {
		if (onEnter) CWidget->clearClicked();
		onShift = false;
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Space) {
		inputText = " ";
		addNewInputText(inputText);
	}
	else if (keyName[param.x()][param.y()] == Qt::Key_Escape || keyName[param.x()][param.y()] == Qt::Key_Tab
		|| keyName[param.x()][param.y()] == Qt::Key_Delete || keyName[param.x()][param.y()] == Qt::Key_Up
		|| keyName[param.x()][param.y()] == Qt::Key_Control || keyName[param.x()][param.y()] == Qt::Key_Meta
		|| keyName[param.x()][param.y()] == Qt::Key_Alt || keyName[param.x()][param.y()] == Qt::Key_ZoomIn
		|| keyName[param.x()][param.y()] == Qt::Key_Delete || keyName[param.x()][param.y()] == Qt::Key_Delete
		|| keyName[param.x()][param.y()] == Qt::Key_Menu || keyName[param.x()][param.y()] == Qt::Key_Left
		|| keyName[param.x()][param.y()] == Qt::Key_Down || keyName[param.x()][param.y()] == Qt::Key_Right
		|| keyName[param.x()][param.y()] == Qt::Key_Help) {
		onShift = false;
	}
	else {
		QString temp = QKeySequence(keyName[param.x()][param.y()]).toString();
		QByteArray byteTemp = temp.toLocal8Bit();
		inputText = byteTemp.data();
		if (!(onCaps ^ onShift)) {
			if (inputText[0] >= 'A' && inputText[0] <= 'z') inputText[0] += 'a' - 'A';
			//else if (inputText[0] >= '0' && inputText[0] <= '9') inputText[0] -= 16;
		}
		addNewInputText(inputText);
	}
}

void KeyboardWidget::addNewInputText(char* inputText){
	onShift = false;

	// if the number of characters is over MAXCHARNUM, userText will be initiated
	if (strlen(userText) == MAXCHARNUM) sprintf(stackText, "%s", inputText);
	else sprintf(stackText, "%s%s", userText, inputText);

	userText = stackText;

	// fill starText with * for masking
	if (strlen(starText) == MAXCHARNUM) strcpy(starText, "");
	strcat(starText, "*");
}

void KeyboardWidget::drawUserText(double x, double y, double z, int fontSize, QString txt) {
	qglColor(Qt::red);
	renderText(x, y, z, txt, QFont("Arial", fontSize, QFont::ExtraLight, false));
}

void KeyboardWidget::gazeInput() {
	if (!probability) {
		if ((*gX >= keyPadSX && *gX <= (keyPadSX + (btnScale[0] + btnXGap) * 13 + (btnScale[1] * 2 + btnXGap)))
			&& (*gY >= keyPadSY && *gY <= (keyPadSY + (btnScale[0] + btnXGap) * 4 + btnScale[0]))) {
			currRowColumn = getRowColumn(*gX, *gY);

			// when some point is blind spot, it's y()-value always has -1 no matter what x()-value is
			if (currRowColumn.y() != -1) {
				if (currRowColumn == prevRowColumn) {
					if ((clock() - startOfDwellTime) / MILISEC >= dwellTime) {
						// get input text by using eyetracker
						gazeRowColumn[0] = currRowColumn;
						classfyingKeys(gazeRowColumn[0]);

						// make the feedback visible 
						if (onFeedback) {
							feedState = true;
							startOfFakeLast = clock();
						}

						// make the fake feedback visible 
						if (onFakeFeed) {
							chosenIndex[0] = gazeRowColumn[0];
							findFakeIndex(chosenIndex, FAKENUM, 1);

							fakeState = true;
							startOfFakeLast = clock();
						}

						startOfDwellTime = clock();
						subFakeInitFlag = true;
						probability = true;
					}
					else {
						if (onSubFake) {
							subFakeActCheck();
						}
					}
				}
				else {
					prevRowColumn = currRowColumn;
					startOfDwellTime = clock();

					if (onSubFake) {
						subFakeActCheck();
					}
				}
			}
			else {
				prevRowColumn.setY(-1);
				startOfDwellTime = clock();

				if (onSubFake) {
					subFakeActCheck();
				}
			}
		}
		else {
			currRowColumn.setY(-1);

			if (onSubFake) {
				subFakeActCheck();
			}
		}
	}
	else {
		if ((clock() - startOfDwellTime) / MILISEC >= dwellTime) {
			calculateProbability();
			
			// make the feedback visible 
			if (onFeedback) {
				feedState = true;
				startOfFakeLast = clock();
			}

			startOfDwellTime = clock();
		}
		else {
			if ((*gX >= keyPadSX - btnXGap && *gX <= (keyPadSX + (btnScale[0] + btnXGap) * 13 + (btnScale[1] + btnXGap) * 2))
				&& (*gY >= keyPadSY - btnYGap && *gY <= (keyPadSY + (btnScale[0] + btnXGap) * 5))) {
				if (onInit) {
					// initiate variable accum
					for (int i = 0; i < 5; i++) {
						for (int j = 0; j < keyMaxNum[i]; j++) {
							keyBtns[i][j].ratio = 0;
						}
					}

					onInit = false;
					startOfDwellTime = clock();
				}

				currRowColumn = getRowColumn(*gX, *gY);

				// when some point is blind spot, it's y()-value always has -1 no matter what x()-value is
				if (currRowColumn.y() != -1) {
					keyBtns[currRowColumn.x()][currRowColumn.y()].ratio += 1;
				}
			}
			else {
				onInit = true;
				startOfDwellTime = clock();
			}
		}
	}
}

void KeyboardWidget::gazePoint(double gazeX, double gazeY, double radius, double color[3]) {
	glPushMatrix();
	glColor4f(color[0], color[1], color[2], 0.7f);
	glBegin(GL_POLYGON);
	glVertex3f(gazeX, gazeY, 0.0);
	for (int j = 0; j <= 360; j++) {
		double angle = j*3.141592 / 180;
		double x = gazeX + radius*cos(angle);
		double y = gazeY + radius*sin(angle);
		glVertex2f(x, y);
	}

	glEnd();
	glPopMatrix();
}

void KeyboardWidget::rendering(GLint imagePath, GLfloat color[3], int width[2], int height[2]) {
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, imagePath);
	glEnable(GL_TEXTURE_2D);
	glTranslatef(0, 0, 0.0);
	glColor4f(color[0], color[1], color[2], 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);	glVertex3f(width[0], height[0], 0);
	glTexCoord2f(0.0, 1.0);	glVertex3f(width[0], height[1], 0);
	glTexCoord2f(1.0, 1.0);	glVertex3f(width[1], height[1], 0);
	glTexCoord2f(1.0, 0.0);	glVertex3f(width[1], height[0], 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void KeyboardWidget::renderingMuti(RowColumn* param, GLfloat color[3], int startIndex, int endIndex) {
	for (int i = startIndex; i < endIndex; i++) {
		rendering(keyBtnTextures[param[i].x()][param[i].y()][0], color,
			keyBtns[param[i].x()][param[i].y()].x, keyBtns[param[i].x()][param[i].y()].y);
	}
}

void KeyboardWidget::calculateProbability() {
	int temp, freqSum = 0, freqNum[3] = { 0, 0, 0 };
	RowColumn tempIndex;

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < keyMaxNum[i]; j++) {
			freqSum += keyBtns[i][j].ratio;

			if (keyBtns[i][j].ratio > freqNum[0]) {
				freqNum[0] = keyBtns[i][j].ratio;

				gazeRowColumn[0].setX(i);
				gazeRowColumn[0].setY(j);

				if (keyBtns[i][j].ratio > freqNum[1]) {
					temp = freqNum[0];
					freqNum[0] = freqNum[1];
					freqNum[1] = temp;

					tempIndex = gazeRowColumn[0];
					gazeRowColumn[0] = gazeRowColumn[1];
					gazeRowColumn[1] = tempIndex;

					if (keyBtns[i][j].ratio > freqNum[2]) {
						temp = freqNum[1];
						freqNum[1] = freqNum[2];
						freqNum[2] = temp;

						tempIndex = gazeRowColumn[1];
						gazeRowColumn[1] = gazeRowColumn[2];
						gazeRowColumn[2] = tempIndex;
					}
				}
			}

			keyBtns[i][j].ratio = 0;
		}
	}

	if (freqNum[2] == 0) {
		ratioNum = 0;
	}
	else if (freqNum[1] == 0) {
		blue_color[2][2] = freqNum[2] / (float)freqSum;

		classfyingKeys(gazeRowColumn[2]);
		ratioNum = 1;
	}
	else if (freqNum[0] == 0) {
		blue_color[2][2] = freqNum[2] / (float)freqSum;
		blue_color[1][1] = freqNum[1] / (float)freqSum;

		classfyingKeys(gazeRowColumn[2]);
		ratioNum = 2;
	}
	else {
		blue_color[2][2] = freqNum[2] / (float)freqSum;
		blue_color[1][1] = freqNum[1] / (float)freqSum;
		blue_color[0][0] = freqNum[0] / (float)freqSum;

		classfyingKeys(gazeRowColumn[2]);
		ratioNum = 3;
	}
}

void KeyboardWidget::feedback() {
	if ((clock() - startOfFakeLast) / MILISEC >= fakeLastTime) feedState = false;
	else {
		for (int i = ratioNum; i > 0; i--)
			renderingMuti(&gazeRowColumn[3-i], blue_color[3-i], 0, 1);
	}
}

void KeyboardWidget::fakeFeedback() {
	if ((clock() - startOfFakeLast) / MILISEC >= fakeLastTime) fakeState = false;
	else renderingMuti(chosenIndex, red_color, 1, FAKENUM + 1);
}

void KeyboardWidget::subFakeFeedback() {
	if (feedState || fakeState) {
		subFakeInitFlag = true;
		subFakeState = false;
	}
	else if ((clock() - startOfSubFakeLast) / MILISEC >= subFakeLastTime) {
		subFakeState = false;
	}
	else {
		if (onGazeProtect) {
			if (gazeProtectCount > gazeProtectNum) renderingMuti(subFakeIndex, red_color, 0, FAKENUM + 1);
			else renderingMuti(subFakeIndex, red_color, 1, FAKENUM + 2);
		}
		else {
			renderingMuti(subFakeIndex, red_color, 0, FAKENUM + 1);
		}
	}
}

void KeyboardWidget::subFakeActCheck() {
	if (subFakeInitFlag) {
		startOfSubFakeUpdate = clock();
		subFakeInitFlag = false;
	}
	else if ((clock() - startOfSubFakeUpdate) / MILISEC >= subFakeUpdate) {
		if (onGazeProtect) {
			gazeProtectCount += 1;

			if (gazeProtectCount > gazeProtectNum) findSubFakeIndex(subFakeIndex, FAKENUM, 1);
			else findSubFakeIndex(subFakeIndex, FAKENUM + 1, 1);
		}
		else {
			findSubFakeIndex(subFakeIndex, FAKENUM, 1);
		}
	}
}

void KeyboardWidget::findFakeIndex(RowColumn* param, int fakeNum, int excl) {
	int nomineeRow, nomineeColumn;

	for (int i = excl; i < excl + fakeNum; i++) {
		nomineeRow = rand() % 5;
		nomineeColumn = rand() % keyMaxNum[nomineeRow];

		for (int j = 0; j < i; j++) {
			if (Isdissatisfied(nomineeRow, nomineeColumn, param[j].x(), param[j].y())) {
				i--;
				break;
			}
			else if (j + 1 == i) {
				param[i].setX(nomineeRow);
				param[i].setY(nomineeColumn);
			}
		}
	}
}

void KeyboardWidget::findSubFakeIndex(RowColumn* param, int fakeNum, int excl) {
	if (currRowColumn.y() != -1) {
		param[0] = currRowColumn;
	}
	else {
		param[0].setX(rand() % 5);
		param[0].setY(rand() % keyMaxNum[param[0].x()]);
	}
	findFakeIndex(param, fakeNum, excl);

	subFakeState = true;
	subFakeInitFlag = true;
	startOfSubFakeLast = clock();
}

bool KeyboardWidget::Isdissatisfied(int nomineeRow, int nomineeColumn, int chosenRow, int chosenColumn) {
	if (nomineeRow == chosenRow) {
		if ((nomineeColumn == chosenColumn - 1) || (nomineeColumn == chosenColumn) || (nomineeColumn == chosenColumn + 1)) {
			return true;
		}
	}
	else if ((nomineeRow == (chosenRow - 1)) || (nomineeRow == (chosenRow + 1))) {
		if ((keyBtns[chosenRow][chosenColumn].x[0] - btnXGap) <= keyBtns[nomineeRow][nomineeColumn].x[0] &&
			(keyBtns[chosenRow][chosenColumn].x[1] + btnXGap) >= keyBtns[nomineeRow][nomineeColumn].x[0]) {
			return true;
		}
		else if ((keyBtns[chosenRow][chosenColumn].x[0] - btnXGap) <= keyBtns[nomineeRow][nomineeColumn].x[1] &&
			(keyBtns[chosenRow][chosenColumn].x[1] + btnXGap) >= keyBtns[nomineeRow][nomineeColumn].x[1]) {
			return true;
		}
		else if ((keyBtns[nomineeRow][nomineeColumn].x[0] - btnXGap) <= keyBtns[chosenRow][chosenColumn].x[0] &&
			(keyBtns[nomineeRow][nomineeColumn].x[1] + btnXGap) >= keyBtns[chosenRow][chosenColumn].x[0]) {
			return true;
		}
		else if ((keyBtns[nomineeRow][nomineeColumn].x[0] - btnXGap) <= keyBtns[chosenRow][chosenColumn].x[1] &&
			(keyBtns[nomineeRow][nomineeColumn].x[1] + btnXGap) >= keyBtns[chosenRow][chosenColumn].x[1]) {
			return true;
		}
	}

	return false;
}

int &KeyboardWidget::getDwellTime() {
	return dwellTime;
}

int & KeyboardWidget::getFakeLastTime() {
	return fakeLastTime;
}

int & KeyboardWidget::getSubFakeLastTime() {
	return subFakeLastTime;
}

int & KeyboardWidget::getSubFakeUpdate() {
	return subFakeUpdate;
}

int & KeyboardWidget::getStartTime() {
	return startOfDwellTime;
}

int & KeyboardWidget::getGazeProtectCount() {
	return gazeProtectCount;
}

int & KeyboardWidget::getGazeProtectNum() {
	return gazeProtectNum;
}

int & KeyboardWidget::getCaliStartTime() {
	return startOfCali;
}

int & KeyboardWidget::getIncrKey() {
	return incrKey;
}

int & KeyboardWidget::getIncrDist() {
	return incrDist;
}

int & KeyboardWidget::getIncrSX() {
	return incrSX;
}

int & KeyboardWidget::getIncrSY() {
	return incrSY;
}

bool & KeyboardWidget::getOnCalibration() {
	return onCalibration;
}

bool & KeyboardWidget::getOnGazeProtect() {
	return 	onGazeProtect;
}

bool & KeyboardWidget::getOnFeedback() {
	return onFeedback;
}

bool & KeyboardWidget::getOnFakeFeed() {
	return onFakeFeed;
}

bool & KeyboardWidget::getOnSubFake() {
	return onSubFake;
}

bool & KeyboardWidget::getOnGazePoint() {
	return onGazePoint;
}

bool & KeyboardWidget::getOnCaps() {
	return onCaps;
}

bool & KeyboardWidget::getOnShift() {
	return onShift;
}

bool & KeyboardWidget::getOnEnter() {
	return onEnter;
}

bool & KeyboardWidget::getOnBackspace() {
	return onBackspace;
}

bool & KeyboardWidget::getOnTuneKeyInfo() {
	return onTuneKeyInfo;
}

bool & KeyboardWidget::getFeedState() {
	return feedState;
}

bool & KeyboardWidget::getFakeState() {
	return fakeState;
}

bool & KeyboardWidget::getSubFakeState() {
	return subFakeState;
}

bool & KeyboardWidget::getSubFakeInitFlag() {
	return subFakeInitFlag;
}

char* & KeyboardWidget::getUserText() {
	return userText;
}

void KeyboardWidget::setCWidgetVars(ControlWidget *param) {
	CWidget = param;
	gX = &((CWidget->getgPoint())[0]);
	gY = &((CWidget->getgPoint())[1]);
	keyboardOnTracker = CWidget->getOnTracker();
}