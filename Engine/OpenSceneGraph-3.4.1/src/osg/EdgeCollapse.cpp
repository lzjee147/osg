#include <osg/EdgeCollapse.h>
#include <osg/Material>

#include <list>
#include <algorithm>
#include <iterator>

using namespace osg;

typedef osg::TriangleIndexFunctor<CollectTriangleOperator> CollectTriangleIndexFunctor;

EdgeCollapse::EdgeCollapse()
{
	_matid = -1;
	_triangleSet.clear();
	_pointSet.clear();
	_originalPointList.clear();
	_costpointset.clear();
	_nocostpointset.clear();
	_edgecollapseinfolist.clear();
	element_ = new osg::Geometry;
}


EdgeCollapse::~EdgeCollapse(void)
{
	std::for_each(_edgeSet.begin(),_edgeSet.end(),dereference_clear());
	_triangleSet.clear();
	_pointSet.clear();
	_originalPointList.clear();
	_costpointset.clear();
	_nocostpointset.clear();
	_edgecollapseinfolist.clear();

}

void EdgeCollapse::InitElement(osg::Geometry* element,int matid)
{
	element_ = element;

	_matid = matid ;

	numverts_ = element_->getVertexArray()->getNumElements();

	_originalPointList.resize(numverts_);
	CopyVertexArrayToPointsVisitor copyVertexArrayToPoints(_originalPointList);
	element_->getVertexArray()->accept(copyVertexArrayToPoints);

	CopyArrayToPointsVisitor        copyArrayToPoints(_originalPointList);

	for(unsigned int ti=0;ti<element_->getNumTexCoordArrays();++ti)
	{
		if (element_->getTexCoordArray(ti))
			element_->getTexCoordArray(ti)->accept(copyArrayToPoints);
	}

	if (element_->getNormalArray() && element_->getNormalArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
		element_->getNormalArray()->accept(copyArrayToPoints);

	if (element_->getColorArray() && element_->getColorArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
		element_->getColorArray()->accept(copyArrayToPoints);

	if (element_->getSecondaryColorArray() && element_->getSecondaryColorArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
		element_->getSecondaryColorArray()->accept(copyArrayToPoints);

	if (element_->getFogCoordArray() && element_->getFogCoordArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
		element_->getFogCoordArray()->accept(copyArrayToPoints);

	for(unsigned int vi=0;vi<element_->getNumVertexAttribArrays();++vi)
	{
		if (element_->getVertexAttribArray(vi) &&  element_->getVertexAttribArray(vi)->getBinding()==osg::Array::BIND_PER_VERTEX)
			element_->getVertexAttribArray(vi)->accept(copyArrayToPoints);
	}

	//// now set the protected points up.
	//for(Simplifier::IndexList::const_iterator pitr=protectedPoints.begin();
	//	pitr!=protectedPoints.end();
	//	++pitr)
	//{
	//	_originalPointList[*pitr]->_protected = true;
	//}

	CollectTriangleIndexFunctor collectTriangles;
	collectTriangles.setEdgeCollapse(this);
	element_->accept(collectTriangles);
}

void EdgeCollapse::InitPointAndTriangleDatas(TriangleSet triangleset)
{
	PointSet::iterator pointitr;
	TriangleSet::iterator trianitr = triangleset.begin();
	for (trianitr;trianitr!= triangleset.end();++trianitr)
	{
		osg::ref_ptr<EdgeCollapse::Triangle> currenttriangle = *trianitr;

		float area = currenttriangle->CalcArea();
		if (area > 1e-6)
		{
			pointitr = _pointSet.find(currenttriangle->_p1);
			if(pointitr!=_pointSet.end())
			{
				currenttriangle->_p1 = const_cast<Point*>(pointitr->get());
			}
			else
			{
				_pointSet.insert(currenttriangle->_p1);
			}

			pointitr = _pointSet.find(currenttriangle->_p2);

			if (pointitr!=_pointSet.end())
			{
				currenttriangle->_p2 = const_cast<Point*>(pointitr->get());
			} 
			else
			{
				_pointSet.insert(currenttriangle->_p2);
			}

			pointitr = _pointSet.find(currenttriangle->_p3);
			if (pointitr!=_pointSet.end())
			{
				currenttriangle->_p3 = const_cast<Point*>(pointitr->get());
			} 
			else
			{
				_pointSet.insert(currenttriangle->_p3);
			}

			currenttriangle->InitOrdeTriangle();
			currenttriangle->InitPointDatas();
			_triangleSet.insert(currenttriangle);
		}

	}
}

void EdgeCollapse::CreateEdgeCollapseList()
{
	CalcQuadricMatrices();

	//ComputeAllEdgeCollapseCosts();
	int i=0;
	PointSet::iterator pointitr = _costpointset.begin();
	for (pointitr;pointitr!=_costpointset.end();pointitr++)
	{
		osg::ref_ptr<Point> currentpoint = *pointitr;
		currentpoint->_finalindex = i;
		i++;
	}

	PointSet::iterator nopointitr = _nocostpointset.begin();
	for (nopointitr;nopointitr!=_nocostpointset.end();nopointitr++)
	{
		osg::ref_ptr<Point> currentpoint = *pointitr;
		currentpoint->_finalindex = i;
		i++;
	}

	_collapseresult._costpointset = _costpointset;
	_collapseresult._nocostpointset = _nocostpointset;
	_collapseresult._triangleSet = _triangleSet;
	_collapseresult._matid = _matid;

	while (_triangleSet.size()>0&&_costpointset.size()!=0)
	{
		PointSet::iterator _costitr = _costpointset.begin();
		osg::ref_ptr<Point> currentpoint = *_costitr;
		BeginCollapse(currentpoint,currentpoint->_mincostpoint);
		//UpdataCollapseCost();
	}

	i =0;
	TriangleSet::iterator itr = _collapseresult._triangleSet.begin();
	for (itr;itr!=_collapseresult._triangleSet.end();itr++)
	{
		osg::ref_ptr<Triangle> currentri = *itr;
		currentri->RestTriangle();
		currentri->_indexnumber = i;
		i++;
	}

	_collapseresult._edgecollapseinfolist = _edgecollapseinfolist;
}


// melax法计算
void EdgeCollapse::ComputeAllEdgeCollapseCosts()
{
	std::set< osg::ref_ptr<Point>,dereference_less >::iterator itr;
	for (itr = _pointSet.begin();itr != _pointSet.end();itr++)
	{
		osg::ref_ptr<Point> currentpoint = *itr;

		ComputeEdgeCostAtVertex(currentpoint);

		_costpointset.insert(currentpoint);
	}
}

void EdgeCollapse::ComputeEdgeCostAtVertex(osg::ref_ptr<Point> currentpoint)
{
	if (currentpoint->_neighborpoint.size() == 0)
	{
		currentpoint->_mincostpoint = NULL;
		currentpoint->_cost = -0.01f;
	}

	currentpoint->_cost = FLT_MAX;
	currentpoint->_mincostpoint = NULL;

	std::set< osg::ref_ptr<Point>,dereference_less >::iterator neibor;
	for (neibor = currentpoint->_neighborpoint.begin();neibor != currentpoint->_neighborpoint.end();neibor++)
	{
		osg::ref_ptr<Point> neiborpoint = *neibor;

		float cost = ComputeEdgeCollapseCost(currentpoint,neiborpoint);

		if (cost<currentpoint->_cost)
		{
			currentpoint->_mincostpoint = neiborpoint;
			currentpoint->_cost = cost;
		}
	}
}

float EdgeCollapse:: ComputeEdgeCollapseCost(osg::ref_ptr<Point> fpoint,osg::ref_ptr<Point> tpoint)
{
	float edgelength = (tpoint->_vertex - fpoint->_vertex).length();
	float curvature=0;

	TriangleSet tranglessameedge;

	std::set< osg::ref_ptr<Triangle>>::iterator itr;
	for (itr = fpoint->_triangles.begin();itr != fpoint->_triangles.end();++itr)
	{
		osg::ref_ptr<Triangle> currenttria = *itr;

		if (currenttria->HasPoint(tpoint))
		{
			tranglessameedge.insert(currenttria);
		}
	}

	for (itr = fpoint->_triangles.begin();itr != fpoint->_triangles.end();++itr)
	{
		osg::ref_ptr<Triangle> tria1 = *itr;
		float mincurv = 1;
		std::set< osg::ref_ptr<Triangle>>::iterator edges;
		for (edges = tranglessameedge.begin();edges != tranglessameedge.end();++edges)
		{
			osg::ref_ptr<Triangle> edgetriange = *edges;

			float dotprod = tria1->_normal * edgetriange->_normal;
			mincurv = min(mincurv,(1-dotprod)/2.0f);
		}

		curvature = max(curvature,mincurv);
	}

	return edgelength*curvature;
}

//二次误差法计算
void EdgeCollapse::CalcQuadricMatrices()
{
	std::set< osg::ref_ptr<Point>,dereference_less >::iterator itr;
	for (itr = _pointSet.begin();itr != _pointSet.end();itr++)
	{
		osg::ref_ptr<Point> currentpoint = *itr;
		osg::Matrixd matrixd;
		if (currentpoint->_qmatrix == matrixd)
		{
			currentpoint->CalcQuadric();
		}
		QuadricCollapseCost(currentpoint);

		if (currentpoint->_mincostpoint!=NULL)
		{
			_costpointset.insert(currentpoint);
		}
		else
		{
			_nocostpointset.insert(currentpoint);
		}

	}

	//BuildEdgeCollapseList();
}

void EdgeCollapse::QuadricCollapseCost(osg::ref_ptr<Point> currentpoint)
{
	double mincost = FLT_MAX;
	PointSet neighborpoint = currentpoint->_neighborpoint;

	osg::Matrixd q1 = currentpoint->_qmatrix;

	std::set< osg::ref_ptr<Point>,dereference_less >::iterator itr;
	for (itr = neighborpoint.begin();itr != neighborpoint.end();itr++)
	{
		osg::ref_ptr<Point> curneibor = *itr;

		osg::Matrixd matrixd;
		if (curneibor->_qmatrix == matrixd)
		{
			curneibor->CalcQuadric();
		}

		osg::Matrixd q2 = curneibor->_qmatrix;

		osg::Matrixd qsum;
		for(int i = 0; i < 4; ++i) {
			for ( int j = 0; j < 4; ++j) {
				qsum(i,j) = q1(i,j) + q2(i,j);
			}
		}

		double triArea = currentpoint->_qtriArea;
		double cost = CalcQuadricError(qsum,curneibor,triArea);

		if (cost < mincost)
		{
			mincost = cost;
			currentpoint->_cost = cost;
			currentpoint->_mincostpoint = curneibor;
		}
	}
}

double EdgeCollapse::CalcQuadricError(osg::Matrixd matrixd,osg::ref_ptr<Point> neiborpoint,double triArea)
{
	double cost;

	osg::Vec4 result;

	const osg::Vec3 vertex_ = neiborpoint->_vertex;
	osg::Vec4 vertexvec(vertex_.x(),vertex_.y(),vertex_.z(),1);

	result = vertexvec * matrixd;

	cost = result * vertexvec;

	if (cost<0)
	{
		cost /= triArea;
	}
	else
	{
		cost *= triArea;
	}
	

	return cost;
}

//void EdgeCollapse::BuildEdgeCollapseList()
//{
//	/*std::set< osg::ref_ptr<Point>,dereference_less >::iterator itr;
//	for (itr = pointset.begin();itr != pointset.end();itr++)
//	{
//		EdgeCollaInfo eci;
//		osg::ref_ptr<Point> currentpoint = *itr;
//
//		eci._vfrom = currentpoint;
//		eci._vto = currentpoint->_mincostpoint;
//
//		setToPointQuadric(currentpoint->_mincostpoint,currentpoint);
//
//		PointSet affectedpoint;
//		UpdateTriangles(eci,affectedpoint);
//
//		PointSet affectequadricdpoint;
//
//	}*/
//
//	for (;;)
//	{
//		if (_costpointset.size() == 0)
//		{
//			break;
//		}
//
//		EdgeCollaInfo eci;
//		osg::ref_ptr<Point> currentpoint = *(_costpointset.begin());
//
//		eci._vfrom = currentpoint;
//		eci._vto = currentpoint->_mincostpoint;
//
//		currentpoint->_bActive = false;
//		_costpointset.erase(_costpointset.begin());
//
//		setToPointQuadric(currentpoint->_mincostpoint,currentpoint);
//
//		if (_edgecollainfolist.size() == 5538)
//		{
//			int aa = 0;
//		}
//
//		PointSet affectedpoint;
//		UpdateTriangles(eci,affectedpoint);
//
//		PointSet affectequadricdpoint;
//		UpdateAffectedPoint(affectedpoint,eci,affectequadricdpoint);
//
//		std::set< osg::ref_ptr<Point>,dereference_less >::iterator itr;
//		for (itr = affectequadricdpoint.begin();itr != affectequadricdpoint.end();itr++)
//		{
//			osg::ref_ptr<Point> curnaffpoint = *itr;
//
//			QuadricCollapseCost(curnaffpoint);
//		}
//
//		_edgecollainfolist.push_back(eci);
//	}
//
//}
//
//void EdgeCollapse::setToPointQuadric(osg::ref_ptr<Point> to,osg::ref_ptr<Point> from)
//{
//	osg::Matrixd qt,qf;
//	qt = to->_qmatrix;
//	qf = from->_qmatrix;
//
//	for(int i = 0; i < 4; ++i) {
//		for ( int j = 0; j < 4; ++j) {
//			qt(i,j) += qf(i,j);
//		}
//	}
//
//	to->_qmatrix = qt;
//}
//
//void EdgeCollapse::UpdateTriangles(EdgeCollaInfo& eci,PointSet& affectedpoint)
//{
//	osg::ref_ptr<Point> currentpoint = eci._vfrom;
//
//	std::set< osg::ref_ptr<Triangle>>& trineighbors = currentpoint->_triangles;
//	std::set< osg::ref_ptr<Triangle>>::iterator itr;
//	for (itr = trineighbors.begin();itr != trineighbors.end();++itr)
//	{
//		osg::ref_ptr<Triangle> currenttria = *itr;
//
//		if (!currenttria->_bActive) continue;
//
//		
//		bool bremovetri = false;
//		if (currenttria->HasPoint(eci._vfrom) && currenttria->HasPoint(eci._vto))
//		{
//			eci._triremoved.insert(currenttria);
//			currenttria->ChangePoint(eci._vfrom,eci._vto);
//			bremovetri = true;
//			currenttria->_bActive = false;
//		}
//		else
//		{
//			currenttria->calcNormal();
//			eci._vto->_triangles.insert(currenttria);
//
//			float area = currenttria->CalcArea();
//			currenttria->ChangePoint(eci._vfrom,eci._vto);
//			if (area < 1e-6)
//			{
//				currenttria->_bActive = false;
//				eci._triremoved.insert(currenttria);
//
//				bremovetri = true;
//			}
//			else
//			{
//				eci._trisaffected.insert(currenttria);
//			}
//		}
//
//		affectedpoint.insert(currenttria->_p1);
//		affectedpoint.insert(currenttria->_p2);
//		affectedpoint.insert(currenttria->_p3);
//
//		if (bremovetri)
//		{
//			currenttria->_p1->_triangles.erase(currenttria);
//			currenttria->_p2->_triangles.erase(currenttria);
//			currenttria->_p3->_triangles.erase(currenttria);
//		}
//	}
//}
//
//void EdgeCollapse::UpdateAffectedPoint(PointSet& affectedpoint,EdgeCollaInfo &eci,PointSet& affectequadricdpoint)
//{
//	std::set< osg::ref_ptr<Point>,dereference_less >::iterator itr;
//	for (itr = affectedpoint.begin();itr != affectedpoint.end();itr++)
//	{
//		osg::ref_ptr<Point> curneipoint = *itr;
//		_costpointset.erase(curneipoint);
//
//		UpdateAffectedPointNeighbors(curneipoint,eci,affectedpoint);
//
//		bool bActiveVert = false;
//		std::set< osg::ref_ptr<Triangle> >::iterator itriange;
//		for (itriange = curneipoint->_triangles.begin();itriange != curneipoint->_triangles.end();itriange++)
//		{
//			osg::ref_ptr<Triangle> currenttrian = *itriange;
//
//			if (currenttrian->_bActive)
//			{
//				bActiveVert = true;
//				break;
//			}
//		}
//		if (bActiveVert)
//		{
//			_costpointset.insert(curneipoint);
//			curneipoint->_bActive = true;
//			affectequadricdpoint.insert(curneipoint);
//			curneipoint->_bActive = false;
//		}
//	}
//}
//
//void EdgeCollapse::UpdateAffectedPointNeighbors(osg::ref_ptr<Point> currentpoint,EdgeCollaInfo &eci,PointSet& affectedpoint)
//{
//	if (currentpoint->_index != eci._vto->_index)
//	{
//		currentpoint->_neighborpoint.insert(eci._vto);
//	}
//	else
//	{
//		std::set< osg::ref_ptr<Point>,dereference_less >::iterator itr;
//		for (itr = affectedpoint.begin();itr != affectedpoint.end();itr++)
//		{
//			if (*itr != eci._vto)
//			{
//				currentpoint->_neighborpoint.insert(*itr);
//			}
//		}
//	}
//
//	currentpoint->_neighborpoint.erase(eci._vfrom);
//}

void EdgeCollapse::BeginCollapse(osg::ref_ptr<Point> fpoint,osg::ref_ptr<Point> tpoint)
{
	EdgeCollapseInfo eci;
	eci._frompoint = fpoint;
	eci._topoint = tpoint;

	if (!fpoint)
	{
		_costpointset.erase(tpoint);
		return;
	}

	PointSet tmp;

	std::set< osg::ref_ptr<Point>,dereference_less >::iterator itr;
	for (itr = fpoint->_neighborpoint.begin();itr != fpoint->_neighborpoint.end();++itr)
	{
		osg::ref_ptr<Point> currenttria = *itr;
		tmp.insert(currenttria);
	}

	std::set< osg::ref_ptr<Triangle>>::iterator itrtrator = fpoint->_triangles.begin();
	for (itrtrator;itrtrator != fpoint->_triangles.end();itrtrator++)
	{
		osg::ref_ptr<Triangle> currenttria = *itrtrator;

		/*if (currenttria->HasPoint(tpoint))
		{
			itrtrator = fpoint->_triangles.erase(itrtrator);
		}
		else
		{
			itrtrator++;
		}*/
		std::set< osg::ref_ptr<Triangle>>::iterator itrset = _triangleSet.find(currenttria);
		if (itrset!=_triangleSet.end())
		{
			osg::ref_ptr<Triangle> currenttria = *itrset;
			_triangleSet.erase(itrset);
			eci._removetriangle.push_back(currenttria);
		}
		
	}

	std::set< osg::ref_ptr<Triangle>>::iterator itrnew = fpoint->_triangles.begin();
	while (itrnew != fpoint->_triangles.end())
	{
		osg::ref_ptr<Triangle> currenttria = *itrnew;

		currenttria->ChangePoint(fpoint,tpoint);

		itrnew = fpoint->_triangles.erase(itrnew);

		eci._affectedtriangle.push_back(currenttria);
	}

	_costpointset.erase(_costpointset.begin());

	std::set< osg::ref_ptr<Point>,dereference_less >::iterator itrtem;
	for (itrtem = tmp.begin();itrtem != tmp.end();++itrtem)
	{
		osg::ref_ptr<Point> tempoint = *itrtem;

		if (_costpointset.find(tempoint)!= _costpointset.end())
		{
			_costpointset.erase(tempoint);
			QuadricCollapseCost(tempoint);
			//ComputeEdgeCostAtVertex(tempoint);
			_costpointset.insert(tempoint);
		}
		else
		{
			QuadricCollapseCost(tempoint);
			//ComputeEdgeCostAtVertex(tempoint);
		}
		
	}

	_edgecollapseinfolist.push_back(eci);
}

void EdgeCollapse::UpdataCollapseCost()
{
	PointSet newcostpointset;
	PointSet::iterator itr = _costpointset.begin();
	for (itr;itr!=_costpointset.end();itr++)
	{
		newcostpointset.insert(*itr);
	}
	_costpointset.clear();

	_costpointset = newcostpointset;
}

void EdgeCollapse::CopyBackToGeometry()
{
	_originalPointList.clear();
	std::copy(_costpointset.begin(),_costpointset.end(),std::back_inserter(_originalPointList));
	std::copy(_nocostpointset.begin(),_nocostpointset.end(),std::back_inserter(_originalPointList));

	CopyPointsToVertexArrayVisitor copyVertexArrayToPoints(_originalPointList);
	element_->getVertexArray()->accept(copyVertexArrayToPoints);

	// copy other per vertex attributes across to local point list.
	CopyPointsToArrayVisitor  copyArrayToPoints(_originalPointList);

	for(unsigned int ti=0;ti<element_->getNumTexCoordArrays();++ti)
	{
		if (element_->getTexCoordArray(ti))
			element_->getTexCoordArray(ti)->accept(copyArrayToPoints);
	}

	if (element_->getNormalArray() && element_->getNormalArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
	{
		element_->getNormalArray()->accept(copyArrayToPoints);

		// now normalize the normals.
		NormalizeArrayVisitor nav;
		element_->getNormalArray()->accept(nav);
	}

	if (element_->getColorArray() && element_->getColorArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
		element_->getColorArray()->accept(copyArrayToPoints);

	if (element_->getSecondaryColorArray() && element_->getSecondaryColorArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
		element_->getSecondaryColorArray()->accept(copyArrayToPoints);

	if (element_->getFogCoordArray() && element_->getFogCoordArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
		element_->getFogCoordArray()->accept(copyArrayToPoints);

	for(unsigned int vi=0;vi<element_->getNumVertexAttribArrays();++vi)
	{
		if (element_->getVertexAttribArray(vi) &&  element_->getVertexAttribArray(vi)->getBinding()==osg::Array::BIND_PER_VERTEX)
			element_->getVertexAttribArray(vi)->accept(copyArrayToPoints);
	}

	 typedef std::set< osg::ref_ptr<Triangle>, dereference_less >    TrianglesSorted;
	 TrianglesSorted trianglesSorted;

	/* for (PointSet::iterator itr =_costpointset.begin(); itr != _costpointset.end(); ++itr)
	 {
		 osg::ref_ptr<Point> currentpoint = *itr;

		 TriangleSet::iterator trineibor;
		 for (trineibor = currentpoint->_triangles.begin();trineibor != currentpoint->_triangles.end();++trineibor)
		 {
			 trianglesSorted.insert(*trineibor);
		 }
	 }*/

	 for(TriangleSet::iterator itr = _triangleSet.begin();
		 itr != _triangleSet.end();
		 ++itr)
	 {
		 trianglesSorted.insert(*itr);
	 }

	 osg::DrawElementsUInt* primitives = new osg::DrawElementsUInt(GL_TRIANGLES,trianglesSorted.size()*3);
	 unsigned int pos = 0;
	 for(TrianglesSorted::iterator titr=trianglesSorted.begin();
		 titr!=trianglesSorted.end();
		 ++titr)
	 {
		 const Triangle* triangle = (*titr).get();
		 (*primitives)[pos++] = triangle->_p1->_index;
		 (*primitives)[pos++] = triangle->_p2->_index;
		 (*primitives)[pos++] = triangle->_p3->_index;
	 }

	 element_->getPrimitiveSetList().clear();
	 element_->addPrimitiveSet(primitives);
}

void EdgeCollapse::InitResult()
{
	element_ = new osg::Geometry;

	if (!_collapseresult._haschanged)
	{
		TriangleSet::iterator itr = _collapseresult._triangleSet.begin();
		for (itr;itr!=_collapseresult._triangleSet.end();itr++)
		{
			osg::ref_ptr<Triangle> currentri = *itr;
			currentri->InitOrdeTriangle();
		}
	}
	else
	{
		TriangleSet::iterator itr = _collapseresult._triangleSet.begin();
		for (itr;itr!=_collapseresult._triangleSet.end();itr++)
		{
			osg::ref_ptr<Triangle> currentri = *itr;
			currentri->RestTriangle();
		}
	}

	_costpointset = _collapseresult._costpointset;
	 _triangleSet = _collapseresult._triangleSet;

	_edgecollapseinfolist = _collapseresult._edgecollapseinfolist;
}

int EdgeCollapse::SetTargetGeometry(int indexnum , int numbertriangles)
{
	for(size_t i = indexnum ; i<_edgecollapseinfolist.size(); i++)
	{
		if (_triangleSet.size()<=numbertriangles||_costpointset.size()==0)
		{
			return i;
			break;
		}

		EdgeCollapseInfo eci = _edgecollapseinfolist[i];

		osg::ref_ptr<Point> frompoint = eci._frompoint;
		osg::ref_ptr<Point> topoint = eci._topoint;

		PointSet::iterator poitr = _costpointset.find(frompoint);
		if (poitr!= _costpointset.end())
		{
			_costpointset.erase(poitr);
		}
		
		TriangleList removetriangle = eci._removetriangle;
		
		for (size_t j = 0; j < removetriangle.size();j++)
		{
			_triangleSet.erase(removetriangle[j]);
		}
		
		TriangleList affectedtriangle = eci._affectedtriangle;

		for (size_t j = 0; j < affectedtriangle.size();j++)
		{
			affectedtriangle[j]->ChangeWithoutUpdataPoint(frompoint,topoint);
		}
	}
	return numbertriangles;
}

bool EdgeCollapse::GetGeometry(osg::ref_ptr<osg::Geometry>& elegeo)
{
	_originalPointList.clear();
	_normal_array = new osg::Vec3Array;
	std::copy(_costpointset.begin(),_costpointset.end(),std::back_inserter(_originalPointList));

	CopyPointsToVertexArrayVisitor copyVertexArrayToPoints(_originalPointList);

	osg::ref_ptr<osg::Vec3Array> vertexarry = new osg::Vec3Array;
	elegeo->setVertexArray(vertexarry);

	elegeo->getVertexArray()->accept(copyVertexArrayToPoints);

	// copy other per vertex attributes across to local point list.
	CopyPointsToArrayVisitor  copyArrayToPoints(_originalPointList);

	osg::ref_ptr<osg::Vec2Array> texarry = new osg::Vec2Array;
	elegeo->setTexCoordArray(0,texarry);
	for(unsigned int ti=0;ti<elegeo->getNumTexCoordArrays();++ti)
	{
		if (elegeo->getTexCoordArray(ti))
			elegeo->getTexCoordArray(ti)->accept(copyArrayToPoints);
	}

	osg::ref_ptr<osg::Vec3Array> normalarry = new osg::Vec3Array;
	normalarry->setBinding(osg::Array::BIND_PER_VERTEX);
	elegeo->setNormalArray(normalarry);

	if (elegeo->getNormalArray() && elegeo->getNormalArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
	{
		elegeo->getNormalArray()->accept(copyArrayToPoints);

		// now normalize the normals.
		NormalizeArrayVisitor nav;
		elegeo->getNormalArray()->accept(nav);
	}

	if (elegeo->getColorArray() && elegeo->getColorArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
		elegeo->getColorArray()->accept(copyArrayToPoints);

	if (elegeo->getSecondaryColorArray() && elegeo->getSecondaryColorArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
		elegeo->getSecondaryColorArray()->accept(copyArrayToPoints);

	if (elegeo->getFogCoordArray() && elegeo->getFogCoordArray()->getBinding()==osg::Array::BIND_PER_VERTEX)
		elegeo->getFogCoordArray()->accept(copyArrayToPoints);

	for(unsigned int vi=0;vi<elegeo->getNumVertexAttribArrays();++vi)
	{
		if (elegeo->getVertexAttribArray(vi) &&  elegeo->getVertexAttribArray(vi)->getBinding()==osg::Array::BIND_PER_VERTEX)
			elegeo->getVertexAttribArray(vi)->accept(copyArrayToPoints);
	}

	typedef std::set< osg::ref_ptr<Triangle>, dereference_less >    TrianglesSorted;
	TrianglesSorted trianglesSorted;

	for(TriangleSet::iterator itr = _triangleSet.begin();
		itr != _triangleSet.end();
		++itr)
	{
		trianglesSorted.insert(*itr);
	}

	osg::DrawElementsUInt* primitives = new osg::DrawElementsUInt(GL_TRIANGLES,trianglesSorted.size()*3);
	unsigned int pos = 0;
	for(TrianglesSorted::iterator titr=trianglesSorted.begin();
		titr!=trianglesSorted.end();
		++titr)
	{
		const Triangle* triangle = (*titr).get();
		(*primitives)[pos++] = triangle->_p1->_index;
		(*primitives)[pos++] = triangle->_p2->_index;
		(*primitives)[pos++] = triangle->_p3->_index;
	}

	 if (primitives->size()>0)
	 {
		 elegeo->addPrimitiveSet(primitives);
		 return true;
	 }

	 return false;
}


