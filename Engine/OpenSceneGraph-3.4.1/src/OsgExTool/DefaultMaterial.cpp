
#include <osg/Group>  
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>  
#include <osgUtil/PrintVisitor>  
#include <osgViewer/ViewerEventHandlers>  
#include <osgViewer/Viewer>  
#include <osg/LightModel>
#include <iostream>  
#include <fstream>  
#include <sstream>  

#include <OsgExTool/DefaultMaterial>


DefaultMaterial::DefaultMaterial()
{

}

DefaultMaterial::~DefaultMaterial()
{

}


osg::Group* OsgExTool::DefaultMaterial::createSunLight( osg::Node* node )
{
	osg::LightSource* sunLightSource = new osg::LightSource;

	osg::Light* sunLight = sunLightSource->getLight();

	sunLight->setLightNum(0);
	sunLight->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	sunLight->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	sunLight->setSpecular(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

	//
	osg::Vec3f center = node->getBound().center();

	osg::Vec3f pos = center  + osg::Vec3f(1, 1, 1) * node->getBound().radius() * 3; 

	sunLight->setPosition(osg::Vec4f(pos.x(), pos.y(), pos.z(), 1));


	sunLightSource->setLight( sunLight );
	sunLightSource->setLocalStateSetModes( osg::StateAttribute::ON );
	sunLightSource->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);

	osg::LightModel* lightModel = new osg::LightModel;

	lightModel->setAmbientIntensity(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

	sunLightSource->getOrCreateStateSet()->setAttribute(lightModel);

	sunLightSource->addChild(node);

	return sunLightSource;
}
