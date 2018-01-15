#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QMainWindow>
#include <QImage>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QScreen>
#include <QBuffer>
#include <QDebug>
#include <QShortcut>
#include <QTabWidget>
#include <QStringList>
#include <QString>
#include <QList>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsScene>
#include <QPalette>
#include <QColorDialog>
#include <QGraphicsLineItem>
#include <QPicture>
#include "scribbleArea.h"

class ScribbleWindow : public QMainWindow{
	Q_OBJECT
public:
	ScribbleWindow();
	void createScribbleWindow();
    void work();

public slots:
	void openStack();	
    void setPenWidth();
    void setPenColor();

private:
	QColor penColor;
	int penWidth;
	QTabWidget *sourceImages;
	QStringList stackFiles;
};
#endif
