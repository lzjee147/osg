/**********************************************************************
 *
 *    FILE:           DataOutputStream.cpp
 *
 *    DESCRIPTION:    Implements methods to write simple datatypes to an
 *                    output stream.
 *
 *    CREATED BY:     Rune Schmidt Jensen
 *
 *    HISTORY:        Created 11.03.2003
 *                    Updated for 1D textures - Don Burns 27.1.2004
 *                    Updated for light model - Stan Blinov at 25 august 7512 from World Creation (7.09.2004)
 *
 *    Copyright 2003 VR-C
 **********************************************************************/

#include "DataOutputStream.h"
#include "Exception.h"

#include <osg/Notify>
#include <osg/io_utils>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgDB/fstream>
#include <osgDB/WriteFile>

#include <stdlib.h>
#include <sstream>

using namespace element;


DataOutputStream::DataOutputStream(std::ostream * ostream, const osgDB::ReaderWriter::Options* options)
{
    _verboseOutput = false;

    _includeImageMode = IMAGE_INCLUDE_DATA;

    _includeExternalReferences     = false;
    _writeExternalReferenceFiles   = false;
    _useOriginalExternalReferences = true;
    _maximumErrorToSizeRatio       = 0.001;

    _outputTextureFiles = false;
    _textureFileNameNumber = 0;

    _options = options;

    _compressionLevel = 0;

    if (options) _filename = options->getPluginStringData("filename");

    if (_filename.empty())
    {
        // initialize _filename to a unique identifier in case a real filename is not supplied
        std::ostringstream filenameBuilder;
        filenameBuilder << "file" << ostream; // use address of ostream to formulate unique filename
        _filename = filenameBuilder.str();
    }

    if (_options.get())
    {
        std::string optionsString = _options->getOptionString();

        if(optionsString.find("noTexturesInIVEFile")!=std::string::npos) {
            setIncludeImageMode(IMAGE_REFERENCE_FILE);
        } else if(optionsString.find("includeImageFileInIVEFile")!=std::string::npos) {
            setIncludeImageMode(IMAGE_INCLUDE_FILE);
        } else if(optionsString.find("compressImageData")!=std::string::npos) {
            setIncludeImageMode(IMAGE_COMPRESS_DATA);
        }
        OSG_DEBUG << "ive::DataOutputStream.setIncludeImageMode()=" << getIncludeImageMode() << std::endl;

        setIncludeExternalReferences(optionsString.find("inlineExternalReferencesInIVEFile")!=std::string::npos);
        OSG_DEBUG << "ive::DataOutputStream.setIncludeExternalReferences()=" << getIncludeExternalReferences() << std::endl;

        setWriteExternalReferenceFiles(optionsString.find("noWriteExternalReferenceFiles")==std::string::npos);
        OSG_DEBUG << "ive::DataOutputStream.setWriteExternalReferenceFiles()=" << getWriteExternalReferenceFiles() << std::endl;

        setUseOriginalExternalReferences(optionsString.find("useOriginalExternalReferences")!=std::string::npos);
        OSG_DEBUG << "ive::DataOutputStream.setUseOriginalExternalReferences()=" << getUseOriginalExternalReferences() << std::endl;

        setOutputTextureFiles(optionsString.find("OutputTextureFiles")!=std::string::npos);
        OSG_DEBUG << "ive::DataOutputStream.setOutputTextureFiles()=" << getOutputTextureFiles() << std::endl;

        _compressionLevel =  (optionsString.find("compressed")!=std::string::npos) ? 1 : 0;
        OSG_DEBUG << "ive::DataOutputStream._compressionLevel=" << _compressionLevel << std::endl;

        std::string::size_type terrainErrorPos = optionsString.find("TerrainMaximumErrorToSizeRatio=");
        if (terrainErrorPos!=std::string::npos)
        {
            std::string::size_type endOfToken = optionsString.find_first_of('=', terrainErrorPos);
            std::string::size_type endOfNumber = optionsString.find_first_of(' ', endOfToken);
            std::string::size_type numOfCharInNumber = (endOfNumber != std::string::npos) ?
                    endOfNumber-endOfToken-1 :
                    optionsString.size()-endOfToken-1;

            if (numOfCharInNumber>0)
            {
                std::string numberString = optionsString.substr(endOfToken+1, numOfCharInNumber);
                _maximumErrorToSizeRatio = osg::asciiToDouble(numberString.c_str());

                OSG_DEBUG<<"TerrainMaximumErrorToSizeRatio = "<<_maximumErrorToSizeRatio<<std::endl;
            }
            else
            {
                OSG_DEBUG<<"Error no value to TerrainMaximumErrorToSizeRatio assigned"<<std::endl;
            }
        }
    }

    #ifndef USE_ZLIB
    if (_compressionLevel>0)
    {
        OSG_NOTICE << "Compression not supported in this .ive version." << std::endl;
        _compressionLevel = 0;
    }
    #endif

    _output_ostream = _ostream = ostream;

    if(!_ostream)
    {
        throwException("DataOutputStream::DataOutputStream(): null pointer exception in argument.");
        return;
    }

    writeUInt(ENDIAN_TYPE) ;
    writeUInt(getVersion());

    writeInt(_compressionLevel);

    if (_compressionLevel>0)
    {

        _ostream = &_compressionStream;
    }
}

DataOutputStream::~DataOutputStream()
{
    if (_compressionLevel>0)
    {
        _ostream = _output_ostream;

        std::string compressionString(_compressionStream.str());
        writeUInt(compressionString.size());

        compress(*_output_ostream, compressionString);
    }
}

#ifdef USE_ZLIB

#include <zlib.h>

#define CHUNK 16384
bool DataOutputStream::compress(std::ostream& fout, const std::string& source) const
{
    int ret, flush = Z_FINISH;
    unsigned have;
    z_stream strm;
    unsigned char out[CHUNK];

    int level = 6;
    int stategy = Z_DEFAULT_STRATEGY; // looks to be the best for .osg/.ive files
    //int stategy = Z_FILTERED;
    //int stategy = Z_HUFFMAN_ONLY;
    //int stategy = Z_RLE;

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit2(&strm,
                       level,
                       Z_DEFLATED,
                       15+16, // +16 to use gzip encoding
                       8, // default
                       stategy);
    if (ret != Z_OK)
    return false;

    strm.avail_in = source.size();
    strm.next_in = (Bytef*)(&(*source.begin()));

    /* run deflate() on input until output buffer not full, finish
       compression if all of source has been read in */
    do {
        strm.avail_out = CHUNK;
        strm.next_out = out;
        ret = deflate(&strm, flush);    /* no bad return value */

        if (ret == Z_STREAM_ERROR)
        {
            OSG_NOTICE<<"Z_STREAM_ERROR"<<std::endl;
            return false;
        }

        have = CHUNK - strm.avail_out;

        if (have>0) fout.write((const char*)out, have);

        if (fout.fail())
        {
            (void)deflateEnd(&strm);
            return false;
        }
    } while (strm.avail_out == 0);

    /* clean up and return */
    (void)deflateEnd(&strm);
    return true;
}
#else
bool DataOutputStream::compress(std::ostream& fout, const std::string& source) const
{
    return false;
}
#endif

void DataOutputStream::writeBool(bool b)
{
    char c = b?1:0;
    _ostream->write(&c, CHARSIZE);

    if (_verboseOutput) std::cout<<"read/writeBool() ["<<(int)c<<"]"<<std::endl;
}

void DataOutputStream::writeChar(char c){
    _ostream->write(&c, CHARSIZE);

    if (_verboseOutput) std::cout<<"read/writeChar() ["<<(int)c<<"]"<<std::endl;
}

void DataOutputStream::writeUChar(unsigned char c){
    _ostream->write((char*)&c, CHARSIZE);

    if (_verboseOutput) std::cout<<"read/writeUChar() ["<<(int)c<<"]"<<std::endl;
}

void DataOutputStream::writeUShort(unsigned short s){
    _ostream->write((char*)&s, SHORTSIZE);

    if (_verboseOutput) std::cout<<"read/writeUShort() ["<<s<<"]"<<std::endl;
}

void DataOutputStream::writeShort(short s){
    _ostream->write((char*)&s, SHORTSIZE);

    if (_verboseOutput) std::cout<<"read/writeShort() ["<<s<<"]"<<std::endl;
}

void DataOutputStream::writeUInt(unsigned int s){
    _ostream->write((char*)&s, INTSIZE);

    if (_verboseOutput) std::cout<<"read/writeUInt() ["<<s<<"]"<<std::endl;
}

void DataOutputStream::writeInt(int i){
    _ostream->write((char*)&i, INTSIZE);

    if (_verboseOutput) std::cout<<"read/writeInt() ["<<i<<"]"<<std::endl;
}

void DataOutputStream::writeFloat(float f){
    _ostream->write((char*)&f, FLOATSIZE);

    if (_verboseOutput) std::cout<<"read/writeFloat() ["<<f<<"]"<<std::endl;
}

void DataOutputStream::writeLong(long l){
    _ostream->write((char*)&l, LONGSIZE);

    if (_verboseOutput) std::cout<<"read/writeLong() ["<<l<<"]"<<std::endl;
}

void DataOutputStream::writeULong(unsigned long l){
    _ostream->write((char*)&l, LONGSIZE);

    if (_verboseOutput) std::cout<<"read/writeULong() ["<<l<<"]"<<std::endl;
}

void DataOutputStream::writeDouble(double d){
    _ostream->write((char*)&d, DOUBLESIZE);

    if (_verboseOutput) std::cout<<"read/writeDouble() ["<<d<<"]"<<std::endl;
}

void DataOutputStream::writeString(const std::string& s){
    writeInt(s.size());
    _ostream->write(s.c_str(), s.size());

    if (_verboseOutput) std::cout<<"read/writeString() ["<<s<<"]"<<std::endl;
}

void DataOutputStream::writeCharArray(const char* data, int size){
    _ostream->write(data, size);

    if (_verboseOutput) std::cout<<"read/writeCharArray() ["<<data<<"]"<<std::endl;
}

void DataOutputStream::writeVec2(const osg::Vec2& v){
    writeFloat(v.x());
    writeFloat(v.y());

    if (_verboseOutput) std::cout<<"read/writeVec2() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writeVec3(const osg::Vec3& v){
    writeFloat(v.x());
    writeFloat(v.y());
    writeFloat(v.z());

    if (_verboseOutput) std::cout<<"read/writeVec3() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writeVec4(const osg::Vec4& v){
    writeFloat(v.x());
    writeFloat(v.y());
    writeFloat(v.z());
    writeFloat(v.w());

    if (_verboseOutput) std::cout<<"read/writeVec4() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writeVec2d(const osg::Vec2d& v){
    writeDouble(v.x());
    writeDouble(v.y());

    if (_verboseOutput) std::cout<<"read/writeVec2() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writeVec3d(const osg::Vec3d& v){
    writeDouble(v.x());
    writeDouble(v.y());
    writeDouble(v.z());

    if (_verboseOutput) std::cout<<"read/writeVec3d() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writeVec4d(const osg::Vec4d& v){
    writeDouble(v.x());
    writeDouble(v.y());
    writeDouble(v.z());
    writeDouble(v.w());

    if (_verboseOutput) std::cout<<"read/writeVec4d() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writePlane(const osg::Plane& v)
{
    writeDouble(v[0]);
    writeDouble(v[1]);
    writeDouble(v[2]);
    writeDouble(v[3]);

    if (_verboseOutput) std::cout<<"read/writePlane() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writeVec4ub(const osg::Vec4ub& v){
    writeChar(v.r());
    writeChar(v.g());
    writeChar(v.b());
    writeChar(v.a());

    if (_verboseOutput) std::cout<<"read/writeVec4ub() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writeVec2b(const osg::Vec2b& v){
    writeChar(v.r());
    writeChar(v.g());

    if (_verboseOutput) std::cout<<"read/writeVec2b() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writeVec3b(const osg::Vec3b& v){
    writeChar(v.r());
    writeChar(v.g());
    writeChar(v.b());

    if (_verboseOutput) std::cout<<"read/writeVec3b() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writeVec4b(const osg::Vec4b& v){
    writeChar(v.r());
    writeChar(v.g());
    writeChar(v.b());
    writeChar(v.a());

    if (_verboseOutput) std::cout<<"read/writeVec4b() ["<<v<<"]"<<std::endl;
}

void DataOutputStream::writeQuat(const osg::Quat& q){
    writeFloat(q.x());
    writeFloat(q.y());
    writeFloat(q.z());
    writeFloat(q.w());

    if (_verboseOutput) std::cout<<"read/writeQuat() ["<<q<<"]"<<std::endl;
}

void DataOutputStream::writeBinding(deprecated_osg::Geometry::AttributeBinding b){
    switch(b){
        case deprecated_osg::Geometry::BIND_OFF:                           writeChar((char) 0); break;
        case deprecated_osg::Geometry::BIND_OVERALL:                       writeChar((char) 1); break;
        case deprecated_osg::Geometry::BIND_PER_PRIMITIVE:                 writeChar((char) 2); break;
        case deprecated_osg::Geometry::BIND_PER_PRIMITIVE_SET:             writeChar((char) 3); break;
        case deprecated_osg::Geometry::BIND_PER_VERTEX:                    writeChar((char) 4); break;
        default: throwException("Unknown binding in DataOutputStream::writeBinding()");
    }

    if (_verboseOutput) std::cout<<"read/writeBinding() ["<<b<<"]"<<std::endl;
}

void DataOutputStream::writeArray(const osg::Array* a){
    switch(a->getType()){
        case osg::Array::IntArrayType:
            writeChar((char)0);
            writeIntArray(static_cast<const osg::IntArray*>(a));
            break;
        case osg::Array::UByteArrayType:
            writeChar((char)1);
            writeUByteArray(static_cast<const osg::UByteArray*>(a));
            break;
        case osg::Array::UShortArrayType:
            writeChar((char)2);
            writeUShortArray(static_cast<const osg::UShortArray*>(a));
            break;
        case osg::Array::UIntArrayType:
            writeChar((char)3);
            writeUIntArray(static_cast<const osg::UIntArray*>(a));
            break;
        case osg::Array::Vec4ubArrayType:
            writeChar((char)4);
            writeVec4ubArray(static_cast<const osg::Vec4ubArray*>(a));
            break;
        case osg::Array::FloatArrayType:
            writeChar((char)5);
            writeFloatArray(static_cast<const osg::FloatArray*>(a));
            break;
        case osg::Array::Vec2ArrayType:
            writeChar((char)6);
            writeVec2Array(static_cast<const osg::Vec2Array*>(a));
            break;
        case osg::Array::Vec3ArrayType:
            writeChar((char)7);
            writeVec3Array(static_cast<const osg::Vec3Array*>(a));
            break;
         case osg::Array::Vec4ArrayType:
            writeChar((char)8);
            writeVec4Array(static_cast<const osg::Vec4Array*>(a));
            break;
         case osg::Array::Vec2sArrayType:
             writeChar((char)9);
             writeVec2sArray(static_cast<const osg::Vec2sArray*>(a));
             break;
         case osg::Array::Vec3sArrayType:
             writeChar((char)10);
             writeVec3sArray(static_cast<const osg::Vec3sArray*>(a));
             break;
         case osg::Array::Vec4sArrayType:
             writeChar((char)11);
             writeVec4sArray(static_cast<const osg::Vec4sArray*>(a));
             break;
         case osg::Array::Vec2bArrayType:
             writeChar((char)12);
             writeVec2bArray(static_cast<const osg::Vec2bArray*>(a));
             break;
         case osg::Array::Vec3bArrayType:
             writeChar((char)13);
             writeVec3bArray(static_cast<const osg::Vec3bArray*>(a));
             break;
         case osg::Array::Vec4bArrayType:
             writeChar((char)14);
             writeVec4bArray(static_cast<const osg::Vec4bArray*>(a));
             break;
         case osg::Array::Vec2dArrayType:
             writeChar((char)15);
             writeVec2dArray(static_cast<const osg::Vec2dArray*>(a));
             break;
         case osg::Array::Vec3dArrayType:
             writeChar((char)16);
             writeVec3dArray(static_cast<const osg::Vec3dArray*>(a));
             break;
          case osg::Array::Vec4dArrayType:
             writeChar((char)17);
             writeVec4dArray(static_cast<const osg::Vec4dArray*>(a));
             break;
        default: throwException("Unknown array type in DataOutputStream::writeArray()");
    }
}


void DataOutputStream::writeIntArray(const osg::IntArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeInt(a->index(i));
    }

    if (_verboseOutput) std::cout<<"read/writeIntArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeUByteArray(const osg::UByteArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeChar((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeUByteArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeUShortArray(const osg::UShortArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeUShort((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeUShortArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeUIntArray(const osg::UIntArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeInt((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeUIntArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec4ubArray(const osg::Vec4ubArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeVec4ub((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeVec4ubArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writePackedFloatArray(const osg::FloatArray* a, float maxError)
{
    int size = a->getNumElements();
    writeInt(size);
    if (size==0) return;

    float minValue = (*a)[0];
    float maxValue = minValue;
    for(int i=1; i<size; ++i)
    {
        if ((*a)[i]<minValue) minValue = (*a)[i];
        if ((*a)[i]>maxValue) maxValue = (*a)[i];
    }

    if (minValue==maxValue)
    {
        OSG_DEBUG<<"Writing out "<<size<<" same values "<<minValue<<std::endl;

        writeBool(true);
        writeFloat(minValue);
        return;
    }

    writeBool(false);

    int packingSize = 4;
    if (maxError>0.0f)
    {

        //float byteError = 0.0f;
        float byteMultiplier = 255.0f/(maxValue-minValue);
        float byteInvMultiplier = 1.0f/byteMultiplier;

        //float shortError = 0.0f;
        float shortMultiplier = 65535.0f/(maxValue-minValue);
        float shortInvMultiplier = 1.0f/shortMultiplier;

        float max_error_byte = 0.0f;
        float max_error_short = 0.0f;

        for(int i=0; i<size; ++i)
        {
            float value = (*a)[i];
            unsigned char byteValue = (unsigned char)((value-minValue)*byteMultiplier);
            unsigned short shortValue = (unsigned short)((value-minValue)*shortMultiplier);
            float value_byte = minValue + float(byteValue)*byteInvMultiplier;
            float value_short = minValue + float(shortValue)*shortInvMultiplier;

            float error_byte = fabsf(value_byte - value);
            float error_short = fabsf(value_short - value);

            if (error_byte>max_error_byte) max_error_byte = error_byte;
            if (error_short>max_error_short) max_error_short = error_short;
        }

        OSG_DEBUG<<"maxError "<<maxError<<std::endl;
        OSG_DEBUG<<"Values to write "<<size<<" max_error_byte = "<<max_error_byte<<" max_error_short="<<max_error_short<<std::endl;


        if (max_error_byte < maxError) packingSize = 1;
        else if (max_error_short < maxError) packingSize = 2;

        OSG_DEBUG<<"packingSize "<<packingSize<<std::endl;

    }

    if (packingSize==1)
    {
        writeInt(1);

        writeFloat(minValue);
        writeFloat(maxValue);

        float byteMultiplier = 255.0f/(maxValue-minValue);

        for(int i=0; i<size; ++i)
        {
            unsigned char currentValue = (unsigned char)(((*a)[i]-minValue)*byteMultiplier);
            writeUChar(currentValue);
        }
    }
    else if (packingSize==2)
    {
        writeInt(2);

        writeFloat(minValue);
        writeFloat(maxValue);

        float shortMultiplier = 65535.0f/(maxValue-minValue);

        for(int i=0; i<size; ++i)
        {
            unsigned short currentValue = (unsigned short)(((*a)[i]-minValue)*shortMultiplier);
            writeUShort(currentValue);
        }
    }
    else
    {
        writeInt(4);

        for(int i=0; i<size; ++i)
        {
            writeFloat((*a)[i]);
        }

    }

    if (_verboseOutput) std::cout<<"read/writePackedFloatArray() ["<<size<<"]"<<std::endl;
}


void DataOutputStream::writeFloatArray(const osg::FloatArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeFloat((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeFloatArray() ["<<size<<"]"<<std::endl;
}


void DataOutputStream::writeVec2Array(const osg::Vec2Array* a)
{
    int size = a->size();
    writeInt(size);
    for(int i=0;i<size;i++){
        writeVec2((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeVec2Array() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec3Array(const osg::Vec3Array* a)
{
    int size = a->size();
    writeInt(size);
    for(int i = 0; i < size; i++){
        writeVec3((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeVec3Array() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec4Array(const osg::Vec4Array* a)
{
    int size = a->size();
    writeInt(size);
    for(int i=0;i<size;i++){
        writeVec4((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeVec4Array() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec2sArray(const osg::Vec2sArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeShort((*a)[i].x());
        writeShort((*a)[i].y());
    }

    if (_verboseOutput) std::cout<<"read/writeVec2sArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec3sArray(const osg::Vec3sArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeShort((*a)[i].x());
        writeShort((*a)[i].y());
        writeShort((*a)[i].z());
    }

    if (_verboseOutput) std::cout<<"read/writeVec3sArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec4sArray(const osg::Vec4sArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeShort((*a)[i].x());
        writeShort((*a)[i].y());
        writeShort((*a)[i].z());
        writeShort((*a)[i].w());
    }

    if (_verboseOutput) std::cout<<"read/writeVec4sArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec2bArray(const osg::Vec2bArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeVec2b((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeVec2bArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec3bArray(const osg::Vec3bArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeVec3b((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeVec3bArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec4bArray(const osg::Vec4bArray* a)
{
    int size = a->getNumElements();
    writeInt(size);
    for(int i =0; i<size ;i++){
        writeVec4b((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeVec4bArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec2dArray(const osg::Vec2dArray* a)
{
    int size = a->size();
    writeInt(size);
    for(int i=0;i<size;i++){
        writeVec2d((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeVec2dArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec3dArray(const osg::Vec3dArray* a)
{
    int size = a->size();
    writeInt(size);
    for(int i = 0; i < size; i++){
        writeVec3d((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeVec3dArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeVec4dArray(const osg::Vec4dArray* a)
{
    int size = a->size();
    writeInt(size);
    for(int i=0;i<size;i++){
        writeVec4d((*a)[i]);
    }

    if (_verboseOutput) std::cout<<"read/writeVec4dArray() ["<<size<<"]"<<std::endl;
}

void DataOutputStream::writeMatrixf(const osg::Matrixf& mat)
{
    for(int r=0;r<4;r++)
    {
        for(int c=0;c<4;c++)
        {
            writeFloat(mat(r,c));
        }
    }

    if (_verboseOutput) std::cout<<"read/writeMatrix() ["<<mat<<"]"<<std::endl;
}

void DataOutputStream::writeMatrixd(const osg::Matrixd& mat)
{
    for(int r=0;r<4;r++)
    {
        for(int c=0;c<4;c++)
        {
            writeDouble(mat(r,c));
        }
    }

    if (_verboseOutput) std::cout<<"read/writeMatrix() ["<<mat<<"]"<<std::endl;
}

void DataOutputStream::writeCollapseResult(const osg::EdgeCollapse::CollapseResult& collresult)
{
	//osg::EdgeCollapse::PointSet costpointset = collresult._costpointset;
	writePointSet(collresult._costpointset);

	//osg::EdgeCollapse::PointSet nocostpointset = collresult._nocostpointset;
	writePointSet(collresult._nocostpointset);

	//osg::EdgeCollapse::TriangleSet triangeset = collresult._triangleSet;
	writeTriangleSet(collresult._triangleSet);

	//osg::EdgeCollapse::EdgeCollapseInfoList cloresult = collresult._edgecollapseinfolist;
	writeCollInfoList(collresult._edgecollapseinfolist);

	writeInt(collresult._matid);
}

void DataOutputStream::writePointSet(const osg::EdgeCollapse::PointSet& pointset)
{
	writeInt(pointset.size());
	osg::EdgeCollapse::PointSet::iterator itr = pointset.begin();
	for (itr; itr!=pointset.end();++itr)
	{
		//osg::ref_ptr<osg::EdgeCollapse::Point> point = *itr;
		writePoint(*itr);
	}
}

void DataOutputStream::writePointSets(const osg::EdgeCollapse::PointSet& pointset)
{
	writeInt(pointset.size());

	std::vector<int> indeslist ;
	std::vector<osg::Vec3f> vertexlist;

	osg::EdgeCollapse::PointSet::iterator itr = pointset.begin();
	for (itr; itr!=pointset.end();++itr)
	{
		osg::ref_ptr<osg::EdgeCollapse::Point> point = *itr;
		writePoint(point);
	}
}

void DataOutputStream::writePoint(const osg::ref_ptr<osg::EdgeCollapse::Point>& point)
{
	if (point)
	{
		writeInt(point->_index);
		//point->_mincostpoint->_finalindex;

		writeVec3(point->_vertex);

		//osg::EdgeCollapse::FloatList floatlist = point->_attributes;
		writeInt(point->_attributes.size());
		for(size_t i = 0; i< point->_attributes.size();++i)
		{
			writeFloat(point->_attributes[i]);
		}
	}
	
}

void DataOutputStream::writeTriangleSet(const osg::EdgeCollapse::TriangleSet& triangletset)
{
	writeInt(triangletset.size());
	osg::EdgeCollapse::TriangleSet::iterator itr = triangletset.begin();
	for (itr;itr!=triangletset.end();++itr)
	{
		//osg::ref_ptr<osg::EdgeCollapse::Triangle> currentri = *itr;
		writeTriangle(*itr);
	}
}

void DataOutputStream::writeTriangle(const osg::ref_ptr<osg::EdgeCollapse::Triangle>& triangle)
{
	writeInt(triangle->_p1->_finalindex);
	writeInt(triangle->_p2->_finalindex);
	writeInt(triangle->_p3->_finalindex);
}

void DataOutputStream::writeCollInfoList(const osg::EdgeCollapse::EdgeCollapseInfoList& cloresult)
{
	writeInt(cloresult.size());
	for (size_t i=0;i<cloresult.size();++i)
	{
		//osg::EdgeCollapse::EdgeCollapseInfo edcollinfo = cloresult[i];
		writeInt(cloresult[i]._frompoint->_finalindex);

		writeInt(cloresult[i]._topoint->_finalindex);

		//osg::EdgeCollapse::TriangleList trilist = cloresult[i]._removetriangle;
		writeInt(cloresult[i]._removetriangle.size());
		for (size_t j = 0; j<cloresult[i]._removetriangle.size(); ++j)
		{
			writeInt(cloresult[i]._removetriangle[j]->_indexnumber);
		}
		
		//osg::EdgeCollapse::TriangleList afflist = cloresult[i]._affectedtriangle;
		writeInt(cloresult[i]._affectedtriangle.size());
		for (size_t j = 0; j<cloresult[i]._affectedtriangle.size(); ++j)
		{
			writeInt(cloresult[i]._affectedtriangle[j]->_indexnumber);
		}
	}
	
}

IncludeImageMode DataOutputStream::getIncludeImageMode(const osg::Image* image) const
{
    if (image)
    {
        if (image->getWriteHint()==osg::Image::STORE_INLINE)
        {
            return IMAGE_INCLUDE_DATA;
        }
        else if (image->getWriteHint()==osg::Image::EXTERNAL_FILE)
        {
            return IMAGE_REFERENCE_FILE;
        }
    }
    return getIncludeImageMode();
}

std::string DataOutputStream::getTextureFileNameForOutput()
{
    std::string fileName = osgDB::getNameLessExtension(_filename);
    if (_textureFileNameNumber>0)
    {
        std::ostringstream o;
        o << '_' << _textureFileNameNumber;
        fileName += o.str();
    }

    fileName += ".dds";
    ++_textureFileNameNumber;

    return fileName;
}



void DataOutputStream::setExternalFileWritten(const std::string& filename, bool hasBeenWritten)
{
    _externalFileWritten[filename] = hasBeenWritten;
}

bool DataOutputStream::getExternalFileWritten(const std::string& filename) const
{
    ExternalFileWrittenMap::const_iterator itr = _externalFileWritten.find(filename);
    if (itr != _externalFileWritten.end()) return itr->second;
    return false;
}
