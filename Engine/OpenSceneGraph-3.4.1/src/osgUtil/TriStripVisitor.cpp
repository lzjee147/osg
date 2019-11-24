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
#include <osg/Notify>
#include <osg/TriangleIndexFunctor>
#include <osg/io_utils>

#include <osgUtil/TriStripVisitor>
#include <osgUtil/SmoothingVisitor>

#include <stdio.h>
#include <algorithm>
#include <map>
#include <iterator>

//#include "tristripper/include/tri_stripper.h"

using namespace osg;
using namespace osgUtil;

typedef std::vector<unsigned int> IndexList;

class WriteValue : public osg::ConstValueVisitor
{
    public:
        WriteValue(std::ostream& o):_o(o) {}

        std::ostream& _o;

        virtual void apply(const GLbyte& v) { _o << v; }
        virtual void apply(const GLshort& v) { _o << v; }
        virtual void apply(const GLint& v) { _o << v; }
        virtual void apply(const GLushort& v) { _o << v; }
        virtual void apply(const GLubyte& v) { _o << v; }
        virtual void apply(const GLuint& v) { _o << v; }
        virtual void apply(const GLfloat& v) { _o << v; }
        virtual void apply(const Vec4ub& v) { _o << v; }
        virtual void apply(const Vec2& v) { _o << v; }
        virtual void apply(const Vec3& v) { _o << v; }
        virtual void apply(const Vec4& v) { _o << v; }

    protected:

        WriteValue& operator = (const WriteValue&) { return *this; }
};


struct VertexAttribComparitor
{
    VertexAttribComparitor(osg::Geometry& geometry)
    {
        add(geometry.getVertexArray());
        add(geometry.getNormalArray());
        add(geometry.getColorArray());
        add(geometry.getSecondaryColorArray());
        add(geometry.getFogCoordArray());
        unsigned int i;
        for(i=0;i<geometry.getNumTexCoordArrays();++i)
        {
            add(geometry.getTexCoordArray(i));
        }
        for(i=0;i<geometry.getNumVertexAttribArrays();++i)
        {
            add(geometry.getVertexAttribArray(i));
        }
    }

    void add(osg::Array* array)
    {
        if (array && array->getBinding()==osg::Array::BIND_PER_VERTEX)
        {
            for(ArrayList::const_iterator itr=_arrayList.begin();
                itr!=_arrayList.end();
                ++itr)
            {
                if ((*itr) == array)
                    return;
            }
            _arrayList.push_back(array);
        }
    }

    typedef std::vector<osg::Array*> ArrayList;

    ArrayList _arrayList;

    bool operator() (unsigned int lhs, unsigned int rhs) const
    {
        for(ArrayList::const_iterator itr=_arrayList.begin();
            itr!=_arrayList.end();
            ++itr)
        {
            int compare = (*itr)->compare(lhs,rhs);
            if (compare==-1) return true;
            if (compare==1) return false;
        }
        return false;
    }

    int compare(unsigned int lhs, unsigned int rhs)
    {
        for(ArrayList::iterator itr=_arrayList.begin();
            itr!=_arrayList.end();
            ++itr)
        {
            int compare = (*itr)->compare(lhs,rhs);
            if (compare==-1) return -1;
            if (compare==1) return 1;
        }
//
//         WriteValue wv(std::cout);
//
//         std::cout<<"Values equal"<<std::endl;
//         for(ArrayList::iterator itr=_arrayList.begin();
//             itr!=_arrayList.end();
//             ++itr)
//         {
//             std::cout<<"   lhs["<<lhs<<"]="; (*itr)->accept(lhs,wv);
//             std::cout<<"   rhs["<<rhs<<"]="; (*itr)->accept(rhs,wv);
//             std::cout<<std::endl;
//         }


        return 0;
    }

    void accept(osg::ArrayVisitor& av)
    {
        for(ArrayList::iterator itr=_arrayList.begin();
            itr!=_arrayList.end();
            ++itr)
        {
            (*itr)->accept(av);
        }
    }

protected:

    VertexAttribComparitor& operator = (const VertexAttribComparitor&) { return *this; }

};

class RemapArray : public osg::ArrayVisitor
{
    public:
        RemapArray(const IndexList& remapping):_remapping(remapping) {}

        const IndexList& _remapping;

        template<class T>
        inline void remap(T& array)
        {
            for(unsigned int i=0;i<_remapping.size();++i)
            {
                if (i!=_remapping[i])
                {
                    array[i] = array[_remapping[i]];
                }
            }
            array.erase(array.begin()+_remapping.size(),array.end());
        }

        virtual void apply(osg::Array&) {}
        virtual void apply(osg::ByteArray& array) { remap(array); }
        virtual void apply(osg::ShortArray& array) { remap(array); }
        virtual void apply(osg::IntArray& array) { remap(array); }
        virtual void apply(osg::UByteArray& array) { remap(array); }
        virtual void apply(osg::UShortArray& array) { remap(array); }
        virtual void apply(osg::UIntArray& array) { remap(array); }
        virtual void apply(osg::Vec4ubArray& array) { remap(array); }
        virtual void apply(osg::FloatArray& array) { remap(array); }
        virtual void apply(osg::Vec2Array& array) { remap(array); }
        virtual void apply(osg::Vec3Array& array) { remap(array); }
        virtual void apply(osg::Vec4Array& array) { remap(array); }

protected:

        RemapArray& operator = (const RemapArray&) { return *this; }
};

struct MyTriangleOperator
{

    IndexList                                _remapIndices;


    inline void operator()(unsigned int p1, unsigned int p2, unsigned int p3)
    {
        if (_remapIndices.empty())
        {

        }
        else
        {

        }
    }

};
typedef osg::TriangleIndexFunctor<MyTriangleOperator> MyTriangleIndexFunctor;

void TriStripVisitor::stripify(Geometry& geom)
{
  

}


void TriStripVisitor::mergeTriangleStrips(osg::Geometry::PrimitiveSetList& primitives)
{
    int nbtristrip = 0;
    int nbtristripVertexes = 0;

    for (unsigned int i = 0; i < primitives.size(); ++ i)
    {
        osg::PrimitiveSet* ps = primitives[i].get();
        osg::DrawElements* de = ps->getDrawElements();
        if (de && de->getMode() == osg::PrimitiveSet::TRIANGLE_STRIP)
        {
            ++ nbtristrip;
            nbtristripVertexes += de->getNumIndices();
        }
    }

    if (nbtristrip > 0) {
        osg::notify(osg::NOTICE) << "found " << nbtristrip << " tristrip, "
                                 << "total indices " << nbtristripVertexes
                                 << " should result to " << nbtristripVertexes + nbtristrip * 2
                                 << " after connection" << std::endl;

        osg::DrawElementsUInt* ndw = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP);
        for (unsigned int i = 0; i < primitives.size(); ++ i)
        {
            osg::PrimitiveSet* ps = primitives[i].get();
            if (ps && ps->getMode() == osg::PrimitiveSet::TRIANGLE_STRIP)
            {
                osg::DrawElements* de = ps->getDrawElements();
                if (de)
                {
                    // if connection needed insert degenerate triangles
                    if (ndw->getNumIndices() != 0 && ndw->back() != de->getElement(0))
                    {
                        // duplicate last vertex
                        ndw->addElement(ndw->back());
                        // insert first vertex of next strip
                        ndw->addElement(de->getElement(0));
                    }

                    if (ndw->getNumIndices() % 2 != 0 ) {
                        // add a dummy vertex to reverse the strip
                        ndw->addElement(de->getElement(0));
                    }

                    for (unsigned int j = 0; j < de->getNumIndices(); j++) {
                        ndw->addElement(de->getElement(j));
                    }
                }
            }
        }

        for (int i = primitives.size() - 1 ; i >= 0 ; -- i)
        {
            osg::PrimitiveSet* ps = primitives[i].get();
            // remove null primitive sets and all primitives that have been merged
            // (i.e. all TRIANGLE_STRIP DrawElements)
            if (!ps || (ps && ps->getMode() == osg::PrimitiveSet::TRIANGLE_STRIP))
            {
                primitives.erase(primitives.begin() + i);
            }
        }
        primitives.insert(primitives.begin(), ndw);
    }
}

void TriStripVisitor::stripify()
{
    for(GeometryList::iterator itr=_geometryList.begin();
        itr!=_geometryList.end();
        ++itr)
    {
        stripify(*(*itr));

        // osgUtil::SmoothingVisitor sv;
        // sv.smooth(*(*itr));
    }
}

void TriStripVisitor::apply(Geode& geode)
{
    for(unsigned int i = 0; i < geode.getNumDrawables(); ++i )
    {
        osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
        if (geom) _geometryList.insert(geom);
    }
}
