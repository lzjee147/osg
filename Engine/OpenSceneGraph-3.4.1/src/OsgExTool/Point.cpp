 #include "OsgExTool/Point.h"
// #include <math.h>
//  #include "stdlib.h"
Point::Point(void)
{  
	this->_x = 0.0;
	this->_y = 0.0;
}

Point::Point( double x, double y, double z )
:_x(x)
,_y(y)
{
	_InterpolateVec.push_back(z);
}

Point::Point( double x, double y, double z, float ux, float uy, float uz )
	:_x(x)
	,_y(y)
{
	_InterpolateVec.push_back(z);
	_InterpolateVec.push_back(ux);
	_InterpolateVec.push_back(uy);
	_InterpolateVec.push_back(uz);
}

Point::Point( double x, double y, double z, float dvalue )
:_x(x)
,_y(y)
{
	_InterpolateVec.push_back(z);
	_InterpolateVec.push_back(dvalue);
}
float Point::getDistance(Point& p)
{
	return sqrt( (p._x - this->_x) * (p._x - this->_x) + (p._y - this->_y) * (p._y - this->_y) );

}

Point Point::getPointForVelocity(float factor)
{
	Point p;
	p._x = this->_InterpolateVec[0] * factor + _x;
	p._y = this->_InterpolateVec[1] * factor + _y;
	return p;
}


void Point::writePoint(std::ofstream& ofs)
{
	ofs<<_x<<" "<<_y/*<<endl*/;
	ofs<<this->_InterpolateVec.size()/*<<endl*/;

	int isize = _InterpolateVec.size();
	for (int i = 0; i < isize; i++)
	{
         ofs<<this->_InterpolateVec[i]<<"  ";
	}
	
}

void Point::readPoint(std::ifstream& ifs)
{
	ifs>>_x>>_y;
	int temp = 0;
	ifs>>temp;
    _InterpolateVec.resize(temp);
	for (int i = 0; i < temp; i++)
	{
		ifs>>this->_InterpolateVec[i];
	}
}

Point::~Point(void)
{

}

void Point::ModifyPointPosition( float dXMax, float dXMin, float dYMax, float dYMin )
{

	if (_x < dXMin)
	{
		_x = dXMin;
	}

	if (_x > dXMax)
	{
		_x = dXMax;
	}

	if (_y < dYMin)
	{
		_y = dYMin;
	}

	if (_y > dYMax)
	{
		_y = dYMax;
	}
}