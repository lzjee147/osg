// -*-c++-*-

/*
 * Wavefront OBJ loader for Open Scene Graph
 *
 * Copyright (C) 2001 Ulrich Hertlein <u.hertlein@web.de>
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

 #ifndef RBMWriterNodeVisitor_
 #define RBMWriterNodeVisitor_


#include <string>
#include <stack>
#include <sstream>

#include <osg/Notify>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>

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

#include <map>
#include <set>


class WriteGeometry : public osg::NodeVisitor
{
public:

	std::string path_file_;

	WriteGeometry(std::string path) : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
	{
		path_file_ = path;
	}

	//���������ֵ���������Χ��
	virtual void apply(osg::Geometry& geo)
	{
		osg::Vec3Array* vertices = (osg::Vec3Array*)geo.getVertexArray();


		osg::BoundingBoxf box;

		float* vertexData = new float[vertices->size() * 3 + 6 + 1];

		vertexData[0] = vertices->size() * 3;

		for (osg::Vec3Array::size_type i = 0;i < vertices->size();i++)
		{
			float x = vertices->at(i).x() ;
			float y = vertices->at(i).y() ;
			float z = vertices->at(i).z() ; 

			box.expandBy(vertices->at(i));

			vertexData[3*i + 1]     = x;
			vertexData[3*i + 1 + 1] = y;
			vertexData[3*i +2 + 1]  = z;
	    }

		float x_min = box.xMin();
		float y_min = box.yMin();
		float z_min = box.zMin();

		float x_max = box.xMax();
		float y_max = box.yMax();
		float z_max = box.zMax();
		
		vertexData[vertices->size() * 3 + 1]     = x_min;
		vertexData[vertices->size() * 3 + 1 + 1] = y_min;
		vertexData[vertices->size() * 3 + 2 + 1 ] = z_min;

		vertexData[vertices->size() * 3 + 3 + 1] = x_max;
		vertexData[vertices->size() * 3 + 4 + 1] = y_max;
		vertexData[vertices->size() * 3 + 5 + 1] = z_max;
		

		std::ofstream ouF; 
        ouF.open(path_file_, std::ios::binary);  
        ouF.write(reinterpret_cast<char*>(vertexData), sizeof(float) * (vertices->size() * 3 + 6 + 1));  
          
		
	     int* index = new int[vertices->size()];
	    
	     for(size_t i = 0; i < vertices->size(); i++)
	     {
	    	 index[i] = i ;
	     }
	    
	     ouF.write(reinterpret_cast<char*>(index), sizeof(int) * vertices->size()); 
	    
	      
	    delete [] vertexData;
	    delete [] index;
		

		traverse(geo);

	}

	virtual void apply(osg::Node& node)
	{
		osg::Vec3f center = node.getBound().center();
		
		traverse(node);
	}

	virtual void apply(osg::Geode& node) //
	{
		traverse(node);
	}
};

#endif
