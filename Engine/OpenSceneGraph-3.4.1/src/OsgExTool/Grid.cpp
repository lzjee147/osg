
#include "OsgExTool/Grid.h"
#include "OsgExTool/Point.h"
// #include <fstream>

/*#include <iostream>*/

void BaseGrid::construct2DArray()
{
	//为了避免精度损失引起的误差，X和Y方向的网格数各增加1
	this->m_iNumX = (this->m_dXMax - this->m_dXMin) / this->m_fSpaceX + 2;
	this->m_iNumY = (this->m_dYMax - this->m_dYMin) / this->m_fSpaceY + 2;

	for (int i = 0; i < this->m_iNumY; i++)
	{
		std::vector<Cell> vecCell;

		for (int j = 0; j < this->m_iNumX; j++)
		{
			Cell cell;
			vecCell.push_back(cell);
		}

		this->m_cell.push_back(vecCell);

	}

}

void BaseGrid::getContourRange(float& dXMax, float& dXMin, float& dYMax, float& dYMin)
{
	dXMax = this->m_dXMax;
	dXMin = this->m_dXMin;

	dYMax = this->m_dYMax;
	dYMin = this->m_dYMin;

}

void BaseGrid::setContourRange(float dXMax, float dXMin, float dYMax, float dYMin)
{
	this->m_dXMax = dXMax;
	this->m_dXMin = dXMin;

	this->m_dYMax = dYMax;
	this->m_dYMin = dYMin;
}

BaseGrid::BaseGrid( void )
{

}

BaseGrid::~BaseGrid( void )
{

}
//////////////////////////////////////////////////////////////////////////


SparseGrid::SparseGrid(void)
{
	m_fSpaceX = 800.0;
	m_fSpaceY = 800.0;
}

SparseGrid::~SparseGrid(void)
{

}
void SparseGrid::initializeCGrid(float dXMax, float dXMin, float dYMax, float dYMin, std::vector<Point>& vecPoint)
{
	setContourRange(dXMax, dXMin, dYMax, dYMin);

	construct2DArray();
	//---------------------------------//
	int iTempX = 0;
	int iTempY = 0;

	int isize = vecPoint.size();
	for (int i = 0; i < isize; i++)
	{

		iTempX = (int)( (vecPoint[i]._x - this->m_dXMin) / this->m_fSpaceX );

		iTempY = (int)( (vecPoint[i]._y - this->m_dYMin) / this->m_fSpaceY );

		this->m_cell[iTempY][iTempX].getListPoint().push_back(vecPoint[i]);

	}


}

void SparseGrid::searchCellForRect(Cell& cell, int indexX, int indexY)
{
  	int Length = 3;

    while (cell.getPoinsCount() == 0)
    {
		indexX--;
		indexY--;
		for (int j = 0; j < Length; j++)
		{
			for (int i = 0; i < Length; i++)
			{
                
				if ( (indexY + j) > 0 && (indexY + j) < m_iNumY && (indexX + i) > 0 && (indexX + i) < m_iNumX ) 
				{
					cell.mergerCell( m_cell[indexY + j][indexX + i] );

				}
			}
		}

		Length += 2;
    }
}

//利用相邻单元格的位置索引关系得到需要合并的单元格(两个或者四个)
void SparseGrid::getInterpolateCells(Point* pointInput, Cell& cell)
{
	Point center;
	int iTempX, iTempY;

	iTempX = (int)( (pointInput->_x - this->m_dXMin) / this->m_fSpaceX );
	iTempY = (int)( (pointInput->_y - this->m_dYMin) / this->m_fSpaceY );

	center._x = (iTempX + 0.5) * this->m_fSpaceX;
	center._y = (iTempY + 0.5) * this->m_fSpaceY;


	int type = Cell::getPointPosition(*pointInput, center);
	cell.mergerCell(this->m_cell[iTempY][iTempX]);
	//----------------------------------
	if (type == 0)
	{
		if ( (iTempX - 1) >= 0)
		{

			cell.mergerCell(this->m_cell[iTempY][iTempX - 1]);

		}

		if ( (iTempX - 1) >= 0 && (iTempY - 1) >= 0)
		{
			cell.mergerCell(this->m_cell[iTempY - 1][iTempX - 1]);
		}

		if (  (iTempY- 1) >= 0 )
		{
			cell.mergerCell(this->m_cell[iTempY - 1][iTempX]);

		}

	}
	////-----------------------------------
	else if (type == 1)
	{
		if ( (iTempY + 1) < this->m_iNumY)
		{
			cell.mergerCell(this->m_cell[iTempY + 1][iTempX]);
		}

		if ( (iTempX - 1) >= 0 && (iTempY + 1) < this->m_iNumY)
		{
			cell.mergerCell(this->m_cell[iTempY + 1][iTempX - 1]);
		}

		if (  (iTempX - 1) >= 0 )
		{
			cell.mergerCell(this->m_cell[iTempY][iTempX - 1]);

		}

	}
	//-------------------------------------
	else if (type == 2)
	{
		if ( (iTempX + 1) < this->m_iNumX)
		{
			cell.mergerCell(this->m_cell[iTempY][iTempX + 1]);
		}

		if ( (iTempX + 1) < this->m_iNumX && (iTempY + 1) < this->m_iNumY )
		{
			cell.mergerCell(this->m_cell[iTempY + 1][iTempX + 1]);
		}

		if (  (iTempY + 1) < this->m_iNumY  )
		{
			cell.mergerCell(this->m_cell[iTempY + 1][iTempX]);

		}

	}
	////---------------------------------
	else if (type == 3)// RightBottom
	{
		if ( (iTempY - 1) >= 0 )
		{
			cell.mergerCell(this->m_cell[iTempY - 1][iTempX]);
		}

		if ( (iTempY - 1) >= 0 && (iTempX + 1) < this->m_iNumX )
		{
			cell.mergerCell(this->m_cell[iTempY - 1][iTempX + 1]);
		}

		if (  (iTempX + 1) < this->m_iNumX )
		{
			cell.mergerCell(this->m_cell[iTempY][iTempX + 1]);

		}

	}
    //----------如果周边单元格里面没有点,则按正方形方式搜索
	if (cell.getPoinsCount() == 0)
	{
         searchCellForRect(cell, iTempX, iTempY);
	}
	//---------------------------------

}

void SparseGrid::getVelocityInterpolate(Point& p)
{
	Cell cellTemp;
	//-------------------------------------------
	getInterpolateCells(&p, cellTemp);
	cellTemp.getVelocityInterpolate(p);
	//-------------------------------------------
}


//-------------------------------------------------------------------------

void DemGrid::Construct(float spaceX, float spaceY,
						 float dXMax, float dXMin, float dYMax, float dYMin, std::vector<Point>& vecPoint)

{
	SparseGrid initCGrid;

	if (vecPoint.size() < 1000)
	{
		initCGrid.setSpaceX((dXMax - dXMin) / 2);
		initCGrid.setSpaceY((dYMax - dYMin) / 2);
	}
	else
	{
		initCGrid.setSpaceX((dXMax - dXMin) / 5);
		initCGrid.setSpaceY((dYMax - dYMin) / 5);
	}

	initCGrid.initializeCGrid(dXMax, dXMin, dYMax, dYMin, vecPoint);
	Point p[4];
	p[0]._InterpolateVec.resize(vecPoint[0]._InterpolateVec.size());
	p[1]._InterpolateVec.resize(vecPoint[0]._InterpolateVec.size());
	p[2]._InterpolateVec.resize(vecPoint[0]._InterpolateVec.size());
	p[3]._InterpolateVec.resize(vecPoint[0]._InterpolateVec.size());
	//-----------------------------
	setSpaceX(spaceX);
	setSpaceY(spaceY);
	setContourRange(dXMax, dXMin, dYMax, dYMin);//设置密集网格的包围盒,必需从稀蔬网格得到
	construct2DArray();


	for (int j = 0; j < m_iNumY; j++)
	{	
		for (int i = 0; i < m_iNumX; i++)
		{
			p[0]._x = dXMin + i * spaceX;
			p[0]._y = dYMin + j * spaceY;
			initCGrid.getVelocityInterpolate(p[0]); 

			p[1]._x = dXMin + (i + 1) * spaceX;
			p[1]._y = dYMin + j * spaceY;
			initCGrid.getVelocityInterpolate(p[1]);  

			p[2]._x = dXMin + (i + 1) * spaceX;
			p[2]._y = dYMin + (j + 1) * spaceY;
			initCGrid.getVelocityInterpolate(p[2]); 

			p[3]._x = dXMin + i * spaceX;
			p[3]._y = dYMin + (j + 1) * spaceY;
			initCGrid.getVelocityInterpolate(p[3]); 

			this->m_cell[j][i].setCornerPoint(p[0], p[3], p[2], p[1]);//得到密集网格的四个顶点速度
			this->m_cell[j][i].calculateAverageVelocity();//计算四个顶点速度均值
		}
	}   

}    

void DemGrid::getDemAverageVelocity(Point& inPut)
{
	int i = (int)( (inPut._x - this->m_dXMin) / this->m_fSpaceX );
	int j = (int)( (inPut._y - this->m_dYMin) / this->m_fSpaceY );

	if (i < this->m_iNumX && j < this->m_iNumY)
	{
		inPut =  this->m_cell[j][i].getAverageVelocity();
	}
	else
	{
		inPut = this->m_cell[m_iNumY-1][m_iNumX - 1].getAverageVelocity();
	}
}

void DemGrid::getDemReverseVelocity(Point& inPut)
{
	int i = (int)( (inPut._x - this->m_dXMin) / this->m_fSpaceX );
	int j = (int)( (inPut._y - this->m_dYMin) / this->m_fSpaceY );

	if (i < this->m_iNumX && j < this->m_iNumY)
	{
		this->m_cell[j][i].getReverseVelocity(inPut);
	}
}



void DemGrid::getCGridNods(std::vector< std::vector<Point> >& cowColNodes)
{

	int i, j, k;

	for (j = 0; j < this->m_iNumY; j++)
	{ 
		std::vector<Point> vecP;
		for (i = 0; i < this->m_iNumX; i++)		
		{

			vecP.push_back( this->m_cell[j][i].getCornerPoint()[0]);
		}
		//i = m_iNumX, 所以 i - 1 = m_iNumX
		vecP.push_back(this->m_cell[j][i - 1].getCornerPoint()[3]);

		cowColNodes.push_back(vecP);
	}

	std::vector<Point> vec;
	for (k = 0; k < this->m_iNumX; k++)
	{
		vec.push_back(this->m_cell[this->m_iNumY - 1][k].getCornerPoint()[1]);
	}
	vec.push_back(this->m_cell[this->m_iNumY - 1][k - 1].getCornerPoint()[2]);

	cowColNodes.push_back(vec);

	cowColNodes_ = cowColNodes;
}

float DemGrid::getHeight(int c, int r)
{
	return cowColNodes_[r][c]._InterpolateVec[0];
}

Point DemGrid::getNormal(int c, int r)
{
	float dz_dx;
	if (c == 0)
	{
		dz_dx = (getHeight(c+1,r)-getHeight(c,r))/m_fSpaceX;
	}
	else if (c == m_iNumX)
	{
		dz_dx = (getHeight(c,r)-getHeight(c-1,r))/m_fSpaceX;
	}
	else // assume 0<c<_numColumns-1
	{
		dz_dx = 0.5f*(getHeight(c+1,r)-getHeight(c-1,r))/m_fSpaceX;
	}

	float dz_dy;
	if (r == 0)
	{
		dz_dy = (getHeight(c,r+1)-getHeight(c,r))/m_fSpaceY;
	}
	else if (r == m_iNumY)
	{
		dz_dy = (getHeight(c,r)-getHeight(c,r-1))/m_fSpaceY;
	}
	else // assume 0<r<_numRows-1
	{
		dz_dy = 0.5*(getHeight(c,r+1)-getHeight(c,r-1))/m_fSpaceY;
	}

	return Point(-dz_dx,-dz_dy,1.0f);
}

void DemGrid::writeCGrid(const std::string& path)
{
#if !(defined(_OFCEP_VS_0060))
	setlocale( LC_ALL, "chs" );
#endif

	std::ofstream ofs(path.c_str());

	ofs<<m_dXMax<<"  "<<m_dXMin<<"  "<<m_dYMax<<"  "<<m_dYMin/*<<endl*/;
	ofs<<m_fSpaceX<<"  "<<m_fSpaceY/*<<endl*/;
	ofs<<m_iNumX<<"  "<<m_iNumY/*<<endl*/;

	for (int i = 0; i < this->m_iNumY; i++)
	{
		for (int j = 0; j < this->m_iNumX; j++)
		{
			m_cell[i][j].writeCell(ofs);
			ofs/*<<endl*/;
		}
	}
  ofs.close();

#if !(defined(_OFCEP_VS_0060))
  setlocale( LC_ALL, "C" );
#endif
}

void DemGrid::readCGrid(const std::string& path)
{
#if !(defined(_OFCEP_VS_0060))
  setlocale( LC_ALL, "chs" );
#endif

	std::ifstream ifs(path.c_str());

	ifs>>m_dXMax>>m_dXMin>>m_dYMax>>m_dYMin;
	ifs>>m_fSpaceX>>m_fSpaceY;
	ifs>>m_iNumX>>m_iNumY;

	
	//-----------------------
  {
    for (int i = 0; i < this->m_iNumY; i++)
    {
      std::vector<Cell> vecCell;

      for (int j = 0; j < this->m_iNumX; j++)
      {
        Cell cell;
        vecCell.push_back(cell);
      }

      this->m_cell.push_back(vecCell);

    }
  }
	//-----------------------
  {
    for (int i = 0; i < this->m_iNumY; i++)
    {
      for (int j = 0; j < this->m_iNumX; j++)
      {
        m_cell[i][j].readCell(ifs);
      }
    }
  }
	ifs.close();

#if !(defined(_OFCEP_VS_0060))
  setlocale( LC_ALL, "C" );
#endif
}
//------------------------------------------------------------
