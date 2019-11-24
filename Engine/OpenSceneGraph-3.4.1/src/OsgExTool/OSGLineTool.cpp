#include "OsgExTool/OSGLineTool.h"
#include "OsgExTool/MaterialManger.h"
#include "OsgExTool/OSGPointTool.h"
#include "OsgExTool/OSGGeodeTool.h"

#include <osg/DrawPixels>
#include <osg/PolygonMode>

#include "OsgExTool/MaterialManger.h"


#include <osg/ValueObject>
#include <osg/PolygonOffset>
#include <osg/Depth>
#include <math.h>
osg::ref_ptr<osg::Geode> createSingleLine(std::vector<osg::Vec3f>& points, double width, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geode>   geode = new osg::Geode;
    std::vector<osg::Vec3f> singleLine;

    for(size_t i = 0; i < points.size(); i++)
    {
         singleLine.push_back(points[i]);
    }

    if(!singleLine.empty()){
        geode->addDrawable(createLine(singleLine, color));
    }


    return geode;
}

osg::ref_ptr<osg::Geode> createLaneLine(std::vector<osg::Vec3f>& points, double width, osg::Vec4 color)
{
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;


    for(size_t j = 0; j < points.size() - 1; j++)
    {
          osg::Vec3 f = points[j];
          osg::Vec3 s = points[j + 1];
          osg::Vec3 n(s - f);

          n.normalize();
          osg::Vec3 T(-n.y(), 0, n.x());

          osg::Vec3 f2 = f + T * width * 0.5;
          f = f - T * width * 0.5;

          osg::Vec3 s2 = s + T * width * 0.5;
          s = s - T * width * 0.5;

          v->push_back(f) ;
          v->push_back(f2)    ;

          v->push_back(s2)    ;
          v->push_back(s)     ;

          c->push_back(osg::Vec4(1.0, 0.0, 0.0, 0.8));
          c->push_back(osg::Vec4(1.0, 0.0, 0.0, 0.8));
          c->push_back(osg::Vec4(1.0, 0.0, 0.0, 0.8));
          c->push_back(osg::Vec4(1.0, 0.0, 0.0, 0.8));
    }

   osg::ref_ptr<osg::Geode> geode =  createGeode(v, osg::PrimitiveSet::QUADS, color);
   return geode;
}

osg::ref_ptr<osg::Geode> createSmoothLaneLine(std::vector<osg::Vec3f>& points, double width, osg::Vec4 color, osg::Matrix mat, int div)
{
	if(points.size() == 0)
	{
		return NULL;
	}

	int n = 1;

	for(size_t i = 0; i < n; i++)
	{
		points = justSmoothPoints(points);
	}

	if(div == 1)
	{
		return createGeode(points, color, width, mat);
	}
	else
	{
		return createDotGeode(points, color, width, mat, div);
	}

}


osg::ref_ptr<osg::Geode> createSmoothLaneLine(std::vector<osg::Vec3f>& points, double width, osg::Vec4 color, osg::Vec4 color2, osg::Matrix mat)
{
      int n = 3;

      for(size_t i = 0; i < n; i++)
      {
         points = justSmoothPoints(points);
      }

   return createGeode(points, color, color2, width, mat);
}

osg::ref_ptr<osg::Geode> createSmoothLaneLine(std::vector<osg::Vec2d>& points, double width, osg::Vec4 color, osg::Vec4 color2)
{
    std::vector<osg::Vec3f> points_;

    for(size_t i = 0; i < points.size(); i++)
    {
        points_.push_back( osg::Vec3f(points[i][0], 0, points[i][1]) );
    }

  return  createSmoothLaneLine(points_, width, color, color2);
}


osg::ref_ptr<osg::Node> createLaneX(double A, double B, double C, double D, double start, double end, double width, bool isTest)
{
    if(!isTest)
    {
      if(end -  start <0.0001)
      {
        return 0;
      }
    }

  std::vector<osg::Vec3f> points;

if(!isTest)
{
    double x = 0.0;
    double y = 0.0;

    int num = (50 - start) / 0.3;

    for(size_t i = 0; i < num; i++)
    {
       x = start + 0.3 * i;
       y = A*x*x*x+B*x*x+C*x+D;
       points.push_back(osg::Vec3f(y,0, x));
    }
}
else
{
    for(size_t i = 0; i < 720 * 5; i++)
    {
        points.push_back(osg::Vec3f(1.5 * sin(3.1415926 / 360 * i), 0, 3.1415926 / 360 * i));
    }

    for(size_t i = 0; i < 720 * 5; i++)
    {
        points.push_back(osg::Vec3f(1.5 * sin(3.1415926 / 360 * i) - 2, 0, 3.1415926 / 360 * i));
    }
}
    return createLaneLine(points, width, osg::Vec4(0,1.f,0,1.f));

}


/***********************************
 *  create single line between two point
 * **********************************/
osg::ref_ptr<osg::Geometry> createLine(osg::Vec3f f, osg::Vec3f s, osg::Vec4f color)
{
    osg::ref_ptr<osg::Geometry> geom= new osg::Geometry();

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    c->push_back(color);

    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    v->push_back(f);
    v->push_back(s);

    geom->setVertexArray(v.get());

    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, v->size()));

    osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth(3);
    geom->getOrCreateStateSet()->setAttribute(lw);

    geom->setUseVertexBufferObjects(true);
    geom->setUseDisplayList(false)       ;
    geom->setSupportsDisplayList(false)  ;

    return geom;
}

/***********************************
 *  create single line continous point
 * **********************************/
osg::ref_ptr<osg::Geometry> createLine(std::vector<osg::Vec3f> points, osg::Vec4f color)
{
    osg::ref_ptr<osg::Geometry> geom= new osg::Geometry();

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    c->push_back(color);

    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    for(int i = 0; i < points.size(); i++){
         v->push_back(points[i]);
    }

    geom->setVertexArray(v.get());

    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, v->size()));

    osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth(3);
    geom->getOrCreateStateSet()->setAttribute(lw);

    geom->setUseVertexBufferObjects(true);
    geom->setUseDisplayList(false)       ;
    geom->setSupportsDisplayList(false)  ;

    return geom;
}



