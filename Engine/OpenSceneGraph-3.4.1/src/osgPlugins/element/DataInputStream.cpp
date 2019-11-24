/**********************************************************************
 *
 *    FILE:            DataInputStream.cpp
 *
 *    DESCRIPTION:    Implements methods to read simple datatypes from an
 *                    input stream.
 *
 *    CREATED BY:        Rune Schmidt Jensen
 *
 *    HISTORY:        Created 11.03.2003
 *                    Updated for texture1D by Don Burns, 27.1.2004
 *                      Updated for light model - Stan Blinov at 25 august 7512 from World Creation (7.09.2004)
 *
 *
 *    Copyright 2003 VR-C
 **********************************************************************/

#include "DataInputStream.h"
#include "Exception.h"
#include <osg/Endian>
#include <osg/Notify>
#include <osg/io_utils>
#include <osgDB/ReadFile>
#include <osgDB/FileNameUtils>

#include <stdio.h>
#include <sstream>

using namespace element;
using namespace std;

DataInputStream::DataInputStream(std::istream* istream, const osgDB::ReaderWriter::Options* options)
{
    unsigned int endianType ;

    _loadExternalReferenceFiles = false;

    _verboseOutput = false;

    _istream = istream;
    _owns_istream = false;
    _peeking = false;
    _peekValue = 0;
    _byteswap = 0;

    _options = options;

    if (_options.get())
    {
        setLoadExternalReferenceFiles(_options->getOptionString().find("noLoadExternalReferenceFiles")==std::string::npos);
        OSG_DEBUG << "ive::DataInputStream.setLoadExternalReferenceFiles()=" << getLoadExternalReferenceFiles() << std::endl;
    }

    if(!istream){
        throwException("DataInputStream::DataInputStream(): null pointer exception in argument.");
    }

    endianType = readUInt() ;

    if ( endianType != ENDIAN_TYPE) {
      // Make sure the file is simply swapped
      if ( endianType != OPPOSITE_ENDIAN_TYPE ) {
         throwException("DataInputStream::DataInputStream(): This file has an unreadable endian type.") ;
      }
      OSG_INFO<<"DataInputStream::DataInputStream: Reading a byteswapped file" << std::endl ;
      _byteswap = 1 ;
   }

    _version = readUInt();

    // Are we trying to open a binary .ive file which version are newer than this library.
    if(_version>VERSION){
        throwException("DataInputStream::DataInputStream(): The version found in the file is newer than this library can handle.");
    }

    if (_version>=VERSION_0033)
    {
        int compressionLevel = readInt();

        if (compressionLevel>0)
        {
            OSG_INFO<<"compressed ive stream"<<std::endl;

            unsigned int maxSize = readUInt();

            std::string data;
            data.reserve(maxSize);

            if (!uncompress(*istream, data))
            {
                throwException("Error in uncompressing .ive");
                return;
            }

            _istream = new std::stringstream(data);
            _owns_istream = true;
        }
        else
        {
            OSG_INFO<<"uncompressed ive stream"<<std::endl;
        }
    }
}

DataInputStream::~DataInputStream()
{
    if (_owns_istream) delete _istream;
}

#ifdef USE_ZLIB

#include <zlib.h>

bool DataInputStream::uncompress(std::istream& fin, std::string& destination) const
{
    //#define CHUNK 16384
    #define CHUNK 32768

    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm,
                       15 + 32 // autodected zlib or gzip header
                       );
    if (ret != Z_OK)
    {
        OSG_INFO<<"failed to init"<<std::endl;
        return ret != 0;
    }

    /* decompress until deflate stream ends or end of file */
    do {
        //strm.avail_in = fin.readsome((char*)in, CHUNK);
        fin.read((char *)in, CHUNK);
        strm.avail_in = fin.gcount();

        if (strm.avail_in == 0)
        {
            break;
        }
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);

            switch (ret) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return false;
            }
            have = CHUNK - strm.avail_out;

            destination.append((char*)out, have);

        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);

    return ret == Z_STREAM_END ? true : false;
}
#else
bool DataInputStream::uncompress(std::istream& fin, std::string& destination) const
{
    return false;
}
#endif

bool DataInputStream::readBool(){
    char c=0;
    _istream->read(&c, CHARSIZE);

    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readBool(): Failed to read boolean value.");

    if (_verboseOutput) std::cout<<"read/writeBool() ["<<(int)c<<"]"<<std::endl;

    return c!=0;
}

char DataInputStream::readChar(){
    char c=0;
    _istream->read(&c, CHARSIZE);

    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readChar(): Failed to read char value.");

    if (_verboseOutput) std::cout<<"read/writeChar() ["<<(int)c<<"]"<<std::endl;

    return c;
}

unsigned char DataInputStream::readUChar(){
    unsigned char c=0;
    _istream->read((char*)&c, CHARSIZE);

    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readUChar(): Failed to read unsigned char value.");

    if (_verboseOutput) std::cout<<"read/writeUChar() ["<<(int)c<<"]"<<std::endl;

    return c;
}

unsigned short DataInputStream::readUShort(){
    unsigned short s=0;
    _istream->read((char*)&s, SHORTSIZE);
    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readUShort(): Failed to read unsigned short value.");

    if (_verboseOutput) std::cout<<"read/writeUShort() ["<<s<<"]"<<std::endl;

    if (_byteswap) osg::swapBytes((char *)&s,SHORTSIZE);

    return s;
}

unsigned int DataInputStream::readUInt(){
    unsigned int s=0;
    _istream->read((char*)&s, INTSIZE);

    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readUInt(): Failed to read unsigned int value.");

    if (_byteswap) osg::swapBytes((char *)&s,INTSIZE) ;

    if (_verboseOutput) std::cout<<"read/writeUInt() ["<<s<<"]"<<std::endl;

    return s;
}

int DataInputStream::readInt(){
    if(_peeking){
        _peeking = false;
        return _peekValue;
    }
    int i=0;
    _istream->read((char*)&i, INTSIZE);

    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readInt(): Failed to read int value.");

    if (_byteswap) osg::swapBytes((char *)&i,INTSIZE) ;

    if (_verboseOutput) std::cout<<"read/writeInt() ["<<i<<"]"<<std::endl;

    return i;
}

/**
 * Read an integer from the stream, but
 * save it such that the next readInt call will
 * return the same integer.
 */
int DataInputStream::peekInt(){
    if(_peeking){
        return _peekValue;
    }
    _peekValue  = readInt();
    _peeking = true;
    return _peekValue;
}

float DataInputStream::readFloat(){
    float f=0.0f;
    _istream->read((char*)&f, FLOATSIZE);
    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readFloat(): Failed to read float value.");

    if (_byteswap) osg::swapBytes((char *)&f,FLOATSIZE) ;

    if (_verboseOutput) std::cout<<"read/writeFloat() ["<<f<<"]"<<std::endl;
    return f;
}

long DataInputStream::readLong(){
    long l=0;
    _istream->read((char*)&l, LONGSIZE);
    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readLong(): Failed to read long value.");

    if (_byteswap) osg::swapBytes((char *)&l,LONGSIZE) ;

    if (_verboseOutput) std::cout<<"read/writeLong() ["<<l<<"]"<<std::endl;
    return l;
}

unsigned long DataInputStream::readULong(){
    unsigned long l=0;
    _istream->read((char*)&l, LONGSIZE);
    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readULong(): Failed to read unsigned long value.");

    if (_byteswap) osg::swapBytes((char *)&l,LONGSIZE) ;

    if (_verboseOutput) std::cout<<"read/writeULong() ["<<l<<"]"<<std::endl;

    return l;
}

double DataInputStream::readDouble()
{
    double d=0.0;
    _istream->read((char*)&d, DOUBLESIZE);
    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readDouble(): Failed to read double value.");

    if (_byteswap) osg::swapBytes((char *)&d,DOUBLESIZE) ;
    if (_verboseOutput) std::cout<<"read/writeDouble() ["<<d<<"]"<<std::endl;
    return d;
}

std::string DataInputStream::readString()
{
    std::string s;
    int size = readInt();
    if (size != 0)
    {
        s.resize(size);
        _istream->read((char*)s.c_str(), size);
        //if (_istream->rdstate() & _istream->failbit)
        //   throwException("DataInputStream::readString(): Failed to read string value.");

        if (_verboseOutput) std::cout<<"read/writeString() ["<<s<<"]"<<std::endl;
    }

    return s;
}

void DataInputStream::readCharArray(char* data, int size)
{
    _istream->read(data, size);
    if (_istream->rdstate() & _istream->failbit)
        throwException("DataInputStream::readCharArray(): Failed to read char value.");

    if (_verboseOutput) std::cout<<"read/writeCharArray() ["<<data<<"]"<<std::endl;
}

osg::Vec2 DataInputStream::readVec2()
{
    osg::Vec2 v;
    v.x()=readFloat();
    v.y()=readFloat();

    if (_verboseOutput) std::cout<<"read/writeVec2() ["<<v<<"]"<<std::endl;

    return v;
}

osg::Vec3 DataInputStream::readVec3(){
    osg::Vec3 v;
    v.x()=readFloat();
    v.y()=readFloat();
    v.z()=readFloat();

    if (_verboseOutput) std::cout<<"read/writeVec3() ["<<v<<"]"<<std::endl;

    return v;
}

osg::Vec4 DataInputStream::readVec4(){
    osg::Vec4 v;
    v.x()=readFloat();
    v.y()=readFloat();
    v.z()=readFloat();
    v.w()=readFloat();

    if (_verboseOutput) std::cout<<"read/writeVec4() ["<<v<<"]"<<std::endl;

    return v;
}
osg::Vec2d DataInputStream::readVec2d()
{
    osg::Vec2d v;
    v.x()=readDouble();
    v.y()=readDouble();

    if (_verboseOutput) std::cout<<"read/writeVec2d() ["<<v<<"]"<<std::endl;

    return v;
}

osg::Vec3d DataInputStream::readVec3d(){
    osg::Vec3d v;
    v.x()=readDouble();
    v.y()=readDouble();
    v.z()=readDouble();

    if (_verboseOutput) std::cout<<"read/writeVec3d() ["<<v<<"]"<<std::endl;

    return v;
}

osg::Vec4d DataInputStream::readVec4d(){
    osg::Vec4d v;
    v.x()=readDouble();
    v.y()=readDouble();
    v.z()=readDouble();
    v.w()=readDouble();

    if (_verboseOutput) std::cout<<"read/writeVec4d() ["<<v<<"]"<<std::endl;

    return v;
}

osg::Plane DataInputStream::readPlane(){
    osg::Plane v;

    if (getVersion() <= VERSION_0018)
    {
        v[0]=readFloat();
        v[1]=readFloat();
        v[2]=readFloat();
        v[3]=readFloat();
    }
    else
    {
        // assume double for planes even if Plane::value_type is float
        // to ensure that the .ive format does vary.
        v[0]=readDouble();
        v[1]=readDouble();
        v[2]=readDouble();
        v[3]=readDouble();
    }

    if (_verboseOutput) std::cout<<"read/writePlane() ["<<v<<"]"<<std::endl;

    return v;
}

osg::Vec4ub DataInputStream::readVec4ub(){
    osg::Vec4ub v;
    v.r()=readChar();
    v.g()=readChar();
    v.b()=readChar();
    v.a()=readChar();

    if (_verboseOutput) std::cout<<"read/writeVec4ub() ["<<v<<"]"<<std::endl;

    return v;
}

osg::Quat DataInputStream::readQuat(){
    osg::Quat q;
    q.x()=readFloat();
    q.y()=readFloat();
    q.z()=readFloat();
    q.w()=readFloat();

    if (_verboseOutput) std::cout<<"read/writeQuat() ["<<q<<"]"<<std::endl;

    return q;
}




deprecated_osg::Geometry::AttributeBinding DataInputStream::readBinding(){
    char c = readChar();

    if (_verboseOutput) std::cout<<"readBinding() ["<<(int)c<<"]"<<std::endl;

    switch((int)c){
        case 0:    return deprecated_osg::Geometry::BIND_OFF;
        case 1: return deprecated_osg::Geometry::BIND_OVERALL;
        case 2: return deprecated_osg::Geometry::BIND_PER_PRIMITIVE;
        case 3: return deprecated_osg::Geometry::BIND_PER_PRIMITIVE_SET;
        case 4: return deprecated_osg::Geometry::BIND_PER_VERTEX;
        default:
            throwException("Unknown binding type in DataInputStream::readBinding()");
            return deprecated_osg::Geometry::BIND_OFF;
    }
}

osg::Array* DataInputStream::readArray(){
    char c = readChar();
    switch((int)c){
        case 0: return readIntArray();
        case 1: return readUByteArray();
        case 2: return readUShortArray();
        case 3: return readUIntArray();
        case 4: return readVec4ubArray();
        case 5: return readFloatArray();
        case 6:    return readVec2Array();
        case 7:    return readVec3Array();
        case 8:    return readVec4Array();
        case 9:    return readVec2sArray();
        case 10:   return readVec3sArray();
        case 11:   return readVec4sArray();
        case 12:   return readVec2bArray();
        case 13:   return readVec3bArray();
        case 14:   return readVec4bArray();
        case 15:   return readVec2dArray();
        case 16:   return readVec3dArray();
        case 17:   return readVec4dArray();
        default:
            throwException("Unknown array type in DataInputStream::readArray()");
            return 0;
    }
}

osg::IntArray* DataInputStream::readIntArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::IntArray> a = new osg::IntArray(size);

    _istream->read((char*)&((*a)[0]), INTSIZE*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readIntArray(): Failed to read Int array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeIntArray() ["<<size<<"]"<<std::endl;

    if (_byteswap) {
       for (int  i = 0 ; i < size ; i++ ) osg::swapBytes((char *)&((*a)[i]),INTSIZE) ;
    }

    return a.release();
}

osg::UByteArray* DataInputStream::readUByteArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::UByteArray> a = new osg::UByteArray(size);

    _istream->read((char*)&((*a)[0]), CHARSIZE*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readUByteArray(): Failed to read UByte array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeUByteArray() ["<<size<<"]"<<std::endl;

    return a.release();
}

osg::UShortArray* DataInputStream::readUShortArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::UShortArray> a = new osg::UShortArray(size);

    _istream->read((char*)&((*a)[0]), SHORTSIZE*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readUShortArray(): Failed to read UShort array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeUShortArray() ["<<size<<"]"<<std::endl;

    if (_byteswap)
    {
        for (int i = 0 ; i < size ; i++ )
            osg::swapBytes((char *)&((*a)[i]),SHORTSIZE) ;
    }
    return a.release();
}

osg::UIntArray* DataInputStream::readUIntArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::UIntArray> a = new osg::UIntArray(size);

    _istream->read((char*)&((*a)[0]), INTSIZE*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readUIntArray(): Failed to read UInt array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeUIntArray() ["<<size<<"]"<<std::endl;

    if (_byteswap)
    {
        for (int i = 0 ; i < size ; i++ )
            osg::swapBytes((char *)&((*a)[i]),INTSIZE) ;
    }
    return a.release();
}

osg::Vec4ubArray* DataInputStream::readVec4ubArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec4ubArray> a = new osg::Vec4ubArray(size);

    _istream->read((char*)&((*a)[0]), INTSIZE*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec4ubArray(): Failed to read Vec4ub array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec4ubArray() ["<<size<<"]"<<std::endl;

    return a.release();
}

bool DataInputStream::readPackedFloatArray(osg::FloatArray* a)
{
    int size = readInt();

    a->resize(size);

    if (size == 0)
        return true;

    if (readBool())
    {
        float value = readFloat();

        for(int i=0; i<size; ++i)
        {
            (*a)[i] = value;
        }
    }
    else
    {
        int packingSize = readInt();

        if (packingSize==1)
        {
            float minValue = readFloat();
            float maxValue = readFloat();

            float byteMultiplier = 255.0f/(maxValue-minValue);
            float byteInvMultiplier = 1.0f/byteMultiplier;

            for(int i=0; i<size; ++i)
            {
                unsigned char byte_value = readUChar();
                float value = minValue + float(byte_value)*byteInvMultiplier;
                (*a)[i] = value;
            }
        }
        else if (packingSize==2)
        {
            float minValue = readFloat();
            float maxValue = readFloat();

            float shortMultiplier = 65535.0f/(maxValue-minValue);
            float shortInvMultiplier = 1.0f/shortMultiplier;

            for(int i=0; i<size; ++i)
            {
                unsigned short short_value = readUShort();
                float value = minValue + float(short_value)*shortInvMultiplier;
                (*a)[i] = value;
            }
        }
        else
        {
            for(int i=0; i<size; ++i)
            {
                (*a)[i] = readFloat();
            }
        }
    }

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readFloatArray(): Failed to read float array.");
        return false;
    }

    if (_verboseOutput) std::cout<<"read/writeFloatArray() ["<<size<<"]"<<std::endl;

    return true;
}


osg::FloatArray* DataInputStream::readFloatArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::FloatArray> a = new osg::FloatArray(size);

    _istream->read((char*)&((*a)[0]), FLOATSIZE*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readFloatArray(): Failed to read float array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeFloatArray() ["<<size<<"]"<<std::endl;

    if (_byteswap)
    {
        for (int i = 0 ; i < size ; i++ )
            osg::swapBytes((char *)&((*a)[i]),FLOATSIZE) ;
    }
    return a.release();
}

osg::Vec2Array* DataInputStream::readVec2Array()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec2Array> a = new osg::Vec2Array(size);

    _istream->read((char*)&((*a)[0]), FLOATSIZE*2*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec2Array(): Failed to read Vec2 array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec2Array() ["<<size<<"]"<<std::endl;

    if (_byteswap)
    {
       float *ptr = (float*)&((*a)[0]) ;
       for (int i = 0 ; i < size*2 ; i++ )
       {
          osg::swapBytes((char *)&(ptr[i]), FLOATSIZE) ;
       }
    }
    return a.release();
}

osg::Vec3Array* DataInputStream::readVec3Array()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec3Array> a = new osg::Vec3Array(size);

    _istream->read((char*)&((*a)[0]), FLOATSIZE*3*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec3Array(): Failed to read Vec3 array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec3Array() ["<<size<<"]"<<std::endl;


    if (_byteswap)
    {
       float *ptr = (float*)&((*a)[0]) ;
       for (int i = 0 ; i < size*3 ; i++ )
       {
          osg::swapBytes((char *)&(ptr[i]),FLOATSIZE) ;
       }
    }
    return a.release();
}

osg::Vec4Array* DataInputStream::readVec4Array(){
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec4Array> a = new osg::Vec4Array(size);

    _istream->read((char*)&((*a)[0]), FLOATSIZE*4*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec4Array(): Failed to read Vec4 array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec4Array() ["<<size<<"]"<<std::endl;

    if (_byteswap) {
       float *ptr = (float*)&((*a)[0]) ;
       for (int i = 0 ; i < size*4 ; i++ ) {
          osg::swapBytes((char *)&(ptr[i]),FLOATSIZE) ;
       }
    }
    return a.release();
}

osg::Vec2bArray* DataInputStream::readVec2bArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec2bArray> a = new osg::Vec2bArray(size);

    _istream->read((char*)&((*a)[0]), CHARSIZE * 2 * size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec2bArray(): Failed to read Vec2b array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec2bArray() ["<<size<<"]"<<std::endl;

    return a.release();
}

osg::Vec3bArray* DataInputStream::readVec3bArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec3bArray> a = new osg::Vec3bArray(size);

    _istream->read((char*)&((*a)[0]), CHARSIZE * 3 * size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec3bArray(): Failed to read Vec3b array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec3bArray() ["<<size<<"]"<<std::endl;

    return a.release();
}

osg::Vec4bArray* DataInputStream::readVec4bArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec4bArray> a = new osg::Vec4bArray(size);

    _istream->read((char*)&((*a)[0]), CHARSIZE * 4 * size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec4bArray(): Failed to read Vec4b array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec4bArray() ["<<size<<"]"<<std::endl;

    return a.release();
}

osg::Vec2sArray* DataInputStream::readVec2sArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec2sArray> a = new osg::Vec2sArray(size);

    _istream->read((char*)&((*a)[0]), SHORTSIZE * 2 * size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec2sArray(): Failed to read Vec2s array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec2sArray() ["<<size<<"]"<<std::endl;

    if (_byteswap)
    {
       short *ptr = (short*)&((*a)[0]) ;
       for (int i = 0 ; i < size*2 ; i++ )
       {
          osg::swapBytes((char *)&(ptr[i]), SHORTSIZE) ;
       }
    }

    return a.release();
}

osg::Vec3sArray* DataInputStream::readVec3sArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec3sArray> a = new osg::Vec3sArray(size);

    _istream->read((char*)&((*a)[0]), SHORTSIZE * 3 * size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec3sArray(): Failed to read Vec3s array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec3sArray() ["<<size<<"]"<<std::endl;


    if (_byteswap)
    {
       short *ptr = (short*)&((*a)[0]) ;
       for (int i = 0 ; i < size*3 ; i++ )
       {
          osg::swapBytes((char *)&(ptr[i]), SHORTSIZE) ;
       }
    }

    return a.release();
}

osg::Vec4sArray* DataInputStream::readVec4sArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec4sArray> a = new osg::Vec4sArray(size);

    _istream->read((char*)&((*a)[0]), SHORTSIZE * 4 * size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec4sArray(): Failed to read Vec4s array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec4sArray() ["<<size<<"]"<<std::endl;

    if (_byteswap)
    {
       short *ptr = (short*)&((*a)[0]) ;
       for (int i = 0 ; i < size*4 ; i++ )
       {
          osg::swapBytes((char *)&(ptr[i]), SHORTSIZE) ;
       }
    }

    return a.release();
}

osg::Vec2dArray* DataInputStream::readVec2dArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec2dArray> a = new osg::Vec2dArray(size);

    _istream->read((char*)&((*a)[0]), DOUBLESIZE*2*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec2dArray(): Failed to read Vec2d array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec2dArray() ["<<size<<"]"<<std::endl;

    if (_byteswap)
    {
       double *ptr = (double*)&((*a)[0]) ;
       for (int i = 0 ; i < size*2 ; i++ )
       {
          osg::swapBytes((char *)&(ptr[i]), DOUBLESIZE) ;
       }
    }
    return a.release();
}

osg::Vec3dArray* DataInputStream::readVec3dArray()
{
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec3dArray> a = new osg::Vec3dArray(size);

    _istream->read((char*)&((*a)[0]), DOUBLESIZE*3*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec3dArray(): Failed to read Vec3d array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec3dArray() ["<<size<<"]"<<std::endl;


    if (_byteswap)
    {
       double *ptr = (double*)&((*a)[0]) ;
       for (int i = 0 ; i < size*3 ; i++ )
       {
          osg::swapBytes((char *)&(ptr[i]),DOUBLESIZE) ;
       }
    }
    return a.release();
}

osg::Vec4dArray* DataInputStream::readVec4dArray(){
    int size = readInt();
    if (size == 0)
        return NULL;

    osg::ref_ptr<osg::Vec4dArray> a = new osg::Vec4dArray(size);

    _istream->read((char*)&((*a)[0]), DOUBLESIZE*4*size);

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readVec4dArray(): Failed to read Vec4d array.");
        return 0;
    }

    if (_verboseOutput) std::cout<<"read/writeVec4dArray() ["<<size<<"]"<<std::endl;

    if (_byteswap) {
       double *ptr = (double*)&((*a)[0]) ;
       for (int i = 0 ; i < size*4 ; i++ ) {
          osg::swapBytes((char *)&(ptr[i]),DOUBLESIZE) ;
       }
    }
    return a.release();
}

osg::Matrixf DataInputStream::readMatrixf()
{
    osg::Matrixf mat;
    for(int r=0;r<4;r++)
    {
        for(int c=0;c<4;c++)
        {
            mat(r,c) = readFloat();
        }
    }

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readMatrix(): Failed to read Matrix array.");
        return osg::Matrixf();
    }

    if (_verboseOutput) std::cout<<"read/writeMatrix() ["<<mat<<"]"<<std::endl;


    return mat;
}

osg::Matrixd DataInputStream::readMatrixd()
{
    osg::Matrixd mat;
    for(int r=0;r<4;r++)
    {
        for(int c=0;c<4;c++)
        {
            mat(r,c) = readDouble();
        }
    }

    if (_istream->rdstate() & _istream->failbit)
    {
        throwException("DataInputStream::readMatrix(): Failed to read Matrix array.");
        return osg::Matrixd();
    }

    if (_verboseOutput) std::cout<<"read/writeMatrix() ["<<mat<<"]"<<std::endl;


    return mat;
}

osg::EdgeCollapse::CollapseResult DataInputStream::readCollapseResult()
{
	osg::EdgeCollapse::CollapseResult colresult;

	osg::EdgeCollapse::PointList pointlist;
	colresult._costpointset = readPointSet(pointlist);
	//colresult._costpointlist = pointlist;

	osg::EdgeCollapse::TriangleList trianglelist;
	colresult._triangleSet = readTriangleSet(pointlist,trianglelist);

	colresult._edgecollapseinfolist = readEdgeCollapseInfoList(pointlist,trianglelist);

	colresult._matid = readInt();
	colresult._haschanged = false;

	return colresult;
}

osg::EdgeCollapse::PointSet DataInputStream::readPointSet(osg::EdgeCollapse::PointList& pointlist)
{
	osg::EdgeCollapse::PointSet pointset;
	int pointnumber = readInt();
	for (int i=0;i<pointnumber;++i)
	{
		osg::ref_ptr<osg::EdgeCollapse::Point> point = readPoint();
		point->_cost =i;
		pointset.insert(point);
		pointlist.push_back(point);
	}
	
	int nocostpointnumber = readInt();
	for (int i=0;i<nocostpointnumber;++i)
	{
		osg::ref_ptr<osg::EdgeCollapse::Point> point = readPoint();
		point->_cost = pointnumber+i;
		pointset.insert(point);
		pointlist.push_back(point);
	}

	return pointset;
}

osg::ref_ptr<osg::EdgeCollapse::Point> DataInputStream::readPoint()
{
	osg::ref_ptr<osg::EdgeCollapse::Point> point = new osg::EdgeCollapse::Point;
	point->_index = readInt();

	point->_vertex = readVec3();

	//osg::EdgeCollapse::FloatList floatlist;
	int floatlistnum = readInt();
	for(size_t i = 0; i< floatlistnum;++i)
	{
		point->_attributes.push_back(readFloat());
	}
	//point->_attributes = floatlist;

	return point;
}

osg::EdgeCollapse::TriangleSet DataInputStream::readTriangleSet(osg::EdgeCollapse::PointList pointlist,osg::EdgeCollapse::TriangleList& trianglelist)
{
	osg::EdgeCollapse::TriangleSet triangleset;

	int trianglenumber = readInt();
	for(size_t i = 0; i< trianglenumber;i++)
	{
		osg::ref_ptr<osg::EdgeCollapse::Triangle> currentri = readTriangle(pointlist);
		triangleset.insert(currentri);
		trianglelist.push_back(currentri);
	}

	return triangleset;
}

osg::ref_ptr<osg::EdgeCollapse::Triangle> DataInputStream::readTriangle(osg::EdgeCollapse::PointList pointlist)
{
	osg::ref_ptr<osg::EdgeCollapse::Triangle> currentri = new osg::EdgeCollapse::Triangle;

	currentri->_p1 = pointlist[readInt()];
	currentri->_p2 = pointlist[readInt()];
	currentri->_p3 = pointlist[readInt()];

	return currentri;
}

osg::EdgeCollapse::EdgeCollapseInfoList  DataInputStream::readEdgeCollapseInfoList(osg::EdgeCollapse::PointList& pointlist,osg::EdgeCollapse::TriangleList& trianglelist)
{
	osg::EdgeCollapse::EdgeCollapseInfoList edgelist;
	int listnumber = readInt();
	int tranglesnumber = trianglelist.size();
	for (size_t i=0;i<listnumber;++i)
	{
		osg::EdgeCollapse::EdgeCollapseInfo edcollinfo;
		edcollinfo._frompoint = pointlist[readInt()];

		edcollinfo._topoint = pointlist[readInt()];

		int removetrinum = readInt();
		for (size_t j = 0; j<removetrinum; j++)
		{
			int indexnumber = readInt();
			if (indexnumber>=0&&indexnumber<tranglesnumber)
			{
				edcollinfo._removetriangle.push_back(trianglelist[indexnumber]);
			}
			
		}

		int afftrinum = readInt();
		for (size_t j = 0; j<afftrinum; j++)
		{
			int indexnumber = readInt();
			if (indexnumber>=0&&indexnumber<tranglesnumber)
			{
				edcollinfo._affectedtriangle.push_back(trianglelist[indexnumber]);
			}
		}

		edgelist.push_back(edcollinfo);
	}
	
	return edgelist;
}