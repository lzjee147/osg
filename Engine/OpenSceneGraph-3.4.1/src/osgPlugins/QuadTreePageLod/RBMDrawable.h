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

 #ifndef RBMDrawable_
 #define RBMDrawable_


#include <string>
#include <stack>
#include <sstream>

#include <osg/Notify>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osg/Geode>

#include <osg/Geometry>
#include <osg/State>


#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <osg/RenderInfo>

#include <map>
#include <set>


//自定义实体，自定义包围盒，直接显示
static bool _data = false;

unsigned int ID[3];

class RBMDrawable : public osg::Geometry
{
    public:
        RBMDrawable() 
		{
			_data = false;
		}

        RBMDrawable(const RBMDrawable& teapot,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY):
            osg::Geometry(teapot,copyop) 
		{
		    
		}

        META_Object(myTeapotApp,RBMDrawable)

public:
	    osg::BoundingBox bbox_;

		
		unsigned int index_num;

		void compileGLObjects(osg::RenderInfo& info) const
		{
			osg::State& state = *info.getState();
			unsigned int contextID = state.getContextID();
			osg::GLExtensions* extensions_ = state.get<osg::GLExtensions>();

			int triangle = 3666;

			if (!_data)
			{
				_data = true;


				GLfloat vertex_list[] = {
					0, 50, 0,
					50, 0, 0,
					-50,  0, 0,
				};

				GLint index_list[] = {
					0, 1, 2,
				};

				GLfloat color_list[] = {
					0.2, 0.1, 0.3, 1.0,  
					0.4, 0.4, 0.6, 1.0, 
					0.6, 0.7, 0.3, 1.0,
				};


				int verNum = triangle * 3;

				GLfloat* ver = new GLfloat[verNum * 3];

				GLint* index = new GLint[verNum / 3];

				for (size_t i = 0; i < verNum; i++)
				{
					ver[3 * i] = rand() % 50;
					ver[3 * i + 1] = rand() % 50;
					ver[3 * i + 2] = rand() % 50;
				}

				for (size_t j = 0; j < triangle / 3; j++)
				{
					index[3 * j] = 3 * j;
					index[3 * j + 1] = 3 * j + 1;
					index[3 * j + 2] = 3 * j + 2;
				}


				extensions_->glGenBuffers(1, &ID[0]); 

				//
				extensions_->glBindBuffer(GL_ARRAY_BUFFER_ARB, ID[0]);
				extensions_->glBufferData(GL_ARRAY_BUFFER_ARB,
					sizeof(GLfloat) * triangle * 3, ver, GL_STATIC_DRAW_ARB);

				// extension 
				extensions_->glGenBuffers(1, &ID[1]);
				extensions_->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, ID[1]);
				extensions_->glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB,
					sizeof(GLuint) * triangle, index, GL_STATIC_DRAW_ARB);

				delete [] ver;
				delete [] index;

			}
		}

        virtual void drawImplementation(osg::RenderInfo& info) const
        {
			osg::State& state = *info.getState();
			unsigned int contextID = state.getContextID();
			osg::GLExtensions* extensions_ = state.get<osg::GLExtensions>();

			//顶点数
		
	         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		     extensions_->glBindBuffer(GL_ARRAY_BUFFER_ARB, ID[0]);
		     extensions_->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, ID[1]);
		     
		     glEnableClientState(GL_VERTEX_ARRAY);
		     
		     glVertexPointer(3, GL_FLOAT, 0, NULL);

		     glDrawElements(GL_TRIANGLES,  3666, GL_UNSIGNED_INT, NULL);
			// glDrawArrays(GL_TRIANGLES, 0, 36666);
			//glBegin(GL_LINES);

			//  glVertex3d(0.0, 0.0, 0.0);
			//  glVertex3d(30.0, 0.0, 0.0);

			//glEnd();

			 //

        }

        virtual osg::BoundingBox computeBoundingBox() const
        {
            return osg::BoundingBox(-50, -50, -50, 50, 50, 50);
        }

    protected:

        virtual ~RBMDrawable() 
		{
	
		}

};

#endif
