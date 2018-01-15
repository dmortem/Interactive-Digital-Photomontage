#ifndef MYPOINT_H
#define MYPOINT_H

#include <stdlib.h>
#include <stdio.h>
#include <QPoint>

class myPoint{
public:
	myPoint();
	myPoint(int _xp, int _yp, int _label, int _strokeNumber);
	myPoint(QPoint pos, int _label, int _strokeNumber);

	QPoint pos() const;
    void setX(int x);
    void setY(int y);

	int x;
	int y;
	int label;
	int strokeNumber;	// the "strokeNumber"th stroke
};
inline myPoint::myPoint(){
	x = y = label = strokeNumber = -1;
}

inline myPoint::myPoint(int _xp, int _yp, int _label, int _strokeNumber):x(_xp),y(_yp),label(_label),strokeNumber(_strokeNumber){}

inline myPoint::myPoint(QPoint pos, int _label, int _strokeNumber):label(_label),strokeNumber(_strokeNumber){
	x = pos.x();
	y = pos.y();
}

inline QPoint myPoint::pos() const
{ return QPoint(x,y); }

inline void myPoint::setX(int xpos)
{ x = xpos; }

inline void myPoint::setY(int ypos)
{ y = ypos; }

#endif