#ifndef IVE_DATAOUTPUTSTREAM
#define IVE_DATAOUTPUTSTREAM 1



#include <iostream>        // for ofstream
#include <string>
#include <sstream>

#include <osgGe/Vec2>
#include <osgGe/Vec3>
#include <osgGe/Vec4>
#include <osg/Quat>
#include <osg/Array>
#include <osg/Matrix>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/Uniform>
#include <osgDB/ReaderWriter>

#include <osgTerrain/TerrainTile>
#include <osgVolume/VolumeTile>

#include "DataTypeSize.h"
#include "Exception.h"

#include "IveVersion.h"

#include <osg/StateSet>
#include <osg/ref_ptr>

namespace ive {

class DataOutputStream{

public:
    DataOutputStream(std::ostream* ostream, const osgDB::ReaderWriter::Options* options);
    virtual ~DataOutputStream();

    const osgDB::ReaderWriter::Options* getOptions() const { return _options.get(); }
	 unsigned int getVersion() { return VERSION; }

    void writeBool(bool b);
    void writeChar(char c);
    void writeUChar(unsigned char c);
    void writeUShort(unsigned short s);
    void writeShort(short s);
    void writeUInt(unsigned int s);
    void writeInt(int i);
    void writeFloat(float f);
    void writeLong(long l);
    void writeULong(unsigned long l);
    void writeDouble(double d);
    void writeString(const std::string& s);
    void writeCharArray(const char* data, int size);
    void writeVec2(const osg::Vec2& v);
    void writeVec3(const osg::Vec3& v);
    void writeVec4(const osg::Vec4& v);
    void writeVec2d(const osg::Vec2d& v);
    void writeVec3d(const osg::Vec3d& v);
    void writeVec4d(const osg::Vec4d& v);
    void writePlane(const osg::Plane& v);
    void writeVec4ub(const osg::Vec4ub& v);
    void writeQuat(const osg::Quat& q);
    void writeBinding(deprecated_osg::Geometry::AttributeBinding b);
    void writeArray(const osg::Array* a);
    void writeIntArray(const osg::IntArray* a);
    void writeUByteArray(const osg::UByteArray* a);
    void writeUShortArray(const osg::UShortArray* a);
    void writeUIntArray(const osg::UIntArray* a);
    void writeVec4ubArray(const osg::Vec4ubArray* a);
    void writeVec2b(const osg::Vec2b& v);
    void writeVec3b(const osg::Vec3b& v);
    void writeVec4b(const osg::Vec4b& v);

    void writePackedFloatArray(const osg::FloatArray* a, float maxError);

    void writeFloatArray(const osg::FloatArray* a);
    void writeVec2Array(const osg::Vec2Array* a);
    void writeVec3Array(const osg::Vec3Array* a);
    void writeVec4Array(const osg::Vec4Array* a);
    void writeVec2sArray(const osg::Vec2sArray* a);
    void writeVec3sArray(const osg::Vec3sArray* a);
    void writeVec4sArray(const osg::Vec4sArray* a);
    void writeVec2bArray(const osg::Vec2bArray* a);
    void writeVec3bArray(const osg::Vec3bArray* a);
    void writeVec4bArray(const osg::Vec4bArray* a);
    void writeVec2dArray(const osg::Vec2dArray* a);
    void writeVec3dArray(const osg::Vec3dArray* a);
    void writeVec4dArray(const osg::Vec4dArray* a);
    void writeMatrixf(const osg::Matrixf& mat);
    void writeMatrixd(const osg::Matrixd& mat);

    void writeStateSet(const osg::StateSet* stateset);
    void writeStateAttribute(const osg::StateAttribute* sa);
    void writeUniform(const osg::Uniform* uniform);
    void writeShader(const osg::Shader* shader);
    void writeDrawable(const osg::Drawable* sa);
    void writeShape(const osg::Shape* sa);
    void writeNode(const osg::Node* sa);

    void writeImage(IncludeImageMode mode, osg::Image *image);
    void writeImage(osg::Image *image);

    void writeLayer(const osgTerrain::Layer* layer);
    void writeLocator(const osgTerrain::Locator* locator);

    void writeVolumeLayer(const osgVolume::Layer* layer);
    void writeVolumeLocator(const osgVolume::Locator* locator);
    void writeVolumeProperty(const osgVolume::Property* propety);

    void writeObject(const osg::Object* object);

    void setWriteDirectory(const std::string& directoryName) { _writeDirectory = directoryName; }
    const std::string& getWriteDirectory() const { return _writeDirectory; }

  

    bool compress(std::ostream& fout, const std::string& source) const;


    void setExternalFileWritten(const std::string& filename, bool hasBeenWritten=true);
    bool getExternalFileWritten(const std::string& filename) const;

    void throwException(const std::string& message) { _exception = new Exception(message); }
    void throwException(Exception* exception) { _exception = exception; }
    const Exception* getException() const { return _exception.get(); }

    private:

    std::ostream* _ostream;
    std::ostream* _output_ostream;
    std::string _filename; // not necessary, but optional for use in texture export

    std::stringstream _compressionStream;
    int _compressionLevel;

      bool                _verboseOutput;

    std::string         _writeDirectory;
    bool                _includeExternalReferences;
    bool                _writeExternalReferenceFiles;
    bool                _useOriginalExternalReferences;
    double              _maximumErrorToSizeRatio;

    IncludeImageMode    _includeImageMode;

    bool _outputTextureFiles;
    unsigned int _textureFileNameNumber;

    osg::ref_ptr<const osgDB::ReaderWriter::Options> _options;

    typedef std::map<std::string, bool> ExternalFileWrittenMap;
    ExternalFileWrittenMap _externalFileWritten;

    osg::ref_ptr<Exception> _exception;
};

}
#endif // IVE_DATAOUTPUTSTREAM
