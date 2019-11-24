/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/
#include "OsgExTool/Tex2DDrawable.h"
#include <osg/GL>
#include <osg/Notify>
#include <osg/State>

#include <osg/GLBeginEndAdapter>

//
//class BoxDrawShapeVisitor : public osg::ConstShapeVisitor
//{
//    public:
//
//        BoxDrawShapeVisitor(osg::State& state):
//            _state(state)
//        {
//
//        }
//
//        virtual void apply(const osg::Box&);
//        void setColor(osg::Vec4f color)
//        {
//             color_ = color;
//        }
//
//public:
//   osg::State&                      _state;
//   osg::Vec4f color_;
//
//    protected:
//
//        BoxDrawShapeVisitor& operator = (const BoxDrawShapeVisitor&) { return *this; }
//};
//
//void BoxDrawShapeVisitor::apply(const osg::Box& box)
//{
//    osg::GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();
//
//    // evaluate hints
//    bool createBody = true  ;
//    bool createTop = true   ;
//    bool createBottom = true;
//
//    float dx = box.getHalfLengths().x();
//    float dy = box.getHalfLengths().y();
//    float dz = box.getHalfLengths().z();
//
//    gl.PushMatrix();
//
//    gl.Translated(box.getCenter().x(),box.getCenter().y(),box.getCenter().z());
//
//    if (!box.zeroRotation())
//    {
//        osg::Matrixd rotation(box.computeRotationMatrix());
//        gl.MultMatrixd(rotation.ptr());
//    }
//
//    gl.Begin(GL_QUADS);
//
//    if (true) {
//
//        gl.Color4f(color_[0], color_[1], color_[2], 1.0);
//        gl.Normal3f(0.0f,1.0f,0.0f);
//
//        gl.TexCoord2f(0.0f,1.0f);
//        gl.Vertex3f(dx,dy,dz);
//
//        gl.TexCoord2f(0.0f,0.0f);
//        gl.Vertex3f(dx,dy,-dz);
//
//        gl.TexCoord2f(1.0f,0.0f);
//        gl.Vertex3f(-dx,dy,-dz);
//
//        gl.TexCoord2f(1.0f,1.0f);
//        gl.Vertex3f(-dx,dy,dz);
//
//        gl.Color4f(color_[0], color_[1], color_[2], color_[3]);
//        // +ve x plane
//        gl.Normal3f(1.0f,0.0f,0.0f);
//
//        gl.TexCoord2f(0.0f,1.0f);
//        gl.Vertex3f(dx,-dy,dz);
//
//        gl.TexCoord2f(0.0f,0.0f);
//        gl.Vertex3f(dx,-dy,-dz);
//
//        gl.TexCoord2f(1.0f,0.0f);
//        gl.Vertex3f(dx,dy,-dz);
//
//        gl.TexCoord2f(1.0f,1.0f);
//        gl.Vertex3f(dx,dy,dz);
//
//        gl.Normal3f(-1.0f,0.0f,0.0f);
//
//        gl.TexCoord2f(0.0f,1.0f);
//        gl.Vertex3f(-dx,dy,dz);
//
//        gl.TexCoord2f(0.0f,0.0f);
//        gl.Vertex3f(-dx,dy,-dz);
//
//        gl.TexCoord2f(1.0f,0.0f);
//        gl.Vertex3f(-dx,-dy,-dz);
//
//        gl.TexCoord2f(1.0f,1.0f);
//        gl.Vertex3f(-dx,-dy,dz);
//    }
//
//    if (false) {
//
//        gl.Normal3f(0.0f,0.0f,1.0f);
//
//        gl.TexCoord2f(0.0f,1.0f);
//        gl.Vertex3f(-dx,dy,dz);
//
//        gl.TexCoord2f(0.0f,0.0f);
//        gl.Vertex3f(-dx,-dy,dz);
//
//        gl.TexCoord2f(1.0f,0.0f);
//        gl.Vertex3f(dx,-dy,dz);
//
//        gl.TexCoord2f(1.0f,1.0f);
//        gl.Vertex3f(dx,dy,dz);
//    }
//
//    if (false) {
//        gl.Normal3f(0.0f,0.0f,-1.0f);
//
//        gl.TexCoord2f(0.0f,1.0f);
//        gl.Vertex3f(dx,dy,-dz);
//
//        gl.TexCoord2f(0.0f,0.0f);
//        gl.Vertex3f(dx,-dy,-dz);
//
//        gl.TexCoord2f(1.0f,0.0f);
//        gl.Vertex3f(-dx,-dy,-dz);
//
//        gl.TexCoord2f(1.0f,1.0f);
//        gl.Vertex3f(-dx,dy,-dz);
//    }
//
//    gl.End();
//
//    gl.PopMatrix();
//
//}

Tex2DDrawable::Tex2DDrawable()
{

}

Tex2DDrawable::Tex2DDrawable(osg::Shape* shape)
{

}


Tex2DDrawable::~Tex2DDrawable()
{
}


void Tex2DDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
	glDrawBuffer(GL_FRONT);

    osg::State& state = *renderInfo.getState();
    osg::GLBeginEndAdapter& gl = state.getGLBeginEndAdapter();

	glColor3f(1.0f, 0.0f, 0.0f);

	GLubyte bitShape[20] = {
		0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00,
		0xff, 0x80, 0x7f, 0x00, 0x3e, 0x00, 0x1c, 0x00, 0x08, 0x00
	};

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	glBitmap(9, 10, 0.0, 0.0, 100, 100, bitShape);

	
	//glBegin(GL_LINES);

	//glVertex2d(0, 0);
	//glVertex2d(100, 100);

	//glEnd();


    //if (_shape.valid())
    {
       // gl.Color4f(color_[0], color_[1], color_[2], color_[3]);

		//BoxDrawShapeVisitor dsv(state);
		//dsv.setColor(color_);

		//_shape->accept(dsv);
    }
}


