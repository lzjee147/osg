
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

#include "OsgExTool/Tex2DDrawable.h"

using namespace OsgExTool;

 osg::BoundingBox OsgExTool::OsgExToolSet::GetBSForFiles( std::vector<std::string>& pathList)
 {
	 osg::BoundingBox box;

	 box.init();

	 if (osgDB::fileExists("D://projection//box.txt"))
	 {
		 std::ifstream ifs("D://projection//box.txt");

		 float xmin, ymin, zmin, xmax, ymax, zmax;

         ifs>>xmin>>ymin>>zmin>>xmax>>ymax>>zmax;

		 ifs.close();

		 box.set(xmin, ymin, zmin, xmax, ymax, zmax);

		 return box;
	 }

	 std::vector<std::string>::iterator it = pathList.begin();

	 for(int i = 0; it != pathList.end(); it++, i++)
	 {
		 osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(*it);

		 if (node)
		 {
		     node->IsCalBound(true);
		     node->dirtyBound()    ;
		    
		     osg::ComputeBoundsVisitor boundVisitor;
		    
		     node->accept(boundVisitor);
		    
		     box.expandBy( boundVisitor.getBoundingBox() );
		    
             DebugTool::DebugFunction::OutputDebugInfo(pathList.size());
		    
		     DebugTool::DebugFunction::OutputDebugInfo(i);
		 }
	 }


	 if (!osgDB::fileExists("D://projection//box.txt"))
	 {
		 std::ofstream out("D://projection//box.txt");

		 out<<box.xMin()<<" "<<box.yMin()<<" "<<box.zMin()<<std::endl; 

		 out<<box.xMax()<<" "<<box.yMax()<<" "<<box.zMax()<<std::endl; 

		 out.close();
	 }


	 return box;
 }

 osg::BoundingBox OsgExTool::OsgExToolSet::GetBSForDirectory( std::string path )
 {
	 std::vector<std::string> pathList;

	 BaseTool::BaseToolSet::visit(path, 0, pathList);

	 return GetBSForFiles(pathList);
 }

 osg::BoundingBox OsgExTool::OsgExToolSet::GetBSForNode(osg::ref_ptr<osg::Node> node)
 {
	 if (node)
	 {
		 node->IsCalBound(true);
		 node->dirtyBound()    ;

		 osg::ComputeBoundsVisitor boundVisitor;
		 node->accept(boundVisitor);
		
		 return boundVisitor.getBoundingBox();
	 }

	 return osg::BoundingBox();
 }

 void OsgExTool::OsgExToolSet::WriteLodFile()
 {
	 //读数据

	 float lod[10] = {1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1};

	 size_t num = 10;

	 //生成文件夹
	 std::string Direcotory("D://lod//");

	 for (size_t i = 0; i < num; i++)
	 {
	 	std::stringstream ss;

	 	ss<<i;

	 	std::string str;

	 	ss>>str;

	 	str = Direcotory + str;

	 	osgDB::makeDirectory(str);
	 }
 }


void OsgExTool::OsgExToolSet::CreateLod()   
{
	//生成lod
	std::string Direcotoryd("D://lod//lod");

	std::string Direcotory("D://lod//");

	osgDB::makeDirectory(Direcotoryd);

	for (size_t k = 0; k < 10; k ++)
	{
		std::stringstream ss;

		ss<<k;

		std::string str;

		ss>>str;

		std::string d =  Direcotory + str;

		std::string out_d = Direcotoryd + std::string("//") + str + std::string(".ive");

		OsgExTool::MergeGeometryVisitor::GetMergeGeometry(DebugFunction::GetGroupForDirectory(d), out_d);
	}
}


void OsgExToolSet::TransformGeometry(osg::Geometry& geometry)
{
	//改写成三角形
	osg::Vec3Array* vertice = (osg::Vec3Array*)(geometry.getVertexArray() );
	osg::Vec3Array* normal = (osg::Vec3Array*)(geometry.getNormalArray() ) ;

	osg::Vec2Array* texcoord = (osg::Vec2Array*)(geometry.getTexCoordArray(0));


	typedef struct strNVC
	{
		osg::Vec3 v;
		osg::Vec3 n;
		osg::Vec2 t;

	} NVC;

	std::vector<NVC> nvc_vec;

	osg::DrawElementsUInt* drawArrays = dynamic_cast<osg::DrawElementsUInt*> (geometry.getPrimitiveSet(0));

	for (size_t i = 0; i < drawArrays->size(); i++)
	{
		NVC nvc;

		nvc.v = vertice->at(drawArrays->at(i));

		nvc.n = normal->at(drawArrays->at(i));

		if (texcoord)
		{
		    nvc.t = texcoord->at(drawArrays->at(i));
		}

		nvc_vec.push_back(nvc);
	}

	vertice->clear();
	normal->clear() ;

	if (texcoord)
	{
		texcoord->clear()  ;
	}
	

	for (size_t i = 0; i < nvc_vec.size(); i++)
	{
		vertice->push_back(nvc_vec[i].v);

		normal->push_back(nvc_vec[i].n) ;

		if (texcoord)
		{
			texcoord->push_back(nvc_vec[i].t)  ;
		}
	}

	geometry.setPrimitiveSet(0,  new osg::DrawArrays(GL_TRIANGLES, 0, vertice->size()) );
}



//void Bim_Interactiv::AddMarks(const osgGA::GUIEventAdapter& event)
//{
//	osg::Vec3 currentpoint = GetPickPoint(event,root_group_);
//
//	std::wstring markstr = L"阿卡迪亚小区";
//	AddMark(currentpoint,markstr,markstr);
//}

//二维显示 ，没有测试过
osg::ref_ptr<osg::Geode> OsgExToolSet::AddMark(osg::Vec3 pos,std::wstring strname,std::wstring textstr)
{
	osg::ref_ptr<osg::Geode> geode_ = new osg::Geode;
	std::string fontdir = "\\font\\迷你简竹节.ttf";

	osg::ref_ptr<osgText::Text> texttest = new osgText::Text;
	texttest->setFont(osgText::readFontFile(fontdir));
	texttest->setText(textstr.c_str());
	texttest->setCharacterSize(32.0f);
	texttest->setAutoRotateToScreen(true);
	texttest->setColor(osg::Vec4(1.0,1.0,1.0,1.0));
	texttest->setPosition(pos);
 	texttest->setAlignment(osgText::Text::RIGHT_BOTTOM);
 	texttest->setAxisAlignment(osgText::Text::SCREEN);
	texttest->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);

	geode_->addDrawable(texttest);

	geode_->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	geode_->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	geode_->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geode_->getOrCreateStateSet()->setRenderBinDetails(1, "RenderBin");

	geode_->setName(osg::StringTools::WString2String(strname));
	
	return geode_;
};

osg::ref_ptr<osg::Geode> OsgExToolSet::CreateAxis(double clinder_height, double clinder_radius, double cone_height, double cone_radius)
{
	osg::ref_ptr<osg::Geode> axis = new osg::Geode();

	axis->setNodeMask(3);

	osg::TessellationHints *hins = new osg::TessellationHints;
	hins->setDetailRatio(1.0f); //设置圆柱的精度为0.1，值越小，精度越小
	
	{//x
	   osg::ref_ptr<osg::Cylinder> cyx = new osg::Cylinder;
	   osg::ref_ptr<osg::ShapeDrawable> sdx = new osg::ShapeDrawable(cyx);
	   cyx->setCenter(osg::Vec3(0,0,0));
	   cyx->setHeight(clinder_height);
	   cyx->setRadius(clinder_radius);
	   sdx->setColor(osg::Vec4(0.5, 0.0, 0.0, 1.0));
	   sdx->setTessellationHints(hins);

	   axis->addDrawable(sdx);
	}

	{//xcone
		osg::ref_ptr<osg::Cone> cyx = new osg::Cone;
		osg::ref_ptr<osg::ShapeDrawable> sdx = new osg::ShapeDrawable(cyx);
		cyx->setCenter(osg::Vec3(0,0,clinder_height * 0.5));
		cyx->setHeight(cone_height);
		cyx->setRadius(cone_radius);
		sdx->setColor(osg::Vec4(0.5, 0.0, 0.0, 1.0));
		sdx->setTessellationHints(hins);

		axis->addDrawable(sdx);
	}

	{//y
		osg::ref_ptr<osg::Cylinder> cyy = new osg::Cylinder;  
		osg::ref_ptr<osg::ShapeDrawable> sdy = new osg::ShapeDrawable(cyy);
		cyy->setCenter(osg::Vec3(0,0,0));
		cyy->setHeight(clinder_height);
		cyy->setRadius(clinder_radius);

		osg::Quat quat;
		quat.makeRotate(osg::PI_2, osg::Vec3(0, 1, 0));
		cyy->setRotation(quat);
		sdy->setColor(osg::Vec4(0.0, 0.5, 0.0, 1.0));
		sdy->setTessellationHints(hins);

		axis->addDrawable(sdy);
	}

	{//ycone
		osg::ref_ptr<osg::Cone> cyy = new osg::Cone;  
	    osg::ref_ptr<osg::ShapeDrawable> sdy = new osg::ShapeDrawable(cyy);
	    cyy->setCenter(osg::Vec3(clinder_height * 0.5, 0, 0));
	    cyy->setHeight(cone_height);
	    cyy->setRadius(cone_radius);

	    osg::Quat quat;
	    quat.makeRotate(osg::PI_2, osg::Vec3(0, 1, 0));
	    cyy->setRotation(quat);
	    sdy->setColor(osg::Vec4(0.0, 0.5, 0.0, 1.0));
	    sdy->setTessellationHints(hins);

		axis->addDrawable(sdy);
	}

	{//z
		osg::ref_ptr<osg::Cylinder> cyz = new osg::Cylinder;  
		osg::ref_ptr<osg::ShapeDrawable> sdz = new osg::ShapeDrawable(cyz);
		cyz->setCenter(osg::Vec3(0,0,0));
		cyz->setHeight(clinder_height);
		cyz->setRadius(clinder_radius);

		osg::Quat quat;
		quat.makeRotate(osg::PI_2, osg::Vec3(1, 0, 0));
		cyz->setRotation(quat);
		sdz->setColor(osg::Vec4(0.0, 0.0, 0.5, 1.0));
		sdz->setTessellationHints(hins);

		axis->addDrawable(sdz);
	}

	{//z cone
		osg::ref_ptr<osg::Cone> cyz = new osg::Cone;  
		osg::ref_ptr<osg::ShapeDrawable> sdz = new osg::ShapeDrawable(cyz);
		cyz->setCenter(osg::Vec3(0, clinder_height * 0.5,0));
		cyz->setHeight(cone_height);
		cyz->setRadius(cone_radius);

		osg::Quat quat;
		quat.makeRotate(-osg::PI_2, osg::Vec3(1, 0, 0));
		cyz->setRotation(quat);
		sdz->setColor(osg::Vec4(0.0, 0.0, 0.5, 1.0));
		sdz->setTessellationHints(hins);

		axis->addDrawable(sdz);
	}


	return axis;
}

//
osg::ref_ptr<osg::Geode> OsgExTool::OsgExToolSet::createAxisPlane(double width)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	
	width *= 0.5;

	double color = 1.0;
	double alpha = 0.5;

	{
	  osg::Vec3d v1(width, width, 0)  ;
	  osg::Vec3d v2(width, -width, 0) ;
	  osg::Vec3d v3(-width, -width, 0);
	  osg::Vec3d v4(-width, width, 0) ;
	  geode->addChild( CreateQuad(v1, v2, v3, v4, osg::Vec4(color, 0.0, 0.0, alpha)) );
	}

	{
		osg::Vec3d v1(width, 0, width)  ;
		osg::Vec3d v2(width, 0, -width) ;
		osg::Vec3d v3(-width,0,  -width);
		osg::Vec3d v4(-width,0, width)  ;
		geode->addChild( CreateQuad(v1, v2, v3, v4, osg::Vec4(0.0, color, 0.0, alpha)) );
	}

	{
		osg::Vec3d v1(0, width, width)  ;
		osg::Vec3d v2(0, width, -width) ;
		osg::Vec3d v3(0, -width, -width);
		osg::Vec3d v4(0, -width, width) ;
		geode->addChild( CreateQuad(v1, v2, v3, v4, osg::Vec4(0.0, 0.0, color, alpha)) );
	}

	geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	return geode;
}

osg::ref_ptr<osg::Geode> OsgExToolSet::CreateLine(osg::Vec3d f, osg::Vec3d s)
{
     osg::Geode *geode = new osg::Geode();

	 osg::ref_ptr<osg::Vec3Array> vecarry1 = new osg::Vec3Array();

	 osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	 {
		vecarry1->push_back(f);
	    vecarry1->push_back(s);

		geometry->setVertexArray(vecarry1.get());
		geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vecarry1->size()));
	 }

	 geode->addDrawable(geometry.get());

	 return geode;
}


osg::Group* OsgExToolSet::createSunLight(osg::Node* node)
{
	osg::LightSource* sunLightSource = new osg::LightSource;

	osg::Light* sunLight = sunLightSource->getLight();

	sunLight->setLightNum(0);
	sunLight->setAmbient(osg::Vec4(0.6f,0.6f,0.6f,1.0f)) ;
	sunLight->setDiffuse(osg::Vec4(0.6f,0.6f,0.6f,1.0f)) ;
	sunLight->setSpecular(osg::Vec4(0.6f,0.6f,0.6f,1.0f));

	//影响包围盒的位置,直接放中心，以后改进
	osg::Vec3f center = node->getBound().center();
	osg::Vec3f pos = center; 
	sunLight->setPosition(osg::Vec4f(pos.x(), pos.y(), pos.z(), 1));

   // sunLight->setDirection(osg::Vec3(1, 1, 1));
	
	sunLightSource->setLight( sunLight );
	sunLightSource->setLocalStateSetModes( osg::StateAttribute::ON );
	sunLightSource->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);

	osg::LightModel* lightModel = new osg::LightModel;

	lightModel->setAmbientIntensity(osg::Vec4(0.6f,0.6f,0.6f,1.0f));

	sunLightSource->getOrCreateStateSet()->setAttribute(lightModel);

	sunLightSource->addChild(node);

	return sunLightSource;
}


void OsgExToolSet::create_specular_highlights(osg::Node *node)
{
	osg::StateSet *ss = node->getOrCreateStateSet();

	// create and setup the texture object
	osg::TextureCubeMap *tcm = new osg::TextureCubeMap;
	tcm->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP);
	tcm->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP);
	tcm->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP);
	tcm->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	tcm->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);    

	// generate the six highlight map images (light direction = [1, 1, -1])
	osgUtil::HighlightMapGenerator *mapgen = new osgUtil::HighlightMapGenerator(
		osg::Vec3(1, 1, -1),            // light direction
		osg::Vec4(1, 0.9f, 0.8f, 1),    // light color
		8);                             // specular exponent

	mapgen->generateMap();

	// assign the six images to the texture object
	tcm->setImage(osg::TextureCubeMap::POSITIVE_X, mapgen->getImage(osg::TextureCubeMap::POSITIVE_X));
	tcm->setImage(osg::TextureCubeMap::NEGATIVE_X, mapgen->getImage(osg::TextureCubeMap::NEGATIVE_X));
	tcm->setImage(osg::TextureCubeMap::POSITIVE_Y, mapgen->getImage(osg::TextureCubeMap::POSITIVE_Y));
	tcm->setImage(osg::TextureCubeMap::NEGATIVE_Y, mapgen->getImage(osg::TextureCubeMap::NEGATIVE_Y));
	tcm->setImage(osg::TextureCubeMap::POSITIVE_Z, mapgen->getImage(osg::TextureCubeMap::POSITIVE_Z));
	tcm->setImage(osg::TextureCubeMap::NEGATIVE_Z, mapgen->getImage(osg::TextureCubeMap::NEGATIVE_Z));

	// enable texturing, replacing any textures in the subgraphs
	ss->setTextureAttributeAndModes(0, tcm, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

	// texture coordinate generation
	osg::TexGen *tg = new osg::TexGen;
	tg->setMode(osg::TexGen::REFLECTION_MAP);
	ss->setTextureAttributeAndModes(0, tg, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

	// use TexEnvCombine to add the highlights to the original lighting
	osg::TexEnvCombine *te = new osg::TexEnvCombine;    
	te->setCombine_RGB(osg::TexEnvCombine::ADD);
	te->setSource0_RGB(osg::TexEnvCombine::TEXTURE);
	te->setOperand0_RGB(osg::TexEnvCombine::SRC_COLOR);
	te->setSource1_RGB(osg::TexEnvCombine::PRIMARY_COLOR);
	te->setOperand1_RGB(osg::TexEnvCombine::SRC_COLOR);
	ss->setTextureAttributeAndModes(0, te, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
}


osg::ref_ptr<osg::Image> getNormalImage()
{
	osg::Vec3 origin(0.0f, 0.0f, 0.0f);

	unsigned int numColumns = 40;
	unsigned int numRows = 40   ;

	unsigned int r;
	unsigned int c;

	osg::HeightField* grid = new osg::HeightField;
	grid->allocate(numColumns, numRows);
	grid->setOrigin(origin); 

	grid->setXInterval(1.0);
	grid->setYInterval(1.0);

	for(r = 0; r < numRows; ++r)
	{
		for(c = 0; c < numColumns;++c)
		{
			grid->setHeight(c, r, rand() % 100);
		}
	}
	
	unsigned char* data = new unsigned char[24 * numRows * numColumns];

	for(r = 0; r < numRows; ++r)
	{
		for(c = 0; c < numColumns;++c)
		{
			osg::Vec3f n = grid->getNormal(c, r);

			data[3 * (r * numColumns + c)] = (n[0] + 1) * 0.5;
			data[3 * (r * numColumns + c)] = (n[1] + 1) * 0.5;
			data[3 * (r * numColumns + c)] = (n[2] + 1) * 0.5;
		}
	}

	//

	osg::ref_ptr<osg::Image> image_ = new osg::Image();
	image_->setImage(numColumns, numRows, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data, osg::Image::USE_NEW_DELETE, 1);

	delete  [] data;

	return image_;
}

osg::ref_ptr<osg::Geode> OsgExToolSet::createHeightField()
{
	osg::Vec3 origin(0.0f, 0.0f, 0.0f);

	unsigned int numColumns = 40;
	unsigned int numRows = 40   ;

	unsigned int r;
	unsigned int c;

    osg::HeightField* grid = new osg::HeightField;
	grid->allocate(numColumns, numRows);
	grid->setOrigin(origin); 

	grid->setXInterval(1.0);
	grid->setYInterval(1.0);

	for(r = 0; r < numRows; ++r)
	{
		for(c = 0; c < numColumns;++c)
		{
			grid->setHeight(c, r, rand() % 5);
		}
	}

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(new osg::ShapeDrawable(grid)) ;

	return geode;
}

osg::ref_ptr<osg::StateSet> OsgExTool::OsgExToolSet::GetLighting()
{
	char vertexShaderSource[] =
		"varying vec3 normal;\n"
		"uniform vec3 dir;\n"
		"uniform vec4 diffuse;\n"
		"uniform vec3 hf;\n"
		"void main(void)\n"
		"{\n"
		"    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
		"    normal = normalize(gl_NormalMatrix * gl_Normal);\n"
		"    dir = normalize(vec3(gl_LightSource[0].position));\n"
		"    diffuse = gl_LightSource[0].diffuse;\n"
		"    hf = normalize(gl_LightSource[0].halfVector.xyz);\n"
		"}\n";

	char fragmentShaderSource[] =
		"uniform sampler2D baseTexture;\n"
		"varying vec3 normal\n"
		"uniform vec3 dir;\n"
		"uniform vec4 diffuse;\n"
		"void main(void)\n"
		"{\n"
		"    vec2 v = gl_TexCoord[0].st; \n"
		"    vec3 n = normalize(normal); \n"
		"    float NdotL = max( dot(n, dir), 0);\n"
		"    vec4 color = diffuse * dir * NdotL;\n"
		"    float Hv = max( dot(n, hf), 0);\n"
		"    color += gl_LightSource[0].specular * pow(Hv, 100);\n"
		"    gl_FragColor = color * texture2D(baseTexture, v) + vec4(0.5, 0.5, 0.5, 1.0); \n"
		"}\n";

	osg::ref_ptr<osg::Program> program = new osg::Program;

	program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShaderSource));
	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSource));
	program->setName( "demo" );


	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

	//osg::Uniform* baseTextureSampler = new osg::Uniform("baseTexture",0);
	//stateset->addUniform(baseTextureSampler);

	stateset->setAttribute(program);

	return stateset;
}


void OsgExToolSet::GetFileShader(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::Program> program = new osg::Program;
	osg::ref_ptr<osg::Shader>  vertShader = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader>  fragShader = new osg::Shader(osg::Shader::FRAGMENT);

	program->addShader(vertShader);
	program->addShader(fragShader);

	if(!vertShader->loadShaderSourceFromFile("D:\\light.vert"))
	{
		printf("load vertex shader error !\n");
	}
	if(!fragShader->loadShaderSourceFromFile("D:\\light.frag"))
	{
		printf("load fragment shader error !\n");
	}

	node->getOrCreateStateSet()->setAttribute(program, osg::StateAttribute::ON);
}

osg::ref_ptr<osg::Geometry> OsgExToolSet::CreateQuad(osg::Vec3d v1, osg::Vec3d v2, osg::Vec3d v3, osg::Vec3f v4, osg::Vec4 color)
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;  

	osg::ref_ptr<osg::Vec3Array> va = new osg::Vec3Array()   ; 
	osg::ref_ptr<osg::Vec4Array> colorvec = new osg::Vec4Array();

	colorvec->push_back(color);

	va->push_back(v1);
	va->push_back(v2);
	va->push_back(v3);
	va->push_back(v4);

	geom->setVertexArray( va.get() ); 
	geom->setColorArray(colorvec);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::ref_ptr<osg::DrawElementsUInt> quads = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

	quads->push_back(0);
	quads->push_back(1);
	quads->push_back(2);
	quads->push_back(3);

	geom->addPrimitiveSet(quads.get());

	return geom;
}

osg::ref_ptr<osg::Geode> OsgExToolSet::CreateQuadGeode(osg::Vec3d v1, osg::Vec3d v2, osg::Vec3d v3, osg::Vec3f v4, osg::Vec4 color)
{
	 osg::ref_ptr<osg::Geode> geode = new osg::Geode();

	 geode->addChild(CreateQuad(v1, v2, v3, v4, color));

	 return geode;
}

osg::ref_ptr<osg::MatrixTransform> OsgExToolSet::createMatrixNode(osg::ref_ptr<osg::Node> node, osg::Vec3f start, osg::Vec3f end, osg::Vec3f pos)
{
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();

	osg::Matrixf trs;
	trs.makeIdentity();

	trs.makeTranslate(pos);

	osg::Matrixf rot;
	rot.makeRotate(start, end);

	osg::Matrixf mat = rot * trs;

	mt->setMatrix(mat);
	mt->addChild(node);

	return mt;
}

osg::ref_ptr<osg::Geode> OsgExToolSet::createTexText(const osg::Vec3& pos,float radius)
{
	osg::ref_ptr<osg::Geode> geode  = new osg::Geode;

	osg::ref_ptr<osgText::Text> text = new osgText::Text;

	text->setFont("D://xxx.ttf");
	text->setPosition(pos);
	text->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);
	text->setAxisAlignment(osgText::Text::XY_PLANE);
	text->setText(L"阿卡迪亚小区");

	geode->addDrawable(text);

	osg::Material* front = new osg::Material;
	front->setAlpha(osg::Material::FRONT_AND_BACK,1);
	front->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.2,0.2,0.2,1.0));
	front->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(.0,.0,1.0,1.0));
	geode->getOrCreateStateSet()->setAttributeAndModes(front);

	return geode;    
}

//读入文字，直接放入大纹理
//生成多个四边形，一个四边形对应一个纹理
osg::ref_ptr<osg::Geode> OsgExToolSet::createBigTexText(const osg::Vec3& pos,float radius)
{
	osg::ref_ptr<osg::Geode> geode  = new osg::Geode;

	osg::ref_ptr<osgText::BigTextureText> text = new osgText::BigTextureText;

	//加载字库
	text->setFont("D://xxx.ttf");

	//text->s


	text->setPosition(pos);
	text->setDrawMode(osgText::BigTextureText::TEXT | osgText::BigTextureText::BOUNDINGBOX);
	text->setAxisAlignment(osgText::BigTextureText::XY_PLANE);
	text->addText(L"阿卡迪亚小区");

	geode->addDrawable(text);

	osg::Material* front = new osg::Material;
	front->setAlpha(osg::Material::FRONT_AND_BACK,1);
	front->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.2,0.2,0.2,1.0));
	front->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(.0,.0,1.0,1.0));
	geode->getOrCreateStateSet()->setAttributeAndModes(front);

	return text->createBigGeode();    
}

osg::ref_ptr<osg::Geode> OsgExToolSet::create3DText(const osg::Vec3& pos,float radius)
{
	osg::ref_ptr<osg::Geode> geode  = new osg::Geode;

	osg::ref_ptr<osgText::Text3D> text = new osgText::Text3D;

	text->setFont("D://xxx.ttf");
//	text1->setCharacterSize(characterSize);
//	text1->setCharacterDepth(characterDepth);
	text->setPosition(pos);
	text->setDrawMode(osgText::Text3D::TEXT | osgText::Text3D::BOUNDINGBOX);
	text->setAxisAlignment(osgText::Text3D::XY_PLANE);
	text->setText(L"阿卡迪亚小区");

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


osg::ref_ptr<osg::Geode> OsgExToolSet::createBoxsGeode(std::vector<osg::Vec3f>& points, double width, double height, osg::Vec4 color)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

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

		//
		osg::Matrixf rot;
		rot.makeIdentity();
		rot.makeRotate(osg::Vec3f(0, 0, 1), n);

		osg::Quat quat = rot.getRotate();

		double zWidth = (f - s).length();

		osg::Vec3f center = (f + s) * 0.5 + osg::Vec3f(0, height * 0.5, 0);

		osg::ref_ptr<osg::Box> box = new osg::Box(osg::Vec3f(center[0], -center[1], center[2]), width, 2.0, zWidth );
		osg::ref_ptr<osg::ShapeDrawable> sp = new osg::ShapeDrawable(box.get());
		sp->setColor(color);

		box->setRotation(quat);

		geode->addDrawable(sp.get());
	}

	geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	return geode;
}

//二维图形
osg::ref_ptr<osg::Projection> OsgExToolSet::createNormalText()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	osg::ref_ptr<osg::MatrixTransform> modeview = new osg::MatrixTransform();

	osg::ref_ptr<Tex2DDrawable> tex2d = new Tex2DDrawable();

	geode->addChild(tex2d);

	modeview->addChild(geode);

	modeview->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	modeview->setMatrix(osg::Matrix::identity());
	modeview->addChild(geode);

	osg::ref_ptr<osg::Projection> projection = new osg::Projection;

	projection->setMatrix(osg::Matrix::ortho2D(0, 800, 0, 480));
	projection->addChild(modeview);

	return projection;
}


osg::ref_ptr<osg::Geode> OsgExToolSet::createPixelText()
{
	osg::ref_ptr<osg::Geode> pGeode = new osg::Geode();
	osg::ref_ptr<osgText::Text> pText = 0;

	
	pText = new osgText::Text();

	pText->setColor(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
	pText->setPosition(osg::Vec3f(0, 0, 0));
	pText->setAxisAlignment(osgText::Text::XZ_PLANE);

	pText->setCharacterSize(20.0);

	pText->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
	pText->setFontResolution(32, 32);
	pText->setAlignment(osgText::Text::LEFT_CENTER);

	pText->setLayout(osgText::Text::LEFT_TO_RIGHT);
	pText->setBackdropType(osgText::Text::NONE);

	pText->setColorGradientMode(osgText::Text::SOLID);

	//QString version = "V1.16.0";
	//QString text = QString("OsgDemo %1(作者:红模仿 QQ21497936 博客地址: blog.csdn.net/qq21497936)").arg(version);
	//pText->setText(text.toStdString(), osgText::String::ENCODING_UTF8);

	pText->setText("xxxx");
	//pText->setFont(osgText::readFontFile("fonts/simsun.ttc"));
	pText->setAxisAlignment(osgText::Text::SCREEN);

	pGeode->addDrawable(pText.get());

	return pGeode;
}


osg::ref_ptr<osg::Geode> OsgExToolSet::FontGeode()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();

	return geode;
}

osg::ref_ptr<osg::Camera> OsgExToolSet::RTTCamera()
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;

	camera->setViewport(0, 0, 256, 256);

	osg::Image* img = new osg::Image;
	img->allocateImage(256, 256, 1, GL_RGBA, GL_FLOAT);

	osg::Texture2D* texture2D = new osg::Texture2D;


	texture2D->setTextureSize(256, 256);
	texture2D->setInternalFormat(GL_RGBA);

	camera->attach(osg::Camera::COLOR_BUFFER, img, 0, 0);
	texture2D->setImage(0, img);

//	camera->setProjectionMatrixAsFrustum(-proj_right,proj_right,-proj_top,proj_top,znear,zfar);
//  
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
//	camera->setViewMatrixAsLookAt(bs.center()-osg::Vec3(0.0f,2.0f,0.0f)*bs.radius(),bs.center(),osg::Vec3(0.0f,0.0f,1.0f));

	camera->setRenderOrder(osg::Camera::PRE_RENDER);
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

	//camera->addChild(node);
  
	return camera;
}

