#include <windows.h>
#include <iostream>

#include <osgViewer/Viewer>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
//����һ���ı��νڵ�
osg::ref_ptr <osg::Node> cteateQuad(){
//����һ��Ҷ�ڵ����
osg::ref_ptr <osg::Geode > geode = new osg::Geode ();
//����һ�����������
osg::ref_ptr <osg::Geometry >geom=new osg::Geometry ();
//��Ӷ������� ע�ⶥ������˳������ʱ��
osg::ref_ptr <osg::Vec3Array >v=new osg::Vec3Array ();
//�������
v->push_back (osg::Vec3 (0,0,0));
v->push_back (osg::Vec3 (1,0,0));
v->push_back (osg::Vec3 (1,0,1));
v->push_back (osg::Vec3 (0,0,1));
//���ö�������
geom->setVertexArray (v.get ());

//��������������
osg::ref_ptr <osg::Vec2Array >vt=new osg::Vec2Array ();
 //�����������
vt->push_back (osg::Vec2 (0,0));
vt->push_back (osg::Vec2 (1,0));
vt->push_back (osg::Vec2 (1,1));
vt->push_back (osg::Vec2 (0,1));

 //������������
geom->setTexCoordArray(0,vt.get ());

 //������ɫ����
osg::ref_ptr <osg::Vec4Array >vc=new osg::Vec4Array ();
//�������
vc->push_back (osg::Vec4 (1,0,0,1));
vc->push_back (osg::Vec4 (0,1,0,1));
vc->push_back (osg::Vec4 (0,0,1,1));
vc->push_back (osg::Vec4 (1,1,0,1));
//������ɫ����
geom->setColorArray (vc.get ());
//������ɫ�İ󶨷�ʽΪ��������
geom->setColorBinding (osg::Geometry ::BIND_PER_VERTEX );
//������������
osg::ref_ptr <osg::Vec3Array >nc=new osg::Vec3Array ();
//��ӷ���
nc->push_back (osg::Vec3 (0,-1,0));
//���÷���
geom->setNormalArray (nc.get ());
//���÷���Ϊȫ������
geom->setNormalBinding (osg::Geometry ::BIND_OVERALL );
//���ͼԪ
geom->addPrimitiveSet (new osg::DrawArrays (osg::PrimitiveSet ::QUADS ,0,4));
//��ӵ�Ҷ�ӽڵ�
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