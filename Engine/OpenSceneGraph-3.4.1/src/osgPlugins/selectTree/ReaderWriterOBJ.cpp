// -*-c++-*-

/*
 * Wavefront OBJ loader for Open Scene Graph
 *
 * Copyright (C) 2001,2007 Ulrich Hertlein <u.hertlein@sandbox.de>
 *
 * Modified by Robert Osfield to support per Drawable coord, normal and
 * texture coord arrays, bug fixes, and support for texture mapping.
 *
 * Writing support added 2007 by Stephan Huber, http://digitalmind.de,
 * some ideas taken from the dae-plugin
 *
 * The Open Scene Graph (OSG) is a cross platform C++/OpenGL library for
 * real-time rendering of large 3D photo-realistic models.
 * The OSG homepage is http://www.openscenegraph.org/
 */

#if defined(_MSC_VER)
    #pragma warning( disable : 4786 )
#endif

#include <stdlib.h>
#include <string>

#include <osg/Notify>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osgGe/Vec3f>

#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <osg/TexMat>

#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <osgUtil/TriStripVisitor>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/Tessellator>
#include <osgUtil/OctreeBuilder>
#include <osg/Geometry>

#include "RBMWriterNodeVisitor.h"
#include "RBMDrawable.h"

#include "DataOutputStream.h"
#include "DataInputStream.h"

#include "RBMDrawable.h"

#include "obj.h"

#include "Image.h"


#include <map>
#include <set>

class ReaderWriterOBJ : public osgDB::ReaderWriter
{
public:
    ReaderWriterOBJ()
    {
        supportsExtension("obj","Alias Wavefront OBJ format");
        supportsOption("noRotation","Do not do the default rotate about X axis");
        supportsOption("noTesselateLargePolygons","Do not do the default tesselation of large polygons");
        supportsOption("noTriStripPolygons","Do not do the default tri stripping of polygons");
        supportsOption("generateFacetNormals","generate facet normals for verticies without normals");
        supportsOption("noReverseFaces","avoid to reverse faces when normals and triangles orientation are reversed");

        supportsOption("DIFFUSE=<unit>", "Set texture unit for diffuse texture");
        supportsOption("AMBIENT=<unit>", "Set texture unit for ambient texture");
        supportsOption("SPECULAR=<unit>", "Set texture unit for specular texture");
        supportsOption("SPECULAR_EXPONENT=<unit>", "Set texture unit for specular exponent texture");
        supportsOption("OPACITY=<unit>", "Set texture unit for opacity/dissolve texture");
        supportsOption("BUMP=<unit>", "Set texture unit for bumpmap texture");
        supportsOption("DISPLACEMENT=<unit>", "Set texture unit for displacement texture");
        supportsOption("REFLECTION=<unit>", "Set texture unit for reflection texture");

    }

    virtual const char* className() const { return "Wavefront OBJ Reader"; }

    virtual ReadResult readNode(const std::string& fileName, const osgDB::ReaderWriter::Options* options) const;

    virtual ReadResult readNode(std::istream& fin, const Options* options) const;

    virtual WriteResult writeObject(const osg::Object& obj,const std::string& fileName,const Options* options=NULL) const
    {
        const osg::Node* node = dynamic_cast<const osg::Node*>(&obj);
        if (node)
            return writeNode(*node, fileName, options);
        else
            return WriteResult(WriteResult::FILE_NOT_HANDLED);
    }

	//输
    virtual WriteResult writeNode(const osg::Node& node,const std::string& fileName,const Options* options =NULL) const
    {
		//WriteGeometry writeGeometry(fileName);

		//osg::Node* nodee = const_cast<osg::Node*>(&node);
		//nodee->accept(writeGeometry);


		// code for setting up the database path so that internally referenced file are searched for on relative paths.
		osg::ref_ptr<Options> local_opt = options ? static_cast<Options*>(options->clone(osg::CopyOp::SHALLOW_COPY)) : new Options;
		if(local_opt->getDatabasePathList().empty())
			local_opt->setDatabasePath(osgDB::getFilePath(fileName));

		local_opt->setPluginStringData("filename",fileName);

		osgDB::ofstream fout(fileName.c_str(), std::ios::out | std::ios::binary);

		if (!fout) return WriteResult::ERROR_IN_WRITING_FILE;

		WriteResult result = writeNode(node, fout, local_opt.get());
		fout.close();

        return WriteResult(WriteResult::FILE_SAVED);
    }


    virtual WriteResult writeObject(const osg::Object& obj,std::ostream& fout,const Options* options=NULL) const
    {
        const osg::Node* node = dynamic_cast<const osg::Node*>(&obj);
        if (node)
            return writeNode(*node, fout, options);
        else
            return WriteResult(WriteResult::FILE_NOT_HANDLED);
    }

	//写数据
    virtual WriteResult writeNode(const osg::Node& node,std::ostream& fout,const Options* options =NULL) const
    {
		ive::DataOutputStream out(&fout, options);

		const osg::Geode* geode = dynamic_cast<const osg::Geode*>(&node);

		deprecated_osg::Geometry* geometry = (deprecated_osg::Geometry*)geode->getDrawable(0);
		//osg::Geometry* geometry = (osg::Geometry*)geode->getDrawable(0);

		//第二版本
	    osg::ref_ptr<osg::Vec3Array> box = new osg::Vec3Array(2);

		osg::BoundingBoxf box_ = geometry->getBoundingBox();

		box->push_back(osg::Vec3f(box_.xMin(), box_.yMin(), box_.zMin()));
		box->push_back(osg::Vec3f(box_.xMax(), box_.yMax(), box_.zMax()));

		out.writeArray(box);

	
		out.writeArray(geometry->getVertexArray());
		
		out.writeBinding(geometry->getNormalBinding());

		out.writeArray(geometry->getNormalArray());

		out.writeBinding(geometry->getColorBinding());
		out.writeArray(geometry->getColorArray());

		out.writeArray(geometry->getTexCoordArray(0));

       return WriteResult(WriteResult::FILE_SAVED);
    }


	
protected:

     struct ObjOptionsStruct {
        bool rotate;
        bool noTesselateLargePolygons;
        bool noTriStripPolygons;
        bool generateFacetNormals;
        bool fixBlackMaterials;
        bool noReverseFaces;
        // This is the order in which the materials will be assigned to texture maps, unless
        // otherwise overriden
        typedef std::vector< std::pair<int,obj::Material::Map::TextureMapType> > TextureAllocationMap;
        TextureAllocationMap textureUnitAllocation;
    };

    typedef std::map< std::string, osg::ref_ptr<osg::StateSet> > MaterialToStateSetMap;


    inline osg::Vec3 transformVertex(const osg::Vec3& vec, const bool rotate) const ;
    inline osg::Vec3 transformNormal(const osg::Vec3& vec, const bool rotate) const ;

    ObjOptionsStruct parseOptions(const Options* options) const;


};

inline osg::Vec3 ReaderWriterOBJ::transformVertex(const osg::Vec3& vec, const bool rotate) const
{
    if(rotate==true)
    {
        return osg::Vec3(vec.x(),-vec.z(),vec.y());
    }
    else
    {
        return vec;
    }
}

inline osg::Vec3 ReaderWriterOBJ::transformNormal(const osg::Vec3& vec, const bool rotate) const
{
    if(rotate==true)
    {
        return osg::Vec3(vec.x(),-vec.z(),vec.y());
    }
    else
    {
        return vec;
    }
}


// register with Registry to instantiate the above reader/writer.
REGISTER_OSGPLUGIN(obj, ReaderWriterOBJ)


ReaderWriterOBJ::ObjOptionsStruct ReaderWriterOBJ::parseOptions(const osgDB::ReaderWriter::Options* options) const
{
    ObjOptionsStruct localOptions;
    localOptions.rotate = true;
    localOptions.noTesselateLargePolygons = false;
    localOptions.noTriStripPolygons = false;
    localOptions.generateFacetNormals = false;
    localOptions.fixBlackMaterials = true;
    localOptions.noReverseFaces = false;

    if (options!=NULL)
    {
        std::istringstream iss(options->getOptionString());
        std::string opt;
        while (iss >> opt)
        {
            // split opt into pre= and post=
            std::string pre_equals;
            std::string post_equals;

            size_t found = opt.find("=");
            if(found!=std::string::npos)
            {
                pre_equals = opt.substr(0,found);
                post_equals = opt.substr(found+1);
            }
            else
            {
                pre_equals = opt;
            }

            if (pre_equals == "noRotation")
            {
                localOptions.rotate = false;
            }
            else if (pre_equals == "noTesselateLargePolygons")
            {
                localOptions.noTesselateLargePolygons = true;
            }
            else if (pre_equals == "noTriStripPolygons")
            {
                localOptions.noTriStripPolygons = true;
            }
            else if (pre_equals == "generateFacetNormals")
            {
                localOptions.generateFacetNormals = true;
            }
            else if (pre_equals == "noReverseFaces")
            {
                localOptions.noReverseFaces = true;
            }
            else if (post_equals.length()>0)
            {
                obj::Material::Map::TextureMapType type = obj::Material::Map::UNKNOWN;
                // Now we check to see if we have anything forcing a texture allocation
                if        (pre_equals == "DIFFUSE")            type = obj::Material::Map::DIFFUSE;
                else if (pre_equals == "AMBIENT")            type = obj::Material::Map::AMBIENT;
                else if (pre_equals == "SPECULAR")            type = obj::Material::Map::SPECULAR;
                else if (pre_equals == "SPECULAR_EXPONENT") type = obj::Material::Map::SPECULAR_EXPONENT;
                else if (pre_equals == "OPACITY")            type = obj::Material::Map::OPACITY;
                else if (pre_equals == "BUMP")                type = obj::Material::Map::BUMP;
                else if (pre_equals == "DISPLACEMENT")        type = obj::Material::Map::DISPLACEMENT;
                else if (pre_equals == "REFLECTION")        type = obj::Material::Map::REFLECTION;

                if (type!=obj::Material::Map::UNKNOWN)
                {
                    int unit = atoi(post_equals.c_str());    // (probably should use istringstream rather than atoi)
                    localOptions.textureUnitAllocation.push_back(std::make_pair(unit,(obj::Material::Map::TextureMapType) type));
                    OSG_NOTICE<<"Obj Found map in options, ["<<pre_equals<<"]="<<unit<<std::endl;
                }
            }
        }
    }
    return localOptions;
}


osg::Program* createShader()
{
	osg::Program* pgm = new osg::Program;
	pgm->setName( "osgshader2 demo" );

	pgm->addShader(osg::Shader::readShaderFile(osg::Shader::VERTEX, osgDB::findDataFile("D://lod.vert")));
	pgm->addShader(osg::Shader::readShaderFile(osg::Shader::FRAGMENT, osgDB::findDataFile("D://lod.frag")));

	return pgm;
}

osgDB::ReaderWriter::ReadResult ReaderWriterOBJ::readNode(const std::string& file, const osgDB::ReaderWriter::Options* options) const
{

	osgDB::ifstream istream(file.c_str(), std::ios::in | std::ios::binary);

	ive::DataInputStream in(&istream, options);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();


	//RBMDrawable* geometry = new RBMDrawable();
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

	osg::ref_ptr<osg::Vec3Array> box = (osg::Vec3Array*)in.readArray();
	
	geometry->setVertexArray(in.readArray());

	deprecated_osg::Geometry::AttributeBinding binding = in.readBinding();

	geometry->setNormalArray(in.readArray());
	geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);


	binding = in.readBinding();
	geometry->setColorArray(in.readArray());
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	geometry->setTexCoordArray(0, in.readArray(), osg::Array::BIND_PER_VERTEX);

	////严重影响速度，这几行代码
	geometry->setUseVertexBufferObjects(true);
	geometry->setSupportsDisplayList(false);
	geometry->setUseDisplayList(false);

	osg::Vec3Array* va = (osg::Vec3Array*)geometry->getVertexArray();
	geometry->addPrimitiveSet( new osg::DrawArrays(GL_TRIANGLES, 0, va->size()) );

	geode->addDrawable(geometry);

	osg::Vec3f center = ( box->at(0) + box->at(1) ) * 0.5;

	float radius = ( box->at(0) - box->at(1) ).length() * 0.5;

	geode->setInitialBound(osg::BoundingSphere(center, radius));

	geode->IsCalBound(true);


    return geode; 
}

osgDB::ReaderWriter::ReadResult ReaderWriterOBJ::readNode(std::istream& fin, const Options* options) const
{

	
   
    return ReadResult::FILE_NOT_HANDLED;
}
