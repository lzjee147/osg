/**********************************************************************
 *
 *    FILE:            Text.cpp
 *
 *    DESCRIPTION:    Read/Write osgText::Text in binary format to disk.
 *
 *    CREATED BY:        Auto generated by iveGenerator
 *                    and later modified by Rune Schmidt Jensen.
 *
 *    HISTORY:        Created 27.3.2003
 *
 *    Copyright 2003 VR-C
 **********************************************************************/

#include "Exception.h"
#include "DrawPixels.h"
#include "Drawable.h"
#include "Object.h"

#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osg/Notify>

#include <osgDB/ReadFile>

using namespace ive;

class GetDrawPixelsImage
{
public:
	GetDrawPixelsImage()
	{
		image_ = osgDB::readImageFile(osgDB::getCurrentWorkingDirectory()+"\\picture\\bz.png");
	}

	osg::Image*  getImage()
	{
		return image_;
	}

private:
	osg::Image* image_;

};

GetDrawPixelsImage GetDrawPixelsImage_;

void DrawPixels::write(DataOutputStream* out){
 
    out->writeInt(DRAWPIXELS);

	osg::Object*  obj = dynamic_cast<osg::Object*>(this);
	if(obj){
		((ive::Drawable*)(obj))->write(out);
	}
	else
		out_THROW_EXCEPTION("Text::write(): Could not cast this osgText::Text to an osg::Drawable.");


	osg::Vec3 pos = getPosition();

	out->writeVec3(pos);
}

void DrawPixels::read(DataInputStream* in){
   
    int id = in->peekInt();
	if(id == DRAWPIXELS){
		// Read Text3D's identification.
		id = in->readInt();
		// If the osg class is inherited by any other class we should also read this from file.
		osg::Object*  obj = dynamic_cast<osg::Object*>(this);
		if(obj){
			((ive::Drawable*)(obj))->read(in);
		}
		else
			in_THROW_EXCEPTION("Text::read(): Could not cast this osgText::Text to an osg::Drawable.");

		osg::Vec3 pos = in->readVec3();
		setPosition(pos);

		setImage(GetDrawPixelsImage_.getImage());
	}
}