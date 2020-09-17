#define _DIALOG_H_
#ifdef _DIALOG_H_
#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QPushbutton>
#include <QComboBox>
#include <QLabel>

class Dialog : public QDialog {
	Q_OBJECT

public:
	Dialog();
	~Dialog();

public:
	int getResolution();

private slots:
	void onOKButtonClicked();

private:
	QComboBox *resolution;
	QPushButton *okButton;
	QLabel *resolutionLabel;
	int resolutionIndex;
};
#endif