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

#include <osg/Geometry>

#include "DataOutputStream.h"
#include "DataInputStream.h"

#include "obj.h"

#include "Image.h"

#include <map>
#include <set>

#include <osgDB/WriteFile>

using namespace osg;
using namespace osgDB;

class ReaderWriterquadTree : public osgDB::ReaderWriter
{
public:
    ReaderWriterquadTree()
    {
        supportsExtension("QuadTreePageLod","Alias Wavefront quadTree format");
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
        return WriteResult(WriteResult::FILE_NOT_HANDLED);
    }


    virtual WriteResult writeObject(const osg::Object& obj,std::ostream& fout,const Options* options=NULL) const
    {
        const osg::Node* node = dynamic_cast<const osg::Node*>(&obj);
        if (node)
            return writeNode(*node, fout, options);
        else
            return WriteResult(WriteResult::FILE_NOT_HANDLED);
    }

	// 
    virtual WriteResult writeNode(const osg::Node& node,std::ostream& fout,const Options* options =NULL) const
    {
		ive::DataOutputStream out(&fout, options);

         return WriteResult(WriteResult::FILE_SAVED);
    }

	
	void writeFourthNodeT(const osgDB::FourthNode& node, ive::DataOutputStream& out) const
	{
		//写出文件列表
		out.writeInt(node.file_names.size());

		for (size_t i = 0; i < node.file_names.size(); i++)
		{
			out.writeString(node.file_names[i]);
		}

		out.writeBool(node.update_model_);

		out.writeString(node.block_name_);

		//if (node.group_)
		//{
		//	int num = node.group_->getNumChildren();
		//}

		//if (node.group_)
		//{
		//	std::string file_path = osgDB::getFilePath(node.block_name_) + std::string("/Data/") +osgDB::getSimpleFileName(node.block_name_);;
		//	osgDB::writeNodeFile(*node.group_, file_path);
		//}
		//重写

		int num = node.geodes_box_.size();

		out.writeInt(num);

		std::map<std::string, osg::BoundingBox>::const_iterator it = node.geodes_box_.begin();

		for (; it != node.geodes_box_.end(); it++)
		{
			out.writeString(it->first);

			out.writeFloat(it->second.xMin());
			out.writeFloat(it->second.yMin());
			out.writeFloat(it->second.zMin());

			out.writeFloat(it->second.xMax());
			out.writeFloat(it->second.yMax());
			out.writeFloat(it->second.zMax());
		}

		out.writeString(node.block_name_10_);

		if (node.node[0])
		{
			writeFourthNodeT(*node.node[0], out);
		}

		if (node.node[1])
		{
			writeFourthNodeT(*node.node[1], out);
		}

		if (node.node[2])
		{
			writeFourthNodeT(*node.node[2], out);
		}

		if (node.node[3])
		{
			writeFourthNodeT(*node.node[3], out);
		}
	}

	virtual WriteResult writeFourthNode(const osgDB::FourthNode& node,std::ostream& fout,const Options* options =NULL) const
	{
		ive::DataOutputStream out(&fout, options);

		//写出ID，文件名
		out.writeInt(node.max_num_);

		out.writeInt(node.total);

		out.writeFloat(node.box.xMin());
		out.writeFloat(node.box.yMin());
		out.writeFloat(node.box.zMin());

		out.writeFloat(node.box.xMax());
		out.writeFloat(node.box.yMax());
		out.writeFloat(node.box.zMax());

		//写出id_name

		out.writeInt(node.id_name.size());

		for (std::map<int, std::string>::const_iterator it = node.id_name.begin(); it != node.id_name.end(); it++)
		{
			out.writeInt(it->first);

			out.writeString(it->second);
		}

		writeFourthNodeT(node, out);

		return WriteResult(WriteResult::FILE_SAVED);
	}

	virtual WriteResult writeFourthNode(const osgDB::FourthNode &node, const std::string& fileName, const osgDB::ReaderWriter::Options *options) const
	{
		std::string ext = getFileExtension(fileName);
		if (!acceptsExtension(ext)) return WriteResult::FILE_NOT_HANDLED;


		osgDB::ofstream fout(fileName.c_str(), std::ios::out | std::ios::binary);

		
		if (!fout) return WriteResult::ERROR_IN_WRITING_FILE;

		WriteResult result = writeFourthNode(node, fout, options);
		fout.close();

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

inline osg::Vec3 ReaderWriterquadTree::transformVertex(const osg::Vec3& vec, const bool rotate) const
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

inline osg::Vec3 ReaderWriterquadTree::transformNormal(const osg::Vec3& vec, const bool rotate) const
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
REGISTER_OSGPLUGIN(quadTree, ReaderWriterquadTree)


ReaderWriterquadTree::ObjOptionsStruct ReaderWriterquadTree::parseOptions(const osgDB::ReaderWriter::Options* options) const
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

void readFourthNode(osgDB::FourthNode& node, ive::DataInputStream& in)
{
	size_t num = in.readInt();

	for (size_t i = 0; i < num; i++)
	{
		node.file_names.push_back(in.readString());
	}

	node.update_model_ = in.readBool();

	node.block_name_ = in.readString();

	//std::string file_path = osgDB::getFilePath(node.block_name_) + std::string("/Data/") +osgDB::getSimpleFileName(node.block_name_);;
	//node.group_ = (osg::Group*)osgDB::readNodeFile(file_path);

	//if (node.group_ && node.group_->getNumChildren())
	//{
	//	for (size_t i = 0; i < node.group_->getNumChildren(); i++)
	//	{
	////		node.group_->getChild(i)->getOrCreateUserDataContainer()->addUserObject(&node);
	//	}
	//}

	//读入文件名和保卫盒

	int num_name = in.readInt();

	for (int i = 0; i < num_name; i++)
	{
		std::string name = in.readString();

		float xmin = in.readFloat();
		float ymin = in.readFloat();
		float zmin = in.readFloat();

		float xmax = in.readFloat();
		float ymax = in.readFloat();
		float zmax = in.readFloat();

		osg::BoundingBox box;
		box.set(osg::Vec3d(xmin, ymin, zmin), osg::Vec3d(xmax, ymax, zmax));

		node.geodes_box_.insert(std::make_pair(name, box));
	}

	

	node.block_name_10_ = in.readString();


	if (node.node[0])
	{
		readFourthNode(*node.node[0], in);
	}

	if (node.node[1])
	{
		readFourthNode(*node.node[1], in);
	}

	if (node.node[2])
	{
		readFourthNode(*node.node[2], in);
	}

	if (node.node[3])
	{
		readFourthNode(*node.node[3], in);
	}
}

osgDB::ReaderWriter::ReadResult ReaderWriterquadTree::readNode(const std::string& file, const osgDB::ReaderWriter::Options* options) const
{
	std::string ext = osgDB::getLowerCaseFileExtension(file);
	if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

	std::string fileName = osgDB::findDataFile( file, options );
	if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;


	osgDB::ifstream istream(file.c_str(), std::ios::in | std::ios::binary);

	ive::DataInputStream in(&istream, options);

	osgDB::OctreeBuilder*  builder = new osgDB::OctreeBuilder();

	osgDB::FourthNode* node = new osgDB::FourthNode();

	node->max_num_ = in.readInt();

	int num = in.readInt();

	float xmin = in.readFloat();
	float ymin = in.readFloat();
	float zmin = in.readFloat();

	float xmax = in.readFloat();
	float ymax = in.readFloat();
	float zmax = in.readFloat();

	node->box.set(osg::Vec3d(xmin, ymin, zmin), osg::Vec3d(xmax, ymax, zmax));

	node->total = num;
	builder->build(node, num,osgDB::getFilePath(file));

	int id_name = in.readInt();

	for (size_t i = 0; i < id_name; i++)
	{
		node->id_name.insert(std::make_pair(in.readInt(), in.readString()));
	}

	
	readFourthNode(*node, in);

    return builder; 
}

osgDB::ReaderWriter::ReadResult ReaderWriterquadTree::readNode(std::istream& fin, const Options* options) const
{

	
   
    return ReadResult::FILE_NOT_HANDLED;
}
