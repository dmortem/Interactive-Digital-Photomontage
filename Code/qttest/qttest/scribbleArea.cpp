#include <QtWidgets>
#ifndef QT_NO_PRINTER
#endif
#include <QtGui>
#include "scribbleArea.h"


ScribbleArea::ScribbleArea(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
	label = 0;
	strokeNumber = 0;
    scribbling = false;
    myPenWidth = 10;
	myPenColor = Qt::blue;
}

ScribbleArea::ScribbleArea(const QString &fileName, const int label, int penWidth, QColor penColor, QWidget *parent) 
	: QWidget(parent)
{	
	setAttribute(Qt::WA_StaticContents);
	openImage(fileName);
	this->label = label;
	strokeNumber = 0;
	filename = fileName;	
	scribbling = false;
    myPenWidth = penWidth;
    myPenColor = penColor;	
}


bool ScribbleArea::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

	setMinimumSize(loadedImage.size());		
	setMaximumSize(loadedImage.size());
    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    update();
    return true;
}

void ScribbleArea::setPenColor(const QColor &newColor)
{
    myPenColor = newColor;
}

void ScribbleArea::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
}

void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
		scribbling = true;

		QSize s = image.size();
		if(lastPoint.x() >= 0 && lastPoint.x() <= s.width() &&
				lastPoint.y() >=0 && lastPoint.y() <= s.height() )
			myPointVector.push_back(myPoint(event->pos(), label, strokeNumber));
    }
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event)
{
	if ((event->buttons() & Qt::LeftButton) && scribbling){
        drawLineTo(event->pos());

		QSize s = image.size();
		if(lastPoint.x() >= 0 && lastPoint.x() <= s.width() && lastPoint.y() >=0 && lastPoint.y() <= s.height() )
			myPointVector.push_back(myPoint(event->pos(), label, strokeNumber));
	}
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        drawLineTo(event->pos());
		scribbling = false;

		QSize s = image.size();
		if(lastPoint.x() >= 0 && lastPoint.x() <= s.width() && lastPoint.y() >=0 && lastPoint.y() <= s.height() )
			myPointVector.push_back(myPoint(event->pos(), label, strokeNumber));
    }
}

void ScribbleArea::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	QRect dirtyRect = event->rect();
	painter.drawImage(dirtyRect, image, dirtyRect);
}

void ScribbleArea::resizeEvent(QResizeEvent *event)
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void ScribbleArea::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);

    int rad = (myPenWidth / 2) + 2;
    update(QRect(lastPoint, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}

void ScribbleArea::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}
