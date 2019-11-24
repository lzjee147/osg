#include "OsgExTool/OSGGeodeTool.h"
#include "OsgExTool/OSGArrowTool.h"
#include "OsgExTool/MaterialManger.h"

#include <osg/DrawPixels>
#include <osg/PolygonMode>




#include <osg/ValueObject>
#include <osg/PolygonOffset>
#include <osg/Depth>



osg::ref_ptr<osg::Node> createArrow(osg::Vec3d s, osg::Vec3d f, double width, double height, osg::Vec4 color)
{
    osg::Vec3d vec = s - f;

    vec.normalize();

    osg::Vec3d lv(-vec[1],0,  vec[0]);
    osg::Vec3d rv(vec[1], 0, -vec[0]);

    osg::Vec3d rvp = s + rv * width * 0.5;
    osg::Vec3d lvp = s + lv * width * 0.5;

    osg::Vec3d vecp = s + vec * height;
    osg::Vec3d vecp2 = s + vec * (height + 0.1);

    double a = 0.6;
    osg::Vec4 alpha_color(color[0], color[1], color[2], a);

    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();

     v->push_back(rvp)   ;
     v->push_back(lvp)   ;
     v->push_back(vecp)  ;

     c->push_back(color);
     c->push_back(color);
     c->push_back(color);

     //smooth
     osg::Vec3d rvp2 = s + rv * (width * 0.5 + 0.1);
     osg::Vec3d lvp2 = s + lv * (width * 0.5 + 0.1);

     v->push_back(lvp)  ;
     v->push_back(lvp2) ;
     v->push_back(vecp) ;

     c->push_back(color);
     c->push_back(alpha_color);
     c->push_back(color);

     v->push_back(lvp2)   ;
     v->push_back(vecp)   ;
     v->push_back(vecp2)  ;

     c->push_back(alpha_color);
     c->push_back(color);
     c->push_back(alpha_color);


    return createGeode_color(v, c, osg::PrimitiveSet::TRIANGLES);
}


osg::ref_ptr<osg::Geode> createArrowAnimation(osg::Vec2d point, double angle, double h, double l, osg::Vec4 color, osg::Vec2d trans)
{
    osg::ref_ptr<osg::Geometry> geom= new osg::Geometry();

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();

    osg::Vec2d vv_normal(sin(angle * osg::PI / 180.0), -cos(angle * osg::PI / 180.0) );
    osg::Vec2d vv( l * sin(angle * osg::PI / 180.0), -l * cos(angle * osg::PI / 180.0) );

    osg::Vec2d p2 = point + vv;
    osg::Vec2d p4 = point + osg::Vec2d(0, -1) * h;
    osg::Vec2d p3 = p4 + vv;

    v->push_back(osg::Vec3f(point[0] + trans[0], 0, point[1]+ trans[1]));
    v->push_back(osg::Vec3f(p2[0]+ trans[0], 0, p2[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(p3[0]+ trans[0], 0, p3[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(p4[0]+ trans[0], 0, p4[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(color);
    c->push_back(color);

    //Antialiasing top
    double thcik_ans = 0.2;
    double alpha = 0.3;

    osg::Vec2d p5 = point + osg::Vec2d(0, 1) * thcik_ans;
    osg::Vec2d p6 = p5 + vv;

    v->push_back(osg::Vec3f(point[0] + trans[0], 0, point[1]+ trans[1]));
    v->push_back(osg::Vec3f(p2[0]+ trans[0], 0, p2[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(p6[0]+ trans[0], 0, p6[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(p5[0]+ trans[0], 0, p5[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //Antialiasing bottom
    osg::Vec2d p7 = p4 + osg::Vec2d(0, -1) * thcik_ans;
    osg::Vec2d p8 = p7 + vv;

    v->push_back(osg::Vec3f(p4[0] + trans[0], 0, p4[1]+ trans[1]));
    v->push_back(osg::Vec3f(p3[0]+ trans[0], 0, p3[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(p8[0]+ trans[0], 0, p8[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(p7[0]+ trans[0], 0, p7[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    //
    v->push_back(osg::Vec3f(-point[0]+ trans[0], 0, point[1]+ trans[1]));
    v->push_back(osg::Vec3f(-p2[0]+ trans[0], 0, p2[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(-p3[0]+ trans[0], 0, p3[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(-p4[0]+ trans[0], 0, p4[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(color);
    c->push_back(color);

 //
    //Antialiasing
    v->push_back(osg::Vec3f(-point[0] + trans[0], 0, point[1]+ trans[1]));
    v->push_back(osg::Vec3f(-p2[0]+ trans[0], 0, p2[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(-p6[0]+ trans[0], 0, p6[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(-p5[0]+ trans[0], 0, p5[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //Antialiasing
    //Antialiasing bottom
    v->push_back(osg::Vec3f(-p4[0] + trans[0], 0, p4[1]+ trans[1]));
    v->push_back(osg::Vec3f(-p3[0]+ trans[0], 0, p3[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(-p8[0]+ trans[0], 0, p8[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(-p7[0]+ trans[0], 0, p7[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //left
    osg::Vec2d p9 = point + vv_normal * 0.2 + vv;
    osg::Vec2d p10 = p5 + vv_normal * 0.2 + vv;

    //
    osg::Vec2d p11 = p4 + vv_normal * 0.1 + vv;
    osg::Vec2d p12 = p7 + vv_normal * 0.1 + vv;

    v->push_back(osg::Vec3f(p6[0] + trans[0], 0, p6[1]+ trans[1]));
    v->push_back(osg::Vec3f(p10[0]+ trans[0], 0, p10[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(p9[0]+ trans[0], 0, p9[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(p2[0]+ trans[0], 0, p2[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(p2[0] + trans[0], 0, p2[1]+ trans[1]));
    v->push_back(osg::Vec3f(p9[0]+ trans[0], 0, p9[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(p11[0]+ trans[0], 0, p11[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(p3[0]+ trans[0], 0, p3[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(p3[0] + trans[0], 0, p3[1]+ trans[1]));
    v->push_back(osg::Vec3f(p11[0]+ trans[0], 0, p11[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(p12[0]+ trans[0], 0, p12[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(p8[0]+ trans[0], 0, p8[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //right
    v->push_back(osg::Vec3f(-p6[0] + trans[0], 0, p6[1]+ trans[1]));
    v->push_back(osg::Vec3f(-p10[0]+ trans[0], 0, p10[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(-p9[0]+ trans[0], 0, p9[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(-p2[0]+ trans[0], 0, p2[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(-p2[0] + trans[0], 0, p2[1]+ trans[1]));
    v->push_back(osg::Vec3f(-p9[0]+ trans[0], 0, p9[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(-p11[0]+ trans[0], 0, p11[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(-p3[0]+ trans[0], 0, p3[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(-p3[0] + trans[0], 0, p3[1]+ trans[1]));
    v->push_back(osg::Vec3f(-p11[0]+ trans[0], 0, p11[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(-p12[0]+ trans[0], 0, p12[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(-p8[0]+ trans[0], 0, p8[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    geom->setVertexArray(v.get());
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, v->size()));
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    geode->addDrawable(geom.get());
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    return geode;
}

osg::ref_ptr<osg::Geode> createNormalArrow(double angle, double h, double l, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();

    v->push_back(osg::Vec3f(0, 0, h));
    v->push_back(osg::Vec3f(l * sin(angle * osg::PI / 180.0), 0, h - l * cos(angle * osg::PI / 180.0)));
    v->push_back(osg::Vec3f(0, 0, 0));

    v->push_back(osg::Vec3f(0, 0, h));
    v->push_back(osg::Vec3f(0, 0, 0));
    v->push_back(osg::Vec3f(-l * sin(angle * osg::PI / 180.0), 0, h -l * cos(angle * osg::PI / 180.0)));

    c->push_back(color);
    c->push_back(color);
    c->push_back(color);

    c->push_back(color);
    c->push_back(color);
    c->push_back(color);
    //ans
    double thick_up   = 0.15;
    double thcik_down = 0.15;
    double alpha = 0.3;

    v->push_back(osg::Vec3f(0, 0, h + thick_up));
    v->push_back(osg::Vec3f(l * sin(angle * osg::PI / 180.0), 0, h - l * cos(angle * osg::PI / 180.0)));
    v->push_back(osg::Vec3f(0, 0, h));

    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(0, 0, 0));
    v->push_back(osg::Vec3f(l * sin(angle * osg::PI / 180.0), 0, h - l * cos(angle * osg::PI / 180.0)));
    v->push_back(osg::Vec3f(0, 0, -thcik_down));

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));


    v->push_back(osg::Vec3f(0, 0, h + thick_up));
    v->push_back(osg::Vec3f(-l * sin(angle * osg::PI / 180.0), 0, h - l * cos(angle * osg::PI / 180.0)));
    v->push_back(osg::Vec3f(0, 0, h));

    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(0, 0, 0));
    v->push_back(osg::Vec3f(-l * sin(angle * osg::PI / 180.0), 0, h - l * cos(angle * osg::PI / 180.0)));
    v->push_back(osg::Vec3f(0, 0, -thcik_down));

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //
    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    geom->setVertexArray(v.get());
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, v->size()));
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    geode->addDrawable(geom.get());
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    return geode;
}

osg::ref_ptr<osg::Geode> createErectArrowAnimation(osg::Vec2d point, double angle, double h, double l, osg::Vec4 color, osg::Vec2d trans)
{
    osg::ref_ptr<osg::Geometry> geom= new osg::Geometry();

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();

    osg::Vec2d vv( l * sin(angle * osg::PI / 180.0), -  cos(angle * osg::PI / 180.0) );

    osg::Vec2d vv_normal = vv;
    vv_normal.normalize();

    osg::Vec2d p2 = point + vv;
    osg::Vec2d p4 = point + osg::Vec2d(0, -1) * h;
    osg::Vec2d p3 = p4 + vv;

    v->push_back(osg::Vec3f(0, point[0] + trans[0],  point[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, p2[0]+ trans[0],   p2[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, p3[0]+ trans[0], p3[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, p4[0]+ trans[0],  p4[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(color);
    c->push_back(color);

    //Antialiasing top
    double thcik_ans = 0.2;
    double alpha = 0.3;

    osg::Vec2d p5 = point + osg::Vec2d(0, 1) * thcik_ans;
    osg::Vec2d p6 = p5 + vv;

    v->push_back(osg::Vec3f(0, point[0] + trans[0], point[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, p2[0]+ trans[0],  p2[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, p6[0]+ trans[0], p6[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, p5[0]+ trans[0],  p5[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //Antialiasing bottom
    osg::Vec2d p7 = p4 + osg::Vec2d(0, -1) * thcik_ans;
    osg::Vec2d p8 = p7 + vv;

    v->push_back(osg::Vec3f(0, p4[0] + trans[0], p4[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, p3[0]+ trans[0], p3[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, p8[0]+ trans[0], p8[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, p7[0]+ trans[0],  p7[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    //

    v->push_back(osg::Vec3f(0, -point[0]+ trans[0], point[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, -p2[0]+ trans[0],  p2[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, -p3[0]+ trans[0],  p3[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, -p4[0]+ trans[0],  p4[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(color);
    c->push_back(color);

 //
    //Antialiasing
    v->push_back(osg::Vec3f(0, -point[0] + trans[0],  point[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, -p2[0]+ trans[0],  p2[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, -p6[0]+ trans[0],  p6[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, -p5[0]+ trans[0], p5[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //Antialiasing
    //Antialiasing bottom
    v->push_back(osg::Vec3f(0, -p4[0] + trans[0],  p4[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, -p3[0]+ trans[0],  p3[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, -p8[0]+ trans[0],  p8[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, -p7[0]+ trans[0],  p7[1]+ trans[1])) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //left
    osg::Vec2d p9 = point + vv_normal * 0.2 + vv;
    osg::Vec2d p10 = p5 + vv_normal * 0.2 + vv;

    //
    osg::Vec2d p11 = p4 + vv_normal * 0.1 + vv;
    osg::Vec2d p12 = p7 + vv_normal * 0.1 + vv;

    v->push_back(osg::Vec3f(0, p6[0] + trans[0], p6[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, p10[0]+ trans[0], p10[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, p9[0]+ trans[0], p9[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, p2[0]+ trans[0],  p2[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(0, p2[0] + trans[0], p2[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, p9[0]+ trans[0], p9[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, p11[0]+ trans[0], p11[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, p3[0]+ trans[0], p3[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(0, p3[0] + trans[0], p3[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, p11[0]+ trans[0], p11[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, p12[0]+ trans[0], p12[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, p8[0]+ trans[0], p8[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //right
    v->push_back(osg::Vec3f(0, -p6[0] + trans[0], p6[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, -p10[0]+ trans[0],  p10[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, -p9[0]+ trans[0], p9[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, -p2[0]+ trans[0],  p2[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(0, -p2[0] + trans[0], p2[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, -p9[0]+ trans[0], p9[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, -p11[0]+ trans[0], p11[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, -p3[0]+ trans[0],  p3[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(0, -p3[0] + trans[0], p3[1]+ trans[1]));
    v->push_back(osg::Vec3f(0, -p11[0]+ trans[0], p11[1]+ trans[1])) ;
    v->push_back(osg::Vec3f(0, -p12[0]+ trans[0], p12[1]+ trans[1]))  ;
    v->push_back(osg::Vec3f(0, -p8[0]+ trans[0], p8[1]+ trans[1])) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));


    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    geom->setVertexArray(v.get());
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, v->size()));

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    geode->addDrawable(geom.get());
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    return geode;
}

// Triangle
osg::ref_ptr<osg::Geode> createErectArrow(osg::Vec2d f, osg::Vec2d s)
{
    return createArrowAnimation(osg::Vec2d(0, 1), 5, 1, 6, osg::Vec4(0.0, 1.0, 0.0, 1.0), s);
}


osg::ref_ptr<osg::MatrixTransform> createErectArrow(osg::Vec3f f, osg::Vec3f s, double w, double h, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geode> geode = createErectArrowAnimation(osg::Vec2d(0, 1), 5, 1, 12, osg::Vec4(0.0, 1.0, 0.0, 1.0));
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();

    transform->addChild(geode);
    osg::Matrixf trs;
    trs.makeTranslate(f);

    osg::Matrixf rot;
    osg::Vec3f vec = s - f;

    rot.makeRotate(osg::Vec3f(0, 0, 1), osg::Vec3f(vec[0], 0, vec[1]));

    osg::Vec3f vecc(vec[0], 0, vec[1]);
    osg::Vec3f z(0, 0, 1);

    osg::Vec3f dir = z ^ vecc;
    dir.normalize();

    double angle_ = abs(z * vecc);

    if(dir[1] > 0)
    {
        //OSGRenderParameter::erect_arrow_rotate_x_ = -angle_ * 10;
    }
    else if(dir[1] <= 0)
    {
        //OSGRenderParameter::erect_arrow_rotate_x_ = angle_ *10;
    }

    osg::Matrixf rotvec;
   // rotvec.makeRotate(OSGRenderParameter::erect_arrow_rotate_x_/ 180 * osg::PI, vecc);

    osg::Matrixf mat = rot  * trs;
    transform->setMatrix(mat);

    return transform;
}


void createCommonArrow(osg::ref_ptr<osg::MatrixTransform> mt, std::vector<osg::Vec3f>& div_point, osg::Vec4 color, osg::Matrix mat, osg::Matrix mvp)
{
    for(size_t i = 0; i < div_point.size() - 1; i++)
    {
        osg::Vec3f f = div_point[i];
        osg::Vec3f s = div_point[i+1];

        osg::Vec3f screenf = f * mvp;
        osg::Vec3f screens = s * mvp;

        osg::Vec3f v = screens - screenf;
        v.normalize();

        double a = osg::Vec3f(v[0], v[1], 0) * osg::Vec3(1, 0, 0);

         if(a > 0.95 || a < -0.95)
         {
            osg::Vec3f scf = div_point[i] * mat   ;
            osg::Vec3f scs = div_point[i+1] * mat ;

            mt->addChild( createNormalArrow(osg::Vec2d(scf[0], scf[2]), osg::Vec2d(scs[0], scs[2]), color) );
          }
          else
          {
             mt->addChild(createDefArrow(osg::Vec2d(f[0], f[2]), osg::Vec2d(s[0], s[2]), false, color));
          }
      }
}


osg::ref_ptr<osg::Billboard> createArrowBillboard(osg::Vec2d point, double angle, double h, double l, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geometry> geom= new osg::Geometry();

    osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array();
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();

    osg::Vec2d vv_normal(sin(angle * osg::PI / 180.0), -cos(angle * osg::PI / 180.0) );
    osg::Vec2d vv( l * sin(angle * osg::PI / 180.0), -l * cos(angle * osg::PI / 180.0) );

    osg::Vec2d p2 = point + vv;
    osg::Vec2d p4 = point + osg::Vec2d(0, -1) * h;
    osg::Vec2d p3 = p4 + vv;

    v->push_back(osg::Vec3f(point[0] , point[1], 0));
    v->push_back(osg::Vec3f(p2[0],  p2[1], 0)) ;
    v->push_back(osg::Vec3f(p3[0],  p3[1], 0))  ;
    v->push_back(osg::Vec3f(p4[0],  p4[1], 0)) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(color);
    c->push_back(color);

    //Antialiasing top
    double thcik_ans = 0.2;
    double alpha = 0.3;

    osg::Vec2d p5 = point + osg::Vec2d(0, 1) * thcik_ans;
    osg::Vec2d p6 = p5 + vv;

    v->push_back(osg::Vec3f(point[0] , point[1], 0));
    v->push_back(osg::Vec3f(p2[0],  p2[1], 0)) ;
    v->push_back(osg::Vec3f(p6[0],  p6[1], 0))  ;
    v->push_back(osg::Vec3f(p5[0],  p5[1], 0)) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //Antialiasing bottom
    osg::Vec2d p7 = p4 + osg::Vec2d(0, -1) * thcik_ans;
    osg::Vec2d p8 = p7 + vv;

    v->push_back(osg::Vec3f(p4[0] , p4[1], 0));
    v->push_back(osg::Vec3f(p3[0],  p3[1], 0)) ;
    v->push_back(osg::Vec3f(p8[0],  p8[1], 0))  ;
    v->push_back(osg::Vec3f(p7[0],  p7[1], 0)) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    //


    v->push_back(osg::Vec3f(-point[0], point[1], 0));
    v->push_back(osg::Vec3f(-p2[0],  p2[1], 0)) ;
    v->push_back(osg::Vec3f(-p3[0],  p3[1], 0))  ;
    v->push_back(osg::Vec3f(-p4[0],  p4[1], 0)) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(color);
    c->push_back(color);

 //
    //Antialiasing
    v->push_back(osg::Vec3f(-point[0] , point[1], 0));
    v->push_back(osg::Vec3f(-p2[0],  p2[1], 0)) ;
    v->push_back(osg::Vec3f(-p6[0],  p6[1], 0))  ;
    v->push_back(osg::Vec3f(-p5[0],  p5[1], 0)) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //Antialiasing
    //Antialiasing bottom
    v->push_back(osg::Vec3f(-p4[0] , p4[1], 0));
    v->push_back(osg::Vec3f(-p3[0],  p3[1], 0)) ;
    v->push_back(osg::Vec3f(-p8[0],  p8[1], 0))  ;
    v->push_back(osg::Vec3f(-p7[0],  p7[1], 0)) ;

    c->push_back(color);
    c->push_back(color);
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //left
    osg::Vec2d p9 = point + vv_normal * 0.2 + vv;
    osg::Vec2d p10 = p5 + vv_normal * 0.2 + vv;

    //
    osg::Vec2d p11 = p4 + vv_normal * 0.1 + vv;
    osg::Vec2d p12 = p7 + vv_normal * 0.1 + vv;

    v->push_back(osg::Vec3f(p6[0] ,  p6[1], 0));
    v->push_back(osg::Vec3f(p10[0],  p10[1], 0)) ;
    v->push_back(osg::Vec3f(p9[0],  p9[1], 0))  ;
    v->push_back(osg::Vec3f(p2[0],  p2[1], 0)) ;

    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(p2[0] , p2[1], 0));
    v->push_back(osg::Vec3f(p9[0],  p9[1], 0)) ;
    v->push_back(osg::Vec3f(p11[0], p11[1], 0))  ;
    v->push_back(osg::Vec3f(p3[0],  p3[1], 0)) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(p3[0] , p3[1], 0));
    v->push_back(osg::Vec3f(p11[0], p11[1], 0)) ;
    v->push_back(osg::Vec3f(p12[0], p12[1], 0))  ;
    v->push_back(osg::Vec3f(p8[0], p8[1], 0)) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    //right
    v->push_back(osg::Vec3f(-p6[0] , p6[1], 0));
    v->push_back(osg::Vec3f(-p10[0], p10[1], 0)) ;
    v->push_back(osg::Vec3f(-p9[0],  p9[1], 0))  ;
    v->push_back(osg::Vec3f(-p2[0],  p2[1], 0)) ;

    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(-p2[0],  p2[1], 0));
    v->push_back(osg::Vec3f(-p9[0],  p9[1], 0)) ;
    v->push_back(osg::Vec3f(-p11[0], p11[1], 0))  ;
    v->push_back(osg::Vec3f(-p3[0], p3[1], 0)) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color));

    v->push_back(osg::Vec3f(-p3[0] , p3[1], 0));
    v->push_back(osg::Vec3f(-p11[0], p11[1], 0)) ;
    v->push_back(osg::Vec3f(-p12[0], p12[1], 0))  ;
    v->push_back(osg::Vec3f(-p8[0], p8[1], 0)) ;

    c->push_back(osg::Vec4(color));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));
    c->push_back(osg::Vec4(color[0], color[1], color[2], alpha));

    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    geom->setVertexArray(v.get());

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, v->size()));

    osg::ref_ptr<osg::Billboard> geode = new osg::Billboard;

    geode->addDrawable(geom.get());

    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);


    return geode;
}

osg::ref_ptr<osg::Projection> createNormalArrow(osg::Vec2d f, osg::Vec2d s, osg::Vec4 color)
{
    std::vector<osg::Vec4d> c;

    osg::ref_ptr<osg::Billboard> billBoard = createArrowBillboard(osg::Vec2d(0, 0), 60, 20, 25, color);

    osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();

    modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    modeview->setMatrix(osg::Matrix::identity());

    billBoard->setMode(osg::Billboard::AXIAL_ROT);

    osg::ref_ptr<osg::StateSet> stateset = billBoard->getStateSet();

    stateset->setRenderBinDetails(1, "RenderBin");
    stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth(1);
    stateset->setAttribute(lw);

    billBoard->setPosition(0, osg::Vec3f(f[0], 0, f[1]));
    billBoard->setPosition(1, osg::Vec3f(s[0], 0, s[1]));

    billBoard->setPosition(2, osg::Vec3f(100, 600, 100));

    modeview->addChild(billBoard);

    osg::ref_ptr<osg::Projection> projection = new osg::Projection;

  #ifdef AR_HUD_DEVICE
    projection->setMatrix(osg::Matrix::ortho2D(0, 800, 0, 480));
  #else
    projection->setMatrix(osg::Matrix::ortho2D(0, 1024, 0, 600));
  #endif

    projection->addChild(modeview);

    return projection;
}

osg::ref_ptr<osg::MatrixTransform> createDefArrow(osg::Vec2d f, osg::Vec2d s, bool bjust, osg::Vec4f color)
{
    osg::ref_ptr<osg::Geode> geode = NULL;

    if(bjust)
    {
      // geode = createErectArrowAnimation(osg::Vec2d(0, 1), OSGRenderParameter::erect_arrow_rotate_y_, 1.2, 23, color);
    }
    else
    {
      // geode = createErectArrowAnimation(osg::Vec2d(0, 1), OSGRenderParameter::erect_arrow_rotate_y_, 2, 20, color);
    }

    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);


    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();

    transform->addChild(geode);


    osg::Vec2f vecy = s - f;
    vecy.normalize();

    osg::Matrixf trs;

    trs.makeTranslate(osg::Vec3f(s[0], 0, s[1]));

    osg::Matrixf rot;

    rot.makeRotate(osg::Vec3f(0, 0, 1), osg::Vec3f(vecy[0], 0, vecy[1]));

    osg::Vec3f vecc(vecy[0], 0, vecy[1]);

    osg::Vec3f z(0, 0, 1);

    osg::Vec3f dir = z ^ vecc;
    dir.normalize();

    double angle = abs(z * vecc);

//    if(dir[1] > 0)
//    {
//        OSGRenderParameter::erect_arrow_rotate_x_ = -angle * 5;
//    }
//    else if(dir[1] <= 0)
//    {
//        OSGRenderParameter::erect_arrow_rotate_x_ = angle *5;
//    }

   // double xxxx = OSGRenderParameter::erect_arrow_rotate_x_;

    osg::Matrixf rotvec;
  //  rotvec.makeRotate(OSGRenderParameter::erect_arrow_rotate_x_/ 180 * 3.14, vecc);

    osg::Matrixf mat = rot * rotvec * trs;

    transform->setMatrix(mat);

    return transform;
}


