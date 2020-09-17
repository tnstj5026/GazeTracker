#include "dialog.h"

Dialog::Dialog() {
	setWindowTitle(tr("Configuration"));
	setWindowFlags(Qt::WindowTitleHint | Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint);
		
	resolutionLabel = new QLabel(tr("Resolution:"));
	resolution = new QComboBox(this);
	okButton = new QPushButton("OK", this);
	
	QStringList items;
	items << "720 X 576" << "1280 X 720" << "1920 X 1080" << "3840 X 2160";
	resolution->addItems(items);
	
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(resolutionLabel, 0, 0, 1, 1);
	layout->addWidget(resolution, 1, 0, 1, 3);
	layout->addWidget(okButton, 2, 2, 1, 1);
	
	this->setLayout(layout);

	connect(okButton, SIGNAL(clicked()), this, SLOT(onOKButtonClicked()));
}

Dialog::~Dialog() {
}

int Dialog::getResolution() {
	return resolutionIndex;
}

void Dialog::onOKButtonClicked() {
	resolutionIndex = resolution->currentIndex();
	this->setResult(resolution->currentIndex());
	this->close();
}
