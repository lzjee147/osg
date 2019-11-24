#include "OsgExTool/MaterialManger.h"

#include <osg/Texture2D>

#include <osg/PolygonMode>
#include <osg/BlendFunc>

#include "OsgExTool/OSGUniverseTool.h"

MaterialManager* MaterialManager::instance = new MaterialManager();

static char * vertexShader = {
    "void main(void ){\n"
        "gl_TexCoord[0] = gl_MultiTexCoord0;\n"
        "gl_Position = ftransform();\n"
    "}\n"
};
static char * fragShader = {
    "uniform sampler2D tex;\n"
    "void main(void){\n"
    "float x = gl_TexCoord[0].s;\n"
    "float y = gl_TexCoord[0].t;\n"
    "gl_FragColor= texture2D(tex, vec2(x, y));\n"
    "}\n"
};

static char * vertexShaderAntialiasing = {
    "void main(void ){\n"
        "gl_Position = ftransform();\n"
    "}\n"
};
static char * fragShaderAntialiasing = {
    "void main(void){\n"
    "gl_FragColor= vec4(1.0, 0.0, 0.0, 0.5s);\n"
    "}\n"
};

void MaterialManager::registration(int id, osg::ref_ptr<osg::StateSet> stateset)
{
    material_list_.insert(std::make_pair(id,  stateset))  ;
}

void MaterialManager::registrationAll()
{
    //Order of attention by lzj
    material_list_.insert(std::make_pair(texture_green    ,  getStateSetForPath(":/texture/vehicle_green.png")))     ;
    material_list_.insert(std::make_pair(texture_red      ,  getStateSetForPath(":/texture/vehicle_red.png")))       ;
    material_list_.insert(std::make_pair(texture_yellow   ,  getStateSetForPath(":/texture/vehicle_yellow.png")))    ;
   
    material_list_.insert(std::make_pair(state_transparent,  getTransparentStateSet()));
}

osg::ref_ptr<osg::StateSet> MaterialManager::getFrameLineStateSet()
{
      osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
      polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

      osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

      stateset->setAttribute( polygonMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

      return stateset;
}

osg::ref_ptr<osg::StateSet> MaterialManager::getTransparentStateSet()
{
     osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

     stateset->setRenderBinDetails(12, "RenderBin");
     stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

     stateset->setMode(GL_BLEND,osg::StateAttribute::ON);

     //stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

     return stateset;
}

osg::ref_ptr<osg::StateSet> MaterialManager::getPolygonAntialiasing()
{
    osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

    //stateset->setMode(GL_POLYGON_SMOOTH, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    stateset->setMode(GL_BLEND,osg::StateAttribute::ON);

    osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();

    blendFunc->setSource(osg::BlendFunc::ONE_MINUS_SRC_COLOR)     ;
 //   blendFunc->setDestination(osg::BlendFunc::ONE_MINUS_SRC_COLOR);

    stateset->setAttributeAndModes(blendFunc);

    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    return stateset;
}

osg::ref_ptr<osg::StateSet> MaterialManager::getLineAntialiasing()
{
    osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

    //stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    stateset->setMode(GL_BLEND,osg::StateAttribute::ON);

    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    return stateset;
}

osg::ref_ptr<osg::StateSet> MaterialManager::getStateSetForImage(osg::ref_ptr<osg::Image> image_red, bool transparent)
{
     osg::ref_ptr<osg::Texture2D> texture_red = new osg::Texture2D;

     texture_red->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
     texture_red->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);

     texture_red->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
     texture_red->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);

     texture_red->setImage(image_red);

     osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

     stateset->setTextureAttributeAndModes(0, texture_red, osg::StateAttribute::ON);

     osg::ref_ptr<osg::Program> program = new osg::Program;

     program->addShader(new osg::Shader(osg::Shader::FRAGMENT,fragShader));
     program->addShader(new osg::Shader(osg::Shader::VERTEX,vertexShader));

     stateset->setAttributeAndModes(program,osg::StateAttribute::ON);
     stateset->setRenderBinDetails(10, "RenderBin");

     if(transparent)
     {
         stateset->setMode(GL_BLEND,osg::StateAttribute::ON)       ;
         stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
     }

     return stateset;
}

osg::ref_ptr<osg::StateSet> MaterialManager::getStateSetForPath(std::string path, bool transparent)
{
    osg::ref_ptr<osg::Image> image_red = getImageFromPath(path);
    osg::ref_ptr<osg::StateSet> stateset =  getStateSetForImage(image_red);

    return stateset;
}

osg::ref_ptr<osg::StateSet> MaterialManager::getStateSetForIconId(int iconId, bool transparent)
{
    osg::ref_ptr<osg::Image> image_red = getImageFromIconId(iconId);
    osg::ref_ptr<osg::StateSet> stateset =  getStateSetForImage(image_red, transparent);

    return stateset;
}

osg::ref_ptr<osg::StateSet> MaterialManager::getMaterialForId(unsigned int Id, bool transparent)
{
    std::map< unsigned int , osg::ref_ptr<osg::StateSet> >::iterator it = material_list_.find(Id);

    if(it != material_list_.end())
    {
        return it->second;
    }

    osg::ref_ptr<osg::StateSet> staseset = getStateSetForIconId(Id, transparent);
    material_list_.insert(std::make_pair(Id    ,  staseset))     ;

    return staseset;
}




