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

#include <map>
#include <set>
#include <osg/Element>
#include "DataInputStream.h"
#include "DataOutputStream.h"

using namespace osg;

class ReaderWriterElement : public osgDB::ReaderWriter
{
public:
    ReaderWriterElement()
    {
        supportsExtension("bimele","OpenSceneGraph bimelement binary format");
		supportsOption("compressed","Export option, use zlib compression to compress the data in the .ive ");
		supportsOption("noTexturesInIVEFile","Export option");
		supportsOption("includeImageFileInIVEFile","Export option");
		supportsOption("compressImageData","Export option");
		supportsOption("inlineExternalReferencesInIVEFile","Export option");
		supportsOption("noWriteExternalReferenceFiles","Export option");
		supportsOption("useOriginalExternalReferences","Export option");
		supportsOption("TerrainMaximumErrorToSizeRatio=value","Export option that controls error matric used to determine terrain HeightField storage precision.");
		supportsOption("noLoadExternalReferenceFiles","Import option");
		supportsOption("OutputTextureFiles","Write out the texture images to file");
    }

    virtual const char* className() const { return "BimElement Reader/Writer"; }

	virtual ReadResult readNode(const std::string& file, const Options* options) const
	{
		std::string ext = osgDB::getLowerCaseFileExtension(file);
		if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

		std::string fileName = osgDB::findDataFile( file, options );
		if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;


		osgDB::ifstream istream(file.c_str(), std::ios::in | std::ios::binary);

		osg::Element* _element = readElementNode(istream,options);

		return _element; 
	}

	 osg::Element* readElementNode(std::istream& fin, const Options* options) const
	 {
		 element::DataInputStream in(&fin,options);

		 osg::Element* ele = new osg::Element;

		 ele->_elename = in.readString();
		 ele->_eleid = in.readInt();

		 //std::vector<osg::EdgeCollapse::CollapseResult> collapseresult;
		 int numberresult = in.readInt();
		 for (size_t i = 0 ; i< numberresult ; ++i)
		 {
			 //osg::EdgeCollapse::CollapseResult colresult = in.readCollapseResult();
			 ele->_collapse_result.push_back(in.readCollapseResult());
		 }

		// ele->_collapse_result = collapseresult;

		 return ele;
	 }



    virtual WriteResult writeElement(const osg::Element& ele,const std::string& fileName,const Options* options=NULL) const
    {
		if (!acceptsExtension(osgDB::getFileExtension(fileName)))
			return WriteResult(WriteResult::FILE_NOT_HANDLED);

		osgDB::ofstream fout(fileName.c_str(), std::ios::out | std::ios::binary);
		if (!fout) return WriteResult::ERROR_IN_WRITING_FILE;

		WriteResult result = writeElement(ele, fout, options);
		fout.close();

		return result;
    }

    virtual WriteResult writeElement(const Element& ele,std::ostream& fout,const osgDB::ReaderWriter::Options* options) const
    {
        // writing to a stream does not support materials
        element::DataOutputStream out(&fout,options);

		out.writeString(ele._elename);

		out.writeInt(ele._eleid);

		//std::vector<EdgeCollapse::CollapseResult> collapseresult = ele._collapse_result;
		int numberresult = ele._collapse_result.size();

		out.writeInt(numberresult);

		for(size_t i = 0 ; i< numberresult ; ++i)
		{
			//EdgeCollapse::CollapseResult colresult = collapseresult[i];
			out.writeCollapseResult(ele._collapse_result[i]);
		}

        return WriteResult(WriteResult::FILE_SAVED);
    }

protected:


     struct ElementOptionsStruct {
        bool rotate;
        bool noTesselateLargePolygons;
        bool noTriStripPolygons;
        bool generateFacetNormals;
        bool fixBlackMaterials;
        bool noReverseFaces;
        // This is the order in which the materials will be assigned to texture maps, unless
        // otherwise overriden
    };

    ElementOptionsStruct parseOptions(const Options* options) const;
};


// register with Registry to instantiate the above reader/writer.
REGISTER_OSGPLUGIN(bimele, ReaderWriterElement)

ReaderWriterElement::ElementOptionsStruct ReaderWriterElement::parseOptions(const osgDB::ReaderWriter::Options* options) const
{
    ElementOptionsStruct localOptions;
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
               
            }
        }
    }
    return localOptions;
}
