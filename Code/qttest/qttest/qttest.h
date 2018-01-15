#ifndef QTTEST_H
#define QTTEST_H

#include <QtWidgets/QMainWindow>
#include "ui_qttest.h"

class qttest : public QMainWindow
{
	Q_OBJECT

public:
	qttest(QWidget *parent = 0);
	~qttest();

private:
	Ui::qttestClass ui;
};

#endif // QTTEST_H
