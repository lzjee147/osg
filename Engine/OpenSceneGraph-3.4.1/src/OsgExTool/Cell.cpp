#include "OsgExTool/Cell.h"

Cell::Cell(void)
{

}

std::list<Point>& Cell::getListPoint()
{
	return m_vListPoint;
} 

//利用p和center的坐标值相对大小进行判断
int Cell::getPointPosition(Point& p, Point& center)
{
	if (p._x > center._x && p._y > center._y)
	{
		return Cell::RightTop;
	} 
	else if (p._x > center._x && p._y < center._y)
	{
		return Cell::RightBottom;
	}
	else if (p._x < center._x && p._y > center._y)
	{
		return Cell::LeftTop;
	}
	else
	{
		return Cell::LeftBottom;
	}

}

//反距离插值公式 p.ux = ( p1.ux / dis1 * dis1 + p2.ux / dis2 * dis2 ...... ) / ( 1 / dis1 * dis1 + 1 / dis2 * dis2 ......); 
void Cell::getVelocityInterpolate(Point& p)
{
	getVelocityInterpolate(p, this->m_vListPoint);
}

int Cell::getPoinsCount()
{
	return m_vListPoint.size();
}

void Cell::mergerCell(Cell& cell)
{
	std::list<Point>::const_iterator it;
	for (it = cell.getListPoint().begin(); it != cell.getListPoint().end(); it++)
	{
		this->m_vListPoint.push_back(*it);
	}

}

void Cell::setCornerPoint(Point pointLeftBottom, Point pointLeftTop, Point pointRightTop, Point pointRightBottom)
{
	this->m_CornerPoint[0] = pointLeftBottom;
	this->m_CornerPoint[1] = pointLeftTop;

	this->m_CornerPoint[2] = pointRightTop;
	this->m_CornerPoint[3] = pointRightBottom;
}

void Cell::getReverseVelocity(Point& input)
{
	std::list<Point> temp;
	for (int i = 0; i < 4; i++)
	{
		temp.push_back(this->m_CornerPoint[i]);
	}

	getVelocityInterpolate(input, temp);

}

void Cell::calculateAverageVelocity()
{
	this->m_averageVelocity._x = 0.0;
	this->m_averageVelocity._y = 0.0;

	int isize = this->m_CornerPoint[0]._InterpolateVec.size();
	for(int i = 0; i < isize; i++)
	{
		this->m_averageVelocity._InterpolateVec.push_back( (this->m_CornerPoint[0]._InterpolateVec[i] + this->m_CornerPoint[1]._InterpolateVec[i] + this->m_CornerPoint[2]._InterpolateVec[i] + this->m_CornerPoint[3]._InterpolateVec[i]) / 4 );
	}
}

void Cell::writeCell(std::ofstream& ofs)
{
	for (int i = 0; i < 4; i++)
	{
		this->m_CornerPoint[i].writePoint(ofs);
		ofs<<" ";
	}

}

void Cell::readCell(std::ifstream& ifs)
{
	for (int i = 0; i < 4; i++)
	{
		this->m_CornerPoint[i].readPoint(ifs);

	}
}

void Cell::getVelocityInterpolate(Point& p, std::list<Point>& listPoint)
{
	float dTempx = 0;
	float dTempy = 0;
	float dTemp = 0;

	std::vector<float> vecTemp(p._InterpolateVec.size());

	std::list<Point>::iterator it;
	for (it = listPoint.begin(); it != listPoint.end(); it++)
	{

		float dis = (*it).getDistance(p);

		if (dis < 1.0)
		{
			p._InterpolateVec = (*it)._InterpolateVec;
			return;
		}

		dis *= dis;
		dTemp += 1 / dis;

		for (int i = 0; i < (int)vecTemp.size(); i++)
		{
			vecTemp[i] += (*it)._InterpolateVec[i] / dis;

		}
	}

	if (dTemp < 0.000001)
	{
		dTemp = 0.000001;
	}

	for (unsigned int j = 0; j < vecTemp.size(); j++)
	{
		p._InterpolateVec[j] = vecTemp[j] / dTemp;
	}
}

Cell::~Cell(void)
{

}
