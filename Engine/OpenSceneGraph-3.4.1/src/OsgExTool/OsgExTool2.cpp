
#include <osg/Group>  
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>  
#include <osgUtil/PrintVisitor>  
#include <osgViewer/ViewerEventHandlers>  
#include <osgViewer/Viewer>  
#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <OsgExTool/OsgExTool>
#include <OsgExTool/OsgExTool2>
#include <DebugTool/DebugFunction>

#include <osg/ComputeBoundsVisitor>
#include <BaseTool/BaseTool>

#include <osgDB/FileUtils>

#include <OsgExTool/MergeGeometry>
#include <OsgExTool/BigTextureText>

#include <osg/LightModel>

//
#include <osgUtil/ReflectionMapGenerator>
#include <osgUtil/HighlightMapGenerator>
#include <osgUtil/HalfWayMapGenerator>

#include <osg/TexEnvCombine>

#include <osgText/Text3D>

#include <osg/Stencil>

#include "OsgExTool/Tex2DDrawable.h"

using namespace OsgExTool;

void setHideRectScope(osg::StateSet* state)
{
	osg::Stencil* stencil = new osg::Stencil;

	stencil->setFunction(osg::Stencil::ALWAYS, 1, 0xFF);
	stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::KEEP);

	state->setRenderBinDetails(1, "RenderBin" );
	state->setAttributeAndModes(stencil);

	state->setAttribute(stencil, osg::StateAttribute::OVERRIDE);

     osg::ColorMask* colourMask = new osg::ColorMask(false, false, false, false);
	 state->setAttribute(colourMask, osg::StateAttribute::OVERRIDE);
}

void setHideArrow(osg::StateSet* state)
{
	osg::Stencil* stencil = new osg::Stencil;

	stencil->setFunction(osg::Stencil::NOTEQUAL, 0x1, 0xFF);

	stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::KEEP);

	state->setRenderBinDetails(100, "RenderBin" );

	state->setAttributeAndModes(stencil);

	state->setAttribute(stencil, osg::StateAttribute::OVERRIDE);
}

osg::ref_ptr<osg::Group> OsgExTool::OsgExToolSet2::StencilTest()
 {
	  osg::ref_ptr<osg::Geode> g1 = OsgExTool::OsgExToolSet::CreateQuadGeode(osg::Vec3d(0, 0, 0), osg::Vec3d(100, 0, 0), osg::Vec3d(100, 20, 0), osg::Vec3d(0, 20, 0), osg::Vec4(1.0, 0, 0, 1.0));

	  setHideRectScope(g1->getOrCreateStateSet());

	  osg::ref_ptr<osg::Geode> g2 = OsgExTool::OsgExToolSet::CreateQuadGeode(osg::Vec3d(50, -100, 0), osg::Vec3d(70, -100, 0), osg::Vec3d(70, 100, 0), osg::Vec3d(50, 100, 0), osg::Vec4(0.0, 1.0, 0.0, 1.0));

	  setHideArrow(g2->getOrCreateStateSet()) ;

	  osg::ref_ptr<osg::Group> group = new osg::Group();

	  group->addChild(g1);
	  group->addChild(g2);

	  return group;
 }


