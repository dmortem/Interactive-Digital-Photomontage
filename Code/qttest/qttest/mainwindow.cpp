#include "mainwindow.h"
#include "scribbleArea.h"
#include <stdio.h>
#include <QtWidgets>
#include <QtGui>


MainWindow::MainWindow()
{
    scribbleArea = new ScribbleArea;	
	setCentralWidget(scribbleArea);
	scribbleWindow = new ScribbleWindow();

	createActions();
    createMenus();

	setWindowTitle(tr("A Stupid InteractiveDigitalPhotomontage By D mortem"));
	resize(600, 400);
}

void MainWindow::open()
{		
	QString fileName = QFileDialog::getOpenFileName(this,
								tr("Open File"), QDir::currentPath());
	if (!fileName.isEmpty()){
        scribbleArea->openImage(fileName);		// 打开文件
	}
}

void MainWindow::clear(){
	scribbleWindow->createScribbleWindow();		// 重新创建画布
}

void MainWindow::work()
{	
    scribbleWindow->work();					// 开始计算
	scribbleArea->openImage("result.jpg");	// 打开计算完成的结果
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open"), this);
	openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	openStackAct = new QAction(tr("&Open Stack"), this);
	openStackAct->setShortcut(tr("Ctrl+S"));
    connect(openStackAct, SIGNAL(triggered()), scribbleWindow, SLOT(openStack()));

	workAct = new QAction(tr("&Calculate"), this);
	workAct->setShortcut(tr("Ctrl+W"));
	connect(workAct, SIGNAL(triggered()), this, SLOT(work()));

	clearScreenAct = new QAction(tr("&Clear"), this);
	clearScreenAct->setShortcut(tr("Ctrl+W"));
    connect(clearScreenAct, SIGNAL(triggered()), this, SLOT(clear()));

    penColorAct = new QAction(tr("&Pen Color"), this);
	penColorAct->setShortcut(tr("Ctrl+Y"));
    connect(penColorAct, SIGNAL(triggered()), scribbleWindow, SLOT(setPenColor()));

    penWidthAct = new QAction(tr("&Pen Width"), this);
	penWidthAct->setShortcut(tr("Ctrl+K"));
    connect(penWidthAct, SIGNAL(triggered()), scribbleWindow, SLOT(setPenWidth()));
}

void MainWindow::createMenus()
{
	// file菜单
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
	fileMenu->addAction(openStackAct);
	fileMenu->addAction(workAct);
	fileMenu->addAction(clearScreenAct);

	// option菜单
    optionMenu = new QMenu(tr("&Options"), this);
    optionMenu->addAction(penColorAct);
    optionMenu->addAction(penWidthAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(optionMenu);
}