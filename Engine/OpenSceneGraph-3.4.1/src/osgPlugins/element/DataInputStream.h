#ifndef IVE_DATAINPUTSTREAM
#define IVE_DATAINPUTSTREAM 1


#include <iostream>        // for ifstream
#include <string>
#include <map>
#include <vector>



#include <osgGe/Vec2>
#include <osgGe/Vec3>
#include <osgGe/Vec4>
#include <osgGe/Vec2d>
#include <osgGe/Vec3d>
#include <osgGe/Vec4d>
#include <osg/Quat>
#include <osg/Array>
#include <osg/Matrix>
#include <osg/Image>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Uniform>
#include <osg/ref_ptr>

#include <osgTerrain/TerrainTile>
#include <osgVolume/VolumeTile>

#include <osgDB/ReaderWriter>
#include "Exception.h"
#include "IveVersion.h"
#include "DataTypeSize.h"


namespace element{

class DataInputStream{

public:
    DataInputStream(std::istream* istream, const osgDB::ReaderWriter::Options* options);
    ~DataInputStream();

    const osgDB::ReaderWriter::Options* getOptions() const { return _options.get(); }

    inline unsigned int getVersion() const { return _version; }
    bool readBool();
    char readChar();
    unsigned char readUChar();
    unsigned short readUShort();
    unsigned int readUInt();
    int readInt();
    int peekInt();
    float readFloat();
    long readLong();
    unsigned long readULong();
    double readDouble();
    std::string readString();
    void readCharArray(char* data, int size);

    osg::Vec2 readVec2();
    osg::Vec3 readVec3();
    osg::Vec4 readVec4();
    osg::Vec2d readVec2d();
    osg::Vec3d readVec3d();
    osg::Vec4d readVec4d();
    osg::Plane readPlane();
    osg::Vec4ub readVec4ub();
    osg::Quat readQuat();
    osg::Matrixf readMatrixf();
    osg::Matrixd readMatrixd();
    deprecated_osg::Geometry::AttributeBinding readBinding();
    osg::Array* readArray();
    osg::IntArray* readIntArray();
    osg::UByteArray* readUByteArray();
    osg::UShortArray* readUShortArray();
    osg::UIntArray* readUIntArray();
    osg::Vec4ubArray* readVec4ubArray();
    bool readPackedFloatArray(osg::FloatArray* floatArray);
    osg::FloatArray* readFloatArray();
    osg::Vec2Array* readVec2Array();
    osg::Vec3Array* readVec3Array();
    osg::Vec4Array* readVec4Array();
    osg::Vec2bArray* readVec2bArray();
    osg::Vec3bArray* readVec3bArray();
    osg::Vec4bArray* readVec4bArray();
    osg::Vec2sArray* readVec2sArray();
    osg::Vec3sArray* readVec3sArray();
    osg::Vec4sArray* readVec4sArray();
    osg::Vec2dArray* readVec2dArray();
    osg::Vec3dArray* readVec3dArray();
    osg::Vec4dArray* readVec4dArray();

	osg::EdgeCollapse::CollapseResult readCollapseResult();

	osg::EdgeCollapse::PointSet readPointSet(osg::EdgeCollapse::PointList& pointlist);
	osg::ref_ptr<osg::EdgeCollapse::Point> readPoint();

	osg::EdgeCollapse::TriangleSet readTriangleSet(osg::EdgeCollapse::PointList pointlist,osg::EdgeCollapse::TriangleList& trianglelist);
	osg::ref_ptr<osg::EdgeCollapse::Triangle> readTriangle(osg::EdgeCollapse::PointList pointlist);

	osg::EdgeCollapse::EdgeCollapseInfoList readEdgeCollapseInfoList(osg::EdgeCollapse::PointList& pointlist,osg::EdgeCollapse::TriangleList& trianglelist);

    // Set and get if must be generated external reference ive files
    void setLoadExternalReferenceFiles(bool b) {_loadExternalReferenceFiles=b;};
    bool getLoadExternalReferenceFiles() {return _loadExternalReferenceFiles;};

    bool                _verboseOutput;
    std::istream*       _istream;
    int                 _byteswap;

    bool                _owns_istream;

    bool uncompress(std::istream& fin, std::string& destination) const;

    void throwException(const std::string& message) { _exception = new Exception(message); }
    void throwException(Exception* exception) { _exception = exception; }
    const Exception* getException() const { return _exception.get(); }

private:


    int                 _version;
    bool                _peeking;
    int                 _peekValue;

    bool _loadExternalReferenceFiles;

    osg::ref_ptr<const osgDB::ReaderWriter::Options> _options;

    osg::ref_ptr<Exception> _exception;
};

}
#endif // IVE_DATAINPUTSTREAM
