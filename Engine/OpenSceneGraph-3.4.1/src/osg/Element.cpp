/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#include <osg/Element>
#include <osg/Group>
#include <osg/NodeVisitor>
#include <osg/Notify>
#include <osg/OccluderNode>
#include <osg/Transform>
#include <osg/UserDataContainer>
#include <osg/Texture2D>
#include <osg/TexMat>
#include <osg/Material>
#include <Windows.h>
#include <algorithm>

using namespace osg;

Element::Element()
{
	_collapse_result.clear();
	_numberverter = 0;
	_numberfaces = 0;
}

Element::~Element()
{
	for (size_t i = 0 ; i<_collapse_result.size();++i)
	{
		std::for_each(_collapse_result[i]._costpointset.begin(),_collapse_result[i]._costpointset.end(),dereference_clear());
		std::for_each(_collapse_result[i]._nocostpointset.begin(),_collapse_result[i]._nocostpointset.end(),dereference_clear());
		std::for_each(_collapse_result[i]._triangleSet.begin(),_collapse_result[i]._triangleSet.end(),dereference_clear());
	}

	_collapse_result.clear();
}

void Element::GetCollapseResult(std::vector<EdgeCollapse::CollapseResult> collapseresult)
{
	_collapse_result = collapseresult;

	for(size_t i = 0 ; i< collapseresult.size() ; i++)
	{
		_numberverter += collapseresult[i]._costpointset.size();

		_numberverter += collapseresult[i]._nocostpointset.size();

		_numberfaces += collapseresult[i]._triangleSet.size();
	}
}

bool Element::GetPointInthePlane(osg::ref_ptr<EdgeCollapse::Point>& intersectpoint,osg::ref_ptr<EdgeCollapse::Point> beginpoint,osg::ref_ptr<EdgeCollapse::Point> endpoint,osg::Plane splitplane)
{
	osg::Vec3 beginend = endpoint->_vertex - beginpoint->_vertex;
	float num,den,interp;
	num = splitplane.getNormal()*beginpoint->_vertex+splitplane.asVec4().w();
	den = splitplane.getNormal()*beginend;
	interp = 0.5;
	if (::abs(den)>1.0e-12)
	{
		interp = -num/den;
		interp =  max(0.000001,min(0.999999,interp));
	}

	float interp2 = 1.0 -interp;
	intersectpoint->_vertex.x() = beginpoint->_vertex.x()*interp2 + endpoint->_vertex.x()*interp;
	intersectpoint->_vertex.y() = beginpoint->_vertex.y()*interp2 + endpoint->_vertex.y()*interp;
	intersectpoint->_vertex.z() = beginpoint->_vertex.z()*interp2 + endpoint->_vertex.z()*interp;

	for (size_t i =0;i<beginpoint->_attributes.size();++i)
	{
		intersectpoint->_attributes.push_back(beginpoint->_attributes[i]+(endpoint->_attributes[i]-beginpoint->_attributes[i])*interp);
	}

	//intersectpoint->_cost = FLT_MAX;
	
	return true;
}

void Element::SplitElement(osg::BoundingBox splitbox,osg::BoundingBox splitboxend,osg::Plane splitplane,osg::ref_ptr<Element>& begin_half_ele,osg::ref_ptr<Element>& end_half_ele)
{
	for(size_t i = 0 ; i< _collapse_result.size() ; ++i)
	{
		EdgeCollapse::PointSet begin_half_point;
		EdgeCollapse::PointSet end_half_point;

		EdgeCollapse::TriangleSet begin_half_triangle;
		EdgeCollapse::TriangleSet end_half_triangle;

		EdgeCollapse::TriangleSet::iterator trianitr = _collapse_result[i]._triangleSet.begin();
		for (trianitr;trianitr!= _collapse_result[i]._triangleSet.end();++trianitr)
		{
			osg::ref_ptr<EdgeCollapse::Triangle> currenttriangle = *trianitr;
			currenttriangle->InitOrdeTriangle();

			EdgeCollapse::PointList pointbegin, pointend;

			float fdist;
			fdist = splitplane.distance(currenttriangle->_p1->_vertex);
			(fdist >= 0.0) ? pointend.push_back(currenttriangle->_p1) : pointbegin.push_back(currenttriangle->_p1);
			
			fdist = splitplane.distance(currenttriangle->_p2->_vertex);
			(fdist >= 0.0) ? pointend.push_back(currenttriangle->_p2) : pointbegin.push_back(currenttriangle->_p2);

			fdist = splitplane.distance(currenttriangle->_p3->_vertex);
			(fdist >= 0.0) ? pointend.push_back(currenttriangle->_p3) : pointbegin.push_back(currenttriangle->_p3);

			if (pointbegin.size()==3)
			{
				osg::ref_ptr<EdgeCollapse::Triangle> begintrian = new EdgeCollapse::Triangle;

				begintrian->_p1 = pointbegin[0]->GetCopePoint();
				begintrian->_p2 = pointbegin[1]->GetCopePoint();
				begintrian->_p3 = pointbegin[2]->GetCopePoint();

				begin_half_triangle.insert(begintrian);
			}
			else if (pointbegin.size()==0)
			{
				osg::ref_ptr<EdgeCollapse::Triangle> endtrian = new EdgeCollapse::Triangle;

				endtrian->_p1 = pointend[0]->GetCopePoint();
				endtrian->_p2 = pointend[1]->GetCopePoint();
				endtrian->_p3 = pointend[2]->GetCopePoint();

				end_half_triangle.insert(endtrian);
			}
			else if (pointbegin.size()==1)
			{
				osg::ref_ptr<EdgeCollapse::Point> intersectpoint1 = new EdgeCollapse::Point;
				GetPointInthePlane(intersectpoint1,pointbegin[0],pointend[0],splitplane);

				osg::ref_ptr<EdgeCollapse::Point> intersectpoint2 = new EdgeCollapse::Point;
				GetPointInthePlane(intersectpoint2,pointbegin[0],pointend[1],splitplane);

				osg::ref_ptr<EdgeCollapse::Triangle> begintrian = new EdgeCollapse::Triangle;
				begintrian->_p1 = pointbegin[0]->GetCopePoint();
				begintrian->_p2 = intersectpoint1->GetCopePoint();
				begintrian->_p3 = intersectpoint2->GetCopePoint();

				begin_half_triangle.insert(begintrian);
				
				osg::ref_ptr<EdgeCollapse::Triangle> endtrian1 = new EdgeCollapse::Triangle;
				osg::ref_ptr<EdgeCollapse::Triangle> endtrian2 = new EdgeCollapse::Triangle;

				endtrian1->_p1 = pointend[0]->GetCopePoint();
				endtrian1->_p2 = intersectpoint2->GetCopePoint();
				endtrian1->_p3 = intersectpoint1->GetCopePoint();
				end_half_triangle.insert(endtrian1);

				endtrian2->_p1 = pointend[0]->GetCopePoint();
				endtrian2->_p2 = intersectpoint2->GetCopePoint();
				endtrian2->_p3 = pointend[1]->GetCopePoint();
				end_half_triangle.insert(endtrian2);

				//}
				/*else if (splitplane.distance(pointend[0]->_vertex)==0&&splitplane.distance(pointend[1]->_vertex)!=0)
				{
					osg::ref_ptr<EdgeCollapse::Triangle> endtrian1 = new EdgeCollapse::Triangle;
					endtrian1->_p1 = pointend[0];
					endtrian1->_p2 = pointend[1];
					endtrian1->_p3 = intersectpoint22;
					end_half_triangle.insert(endtrian1);

					endtrian1->InitOrdeTriangle();
					endtrian1->InitPointDatas();
				}
				else if (splitplane.distance(pointend[0]->_vertex)!=0&&splitplane.distance(pointend[1]->_vertex)==0)
				{
					osg::ref_ptr<EdgeCollapse::Triangle> endtrian1 = new EdgeCollapse::Triangle;
					endtrian1->_p1 = pointend[0];
					endtrian1->_p2 = pointend[1];
					endtrian1->_p3 = intersectpoint12;
					end_half_triangle.insert(endtrian1);

					endtrian1->InitOrdeTriangle();
					endtrian1->InitPointDatas();
				}*/
			}
			else if (pointbegin.size()==2)
			{
				osg::ref_ptr<EdgeCollapse::Point> intersectpoint1 = new EdgeCollapse::Point;
				GetPointInthePlane(intersectpoint1,pointbegin[0],pointend[0],splitplane);
				
				osg::ref_ptr<EdgeCollapse::Point> intersectpoint2 = new EdgeCollapse::Point;
				GetPointInthePlane(intersectpoint2,pointbegin[1],pointend[0],splitplane);
				
				osg::ref_ptr<EdgeCollapse::Triangle> begintrian1 = new EdgeCollapse::Triangle;
				osg::ref_ptr<EdgeCollapse::Triangle> begintrian2 = new EdgeCollapse::Triangle;

				begintrian1->_p1 = pointbegin[0]->GetCopePoint();
				begintrian1->_p2 = intersectpoint1->GetCopePoint();
				begintrian1->_p3 = intersectpoint2->GetCopePoint();

				begin_half_triangle.insert(begintrian1);

				begintrian2->_p1 = pointbegin[0]->GetCopePoint();
				begintrian2->_p2 = intersectpoint2->GetCopePoint();
				begintrian2->_p3 = pointbegin[1]->GetCopePoint();

				begin_half_triangle.insert(begintrian2);

				osg::ref_ptr<EdgeCollapse::Triangle> endtrian1 = new EdgeCollapse::Triangle;

				endtrian1->_p1 = pointend[0]->GetCopePoint();
				endtrian1->_p2 = intersectpoint2->GetCopePoint();
				endtrian1->_p3 = intersectpoint1->GetCopePoint();
				end_half_triangle.insert(endtrian1);
			}
		}

		EdgeCollapse* beginec = new EdgeCollapse();
		EdgeCollapse* endec = new EdgeCollapse();

		beginec->InitPointAndTriangleDatas(begin_half_triangle);
		endec->InitPointAndTriangleDatas(end_half_triangle);

		beginec->_matid = _collapse_result[i]._matid;
		endec->_matid = _collapse_result[i]._matid;

		beginec->CreateEdgeCollapseList();
		endec->CreateEdgeCollapseList();

		begin_half_ele->_collapse_result.push_back(beginec->_collapseresult);
		begin_half_ele->_numberfaces +=beginec->_triangleSet.size();
		end_half_ele->_collapse_result.push_back(endec->_collapseresult);
		end_half_ele->_numberfaces += endec->_triangleSet.size();

		delete beginec;
		delete endec;
	}
}



