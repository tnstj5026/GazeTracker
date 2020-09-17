#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_
#pragma once

#include "keyboardwidget.h"
#include "controlwidget.h"

#include <QtGui>
#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include <QGLWidget>

class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    MainWindow(int x, int y, int l, int w);
	virtual ~MainWindow();

private:
	QWidget *CentralWidget;
	ControlWidget *CWidget;
	KeyboardWidget *KWidget;
};
#endif // _MAINWINDOW_H_