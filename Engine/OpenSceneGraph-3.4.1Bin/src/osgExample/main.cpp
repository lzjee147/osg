#include <windows.h>
#include <iostream>

#include <osgViewer/Viewer>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
//创建一个四边形节点
osg::ref_ptr <osg::Node> cteateQuad(){
//创建一个叶节点对象
osg::ref_ptr <osg::Geode > geode = new osg::Geode ();
//创建一个几何体对象
osg::ref_ptr <osg::Geometry >geom=new osg::Geometry ();
//添加顶点数据 注意顶点的添加顺序是逆时针
osg::ref_ptr <osg::Vec3Array >v=new osg::Vec3Array ();
//添加数据
v->push_back (osg::Vec3 (0,0,0));
v->push_back (osg::Vec3 (1,0,0));
v->push_back (osg::Vec3 (1,0,1));
v->push_back (osg::Vec3 (0,0,1));
//设置顶点数据
geom->setVertexArray (v.get ());

//创建纹理订点数据
osg::ref_ptr <osg::Vec2Array >vt=new osg::Vec2Array ();
 //添加纹理坐标
vt->push_back (osg::Vec2 (0,0));
vt->push_back (osg::Vec2 (1,0));
vt->push_back (osg::Vec2 (1,1));
vt->push_back (osg::Vec2 (0,1));

 //设置纹理坐标
geom->setTexCoordArray(0,vt.get ());

 //创建颜色数组
osg::ref_ptr <osg::Vec4Array >vc=new osg::Vec4Array ();
//添加数据
vc->push_back (osg::Vec4 (1,0,0,1));
vc->push_back (osg::Vec4 (0,1,0,1));
vc->push_back (osg::Vec4 (0,0,1,1));
vc->push_back (osg::Vec4 (1,1,0,1));
//设置颜色数组
geom->setColorArray (vc.get ());
//设置颜色的绑定方式为单个顶点
geom->setColorBinding (osg::Geometry ::BIND_PER_VERTEX );
//创建法线数组
osg::ref_ptr <osg::Vec3Array >nc=new osg::Vec3Array ();
//添加法线
nc->push_back (osg::Vec3 (0,-1,0));
//设置法线
geom->setNormalArray (nc.get ());
//设置法绑定为全部顶点
geom->setNormalBinding (osg::Geometry ::BIND_OVERALL );
//添加图元
geom->addPrimitiveSet (new osg::DrawArrays (osg::PrimitiveSet ::QUADS ,0,4));
//添加到叶子节点
geode->addDrawable (geom.get ());
 return geode.get ();
}

void main()
{
    osg::ref_ptr<osgViewer::Viewer > viewer = new osgViewer::Viewer();

    osg::ref_ptr<osg::Group> root= new osg::Group();

    root->addChild (cteateQuad());

    viewer->setSceneData (root.get());

    viewer->realize ();

    viewer->run();
}