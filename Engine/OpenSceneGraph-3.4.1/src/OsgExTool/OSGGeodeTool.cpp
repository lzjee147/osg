#include "OsgExTool/OSGGeodeTool.h"
#include "OsgExTool/OSGUniverseTool.h"
#include "OsgExTool/MaterialManger.h"

#include <osg/DrawPixels>
#include <osg/PolygonMode>

#include <osg/ValueObject>
#include <osg/PolygonOffset>
#include <osg/Depth>
#include <math.h>

osg::ref_ptr<osg::Node> createAX_(osg::Vec3 origin, osg::Vec3 x, osg::Vec3 y, osg::Vec3 z)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
    geom->setVertexArray(v.get());

    v->push_back(origin);
    v->push_back(x);

    v->push_back(origin);
    v->push_back(y);

    v->push_back(origin);
    v->push_back(z);

    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
    geom->setColorArray(c.get());

    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    c->push_back(osg::Vec4(1,0.f,0.f,1.f));
    c->push_back(osg::Vec4(1.f,0.f,0.f,1.f));

    c->push_back(osg::Vec4(0,1.f,0.f,1.f));
    c->push_back(osg::Vec4(0.f,1.f,0.f,1.f));

    c->push_back(osg::Vec4(0,0.f,1.f,1.f));
    c->push_back(osg::Vec4(0.f,0.f,1.f,1.f));

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6));
    geode->addDrawable(geom.get());

    return geode;
}

osg::ref_ptr<osg::Geode> createCircle(double radius, osg::Vec3 center, osg::Vec4 color)
{
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;

    for(size_t i = 0; i <= 50; i++)
    {
        v->push_back(center + osg::Vec3( radius * cos(3.1415926 / 50 * i), 0.0, radius * sin(3.1415926 / 50 * i)));
    }

    return createGeode(v, osg::PrimitiveSet::LINE_STRIP, color);
}

osg::ref_ptr<osg::Geode> createCircleFace(osg::Vec3f center, double radius, osg::Vec4f colors)
{
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;

    for(size_t i = 0; i < 50; i++)
	{
		v->push_back(center + osg::Vec3( radius * 0.3 * cos(2 * osg::PI / 50 * i), 0.0, 2 *radius * sin(2 * osg::PI / 50 * i)));
		v->push_back(center + osg::Vec3( radius * 0.3 * cos(2 * osg::PI / 50 * (i + 1 )), 0.0, 2 * radius * sin(2 * osg::PI / 50 * (i + 1 ))));

		v->push_back(center);
    }

    return createGeode(v, osg::PrimitiveSet::TRIANGLES, colors);
}

osg::ref_ptr<osg::Geode> createGeode_color(osg::ref_ptr<osg::Vec3Array> v, GLenum  type, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geometry> geom= new osg::Geometry();

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;
    geom->setColorArray(c.get());

    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    c->push_back(osg::Vec4(color));
    geom->setVertexArray(v.get());

    geom->addPrimitiveSet(new osg::DrawArrays(type, 0, v->size()));
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    geode->addDrawable(geom.get());
    return geode;
}

osg::ref_ptr<osg::Geode> createGeode_color(osg::ref_ptr<osg::Vec3Array> v, osg::ref_ptr<osg::Vec4Array> c, GLenum  type)
{
    osg::ref_ptr<osg::Geometry> geom= new osg::Geometry();

    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    geom->setVertexArray(v.get());
    geom->addPrimitiveSet(new osg::DrawArrays(type, 0, v->size()));

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable(geom.get());

    return geode;
}

osg::ref_ptr<osg::Geode> createGeode(osg::ref_ptr<osg::Vec3Array> v, GLenum type, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geometry> geom= new osg::Geometry();
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();

    c->push_back(color);

    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);
    geom->setVertexArray(v.get());
    geom->addPrimitiveSet(new osg::DrawArrays(type, 0, v->size()));

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable(geom.get());

    return geode;
}

osg::ref_ptr<osg::Geode> createQuadAnimation(double w, double h, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geometry> geom= new osg::Geometry();

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();

    v->push_back(osg::Vec3f(-0.5*w, 0, -0.5*h));
    v->push_back(osg::Vec3f(0.5*w, 0, -0.5*h)) ;
    v->push_back(osg::Vec3f(0.5*w, 0, 0.5*h))  ;
    v->push_back(osg::Vec3f(-0.5*w, 0, 0.5*h)) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(color);
    c->push_back(color);


    double thcik_ans = 0.01;
    double a = 0.3;

    v->push_back(osg::Vec3f(-0.5*w - thcik_ans, 0, 0.5*h));
    v->push_back(osg::Vec3f(-0.5*w - thcik_ans, 0, -0.5*h));
    v->push_back(osg::Vec3f(-0.5*w, 0, -0.5*h));
    v->push_back(osg::Vec3f(-0.5*w, 0, 0.5*h));

    c->push_back(osg::Vec4f(color[0], color[1],color[2], a));
    c->push_back(osg::Vec4f(color[0], color[1],color[2], a));
    c->push_back(color);
    c->push_back(color);


    v->push_back(osg::Vec3f(0.5*w, 0, 0.5*h));
    v->push_back(osg::Vec3f(0.5*w, 0, -0.5*h));
    v->push_back(osg::Vec3f(0.5*w + thcik_ans, 0, -0.5*h));
    v->push_back(osg::Vec3f(0.5*w + thcik_ans, 0, 0.5*h));

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4f(color[0], color[1],color[2], a));
    c->push_back(osg::Vec4f(color[0], color[1],color[2], a));

    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    geom->setVertexArray(v.get());
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, v->size()));

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    geode->addDrawable(geom.get());
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    return geode;
}

osg::ref_ptr<osg::Geode> createGeode(std::vector<osg::Vec3f> points, osg::Vec4f colors, double width, osg::Matrix mat, int div)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Geode>   geode = new osg::Geode;
    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    geom->setVertexArray(v.get());

    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
    geom->setColorArray(c.get());

    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    for(size_t i = 0; i < points.size() - 1; i++)
    {
       createGemetry_(v, c, colors, osg::Vec2f(points[i][0], points[i][2]), osg::Vec2f(points[i + 1][0], points[i + 1][2]), width);
    }

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, v->size()));

    geom->setUseVertexBufferObjects(true);
    geom->setUseDisplayList(false)       ;
    geom->setSupportsDisplayList(false)  ;
    geode->addDrawable(geom.get());


    //geode->getOrCreateStateSet()->setRenderBinDetails(200, "RenderBin");
    geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);


    return geode;
}

osg::ref_ptr<osg::Geode> createDotGeode(std::vector<osg::Vec3f> points, osg::Vec4f colors, double width, osg::Matrix mat, int div)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Geode>   geode = new osg::Geode;
    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    geom->setVertexArray(v.get());

    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
    geom->setColorArray(c.get());

    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    int num = points.size() / div;

    for(size_t i = 0; i <  div; i ++)
    {
       if( (i + 1) % 2 != 0)
       {
          for(size_t j = num * i; j < num * (i + 1) - 1; j++)
          {
            createGemetry_(v, c, colors, osg::Vec2f(points[j][0], points[j][2]), osg::Vec2f(points[j + 1][0], points[j + 1][2]), width);
          }
       }
    }

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, v->size()));

    geom->setUseVertexBufferObjects(true);
    geom->setUseDisplayList(false)       ;
    geom->setSupportsDisplayList(false)  ;
    geode->addDrawable(geom.get());


    //geode->getOrCreateStateSet()->setRenderBinDetails(200, "RenderBin");
    geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);


    return geode;
}

osg::ref_ptr<osg::Geode> createGeode(std::vector<osg::Vec3f> points, osg::Vec4f colors, osg::Vec4f color2, double width, osg::Matrix mat)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Geode>   geode = new osg::Geode;
    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    geom->setVertexArray(v.get());

    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    osg::Vec4f div = (color2 - colors) * (1.0 / (points.size() - 1));

    for(size_t i = 0; i < points.size() - 1; i++)
    {
        osg::Vec4f color = div * i + colors;
       // createGemetry_(v, c, color, points[i], points[i + 1], width);
    }

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, v->size()));
    geode->addDrawable(geom.get());

    //geode->getOrCreateStateSet()->setRenderBinDetails(200, "RenderBin");
    geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

    return geode;
}

osg::ref_ptr<osg::Geode> createGeode(std::vector<osg::Vec3f> points, std::vector<osg::Vec4d> colors, double width)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Geode>   geode = new osg::Geode;
    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    geom->setVertexArray(v.get());

    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
    geom->setColorArray(c.get());

    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    for(size_t i = 0; i < points.size() - 1; i++)
    {
        //createGemetry_(v, c, colors[i], points[i], points[i + 1], width);
    }

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, v->size()));
    geode->addDrawable(geom.get());
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    return geode;
}


osg::ref_ptr<osg::Geode> createCircle(osg::Vec3f center, double radius, osg::Vec4f colorss)
{
    //create point
    std::vector<osg::Vec3d> points;
    std::vector<osg::Vec4d> colors;

    for(size_t i = 0; i <= 30; i++)
    {
        points.push_back(osg::Vec3d(radius * cos(3.1415926 / 30 * i - 3.1415926) , 0.0, radius * 2 * sin(3.1415926/ 30 * i - 3.1415926)) + center);
        colors.push_back( osg::Vec4d( colorss[0], colorss[1], colorss[2], 1.0 ) );
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Geode>   geode = new osg::Geode;

    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;


    geom->setVertexArray(v.get());
    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
    geom->setColorArray(c.get());

    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    double thick = 0.15;
    double cc    = 1.0;
    double a     = 0.3 ;

    double inner_thick = 1.5 + 0.15;

    for(size_t i = 0; i < points.size() - 1; i++)
    {
       osg::Vec4d color = colors[i];
       osg::Vec2d f(points[i][0], points[i][2]);
       osg::Vec2d s(points[i + 1][0], points[i + 1][2]);

       osg::Vec2d nf(f);
       nf.normalize();

       osg::Vec2d ns(s);
       ns.normalize();

       osg::Vec2d f2 = f - nf * thick;
       osg::Vec2d f1 = f - nf * cc ;

       osg::Vec2d s2 = s - ns * thick;
       osg::Vec2d s1 = s - ns * cc ;

       v->push_back(osg::Vec3d(f2[0], 0, f2[1]));
       v->push_back(osg::Vec3d(f[0], 0, f[1]));
       v->push_back(osg::Vec3d(s[0], 0, s[1]));

       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c->push_back(osg::Vec4(color[0], color[1], color[2], a));
       c->push_back(osg::Vec4(color[0], color[1], color[2], a));

       v->push_back(osg::Vec3d(f2[0], 0, f2[1]));
       v->push_back(osg::Vec3d(s[0], 0, s[1]));
       v->push_back(osg::Vec3d(s2[0], 0, s2[1]));

       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c->push_back(osg::Vec4(color[0], color[1], color[2], a));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));

       //thick
       v->push_back(osg::Vec3d(f1[0], 0, f1[1]));
       v->push_back(osg::Vec3d(f2[0], 0, f2[1]));
       v->push_back(osg::Vec3d(s2[0], 0, s2[1]));

       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));

       v->push_back(osg::Vec3d(f1[0], 0, f1[1]));
       v->push_back(osg::Vec3d(s2[0], 0, s2[1]));
       v->push_back(osg::Vec3d(s1[0], 0, s1[1]));

       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1))  ;
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));

       //
       osg::Vec2d s3 = s - ns * 1.15 ;
       osg::Vec2d f3 = f - nf * 1.15 ;

       v->push_back(osg::Vec3d(f3[0], 0, f3[1]))  ;
       v->push_back(osg::Vec3d(f1[0], 0, f1[1]))  ;
       v->push_back(osg::Vec3d(s1[0], 0, s1[1]))  ;

       c->push_back(osg::Vec4(color[0], color[1], color[2], a));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0))  ;
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0))  ;

       v->push_back(osg::Vec3d(f3[0], 0, f3[1]));
       v->push_back(osg::Vec3d(s1[0], 0, s1[1]))  ;
       v->push_back(osg::Vec3d(s3[0], 0, s3[1]));

       c->push_back(osg::Vec4(color[0], color[1], color[2], a));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1))  ;
       c->push_back(osg::Vec4(color[0], color[1], color[2], a));
    }

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, v->size()));
    geode->addDrawable(geom.get());

    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    geode->getOrCreateStateSet()->setRenderBinDetails(200, "RenderBin");
    geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    geode->setUserValue("cicle", center);

    return geode;
}

osg::ref_ptr<osg::Geode> createCircle(std::vector<osg::Vec3d> points, std::vector<osg::Vec4d> colors)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Geode>   geode = new osg::Geode;
    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    geom->setVertexArray(v.get());

    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    double thick = 0.05;
    double cc    = 0.2 ;
    double a     = 0.3 ;

    for(size_t i = 0; i < points.size() - 1; i++)
    {
       osg::Vec4d color = colors[i];
       osg::Vec2d f(points[i][0], points[i][2]);
       osg::Vec2d s(points[i + 1][0], points[i + 1][2]);

       osg::Vec2d nf(f);
       nf.normalize();

       osg::Vec2d ns(s);
       ns.normalize();

       osg::Vec2d f2 = f - nf * thick;
       osg::Vec2d f1 = f - nf * cc ;

       osg::Vec2d s2 = s - ns * thick;
       osg::Vec2d s1 = s - ns * cc ;

       v->push_back(osg::Vec3d(f2[0], 0, f2[1]));
       v->push_back(osg::Vec3d(f[0], 0, f[1]));
       v->push_back(osg::Vec3d(s[0], 0, s[1]));

       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c->push_back(osg::Vec4(color[0], color[1], color[2], a));
       c->push_back(osg::Vec4(color[0], color[1], color[2], a));

       v->push_back(osg::Vec3d(f2[0], 0, f2[1]));
       v->push_back(osg::Vec3d(s[0], 0, s[1]));
       v->push_back(osg::Vec3d(s2[0], 0, s2[1]));

       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c->push_back(osg::Vec4(color[0], color[1], color[2], a));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));

       //thick
       v->push_back(osg::Vec3d(f1[0], 0, f1[1]));
       v->push_back(osg::Vec3d(f2[0], 0, f2[1]))  ;
       v->push_back(osg::Vec3d(s2[0], 0, s2[1]))  ;

       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0))  ;
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0))  ;

       v->push_back(osg::Vec3d(f1[0], 0, f1[1]));
       v->push_back(osg::Vec3d(s2[0], 0, s2[1]))  ;
       v->push_back(osg::Vec3d(s1[0], 0, s1[1]));

       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1))  ;
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));

       osg::Vec2d s3 = s - ns * 0.3 ;
       osg::Vec2d f3 = f - nf * 0.3 ;

       v->push_back(osg::Vec3d(f3[0], 0, f3[1]))  ;
       v->push_back(osg::Vec3d(f1[0], 0, f1[1]))  ;
       v->push_back(osg::Vec3d(s1[0], 0, s1[1]))  ;

       c->push_back(osg::Vec4(color[0], color[1], color[2], a));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0))  ;
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0))  ;

       v->push_back(osg::Vec3d(f3[0], 0, f3[1]));
       v->push_back(osg::Vec3d(s1[0], 0, s1[1]))  ;
       v->push_back(osg::Vec3d(s3[0], 0, s3[1]));

       c->push_back(osg::Vec4(color[0], color[1], color[2], a));
       c->push_back(osg::Vec4(color[0], color[1], color[2], 1))  ;
       c->push_back(osg::Vec4(color[0], color[1], color[2], a));
    }

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, v->size()));
    geode->addDrawable(geom.get());
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    return geode;

}

osg::ref_ptr<osg::Geode> createQuadColor(double x,  double y,  double width , double height,  osg::Vec4 color)
{
    osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array();

    points->push_back(osg::Vec3d(x - width * 0.5, 0, y));
    points->push_back(osg::Vec3d(x + width * 0.5, 0, y));
    points->push_back(osg::Vec3d(x + width * 0.5, 0, y + height));
    points->push_back(osg::Vec3d(x - width * 0.5, 0, y + height));

    return createGeode(points, osg::PrimitiveSet::LINE_LOOP, color);
}


osg::ref_ptr<osg::Geode> createQuadTexture(double x, double y, double width ,double height, int textureID)
{
     osg::ref_ptr<osg::Vec2Array> tex = new osg::Vec2Array();

     tex->push_back(osg::Vec2(0, 0));
     tex->push_back(osg::Vec2(1, 0));
     tex->push_back(osg::Vec2(1, 1));
     tex->push_back(osg::Vec2(0, 1));

     osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array();

     points->push_back(osg::Vec3d(x - width * 0.5, 0, y));
     points->push_back(osg::Vec3d(x + width * 0.5, 0, y));
     points->push_back(osg::Vec3d(x + width * 0.5, 0, y + height));
     points->push_back(osg::Vec3d(x - width * 0.5, 0, y + height));

     osg::ref_ptr<osg::Geode> geode = createGeode(points, osg::PrimitiveSet::QUADS, osg::Vec4(1.0, 0.0, 0.0, 1.0));

    ( (osg::Geometry*)geode->getDrawable(0) )->setTexCoordArray(0, tex, osg::Array::BIND_PER_VERTEX);
    geode->setStateSet(MaterialManager::getInstance()->getMaterialForId(textureID, true));

     return geode;
}


osg::ref_ptr<osg::Geode> createQuadTexture(double width ,double height, int textureID)
{
     osg::ref_ptr<osg::Vec2Array> tex = new osg::Vec2Array();

     tex->push_back(osg::Vec2(0, 0));
     tex->push_back(osg::Vec2(1, 0));
     tex->push_back(osg::Vec2(1, 1));
     tex->push_back(osg::Vec2(0, 1));

     osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array();

     points->push_back(osg::Vec3d(- width * 0.5, 0, 0));
     points->push_back(osg::Vec3d(width * 0.5, 0, 0));
     points->push_back(osg::Vec3d(width * 0.5, 0, height));
     points->push_back(osg::Vec3d(- width * 0.5, 0,  height));

     osg::ref_ptr<osg::Geode> geode = createGeode(points, osg::PrimitiveSet::QUADS, osg::Vec4(1.0, 0.0, 0.0, 1.0));

    ( (osg::Geometry*)geode->getDrawable(0) )->setTexCoordArray(0, tex, osg::Array::BIND_PER_VERTEX);
    geode->setStateSet(MaterialManager::getInstance()->getMaterialForId(textureID, true));

     return geode;
}




