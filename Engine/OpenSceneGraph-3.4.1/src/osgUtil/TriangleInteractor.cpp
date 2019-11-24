#include <osgUtil/LineSegmentIntersector>

#include <osg/Geometry>
#include <osg/Notify>
#include <osg/io_utils>
#include <osg/TriangleFunctor>

#include <osgUtil/TriangleInteractor>

using namespace osgUtil; 


namespace LineSegmentIntersectorUtils
{
	struct TriangleIntersection
	{
		TriangleIntersection(unsigned int index, const osg::Vec3& normal, float r1, const osg::Vec3* v1, float r2, const osg::Vec3* v2, float r3, const osg::Vec3* v3):
	_index(index),
		_normal(normal),
		_r1(r1),
		_v1(v1),
		_r2(r2),
		_v2(v2),
		_r3(r3),
		_v3(v3) {}

	unsigned int        _index;
	const osg::Vec3     _normal;
	float               _r1;
	const osg::Vec3*    _v1;
	float               _r2;
	const osg::Vec3*    _v2;
	float               _r3;
	const osg::Vec3*    _v3;

	protected:

		TriangleIntersection& operator = (const TriangleIntersection&) { return *this; }
	};

	typedef std::multimap<float,TriangleIntersection> TriangleIntersections;


	template<typename Vec3, typename value_type>
	struct TriangleIntersector
	{
		Vec3   _s;
		Vec3   _d;
		value_type       _length;

		int         _index;
		value_type  _ratio;
		bool        _hit;
		bool        _limitOneIntersection;
		TriangleIntersections* _intersections;

		TriangleIntersector()
		{
			_intersections = 0;
			_length = 0.0f;
			_index = 0;
			_ratio = 0.0f;
			_hit = false;
			_limitOneIntersection = false;
		}

		void set(TriangleIntersections* intersections)
		{
			_intersections = intersections;
		}

		void set(const osg::Vec3d& start, const osg::Vec3d& end, value_type ratio=FLT_MAX)
		{
			_hit=false;
			_index = 0;
			_ratio = ratio;

			_s = start;
			_d = end - start;
			_length = _d.length();
			_d /= _length;
		}

		inline void operator () (const osg::Vec3& v1,const osg::Vec3& v2,const osg::Vec3& v3, bool treatVertexDataAsTemporary)
		{
			++_index;

			if (_limitOneIntersection && _hit) return;

			if (v1==v2 || v2==v3 || v1==v3) return;

			Vec3 v12 = v2-v1;
			Vec3 n12 = v12^_d;
			value_type ds12 = (_s-v1)*n12;
			value_type d312 = (v3-v1)*n12;
			if (d312>=0.0f)
			{
				if (ds12<0.0f) return;
				if (ds12>d312) return;
			}
			else                    
			{
				if (ds12>0.0f) return;
				if (ds12<d312) return;
			}

			Vec3 v23 = v3-v2;
			Vec3 n23 = v23^_d;
			value_type ds23 = (_s-v2)*n23;
			value_type d123 = (v1-v2)*n23;
			if (d123>=0.0f)
			{
				if (ds23<0.0f) return;
				if (ds23>d123) return;
			}
			else                    
			{
				if (ds23>0.0f) return;
				if (ds23<d123) return;
			}

			Vec3 v31 = v1-v3;
			Vec3 n31 = v31^_d;
			value_type ds31 = (_s-v3)*n31;
			value_type d231 = (v2-v3)*n31;
			if (d231>=0.0f)
			{
				if (ds31<0.0f) return;
				if (ds31>d231) return;
			}
			else                     
			{
				if (ds31>0.0f) return;
				if (ds31<d231) return;
			}


			value_type r3;
			if (ds12==0.0f) r3=0.0f;
			else if (d312!=0.0f) r3 = ds12/d312;
			else return; 

			value_type r1;
			if (ds23==0.0f) r1=0.0f;
			else if (d123!=0.0f) r1 = ds23/d123;
			else return; 

			value_type r2;
			if (ds31==0.0f) r2=0.0f;
			else if (d231!=0.0f) r2 = ds31/d231;
			else return; 

			value_type total_r = (r1+r2+r3);
			if (total_r!=1.0f)
			{
				if (total_r==0.0f) return; 
				value_type inv_total_r = 1.0f/total_r;
				r1 *= inv_total_r;
				r2 *= inv_total_r;
				r3 *= inv_total_r;
			}

			Vec3 in = v1*r1+v2*r2+v3*r3;
			if (!in.valid())
			{
				return;
			}

			value_type d = (in-_s)*_d;

			if (d<0.0f) return;
			if (d>_length) return;

			Vec3 normal = v12^v23;
			normal.normalize();

			value_type r = d/_length;


			if (treatVertexDataAsTemporary)
			{
				_intersections->insert(std::pair<const float,TriangleIntersection>(r,TriangleIntersection(_index-1,normal,r1,0,r2,0,r3,0)));
			}
			else
			{
				_intersections->insert(std::pair<const float,TriangleIntersection>(r,TriangleIntersection(_index-1,normal,r1,&v1,r2,&v2,r3,&v3)));
			}
			_hit = true;

		}

	};

}

void TriangleInteractor::intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable, const osg::Vec3d& s, const osg::Vec3d& e)
{
	LineSegmentIntersectorUtils::TriangleIntersections intersections;

	typedef LineSegmentIntersectorUtils::TriangleIntersector<osg::Vec3d, osg::Vec3d::value_type> TriangleIntersector;
	osg::TriangleFunctor< TriangleIntersector > ti;

	ti.set(&intersections);
	ti.set(s,e);

	drawable->accept(ti);

	if(intersections.size())
	{
		LineSegmentIntersectorUtils::TriangleIntersections::iterator it = intersections.begin();

		osg::Vec3d v1 = *(it->second._v1);

		osg::Vec3d v2 = *(it->second._v2);

		osg::Vec3d v3 = *(it->second._v3);

		osg::Vec3d v1v2 = (v2 + v1) * 0.5;

		osg::Vec3d v1v3 = (v3 + v1) * 0.5;

		osg::Vec3d v3v2 = (v2 + v3) * 0.5;

		//前三个是顶点
		triangle_.push_back(v1);
		triangle_.push_back(v2);
		triangle_.push_back(v3);

		//后三个是中点
		triangle_.push_back(v1v2);
		triangle_.push_back(v1v3);
		triangle_.push_back(v3v2);
	}
}

std::vector<osg::Vec3d> TriangleInteractor::GetPickPoint()
{
	return triangle_;
}


