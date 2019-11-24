#include "OsgExTool/OSGGridTool.h"
#include "OsgExTool/MaterialManger.h"

#include <osg/DrawPixels>
#include <osg/PolygonMode>

#include <sstream>
#include "OsgExTool/OSGPointTool.h"
#include "OsgExTool/OSGLineTool.h"

#include <osg/ValueObject>
#include <osg/PolygonOffset>
#include <osg/Depth>

#include "OsgExTool/OSGUniverseTool.h"


std::vector<osg::Vec3f> createNormalBill(osg::Vec3f center, double radius, osg::Vec4f color, std::vector<osg::Vec4d>& c)
{
    std::vector<osg::Vec3d> points;

    for(size_t i = 0; i <= 30; i++)
    {
        points.push_back(osg::Vec3d(radius * 2 * cos(3.1415926 / 30 * i - 3.1415926), 0.0, radius * 1.5 * sin(3.1415926/ 30 * i - 3.1415926)));
    }

    double thick = 2;
    double cc    = 10.0;
    double a     = 0.3 ;

    double inner_thick = 1.5 + 0.15;

    std::vector<osg::Vec3f> v;

    for(size_t i = 0; i < points.size() - 1; i++)
    {

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

       v.push_back(osg::Vec3f(f2[0], f2[1], 0));
       v.push_back(osg::Vec3f(f[0], f[1], 0));
       v.push_back(osg::Vec3f(s[0],  s[1], 0));

       c.push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c.push_back(osg::Vec4(color[0], color[1], color[2], a));
       c.push_back(osg::Vec4(color[0], color[1], color[2], a));

       v.push_back(osg::Vec3f(f2[0],  f2[1], 0));
       v.push_back(osg::Vec3f(s[0],  s[1], 0));
       v.push_back(osg::Vec3f(s2[0],  s2[1], 0));

       c.push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c.push_back(osg::Vec4(color[0], color[1], color[2], a));
       c.push_back(osg::Vec4(color[0], color[1], color[2], 1.0));

       //thick
       v.push_back(osg::Vec3f(f1[0],  f1[1], 0));
       v.push_back(osg::Vec3f(f2[0],  f2[1], 0))  ;
       v.push_back(osg::Vec3f(s2[0],  s2[1], 0))  ;

       c.push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c.push_back(osg::Vec4(color[0], color[1], color[2], 1.0))  ;
       c.push_back(osg::Vec4(color[0], color[1], color[2], 1.0))  ;


       v.push_back(osg::Vec3f(f1[0],  f1[1], 0));
       v.push_back(osg::Vec3f(s2[0],  s2[1], 0))  ;
       v.push_back(osg::Vec3f(s1[0], s1[1], 0));

       c.push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       c.push_back(osg::Vec4(color[0], color[1], color[2], 1))  ;
       c.push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
       //
       osg::Vec2d s3 = s - ns * 12 ;
       osg::Vec2d f3 = f - nf * 12 ;

       v.push_back(osg::Vec3f(f3[0],  f3[1], 0))  ;
       v.push_back(osg::Vec3f(f1[0],  f1[1], 0))  ;
       v.push_back(osg::Vec3f(s1[0],  s1[1], 0))  ;

       c.push_back(osg::Vec4(color[0], color[1], color[2], a));
       c.push_back(osg::Vec4(color[0], color[1], color[2], 1.0))  ;
       c.push_back(osg::Vec4(color[0], color[1], color[2], 1.0))  ;

       v.push_back(osg::Vec3f(f3[0],  f3[1], 0));
       v.push_back(osg::Vec3f(s1[0],  s1[1], 0));
       v.push_back(osg::Vec3f(s3[0],  s3[1], 0));

       c.push_back(osg::Vec4(color[0], color[1], color[2], a));
       c.push_back(osg::Vec4(color[0], color[1], color[2], 1))  ;
       c.push_back(osg::Vec4(color[0], color[1], color[2], a));
    }

    return v;
}

osg::ref_ptr<osg::Geode> createGrid(osg::Vec3f left_bottom, osg::Vec3f right_top, int width_div, int height_div)
{
    osg::Vec3f left_top, right_bottom;

    left_top[0] = left_bottom[0]    ;
    left_top[1] = 0;
    left_top[2] = right_top[2]      ;

    //
    right_bottom[0] = right_top[0]   ;
    right_bottom[1] = 0;
    right_bottom[2] = left_bottom[2] ;

    osg::Vec3f vecx = right_bottom - left_bottom;
    vecx.normalize();

    osg::Vec3f vecz = left_top - left_bottom;
    vecz.normalize();

    float divx = (right_bottom - left_bottom).length() / width_div;
    float divz = (left_top - left_bottom).length() / height_div;

    std::vector<osg::Vec3f> grids;

    for(size_t i = 0; i < height_div ; i++)
    {
        for(size_t j = 0; j < width_div; j++)
        {
           osg::Vec3f p1 =  left_bottom + vecz * divz * i + vecx * divx * j;
           osg::Vec3f p2 =  left_bottom + vecz * divz * i  + vecx * divx * (j + 1);

           osg::Vec3f p3 =  left_bottom + vecz * divz * i + vecx * divx * (j + 1) + vecz * divz;
           osg::Vec3f p4 =  left_bottom + vecz * divz * i + vecx * divx * j + vecz * divz;

           grids.push_back(p1);
           grids.push_back(p2);

           grids.push_back(p2);
           grids.push_back(p3);

           grids.push_back(p3);
           grids.push_back(p4);

           grids.push_back(p4);
           grids.push_back(p1);
        }
    }


//function error, need modify
//  osg::ref_ptr<osg::Geode> geode = createQuads(grids, osg::Vec4(1.0, 0.0, 0.0, 1.0));

    osg::ref_ptr<osg::Geode> geode = createLines(grids, osg::Vec4(1.0, 0.0, 0.0, 1.0));

//    geode->setStateSet(MaterialManager::getInstance()->GetFrameLineStateSet());

    return geode;
}

osg::ref_ptr<osg::Geode> createLines(std::vector<osg::Vec3f>& points, osg::Vec4f color)
{
    osg::ref_ptr<osg::Geometry> geom= new osg::Geometry();

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    c->push_back(color);

    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    for(int i = 0; i < points.size() / 2 - 1; i++)
    {
         v->push_back(points[2 * i])    ;
         v->push_back(points[2 * i + 1]);
    }

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    geom->setVertexArray(v.get());

    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, v->size()));

    osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth(3);
    geom->getOrCreateStateSet()->setAttribute(lw);

//    geom->setUseVertexBufferObjects(true);
//    geom->setUseDisplayList(false)       ;
//    geom->setSupportsDisplayList(false)  ;


    geode->addDrawable(geom);

    return geode;
}

osg::ref_ptr<osg::Projection>  createNormalGrid(osg::Vec3f left_bottom, osg::Vec3f right_top, int width_div, int height_div)
{
    osg::Vec3f left_top, right_bottom;

    left_top[0] = left_bottom[0]    ;
    left_top[1] = 0;
    left_top[2] = right_top[2]      ;

    //
    right_bottom[0] = right_top[0]   ;
    right_bottom[1] = 0;
    right_bottom[2] = left_bottom[2] ;

    osg::Vec3f vecx = right_bottom - left_bottom;
    vecx.normalize();

    osg::Vec3f vecz = left_top - left_bottom;
    vecz.normalize();

    float divx = (right_bottom - left_bottom).length() / width_div;
    float divz = (left_top - left_bottom).length() / height_div;

    std::vector<osg::Vec3f> grids;

    for(size_t i = 0; i < height_div ; i++)
    {
        for(size_t j = 0; j < width_div; j++)
        {
           osg::Vec3f p1 =  left_bottom + vecz * divz * i + vecx * divx * j;
           osg::Vec3f p2 =  left_bottom + vecz * divz * i  + vecx * divx * (j + 1);

           osg::Vec3f p3 =  left_bottom + vecz * divz * i + vecx * divx * (j + 1) + vecz * divz;
           osg::Vec3f p4 =  left_bottom + vecz * divz * i + vecx * divx * j + vecz * divz;

           grids.push_back(osg::Vec3f(p1[0], p1[2], 0));
           grids.push_back(osg::Vec3f(p2[0], p2[2], 0));

           grids.push_back(osg::Vec3f(p2[0], p2[2], 0));
           grids.push_back(osg::Vec3f(p3[0], p3[2], 0));

           grids.push_back(osg::Vec3f(p3[0], p3[2], 0));
           grids.push_back(osg::Vec3f(p4[0], p4[2], 0));

           grids.push_back(osg::Vec3f(p4[0], p4[2], 0));
           grids.push_back(osg::Vec3f(p1[0], p1[2], 0));
        }
    }

    std::vector<osg::Vec4d> c;

    osg::ref_ptr<osg::Geode> geode = createLines(grids, osg::Vec4(1.0, 0.0, 0.0, 1.0));


    osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();

    modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    modeview->setMatrix(osg::Matrix::identity());

    for(int j = 0; j < grids.size() / 8 - 1; j++)
    {
        std::stringstream sstr;

        sstr<<j;
        std::string str = sstr.str();

        geode->addChild( createTextGeometry(str, (grids[8 * j] + grids[8 * j + 3]) * 0.5 ) );
    }

    modeview->addChild(geode);

    osg::ref_ptr<osg::Projection> projection = new osg::Projection;

  #ifdef AR_HUD_DEVICE
    projection->setMatrix(osg::Matrix::ortho2D(0, 800, 0, 480));
  #else
    projection->setMatrix(osg::Matrix::ortho2D(0, 1024, 0, 600));
  #endif

    projection->addChild(modeview);

    return projection;
}

osg::ref_ptr<osg::Projection> createNormalLines(std::vector<osg::Vec3f>& points, osg::Vec4f color)
{
    std::vector<osg::Vec4d> c;

    osg::ref_ptr<osg::Geode> geode = createLines(points, osg::Vec4(1.0, 0.0, 0.0, 1.0));


    osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();

    modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    modeview->setMatrix(osg::Matrix::identity());

    modeview->addChild(geode);

    osg::ref_ptr<osg::Projection> projection = new osg::Projection;

  #ifdef AR_HUD_DEVICE
    projection->setMatrix(osg::Matrix::ortho2D(0, 800, 0, 480));
  #else
    projection->setMatrix(osg::Matrix::ortho2D(0, 1024, 0, 600));
  #endif

    projection->addChild(modeview);

    return projection;
}

osg::ref_ptr<osg::Projection> createNormalCircle_(double radius, osg::Vec3f center, osg::Vec4f color)
{
    std::vector<osg::Vec4d> c;

    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    for(size_t i = 0; i <= 30; i++)
    {
        v->push_back(osg::Vec3d(radius * cos(2*3.1415926 / 30 * i) , radius* sin(2*3.1415926/ 30 * i), 0) + center);
    }

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Geode>   geode = new osg::Geode;

    geom->setVertexArray(v.get());

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, v->size()));
    geode->addDrawable(geom.get());

    osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();

    modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    modeview->setMatrix(osg::Matrix::identity());

    modeview->addChild(geode);

    osg::ref_ptr<osg::Projection> projection = new osg::Projection;

  #ifdef AR_HUD_DEVICE
    projection->setMatrix(osg::Matrix::ortho2D(0, 800, 0, 480));
  #else
    projection->setMatrix(osg::Matrix::ortho2D(0, 1024, 0, 600));
  #endif

    projection->addChild(modeview);

    return projection;
}

osg::ref_ptr<osg::Projection> createNormalText(std::vector<std::string>& str, std::vector<osg::Vec3f>& center)
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();

    for(size_t i = 0; i < str.size(); i++)
    {
       modeview->addChild( createTextGeometry(str[i], osg::Vec3f(center[i][0], center[i][2], 0) ));
    }

    modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    modeview->setMatrix(osg::Matrix::identity());

    modeview->addChild(geode);

    osg::ref_ptr<osg::Projection> projection = new osg::Projection;

  #ifdef AR_HUD_DEVICE
    projection->setMatrix(osg::Matrix::ortho2D(0, 800, 0, 480));
  #else
    projection->setMatrix(osg::Matrix::ortho2D(0, 1024, 0, 600));
  #endif

    projection->addChild(modeview);

    return projection;
}


osg::ref_ptr<osg::Geode> createText(std::string str, osg::Vec3f pos, float size, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    osg::ref_ptr<osgText::Text> text = new osgText::Text;

    text->setCharacterSize( size ,5.0);
    text->setAxisAlignment( osgText::TextBase::XZ_PLANE );

   //text->setAutoRotateToScreen(true);

    text->setAlignment(osgText::TextBase::CENTER_BASE_LINE);
    text->setDataVariance(osg::Object::DYNAMIC);
    text->setColor(color);

    //QFont fontt;
    //fontt.setPixelSize(50);
    //fontt.setBold(true);


   // osgText::Font* font = new osgText::Font(new QFontImplementation(fontt));
   // text->setFont(font);
    text->setPosition( pos );
    text->setText(str, osgText::String::ENCODING_UTF8);

    text->getOrCreateStateSet()->setRenderBinDetails(200, "RenderBin");
    text->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    geode->addChild(text);

    return geode;
}

osg::ref_ptr<osg::Projection> createNormalText(std::string str, osg::Vec3f center, float size, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();


    modeview->addChild( createTextGeometry(str, osg::Vec3f(center[0], center[2], 0), size, color ));

    modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    modeview->setMatrix(osg::Matrix::identity());

    modeview->addChild(geode);

    osg::ref_ptr<osg::Projection> projection = new osg::Projection;

  #ifdef AR_HUD_DEVICE
    projection->setMatrix(osg::Matrix::ortho2D(0, 800, 0, 480));
  #else
    projection->setMatrix(osg::Matrix::ortho2D(0, 1024, 0, 600));
  #endif

    projection->addChild(modeview);

    return projection;
}

osg::ref_ptr<osg::Projection> createNormalText(double num, osg::Vec3f center,  float size, osg::Vec4 color)
{
    int dis = num * 10;

    std::string text    ;
    std::stringstream ss;

    ss << dis * 0.1 ;
    text = ss.str();

    size = size * ( 1 - center[2] / 480.0);

    return createNormalText(text, center, size, color);
}


