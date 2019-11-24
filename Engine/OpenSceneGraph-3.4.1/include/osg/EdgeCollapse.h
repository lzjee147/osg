#ifndef OSG_EdgeCollapse
#define OSG_EdgeCollapse 1

#include <string>
#include <vector>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/TriangleIndexFunctor>

#include <set>
#include <list>
#include <algorithm>
#include <osg/Math>

#include <iterator>

using namespace std;
// by Zhx : 用于计算模型简化相关
namespace osg
{
	struct dereference_less
	{
		template<class T, class U>
		inline bool operator() (const T& lhs,const U& rhs) const
		{
			return *lhs < *rhs;
		}
	};

	template<class T>
	bool dereference_check_less(const T& lhs,const T& rhs)
	{
		if (lhs==rhs) return false;
		if (!lhs) return true;
		if (!rhs) return false;
		return *lhs < *rhs;
	}

	struct dereference_clear
	{
		template<class T>
		inline void operator() (const T& t)
		{
			T& non_const_t = const_cast<T&>(t);
			non_const_t->clear();
		}
	};

	class OSG_EXPORT EdgeCollapse
	{
#if 1
		typedef float error_type;
#else
		typedef double error_type;
#endif

	public:
		struct Triangle;
		struct Edge;
		struct Point;

	public:
		EdgeCollapse();
		~EdgeCollapse(void);

	public:
		typedef std::vector<float>                                                  FloatList;
		typedef std::set<osg::ref_ptr<Edge>,dereference_less >                      EdgeSet;
		typedef std::set< osg::ref_ptr<Point>,dereference_less >                    PointSet;
		typedef std::vector< osg::ref_ptr<Point> >                                  PointList;
		typedef std::vector< osg::ref_ptr<Triangle> >                               TriangleList;
		typedef std::set< osg::ref_ptr<Triangle> >                                  TriangleSet;
		typedef std::map< osg::ref_ptr<Triangle>, unsigned int, dereference_less >  TriangleMap;

		struct Point : public osg::Referenced
		{
			Point(): _protected(false),_bActive(true),_cost(NULL), _index(0),_qtriArea(0) {}

			bool _protected;

			unsigned int _index;

			bool _bActive;

			double _cost;

			osg::ref_ptr<Point> _mincostpoint;

			double _qtriArea ;

			osg::Vec3           _vertex;
			FloatList           _attributes;
			TriangleSet         _triangles;
			PointSet           _neighborpoint;
			osg::Matrixd       _qmatrix;

			unsigned int _finalindex;

			void SetCost(double cost) { _cost = cost; }
			double GetCost() const { return _cost; }

			void clear()
			{
				_attributes.clear();
				_triangles.clear();
				_neighborpoint.clear();
				_mincostpoint = 0;
			}

			bool operator < ( const Point& rhs) const
			{
				//if (_index == rhs._index) return false;
				if (GetCost()<rhs.GetCost()) return true;
				else if (rhs.GetCost()<GetCost()) return false;

				if (_vertex < rhs._vertex) return true;
				if (rhs._vertex < _vertex) return false;

				return _attributes < rhs._attributes;
			}

			//bool operator < ( const Point& rhs) const
			//{
			//	// both error metrics are computed
			//	if (GetCost()<rhs.GetCost()) return true;
			//	else if (rhs.GetCost()<GetCost()) return false;
			//	
			//	if (_vertex < rhs._vertex) return true;
			//	else if (rhs._vertex < _vertex) return false;

			//	return _attributes < rhs._attributes;
			//}

			void RemoveNeighBorPoint(osg::ref_ptr<Point> point)
			{
				PointSet::iterator itr = _neighborpoint.find(point);
				if (itr!=_neighborpoint.end())
				{
					_neighborpoint.erase(itr);
				}
			}

			bool isBoundaryPoint() const
			{
				if (_protected) return true;

				for(TriangleSet::const_iterator itr=_triangles.begin();
					itr!=_triangles.end();
					++itr)
				{
					const Triangle* triangle = itr->get();
					if ((triangle->_e1->_p1==this || triangle->_e1->_p2==this) && triangle->_e1->isBoundaryEdge()) return true;
					if ((triangle->_e2->_p1==this || triangle->_e2->_p2==this) && triangle->_e2->isBoundaryEdge()) return true;
					if ((triangle->_e3->_p1==this || triangle->_e3->_p2==this) && triangle->_e3->isBoundaryEdge()) return true;

					//if ((*itr)->isBoundaryTriangle()) return true;
				}
				return false;
			}

			void CalcQuadric()
			{
				std::set<osg::ref_ptr<Triangle>>::iterator itr;
				for (itr = _triangles.begin();itr !=_triangles.end();itr++)
				{
					osg::ref_ptr<Triangle> currenttria = *itr;

					float triArea = 1;
					triArea = currenttria->CalcArea();
					_qtriArea += triArea;

					const osg::Vec3 normal = currenttria->_normal;
					const float a = normal.x();
					const float b = normal.y();
					const float c = normal.z();
					const float d = currenttria->_d;

					_qmatrix(0,0) = a*a;
					_qmatrix(0,1) = a*b;
					_qmatrix(0,2) = a*c;
					_qmatrix(0,3) = a*d;

					_qmatrix(1,0) = b*a;
					_qmatrix(1,1) = b*b;
					_qmatrix(1,2) = b*c;
					_qmatrix(1,3) = b*d;

					_qmatrix(2,0) = c*a;
					_qmatrix(2,1) = c*b;
					_qmatrix(2,2) = c*c;
					_qmatrix(2,3) = c*d;

					_qmatrix(3,0) = d*a;
					_qmatrix(3,1) = d*b;
					_qmatrix(3,2) = d*c;
					_qmatrix(3,3) = d*d;
				}
			}

			osg::ref_ptr<Point> GetCopePoint()
			{
				osg::ref_ptr<Point> newpoint = new Point;
				newpoint->_vertex = _vertex;
				newpoint->_attributes = _attributes;
				return newpoint;
			}
		};

		struct Edge : public osg::Referenced
		{
			Edge(): _errorMetric(0.0), _maximumDeviation(1.0) {}

			void clear()
			{
				_p1 = 0;
				_p2 = 0;
				_triangles.clear();
				_proposedPoint = 0;
			}

			osg::ref_ptr<Point> _p1;
			osg::ref_ptr<Point> _p2;

			TriangleSet _triangles;

			error_type _errorMetric;
			error_type _maximumDeviation;

			osg::ref_ptr<Point> _proposedPoint;

			void setErrorMetric(error_type errorMetric) { _errorMetric = errorMetric; }
			error_type getErrorMetric() const { return _errorMetric; }

			bool operator < ( const Edge& rhs) const
			{
				// both error metrics are computed
				if (getErrorMetric()<rhs.getErrorMetric()) return true;
				else if (rhs.getErrorMetric()<getErrorMetric()) return false;

				if (dereference_check_less(_p1,rhs._p1)) return true;
				if (dereference_check_less(rhs._p1,_p1)) return false;

				return dereference_check_less(_p2,rhs._p2);
			}

			bool operator == ( const Edge& rhs) const
			{
				if (&rhs==this) return true;
				if (*this<rhs) return false;
				if (rhs<*this) return false;
				return true;
			}

			bool operator != ( const Edge& rhs) const
			{
				if (&rhs==this) return false;
				if (*this<rhs) return true;
				if (rhs<*this) return true;
				return false;
			}

			void addTriangle(Triangle* triangle)
			{
				_triangles.insert(triangle);
				// if (_triangles.size()>2) OSG_NOTICE<<"Warning too many triangles ("<<_triangles.size()<<") sharing edge "<<std::endl;
			}

			bool isBoundaryEdge() const
			{
				return _triangles.size()<=1;
			}

			bool isAdjacentToBoundary() const
			{
				return isBoundaryEdge() || _p1->isBoundaryPoint() || _p2->isBoundaryPoint();
			}


			void updateMaxNormalDeviationOnEdgeCollapse()
			{
				//OSG_NOTICE<<"updateMaxNormalDeviationOnEdgeCollapse()"<<std::endl;
				_maximumDeviation = 0.0f;
				for(TriangleSet::iterator itr1=_p1->_triangles.begin();
					itr1!=_p1->_triangles.end();
					++itr1)
				{
					if (_triangles.count(*itr1)==0)
					{
						_maximumDeviation = osg::maximum(_maximumDeviation, (*itr1)->computeNormalDeviationOnEdgeCollapse(this,_proposedPoint.get()));
					}
				}
				for(TriangleSet::iterator itr2=_p2->_triangles.begin();
					itr2!=_p2->_triangles.end();
					++itr2)
				{
					if (_triangles.count(*itr2)==0)
					{
						_maximumDeviation = osg::maximum(_maximumDeviation, (*itr2)->computeNormalDeviationOnEdgeCollapse(this,_proposedPoint.get()));
					}
				}
			}

			error_type getMaxNormalDeviationOnEdgeCollapse() const { return _maximumDeviation; }

		};

		struct Triangle : public osg::Referenced
		{
			Triangle():_bActive(true) {}

			void clear()
			{
				_p1 = 0;
				_p2 = 0;
				_p3 = 0;

				_e1 = 0;
				_e2 = 0;
				_e3 = 0;

				_porde1 = 0;
				_porde2 = 0;
				_porde3 = 0;
			}

			inline bool operator < (const Triangle& rhs) const
			{
				if (dereference_check_less(_p1,rhs._p1)) return true;
				if (dereference_check_less(rhs._p1,_p1)) return false;


				const Point* lhs_lower = dereference_check_less(_p2,_p3) ? _p2.get() : _p3.get();
				const Point* rhs_lower = dereference_check_less(rhs._p2,rhs._p3) ? rhs._p2.get() : rhs._p3.get();

				if (dereference_check_less(lhs_lower,rhs_lower)) return true;
				if (dereference_check_less(rhs_lower,lhs_lower)) return false;

				const Point* lhs_upper = dereference_check_less(_p2,_p3) ? _p3.get() : _p2.get();
				const Point* rhs_upper = dereference_check_less(rhs._p2,rhs._p3) ? rhs._p3.get() : rhs._p2.get();

				return dereference_check_less(lhs_upper,rhs_upper);
			}


			void setOrderedPoints(Point* p1, Point* p2, Point* p3)
			{
				Point* points[3];
				points[0] = p1;
				points[1] = p2;
				points[2] = p3;

				// find the lowest value point in the list.
				unsigned int lowest = 0;
				if (dereference_check_less(points[1],points[lowest])) lowest = 1;
				if (dereference_check_less(points[2],points[lowest])) lowest = 2;

				_p1 = points[lowest];
				_p2 = points[(lowest+1)%3];
				_p3 = points[(lowest+2)%3];
			}

			void update()
			{
				_plane.set(_p1->_vertex,_p2->_vertex,_p3->_vertex);

			}

			osg::Plane computeNewPlaneOnEdgeCollapse(Edge* edge,Point* pNew) const
			{
				const Point* p1 = (_p1==edge->_p1 || _p1==edge->_p2) ? pNew : _p1.get();
				const Point* p2 = (_p2==edge->_p1 || _p2==edge->_p2) ? pNew : _p2.get();
				const Point* p3 = (_p3==edge->_p1 || _p3==edge->_p2) ? pNew : _p3.get();

				return osg::Plane(p1->_vertex,p2->_vertex,p3->_vertex);
			}

			// note return 1 - dotproduct, so that deviation is in the range of 0.0 to 2.0, where 0 is coincident, 1.0 is 90 degrees, and 2.0 is 180 degrees.
			error_type computeNormalDeviationOnEdgeCollapse(Edge* edge,Point* pNew) const
			{
				const Point* p1 = (_p1==edge->_p1 || _p1==edge->_p2) ? pNew : _p1.get();
				const Point* p2 = (_p2==edge->_p1 || _p2==edge->_p2) ? pNew : _p2.get();
				const Point* p3 = (_p3==edge->_p1 || _p3==edge->_p2) ? pNew : _p3.get();

				osg::Vec3 new_normal = (p2->_vertex - p1->_vertex) ^ (p3->_vertex - p2->_vertex);
				new_normal.normalize();

				error_type result = 1.0 - (new_normal.x() * _plane[0] + new_normal.y() * _plane[1] + new_normal.z() * _plane[2]);
				return result;
			}

			error_type distance(const osg::Vec3& vertex) const
			{
				return error_type(_plane[0])*error_type(vertex.x())+
					error_type(_plane[1])*error_type(vertex.y())+
					error_type(_plane[2])*error_type(vertex.z())+
					error_type(_plane[3]);
			}

			bool isBoundaryTriangle() const
			{
				return (_e1->isBoundaryEdge() || _e2->isBoundaryEdge() ||  _e3->isBoundaryEdge());
			}

			void calcNormal()
			{
				osg::Vec3 vec1 = _p1->_vertex;
				osg::Vec3 vec2 = _p2->_vertex;
				osg::Vec3 vec3 = _p3->_vertex;

				osg::Vec3 veca = vec2 - vec1;
				osg::Vec3 vecb = vec3 - vec2;

				_normal = veca ^ vecb;
				_normal.normalize();

				_d = -_normal * vec1;
			}

			bool HasPoint(osg::ref_ptr<Point> point)
			{
				return (point == _p1 || point == _p2 || point == _p3);
			}

			void ChangePoint(osg::ref_ptr<Point> frompoint,osg::ref_ptr<Point> topoint)
			{
				if (frompoint == _p1)
				{
					_p1 = topoint;
				}
				else if (frompoint == _p2)
				{
					_p2 = topoint;
				}
				else if (frompoint == _p3)
				{
					_p3 = topoint;
				}

				topoint->_triangles.insert(this);

				frompoint->RemoveNeighBorPoint(_p1);
				_p1->RemoveNeighBorPoint(frompoint);
				frompoint->RemoveNeighBorPoint(_p2);
				_p2->RemoveNeighBorPoint(frompoint);
				frompoint->RemoveNeighBorPoint(_p3);
				_p2->RemoveNeighBorPoint(frompoint);

				_p1->_neighborpoint.insert(_p2);
				_p1->_neighborpoint.insert(_p3);
				_p2->_neighborpoint.insert(_p1);
				_p2->_neighborpoint.insert(_p3);
				_p3->_neighborpoint.insert(_p1);
				_p3->_neighborpoint.insert(_p2);

				calcNormal();
			}

			void ChangeWithoutUpdataPoint(osg::ref_ptr<Point> frompoint,osg::ref_ptr<Point> topoint)
			{
				if (frompoint == _p1)
				{
					_p1 = topoint;
				}
				else if (frompoint == _p2)
				{
					_p2 = topoint;
				}
				else if (frompoint == _p3)
				{
					_p3 = topoint;
				}
			}

			float CalcArea()
			{
				osg::Vec3 vec1 = _p1->_vertex;
				osg::Vec3 vec2 = _p2->_vertex;
				osg::Vec3 vec3 = _p3->_vertex;

				osg::Vec3 vecA = vec1 - vec2;
				osg::Vec3 vecB = vec3 - vec2;

				osg::Vec3 cross = vecA ^ vecB;

				float area = float(0.5 * cross.length());
				return area;
			}

			void RestTriangle()
			{
				_p1 = _porde1;
				_p2 = _porde2;
				_p3 = _porde3;
			}

			void InitOrdeTriangle()
			{
				_porde1 = _p1;
				_porde2 = _p2;
				_porde3 = _p3;
				calcNormal();
			}

			void InitPointDatas()
			{
				_p1->_neighborpoint.insert(_p2);
				_p1->_neighborpoint.insert(_p3);
				_p2->_neighborpoint.insert(_p1);
				_p2->_neighborpoint.insert(_p3);
				_p3->_neighborpoint.insert(_p1);
				_p3->_neighborpoint.insert(_p2);

				_p1->_triangles.insert(this);
				_p2->_triangles.insert(this);
				_p3->_triangles.insert(this);
			}

			osg::ref_ptr<Point> _p1;
			osg::ref_ptr<Point> _p2;
			osg::ref_ptr<Point> _p3;

			osg::ref_ptr<Edge> _e1;
			osg::ref_ptr<Edge> _e2;
			osg::ref_ptr<Edge> _e3;

			osg::ref_ptr<Point> _porde1;
			osg::ref_ptr<Point> _porde2;
			osg::ref_ptr<Point> _porde3;

			osg::Vec3 _normal;  // normal to plane
			float _d;   // This parameter is the "d" in the
			// plane equation ax + by + cz + d = 0
			// The plane equation of this triangle is used.

			osg::Plane _plane;
			bool _bActive;

			int _indexnumber;
		};

		error_type computeErrorMetric(Edge* edge,Point* point) const
		{
			if (_computeErrorMetricUsingLength)
			{
				error_type dx = error_type(edge->_p1->_vertex.x()) - error_type(edge->_p2->_vertex.x());
				error_type dy = error_type(edge->_p1->_vertex.y()) - error_type(edge->_p2->_vertex.y());
				error_type dz = error_type(edge->_p1->_vertex.z()) - error_type(edge->_p2->_vertex.z());
				return sqrt(dx*dx + dy*dy + dz*dz);
			}
			else if (point)
			{
				typedef std::set< osg::ref_ptr<Triangle> > LocalTriangleSet ;
				LocalTriangleSet triangles;
				std::copy( edge->_p1->_triangles.begin(), edge->_p1->_triangles.end(), std::inserter(triangles,triangles.begin()));
				std::copy( edge->_p2->_triangles.begin(), edge->_p2->_triangles.end(), std::inserter(triangles,triangles.begin()));

				const osg::Vec3& vertex = point->_vertex;
				error_type error = 0.0;

				if (triangles.empty()) return 0.0;

				for(LocalTriangleSet::iterator itr=triangles.begin();
					itr!=triangles.end();
					++itr)
				{
					error += fabs( (*itr)->distance(vertex) );
				}

				// use average of error
				error /= error_type(triangles.size());

				return error;
			}
			else
			{
				return 0;
			}
		}

		Triangle* addTriangle(unsigned int p1, unsigned int p2, unsigned int p3)
		{
			//OSG_NOTICE<<"addTriangle("<<p1<<","<<p2<<","<<p3<<")"<<std::endl;

			// detect if triangle is degenerate.
			if (p1==p2 || p2==p3 || p1==p3) return 0;

			Triangle* triangle = new Triangle;

			Point* points[3];
			points[0] = addPoint(triangle, p1);
			points[1] = addPoint(triangle, p2);
			points[2] = addPoint(triangle, p3);

			// find the lowest value point in the list.
			unsigned int lowest = 0;
			if (dereference_check_less(points[1],points[lowest])) lowest = 1;
			if (dereference_check_less(points[2],points[lowest])) lowest = 2;

			triangle->_p1 = points[lowest];
			triangle->_p2 = points[(lowest+1)%3];
			triangle->_p3 = points[(lowest+2)%3];

			triangle->_porde1 = points[lowest];
			triangle->_porde2 = points[(lowest+1)%3];
			triangle->_porde3 = points[(lowest+2)%3];

			triangle->_e1 = addEdge(triangle, triangle->_p1.get(), triangle->_p2.get());
			triangle->_e2 = addEdge(triangle, triangle->_p2.get(), triangle->_p3.get());
			triangle->_e3 = addEdge(triangle, triangle->_p3.get(), triangle->_p1.get());

			triangle->update();
			triangle->calcNormal();

			float area = triangle->CalcArea();
			if (area > 1e-6)
			{
				_triangleSet.insert(triangle);
			}
			/*else
			{
				triangle->_p1->_triangles.erase(triangle);
				triangle->_p2->_triangles.erase(triangle);
				triangle->_p3->_triangles.erase(triangle);

				_pointSet.erase(triangle->_p1);
				_pointSet.erase(triangle->_p2);
				_pointSet.erase(triangle->_p3);
			}*/

			return triangle;
		}

		Triangle* addTriangle(Point* p1, Point* p2, Point* p3)
		{
			// OSG_NOTICE<<"      addTriangle("<<p1<<","<<p2<<","<<p3<<")"<<std::endl;

			// detect if triangle is degenerate.
			if (p1==p2 || p2==p3 || p1==p3)
			{
				// OSG_NOTICE<<"    **** addTriangle failed - p1==p2 || p2==p3 || p1==p3"<<std::endl;
				return 0;
			}

			Triangle* triangle = new Triangle;

			Point* points[3];
			points[0] = addPoint(triangle, p1);
			points[1] = addPoint(triangle, p2);
			points[2] = addPoint(triangle, p3);

			// find the lowest value point in the list.
			unsigned int lowest = 0;
			if (dereference_check_less(points[1],points[lowest])) lowest = 1;
			if (dereference_check_less(points[2],points[lowest])) lowest = 2;

			triangle->_p1 = points[lowest];
			triangle->_p2 = points[(lowest+1)%3];
			triangle->_p3 = points[(lowest+2)%3];

			triangle->_e1 = addEdge(triangle, triangle->_p1.get(), triangle->_p2.get());
			triangle->_e2 = addEdge(triangle, triangle->_p2.get(), triangle->_p3.get());
			triangle->_e3 = addEdge(triangle, triangle->_p3.get(), triangle->_p1.get());

			triangle->update();

			_triangleSet.insert(triangle);

			return triangle;
		}

		Edge* addEdge(Triangle* triangle, Point* p1, Point* p2)
		{
			// OSG_NOTICE<<"        addEdge("<<p1<<","<<p2<<")"<<std::endl;
			osg::ref_ptr<Edge> edge = new Edge;
			if (dereference_check_less(p1, p2))
			{
				edge->_p1 = p1;
				edge->_p2 = p2;
			}
			else
			{
				edge->_p1 = p2;
				edge->_p2 = p1;
			}

			p1->_neighborpoint.insert(p2);
			p2->_neighborpoint.insert(p1);

			edge->setErrorMetric( computeErrorMetric( edge.get(), edge->_proposedPoint.get()));

			EdgeSet::iterator itr = _edgeSet.find(edge);
			if (itr==_edgeSet.end())
			{
				// OSG_NOTICE<<"          addEdge("<<edge.get()<<") edge->_p1="<<edge->_p1.get()<<" _p2="<<edge->_p2.get()<<std::endl;
				_edgeSet.insert(edge);
			}
			else
			{
				// OSG_NOTICE<<"          reuseEdge("<<edge.get()<<") edge->_p1="<<edge->_p1.get()<<" _p2="<<edge->_p2.get()<<std::endl;
				edge = *itr;
			}

			edge->addTriangle(triangle);

			return edge.get();
		}

		Point* addPoint(Triangle* triangle, unsigned int p1)
		{
			return addPoint(triangle,_originalPointList[p1].get());
		}

		Point* addPoint(Triangle* triangle, Point* point)
		{
			/*bool bfind = false;
			PointSet::iterator itrvertex = _pointSet.begin();
			for (itrvertex;itrvertex!=_pointSet.end();itrvertex++)
			{
			osg::ref_ptr<Point> curpoint = *itrvertex;
			if (curpoint->_vertex == point->_vertex)
			{
			point = const_cast<Point*>(itrvertex->get());
			bfind = true;
			break;
			}
			}

			if (!bfind)
			{
			_pointSet.insert(point);
			}*/

			/*int max = _pointSet.size();

			if (max == 0)
			{
				_pointSet.insert(point);
			}
			else
			{
				Point* pointnew = FindPoint(point,0,max);
				if ( pointnew!= NULL)
				{
					point = pointnew;
				}
			}*/
			

			PointSet::iterator itr = _pointSet.find(point);
			if (itr==_pointSet.end())
			{
				//OSG_NOTICE<<"  addPoint("<<point.get()<<")"<<std::endl;
				_pointSet.insert(point);
			}
			else
			{
				point = const_cast<Point*>(itr->get());
				//OSG_NOTICE<<"  reusePoint("<<point.get()<<")"<<std::endl;
			}

			/*PointSet::iterator itrvertex = _pointSet.lower_bound(point);

			if (itrvertex==_pointSet.end())
			{
				_pointSet.insert(point);
			}
			else
			{
				if ((const_cast<Point*>(itrvertex->get())->_vertex - point->_vertex).length()<1e-6)
				{
					point = const_cast<Point*>(itrvertex->get());
				}
				else
				{
					_pointSet.insert(point);
				}
			}*/
			

			point->_triangles.insert(triangle);

			return point;
		}

		//Point* FindPoint(Point* point,int min,int max)
		//{
		//	if(min > max)
		//	{
		//		return NULL;
		//	}

		//	int mid = (min+max)>>1;

		//	PointSet::iterator itrbegin = _pointSet.begin();
		//	std::advance(itrbegin,min);
		//	osg::ref_ptr<Point> beginpoint = const_cast<Point*>(itrbegin->get());			

		//	PointSet::iterator itrend = _pointSet.begin();
		//	std::advance(itrend,max-1);
		//	
		//	//if (itrend==_pointSet.end())
		//	//{
		//	//	if (point->_vertex == beginpoint->_vertex)
		//	//	{
		//	//		return beginpoint;
		//	//	}
		//	//	else
		//	//	{
		//	//		_pointSet.insert(point);
		//	//		return NULL;
		//	//	}
		//	//}

		//	PointSet::iterator itrmid = _pointSet.begin();
		//	std::advance(itrmid,mid);

		//	osg::ref_ptr<Point> endpoint = const_cast<Point*>(itrend->get());
		//	if ( endpoint->_vertex < point->_vertex)
		//	{
		//		_pointSet.insert(point);
		//		return NULL;
		//	}
		//	else if (point->_vertex < beginpoint->_vertex)
		//	{
		//		_pointSet.insert(point);
		//		return NULL;
		//	}
		//	else if (point->_vertex == beginpoint->_vertex)
		//	{
		//		return beginpoint;
		//	}
		//	else if (point->_vertex == endpoint->_vertex)
		//	{
		//		return endpoint;
		//	}

		//	osg::ref_ptr<Point> midpoint = const_cast<Point*>(itrmid->get());

		//	if (point->_vertex == midpoint->_vertex)
		//	{
		//		return midpoint;
		//	}
		//	else if (point->_vertex < midpoint->_vertex)
		//	{
		//		return FindPoint(point,0,mid);
		//	}
		//	else
		//	{
		//		return FindPoint(point,mid , max);
		//	}
		//}

	public:
		void InitElement(osg::Geometry* element,int matid);

		void InitPointAndTriangleDatas(TriangleSet triangleset);

		void CreateEdgeCollapseList();

		void CalcQuadricMatrices();

		void QuadricCollapseCost(osg::ref_ptr<Point> currentpoint);

		double CalcQuadricError(osg::Matrixd matrixd,osg::ref_ptr<Point> neiborpoint,double triArea);

		/*void BuildEdgeCollapseList();

		void setToPointQuadric(osg::ref_ptr<Point> to,osg::ref_ptr<Point> from);

		void UpdateTriangles(EdgeCollaInfo &eci,PointSet& affectedpoint);

		void UpdateAffectedPoint(PointSet& affectedpoint,EdgeCollaInfo &eci,PointSet& affectequadricdpoint);

		void UpdateAffectedPointNeighbors(osg::ref_ptr<Point> currentpoint,EdgeCollaInfo &eci,PointSet& affectedpoint);*/

		//new 方法
		void ComputeAllEdgeCollapseCosts();

		void ComputeEdgeCostAtVertex(osg::ref_ptr<Point> currentpoint);

		float ComputeEdgeCollapseCost(osg::ref_ptr<Point> fpoint,osg::ref_ptr<Point> tpoint);

		void BeginCollapse(osg::ref_ptr<Point> fpoint,osg::ref_ptr<Point> tpoint);

		void UpdataCollapseCost();

		void CopyBackToGeometry();

		void InitResult();

		int SetTargetGeometry(int indexnum , int numbertriangles);

		bool GetGeometry(osg::ref_ptr<osg::Geometry>& elegeo);

	public:

		typedef std::vector< osg::ref_ptr<osg::Array> > ArrayList;

		bool                            _computeErrorMetricUsingLength;
		EdgeSet                         _edgeSet;
		TriangleSet                     _triangleSet;
		PointSet                        _pointSet;
		PointList                       _originalPointList;
		PointSet						_costpointset;
		PointSet						_nocostpointset;

		struct EdgeCollapseInfo
		{
			osg::ref_ptr<Point> _frompoint;
			osg::ref_ptr<Point> _topoint;

			TriangleList _removetriangle;
			TriangleList _affectedtriangle;
		};

		typedef std::vector<EdgeCollapseInfo> EdgeCollapseInfoList;
		EdgeCollapseInfoList _edgecollapseinfolist;

		struct CollapseResult
		{
			PointList                       _originalPointList;
			PointSet						_costpointset;
			PointList						_costpointlist;
			PointSet						_nocostpointset;
			PointList						_nocostpointlist;
			TriangleSet                     _triangleSet;
			TriangleList                     _trianglelist;

			EdgeCollapseInfoList _edgecollapseinfolist;

			int _matid;
			bool _haschanged;
		};

		CollapseResult _collapseresult;

		osg::ref_ptr<osg::Vec3Array> _normal_array;

	public:
		osg::ref_ptr<osg::Geometry> element_;

		int _matid;

		unsigned int numverts_;
	};

	struct CollectTriangleOperator
	{

		CollectTriangleOperator():_ec(0) {}

		void setEdgeCollapse(EdgeCollapse* ec) { _ec = ec; }

		EdgeCollapse* _ec;

		// for use  in the triangle functor.
		inline void operator()(unsigned int p1, unsigned int p2, unsigned int p3)
		{
			_ec->addTriangle(p1,p2,p3);
		}

	};

	class CopyArrayToPointsVisitor : public osg::ArrayVisitor
	{
	public:
		CopyArrayToPointsVisitor(EdgeCollapse::PointList& pointList):
		  _pointList(pointList) {}

		  template<class T>
		  void copy(T& array)
		  {
			  if (_pointList.size()!=array.size()) return;

			  for(unsigned int i=0;i<_pointList.size();++i)
				  _pointList[i]->_attributes.push_back((float)array[i]);
		  }

		  virtual void apply(osg::Array&) {}
		  virtual void apply(osg::ByteArray& array) { copy(array); }
		  virtual void apply(osg::ShortArray& array) { copy(array); }
		  virtual void apply(osg::IntArray& array) { copy(array); }
		  virtual void apply(osg::UByteArray& array) { copy(array); }
		  virtual void apply(osg::UShortArray& array) { copy(array); }
		  virtual void apply(osg::UIntArray& array) { copy(array); }
		  virtual void apply(osg::FloatArray& array) { copy(array); }

		  virtual void apply(osg::Vec4ubArray& array)
		  {
			  if (_pointList.size()!=array.size()) return;

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  osg::Vec4ub& value = array[i];
				  EdgeCollapse::FloatList& attributes = _pointList[i]->_attributes;
				  attributes.push_back((float)value.r());
				  attributes.push_back((float)value.g());
				  attributes.push_back((float)value.b());
				  attributes.push_back((float)value.a());
			  }
		  }

		  virtual void apply(osg::Vec2Array& array)
		  {
			  if (_pointList.size()!=array.size()) return;

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  osg::Vec2& value = array[i];
				  EdgeCollapse::FloatList& attributes = _pointList[i]->_attributes;
				  attributes.push_back(value.x());
				  attributes.push_back(value.y());
			  }
		  }

		  virtual void apply(osg::Vec3Array& array)
		  {
			  if (_pointList.size()!=array.size()) return;

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  osg::Vec3& value = array[i];
				  EdgeCollapse::FloatList& attributes = _pointList[i]->_attributes;
				  attributes.push_back(value.x());
				  attributes.push_back(value.y());
				  attributes.push_back(value.z());
			  }
		  }

		  virtual void apply(osg::Vec4Array& array)
		  {
			  if (_pointList.size()!=array.size()) return;

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  osg::Vec4& value = array[i];
				  EdgeCollapse::FloatList& attributes = _pointList[i]->_attributes;
				  attributes.push_back(value.x());
				  attributes.push_back(value.y());
				  attributes.push_back(value.z());
				  attributes.push_back(value.w());
			  }
		  }

		  EdgeCollapse::PointList& _pointList;


	protected:

		CopyArrayToPointsVisitor& operator = (const CopyArrayToPointsVisitor&) { return *this; }
	};

	class CopyVertexArrayToPointsVisitor : public osg::ArrayVisitor
	{
	public:
		CopyVertexArrayToPointsVisitor(EdgeCollapse::PointList& pointList):
		  _pointList(pointList) {}

		  virtual void apply(osg::Vec2Array& array)
		  {
			  if (_pointList.size()!=array.size()) return;

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  _pointList[i] = new EdgeCollapse::Point;
				  _pointList[i]->_index = i;

				  osg::Vec2& value = array[i];
				  osg::Vec3& vertex = _pointList[i]->_vertex;
				  vertex.set(value.x(),value.y(),0.0f);
			  }
		  }

		  virtual void apply(osg::Vec3Array& array)
		  {
			  if (_pointList.size()!=array.size()) return;

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  _pointList[i] = new EdgeCollapse::Point;
				  _pointList[i]->_index = i;

				  _pointList[i]->_vertex = array[i];
			  }
		  }

		  virtual void apply(osg::Vec4Array& array)
		  {
			  if (_pointList.size()!=array.size()) return;

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  _pointList[i] = new EdgeCollapse::Point;
				  _pointList[i]->_index = i;

				  osg::Vec4& value = array[i];
				  osg::Vec3& vertex = _pointList[i]->_vertex;
				  vertex.set(value.x()/value.w(),value.y()/value.w(),value.z()/value.w());
			  }
		  }

		  EdgeCollapse::PointList& _pointList;

	protected:

		CopyVertexArrayToPointsVisitor& operator = (const CopyVertexArrayToPointsVisitor&) { return *this; }

	};

	class CopyPointsToArrayVisitor : public osg::ArrayVisitor
	{
	public:
		CopyPointsToArrayVisitor(EdgeCollapse::PointList& pointList):
		  _pointList(pointList),
			  _index(0) {}


		  template<typename T,typename R>
		  void copy(T& array, R /*dummy*/)
		  {
			  array.resize(_pointList.size());

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  if (_index<_pointList[i]->_attributes.size())
				  {
					  float val = (_pointList[i]->_attributes[_index]);
					  array[i] = R (val);
				  }
			  }

			  ++_index;
		  }

		  // use local typedefs if usinged char,short and int to get round gcc 3.3.1 problem with defining unsigned short()
		  typedef unsigned char dummy_uchar;
		  typedef unsigned short dummy_ushort;
		  typedef unsigned int dummy_uint;

		  virtual void apply(osg::Array&) {}
		  virtual void apply(osg::ByteArray& array) { copy(array, char());}
		  virtual void apply(osg::ShortArray& array) { copy(array, short()); }
		  virtual void apply(osg::IntArray& array) { copy(array, int()); }
		  virtual void apply(osg::UByteArray& array) { copy(array, dummy_uchar()); }
		  virtual void apply(osg::UShortArray& array) { copy(array,dummy_ushort()); }
		  virtual void apply(osg::UIntArray& array) { copy(array, dummy_uint()); }
		  virtual void apply(osg::FloatArray& array) { copy(array, float()); }

		  virtual void apply(osg::Vec4ubArray& array)
		  {
			  array.resize(_pointList.size());

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  EdgeCollapse::FloatList& attributes = _pointList[i]->_attributes;
				  array[i].set((unsigned char)attributes[_index],
					  (unsigned char)attributes[_index+1],
					  (unsigned char)attributes[_index+2],
					  (unsigned char)attributes[_index+3]);
			  }
			  _index += 4;
		  }

		  virtual void apply(osg::Vec2Array& array)
		  {
			  array.resize(_pointList.size());

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  EdgeCollapse::FloatList& attributes = _pointList[i]->_attributes;
				  if (_index+1<attributes.size()) array[i].set(attributes[_index],attributes[_index+1]);
			  }
			  _index += 2;
		  }

		  virtual void apply(osg::Vec3Array& array)
		  {
			  array.resize(_pointList.size());

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  EdgeCollapse::FloatList& attributes = _pointList[i]->_attributes;
				  if (_index+2<attributes.size()) array[i].set(attributes[_index],attributes[_index+1],attributes[_index+2]);
			  }
			  _index += 3;
		  }

		  virtual void apply(osg::Vec4Array& array)
		  {
			  array.resize(_pointList.size());

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  EdgeCollapse::FloatList& attributes = _pointList[i]->_attributes;
				  if (_index+3<attributes.size()) array[i].set(attributes[_index],attributes[_index+1],attributes[_index+2],attributes[_index+3]);
			  }
			  _index += 4;
		  }

		  EdgeCollapse::PointList& _pointList;
		  unsigned int _index;

	protected:

		CopyPointsToArrayVisitor& operator = (CopyPointsToArrayVisitor&) { return *this; }
	};

	class NormalizeArrayVisitor : public osg::ArrayVisitor
	{
	public:
		NormalizeArrayVisitor() {}

		template<typename Itr>
		void normalize(Itr begin, Itr end)
		{
			for(Itr itr = begin;
				itr != end;
				++itr)
			{
				itr->normalize();
			}
		}

		virtual void apply(osg::Vec2Array& array) { normalize(array.begin(),array.end()); }
		virtual void apply(osg::Vec3Array& array) { normalize(array.begin(),array.end()); }
		virtual void apply(osg::Vec4Array& array) { normalize(array.begin(),array.end()); }

	};

	class CopyPointsToVertexArrayVisitor : public osg::ArrayVisitor
	{
	public:
		CopyPointsToVertexArrayVisitor(EdgeCollapse::PointList& pointList):
		  _pointList(pointList) {}

		  virtual void apply(osg::Vec2Array& array)
		  {
			  array.resize(_pointList.size());

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  _pointList[i]->_index = i;
				  osg::Vec3& vertex = _pointList[i]->_vertex;
				  array[i].set(vertex.x(),vertex.y());
			  }
		  }

		  virtual void apply(osg::Vec3Array& array)
		  {
			  array.resize(_pointList.size());

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  _pointList[i]->_index = i;
				  array[i] = _pointList[i]->_vertex;
			  }
		  }

		  virtual void apply(osg::Vec4Array& array)
		  {
			  array.resize(_pointList.size());

			  for(unsigned int i=0;i<_pointList.size();++i)
			  {
				  _pointList[i]->_index = i;
				  osg::Vec3& vertex = _pointList[i]->_vertex;
				  array[i].set(vertex.x(),vertex.y(),vertex.z(),1.0f);
			  }
		  }

		  EdgeCollapse::PointList& _pointList;

	protected:

		CopyPointsToVertexArrayVisitor& operator = (const CopyPointsToVertexArrayVisitor&) { return *this; }
	};
}

#endif

