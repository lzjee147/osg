#include "OsgExTool/OSGUniverseTool.h"


#include <osg/DrawPixels>
#include <osg/PolygonMode>

#include "OsgExTool/MaterialManger.h"


#include <osg/ValueObject>
#include <osg/PolygonOffset>
#include <osg/Depth>

#include <osg/ShapeDrawable>

#include <osgText/Text3D>

#include <osg/Material>


osg::ref_ptr<osg::Geode> create3DText(osg::Vec3 pos)
{
    osg::ref_ptr<osg::Geode> geode  = new osg::Geode;
    osg::ref_ptr<osgText::Text3D> text = new osgText::Text3D;

//    text->setFont("D://xxx.ttf");
//	text1->setCharacterSize(characterSize);
//	text1->setCharacterDepth(characterDepth);
    text->setPosition(pos);
    text->setDrawMode(osgText::Text3D::TEXT | osgText::Text3D::BOUNDINGBOX);
    text->setAxisAlignment(osgText::Text3D::XY_PLANE);
    text->setText("ABCD");

    geode->addDrawable(text);

    //osg::ref_ptr<osgText::Style> style = new osgText::Style;
    //osg::ref_ptr<osgText::Bevel> bevel = new osgText::Bevel;
    //bevel->roundedBevel2(0.25);
    //style->setBevel(bevel.get());
    //style->setWidthRatio(0.4f);

    osg::Material* front = new osg::Material;
    front->setAlpha(osg::Material::FRONT_AND_BACK,1);
    front->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.2,0.2,0.2,1.0));
    front->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(.0,.0,1.0,1.0));
    geode->getOrCreateStateSet()->setAttributeAndModes(front);

    return geode;
}

osg::ref_ptr<osgText::Text> createTextGeometry(std::string str, osg::Vec3f pos, float size, osg::Vec4f color)
{
    osg::ref_ptr<osgText::Text> text = new osgText::Text;

    text->setCharacterSize( size );
    text->setAxisAlignment( osgText::TextBase::XY_PLANE );

    text->setAlignment(osgText::TextBase::CENTER_BASE_LINE);

    text->setDataVariance(osg::Object::DYNAMIC);

    text->setColor(color);
    text->setPosition( pos );
    text->setText( str );

    text->getOrCreateStateSet()->setRenderBinDetails(200, "RenderBin");
    text->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    return text;
}

osg::ref_ptr<osg::Projection> createBillboardText(std::string text, osg::Vec3f pos, int size, osg::Vec4 color)
{
    osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();

    modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    modeview->setMatrix(osg::Matrix::identity());

    osg::ref_ptr<osg::Billboard> billBoard = new osg::Billboard();

    billBoard->setMode(osg::Billboard::POINT_ROT_EYE);


    billBoard->addDrawable(createTextGeometry(text, pos, size, color));
    billBoard->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    //


    osg::ref_ptr<osg::StateSet> stateset = billBoard->getStateSet();

    stateset->setRenderBinDetails(1, "RenderBin");
    stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    billBoard->setPosition(0, osg::Vec3f(pos[0], 0, pos[2]));
    billBoard->setPosition(1, osg::Vec3f(1, 0, 1));

    billBoard->setPosition(2, osg::Vec3f(100, 600, 150));
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



osg::ref_ptr<osg::Image> getImageFromIconId(int iconID)
{
    return NULL;
}


osg::ref_ptr<osg::Image> getImageFromPath(std::string path)
{
     return NULL;
}


void createGemetry_(osg::ref_ptr<osg::Vec3Array> v, osg::ref_ptr<osg::Vec4Array> c, osg::Vec4f color, osg::Vec2d f, osg::Vec2d s,  double width)
{
    osg::Vec2d n = s - f;
    n.normalize();

    osg::Vec2d t(-n[1], n[0]);
    double thick = 0.01;
    double a = 0.3;

    osg::Vec2d s1 = s - t * width * 0.5;
    osg::Vec2d s2 = s - t * (width * 0.5 - thick);

    osg::Vec2d s3 = s + t * (width * 0.5 - thick);
    osg::Vec2d s4 = s + t * width * 0.5;

    osg::Vec2d f1 = f - t * width * 0.5;
    osg::Vec2d f2 = f - t * (width * 0.5 - thick);

    osg::Vec2d f3 = f + t * (width * 0.5 - thick);
    osg::Vec2d f4 = f + t * width * 0.5;

    v->push_back(osg::Vec3d(f2[0], 0, f2[1]));
    v->push_back(osg::Vec3d(f1[0], 0, f1[1]));
    v->push_back(osg::Vec3d(s1[0], 0, s1[1]));

    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
    c->push_back(osg::Vec4(color[0], color[1], color[2], a));
    c->push_back(osg::Vec4(color[0], color[1], color[2], a));

    v->push_back(osg::Vec3d(f2[0], 0, f2[1]));
    v->push_back(osg::Vec3d(s1[0], 0, s1[1]));
    v->push_back(osg::Vec3d(s2[0], 0, s2[1]));

    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
    c->push_back(osg::Vec4(color[0], color[1], color[2], a));
    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));

    //
    v->push_back(osg::Vec3d(f3[0], 0, f3[1]));
    v->push_back(osg::Vec3d(f2[0], 0, f2[1]));
    v->push_back(osg::Vec3d(s2[0], 0, s2[1]));

    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));

    v->push_back(osg::Vec3d(f3[0], 0, f3[1]));
    v->push_back(osg::Vec3d(s2[0], 0, s2[1]));
    v->push_back(osg::Vec3d(s3[0], 0, s3[1]));

    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));

    //
    v->push_back(osg::Vec3d(f4[0], 0, f4[1]));
    v->push_back(osg::Vec3d(f3[0], 0, f3[1]));
    v->push_back(osg::Vec3d(s3[0], 0, s3[1]));

    c->push_back(osg::Vec4(color[0], color[1], color[2], a));
    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));

    v->push_back(osg::Vec3d(f4[0], 0, f4[1]));
    v->push_back(osg::Vec3d(s3[0], 0, s3[1]));
    v->push_back(osg::Vec3d(s4[0], 0, s4[1]));

    c->push_back(osg::Vec4(color[0], color[1], color[2], a));
    c->push_back(osg::Vec4(color[0], color[1], color[2], 1.0));
    c->push_back(osg::Vec4(color[0], color[1], color[2], a));
}

osg::ref_ptr<osg::Billboard> createBilloard(std::vector<osg::Vec3f> points, std::vector<osg::Vec4d> colors, GLenum type)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Billboard>   geode = new osg::Billboard;
    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    geode->setMode(osg::Billboard::POINT_ROT_EYE);
    geom->setVertexArray(v.get());

    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    for(size_t i = 0; i < points.size(); i++)
    {
        v->push_back(points[i]);
    }

    for(size_t i = 0; i < colors.size(); i++)
    {
        c->push_back(colors[i]);
    }

    geom->addPrimitiveSet(new osg::DrawArrays(type, 0, v->size()));
    geode->addDrawable(geom.get());

    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    return geode;
}

osg::ref_ptr<osg::Billboard> createBilloard(std::vector<osg::Vec3f> points, osg::Vec4f colors, GLenum type)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Billboard>   geode = new osg::Billboard;
    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;

    geode->setMode(osg::Billboard::POINT_ROT_EYE);
    geom->setVertexArray(v.get());

    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;
    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    for(size_t i = 0; i < points.size(); i++)
    {
        v->push_back(points[i]);
        c->push_back(colors)   ;
    }

    geom->addPrimitiveSet(new osg::DrawArrays(type, 0, v->size()));
    geode->addDrawable(geom.get());
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    return geode;
}

bool isDraw(osg::Vec3f f, osg::Vec3f s,  double width, osg::Matrix mat)
{
    return false;
    //screen pixel
//    osg::Vec3f ff = osg::Vec3f(f[0], 0, f[1]) * mat;
//    osg::Vec3f ss = osg::Vec3f(s[0], 0, s[1]) * mat;

//    osg::Vec2d ffn = osg::Vec2d(ss[0], ss[1]) - osg::Vec2d(ff[0], ff[1]);

  //  ffn.normalize();

//    double angle = acos(ffn[0]) / osg::PI * 180;
//    if((angle > 20  && angle < 160))
//    {
//        return false;
//    }

    osg::Vec3f n = s - f;
    n.normalize();

    osg::Vec3f t(-n[2], 0, n[0]);

    osg::Vec3f s1 = s - t * width * 0.5;
    osg::Vec3f s4 = s + t * width * 0.5;

    osg::Vec3f s1s = s1 * mat;
    osg::Vec3f s4s = s4 * mat;

    osg::Vec3f f1 = f - t * width * 0.5;
    osg::Vec3f f4 = f + t * width * 0.5;

    osg::Vec3f f1s = f1 * mat;
    osg::Vec3f f4s = f4 * mat;

//    if(abs(s1s[0] - f1s[0]) < 0.0001 ||abs(s1s[1] - f1s[1]) < 0.0001)
//        return true;

    double aa = (s4s[0] - f1s[0])/(s1s[0] - f1s[0]) - (s4s[1] - f1s[1])/(s1s[1] - f1s[1]);

    double bb = (f4s[0] - f1s[0])/(s1s[0] - f1s[0]) - (f4s[1] - f1s[1])/(s1s[1] - f1s[1]);
    osg::Vec2d n11 = osg::Vec2d(s1s[0], s1s[1]) - osg::Vec2d(f1s[0], f1s[1]);
    n11.normalize();

    osg::Vec2d n14 = osg::Vec2d(s4s[0], s4s[1]) - osg::Vec2d(f1s[0], f1s[1]);
    n14.normalize();

    double l = n11 * n14;

    return (l > 0.99 || l < -0.99);

}
osg::ref_ptr<osg::Projection> createRect(double x,  double y,  double width , double height, osg::Vec4 color, int type, double line_width, int bin_num)
{
  std::vector<osg::Vec3f> points;

  points.push_back(osg::Vec3f(-0.5, 0, 0))  ;
  points.push_back(osg::Vec3f(0.5, 0, 0))   ;
  points.push_back(osg::Vec3f(0.5, 1.0, 0)) ;
  points.push_back(osg::Vec3f(-0.5, 1.0, 0));

  osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();

  modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  modeview->setMatrix(osg::Matrix::identity());

  osg::ref_ptr<osg::Billboard> billBoard = createBilloard(points, color, type);


  osg::ref_ptr<osg::StateSet> stateset = billBoard->getStateSet();

  stateset->setRenderBinDetails(bin_num, "RenderBin");
  stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

  stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
  stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

  osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth(line_width);
  stateset->setAttribute(lw);

  billBoard->setPosition(0, osg::Vec3f(x, 0, y));
  billBoard->setPosition(1, osg::Vec3f(width, 0, height));

  billBoard->setPosition(2, osg::Vec3f(100, 600, 150));
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


osg::ref_ptr<osg::Geode> createBoxsGeode(std::vector<osg::Vec3f>& points, double width, double height, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;

   
    return geode;
}


osg::ref_ptr<osg::Geometry> createSimpleBox(osg::Vec3 center, osg::Vec3f xyzLength, osg::Vec4 color)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry ;
    osg::ref_ptr<osg::Vec3Array>   v = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array>   n = new osg::Vec3Array;

    geom->setVertexArray(v.get());
    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array;

    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    geom->setNormalArray(n.get());
    geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

    //quadx+
    v->push_back(osg::Vec3f(xyzLength[0] * 0.5,  xyzLength[1] * 0.5, xyzLength[2] * 0.5))  ;
    v->push_back(osg::Vec3f(xyzLength[0] * 0.5,  -xyzLength[1] * 0.5, xyzLength[2] * 0.5)) ;
    v->push_back(osg::Vec3f(xyzLength[0] * 0.5,  -xyzLength[1] * 0.5, -xyzLength[2] * 0.5));
    v->push_back(osg::Vec3f(xyzLength[0] * 0.5,  xyzLength[1] * 0.5, -xyzLength[2] * 0.5)) ;

    n->push_back(osg::Vec3f(1, 0, 0));
    n->push_back(osg::Vec3f(1, 0, 0));
    n->push_back(osg::Vec3f(1, 0, 0));
    n->push_back(osg::Vec3f(1, 0, 0));
    //
    //x-
    v->push_back(osg::Vec3f(-xyzLength[0] * 0.5,  xyzLength[1] * 0.5, xyzLength[2] * 0.5))  ;
    v->push_back(osg::Vec3f(-xyzLength[0] * 0.5,  -xyzLength[1] * 0.5, xyzLength[2] * 0.5)) ;
    v->push_back(osg::Vec3f(-xyzLength[0] * 0.5,  -xyzLength[1] * 0.5, -xyzLength[2] * 0.5));
    v->push_back(osg::Vec3f(-xyzLength[0] * 0.5,  xyzLength[1] * 0.5, -xyzLength[2] * 0.5)) ;

    n->push_back(osg::Vec3f(-1, 0, 0));
    n->push_back(osg::Vec3f(-1, 0, 0));
    n->push_back(osg::Vec3f(-1, 0, 0));
    n->push_back(osg::Vec3f(-1, 0, 0));

    //y-
    v->push_back(osg::Vec3f(xyzLength[0] * 0.5,  -xyzLength[1] * 0.5, xyzLength[0] * 0.5))  ;
    v->push_back(osg::Vec3f(-xyzLength[0] * 0.5,  -xyzLength[1] * 0.5, xyzLength[0] * 0.5)) ;
    v->push_back(osg::Vec3f(-xyzLength[0] * 0.5,  -xyzLength[1] * 0.5, -xyzLength[0] * 0.5));
    v->push_back(osg::Vec3f(xyzLength[0] * 0.5,  -xyzLength[1] * 0.5, -xyzLength[0] * 0.5)) ;


    n->push_back(osg::Vec3f(0, 1, 0));
    n->push_back(osg::Vec3f(0, 1, 0));
    n->push_back(osg::Vec3f(0, 1, 0));
    n->push_back(osg::Vec3f(0, 1, 0));

    geom->getOrCreateStateSet()->setRenderBinDetails(199, "RenderBin");
    geom->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    return geom;
}

osg::ref_ptr<osg::Geode> createBox(osg::Vec3 center, osg::Vec3f half_length, osg::Vec4 color)
{
    osg::ref_ptr<osg::Box> box = new osg::Box;
    box->setCenter(center);
    box->setHalfLengths(half_length);
    osg::ref_ptr<osg::ShapeDrawable> sp = new osg::ShapeDrawable(box.get());

    sp->setColor(color);
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(sp.get());

    geode->getOrCreateStateSet()->setRenderBinDetails(199, "RenderBin");
    geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    return geode;
}

osg::ref_ptr<osg::Projection> createRectTexture(double x,  double y,  double width , double height, int textureID)
{
  std::vector<osg::Vec3f> points;

  points.push_back(osg::Vec3f(-0.5, 0, 0))  ;
  points.push_back(osg::Vec3f(0.5, 0, 0))   ;
  points.push_back(osg::Vec3f(0.5, 1.0, 0)) ;
  points.push_back(osg::Vec3f(-0.5, 1.0, 0));

  osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();

  modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  modeview->setMatrix(osg::Matrix::identity());

  osg::ref_ptr<osg::Billboard> billBoard = createBilloard(points, osg::Vec4f(1.0, 1.0, 1.0, 1.0), osg::PrimitiveSet::QUADS);
  osg::ref_ptr<osg::Vec2Array> tex = new osg::Vec2Array();

  tex->push_back(osg::Vec2(0, 0));
  tex->push_back(osg::Vec2(1, 0));
  tex->push_back(osg::Vec2(1, 1));
  tex->push_back(osg::Vec2(0, 1));

  ( (osg::Geometry*)billBoard->getDrawable(0) )->setTexCoordArray(0, tex, osg::Array::BIND_PER_VERTEX);
  ( (osg::Geometry*)billBoard->getDrawable(0) )->setStateSet(MaterialManager::getInstance()->getMaterialForId(textureID));

  billBoard->setPosition(0, osg::Vec3f(x, 0, y));
  billBoard->setPosition(1, osg::Vec3f(width, 0, height));
  billBoard->setPosition(2, osg::Vec3f(100, 600, 150));

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

osg::ref_ptr<osg::Projection> createPMNode(osg::ref_ptr<osg::Geode> geode, osg::Matrixd modeMatrix, osg::Matrixd proMatrix)
{
  osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();

  modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  modeview->setMatrix(modeMatrix);
  modeview->addChild(geode);

  osg::ref_ptr<osg::Projection> projection = new osg::Projection;

  projection->setMatrix(proMatrix);
  projection->addChild(modeview);

  return projection;
}

osg::ref_ptr<osg::MatrixTransform> createMatrixNode(osg::ref_ptr<osg::Node> node, osg::Vec3f pos)
{
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();

    osg::Matrixf trs;
    trs.makeTranslate(pos);

    osg::Matrixf rot;
    rot.makeRotate(osg::Vec3f(1, 0, 0), osg::Vec3f(0, 0, 1));

    osg::Matrixf scale;
    scale.makeScale(osg::Vec3f(0.1, 0.1, 0.1));


    osg::Matrixf mat = scale * rot * trs;

    transform->setMatrix(mat);
    transform->addChild(node);

    return transform;
}






