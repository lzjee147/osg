

namespace cn
{



//FileZipIO
FileZipIO::FileZipIO(const char* pFileName,const char* pMode):
m_uZipFileOpenMode(0),
m_hZipReadHandle(NULL),
m_hZipWriteHandle(NULL),
m_sCurrOffsetPos(0)
{
	m_vOriginalBuffer.resize(0);

	if(::strchr(pMode,'a') != NULL){
		throw 1;
	}else if(::strchr(pMode,'r') != NULL){
		m_uZipFileOpenMode = 0;
		m_hZipReadHandle = unzOpen(pFileName);	//open zip file
		if(m_hZipReadHandle == NULL){
			throw 1;
		}
		unz_global_info64 gi;
		if (unzGetGlobalInfo64(m_hZipReadHandle, &gi) == UNZ_OK && gi.number_entry == 1){
			  //打开zip文档内的文件，句柄指向了内部文件，为接下来解压读取文件做准备
			unz_file_info64 unzfi;
			if(unzGetCurrentFileInfo64(m_hZipReadHandle,&unzfi,NULL,0,NULL,0,NULL,0) != UNZ_OK){
				throw 1;
			}
			if(unzOpenCurrentFile(m_hZipReadHandle)!=UNZ_OK){
				throw 1;
			}else{
				m_vOriginalBuffer.resize((cn::u32)unzfi.uncompressed_size);
				if(!m_vOriginalBuffer.empty()){
					BYTE *pData =  (BYTE*)&m_vOriginalBuffer[0];
					int nSize = unzReadCurrentFile(m_hZipReadHandle, pData, (cn::u32)unzfi.uncompressed_size);
					if(nSize < 0){
						throw 1;
					}
				}
				m_sCurrOffsetPos = 0;
			}
		}else{
			throw 1;
		}		
	}else if(::strchr(pMode,'w') != NULL ){	
		cn::u32 uDireNum= 0;		const char* pcstrPureFileName=NULL;	
		const char** ppAllPaths = GetPathHierarchy(pFileName,uDireNum,pcstrPureFileName);
		if(pcstrPureFileName == NULL){
			throw 1;
		}		
		m_uZipFileOpenMode = 1;
		m_hZipWriteHandle = zipOpen(pFileName,0);
		if(m_hZipWriteHandle == NULL){
			throw 1;
		}
		zip_fileinfo FileInfo;	ZeroMemory(&FileInfo, sizeof(FileInfo));
		cn::u32 uFileNameLen = strlen(pcstrPureFileName);
		std::string strInnerFileName(pcstrPureFileName,pcstrPureFileName+uFileNameLen-4);//内部文件名去掉ZipFileIO内部加上的.zip后缀
		if (zipOpenNewFileInZip(m_hZipWriteHandle, strInnerFileName.c_str(), &FileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9) != ZIP_OK){
			throw 1;
		}
	}else{
		throw 1;
	}

}

//析构函数
FileZipIO::~FileZipIO()
{
	try
	{
		Close();
	}
	catch (...)
	{
		cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR,
			"FileZipIO::~FileZipIO some error happened when close zip file handle ");
	}
}

//寻址到Zip解压出的内存缓冲区的某个位置上(出错会throw)
void FileZipIO::Seek(SEEK_ORIG eSeekOrig, s64 sOffset)
{
	//seek都是针对内存缓冲内的数据进行
	cn::s64 sPointerOffset = 0;
	if(eSeekOrig == SEEK_ORIG_BEGIN){
		sPointerOffset = sOffset;
	}else if(eSeekOrig == SEEK_ORIG_CUR){
		sPointerOffset = (cn::s64)m_sCurrOffsetPos + sOffset;
	}else if(eSeekOrig == SEEK_ORIG_END){
		sPointerOffset = (cn::s64)m_vOriginalBuffer.size()-1+sOffset;
	}else{
		throw 1;
	}
	if(sPointerOffset < 0){
		throw 1;
	}
	if(sPointerOffset > m_vOriginalBuffer.size()-1){
		m_vOriginalBuffer.resize((cn::u32)sPointerOffset + 1);
	}
	m_sCurrOffsetPos = (cn::u32)sPointerOffset;
	
}

//获取当前访问位置(出错会throw)
cn::s64 FileZipIO::GetCurPos()
{
	return m_sCurrOffsetPos;
}

//获取Zip文件解压后的缓冲区长度(出错会throw)
cn::s64 FileZipIO::GetLength()
{
	return m_vOriginalBuffer.size();
}

//将内部未压缩数据缓冲区中的数据全部清空并压缩后写入到zip文件中(出错会throw)
void FileZipIO::Flush()
{
	if(m_uZipFileOpenMode == 1){
		
		//数据写入zip文件
		if(!m_vOriginalBuffer.empty()){
			if (zipWriteInFileInZip(m_hZipWriteHandle, &m_vOriginalBuffer[0], m_vOriginalBuffer.size()) < 0){
				throw 1;
			}
		}
		//清空Buffer
		m_vOriginalBuffer.resize(0);
		m_sCurrOffsetPos=0;

	}else{
		throw 1;
	}
}



//显式关闭Zip文件(出错会throw)
void FileZipIO::Close()
{
	if(0 == m_uZipFileOpenMode){
		if(UNZ_OK == unzClose(m_hZipReadHandle)){
			m_hZipReadHandle = NULL;
			m_sCurrOffsetPos=0;
			m_vOriginalBuffer.resize(0);
		}else{
			throw 1;
		}		
	}else if(1 == m_uZipFileOpenMode){
		if(m_vOriginalBuffer.size()!=0){
			Flush();
		}
		if(ZIP_OK ==zipClose(m_hZipWriteHandle,(const char*)NULL)){
			m_hZipWriteHandle = NULL;
		}else{
			throw 1;
		}	
	}

}

//表示写入一个结构类型数据的宏
#define _WRITE_TO_DATABUF(type,object,databuf,opPosition) \
	if(opPosition>=0){\
		if(opPosition + sizeof(type) > databuf.size()){ \
			databuf.resize(opPosition + sizeof(type));\
		}\
		if(!databuf.empty()){\
			BYTE *pData = (BYTE *)&databuf[0];\
			::memcpy(pData+opPosition,(BYTE*)&object,sizeof(type));\
			opPosition += sizeof(type);\
		}\
	}else{\
		throw 1;\
	}

//表示读取一个结构类型数据的宏
#define _READ_FROM_DATABUF(type,databuf,opPosition) \
	if(opPosition>=0){\
		if(opPosition + sizeof(type) > databuf.size()){ \
			throw 1;\
		}else{\
			type TempRet; memset(&TempRet,0,sizeof(type)); \
			if(!databuf.empty()){\
				BYTE *pData = (BYTE*)&databuf[0];\
				::memcpy(&TempRet, pData+opPosition,sizeof(type)); \
				opPosition += sizeof(type);\
			}\
			return TempRet; \
		}\
	}else{ \
		throw 1; \
	}
	
	
	

//以下为数据访问相关接口，这些接口出错会throw

void FileZipIO::WriteFloat(float fData)
{
	_WRITE_TO_DATABUF(float,fData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


float FileZipIO::ReadFloat()
{
	_READ_FROM_DATABUF(float,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteDouble(double dData)
{
	_WRITE_TO_DATABUF(double,dData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


double FileZipIO::ReadDouble()
{
	_READ_FROM_DATABUF(double,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteString(const char *pStr, s32 sStrLen)
{
	if(m_sCurrOffsetPos>=0){
		cn::s32 sNewStrLen =0;
		const WCHAR *pNewStr =cn::Literal::CharToWChar(pStr, sStrLen, &sNewStrLen);
		sNewStrLen =-sNewStrLen;
		if(m_sCurrOffsetPos + sizeof(s32)+sizeof(WCHAR)*(-sNewStrLen) > m_vOriginalBuffer.size()){
			m_vOriginalBuffer.resize(m_sCurrOffsetPos + sizeof(s32)+sizeof(WCHAR)*(-sNewStrLen));
		}
		if(!m_vOriginalBuffer.empty()){
			BYTE *pData = (BYTE*)&m_vOriginalBuffer[0];
			::memcpy(pData + m_sCurrOffsetPos,&sNewStrLen,sizeof(s32));		m_sCurrOffsetPos+=sizeof(s32);
			::memcpy(pData + m_sCurrOffsetPos,pNewStr,sizeof(WCHAR)*(-sNewStrLen));		m_sCurrOffsetPos+=sizeof(WCHAR)*(-sNewStrLen);
		}
	}else{ 
		throw 1;
	}
}


void FileZipIO::ReadString(Buffer/*char*/ &bufData)
{
	WCHAR *g_pTempStrBuf2 =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempStrBuf2;

	if(m_vOriginalBuffer.size() != 0){
		cn::s32 sStrLen;
		if(m_sCurrOffsetPos + sizeof(cn::s32) > m_vOriginalBuffer.size()){
			throw 1;
		}else{
			if(!m_vOriginalBuffer.empty()){
				::memcpy(&sStrLen,(BYTE*)&m_vOriginalBuffer[0]+m_sCurrOffsetPos,sizeof(cn::s32));	
				m_sCurrOffsetPos+=sizeof(cn::s32);
			}
		}
		if(sStrLen >= 0){ //若字符串为ANSI存储格式的话

			bufData(sStrLen+1);
			char *pStr =(char *)(bufData.GetData());
			pStr[sStrLen] =0;
			if(m_sCurrOffsetPos + sStrLen > m_vOriginalBuffer.size()){
				throw 1;
			}else{
				if(!m_vOriginalBuffer.empty()){
					::memcpy(pStr,(BYTE*)&m_vOriginalBuffer[0]+m_sCurrOffsetPos,sStrLen);	
					m_sCurrOffsetPos+=sStrLen;
				}
			}

		}else{ //若字符串为Unicode存储格式的话

			sStrLen =-sStrLen;
			if(sStrLen > _MAX_STR_LEN){ //若字符串长度超长，则出错退出
				throw 1;
			}
			if(m_sCurrOffsetPos + sStrLen*sizeof(WCHAR) > m_vOriginalBuffer.size()){
				throw 1;
			}else{
				if(!m_vOriginalBuffer.empty()){
					::memcpy(g_pTempStrBuf2,(BYTE*)&m_vOriginalBuffer[0]+m_sCurrOffsetPos,sStrLen*sizeof(WCHAR));	
					m_sCurrOffsetPos+=sStrLen*sizeof(WCHAR);
					g_pTempStrBuf2[sStrLen]=0;
					cn::s32 sNewStrLen =0;
					const char *pNewStr =cn::Literal::WCharToChar(g_pTempStrBuf2, sStrLen, &sNewStrLen);
					bufData(sNewStrLen+1);
					char *pStr =(char *)(bufData.GetData());
					pStr[sNewStrLen] =0;
					::memcpy(pStr,pNewStr,sizeof(char)*sNewStrLen);
				}
				
			}
		}
	}else{ 
		throw 1;
	}
}

void FileZipIO::WriteData(const void *pData, u32 uDataLen)
{
	if(m_sCurrOffsetPos >=0){
		if(m_sCurrOffsetPos + uDataLen > m_vOriginalBuffer.size()){
			m_vOriginalBuffer.resize(m_sCurrOffsetPos + uDataLen);
		}
		if(!m_vOriginalBuffer.empty()){
			::memcpy((BYTE*)&m_vOriginalBuffer[0]+m_sCurrOffsetPos,pData,uDataLen);
			m_sCurrOffsetPos += uDataLen;
		}
		
	}else{
		throw 1;
	}
}


void FileZipIO::ReadData(void *pData, u32 uDataLen)
{
	if(m_sCurrOffsetPos >=0){
		if(m_sCurrOffsetPos+uDataLen>m_vOriginalBuffer.size()){
			throw 1;
		}else{
			if(!m_vOriginalBuffer.empty()){
				::memcpy(pData,(BYTE*)&m_vOriginalBuffer[0]+m_sCurrOffsetPos,uDataLen);
				m_sCurrOffsetPos += uDataLen;
			}
		}
	}else{
		throw 1;
	}
}


void FileZipIO::WriteS8(s8 sData)
{
	_WRITE_TO_DATABUF(s8,sData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


s8 FileZipIO::ReadS8()
{
	_READ_FROM_DATABUF(s8,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteU8(u8 uData)
{
	_WRITE_TO_DATABUF(u8,uData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


u8 FileZipIO::ReadU8()
{
	_READ_FROM_DATABUF(u8,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteS16(s16 sData)
{
	_WRITE_TO_DATABUF(s16,sData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


s16 FileZipIO::ReadS16()
{
	_READ_FROM_DATABUF(s16,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteU16(u16 uData)
{
	_WRITE_TO_DATABUF(u16,uData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


u16 FileZipIO::ReadU16()
{
	_READ_FROM_DATABUF(u16,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteS32(s32 sData)
{
	//_WRITE_TO_DATABUF(s32,sData,m_vOriginalBuffer,m_sCurrOffsetPos);

	if(m_sCurrOffsetPos>=0){
		if(m_sCurrOffsetPos + sizeof(s32) > m_vOriginalBuffer.size()){ 
			m_vOriginalBuffer.resize(m_sCurrOffsetPos + sizeof(s32));
		}
		if(!m_vOriginalBuffer.empty()){
			::memcpy((BYTE*)&m_vOriginalBuffer[0]+m_sCurrOffsetPos,(void*)&sData,sizeof(s32));
			m_sCurrOffsetPos += sizeof(s32);
		}
	}else{
	throw 1;
	}
}


s32 FileZipIO::ReadS32()
{
	_READ_FROM_DATABUF(s32,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteU32(u32 uData)
{
	_WRITE_TO_DATABUF(u32,uData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


u32 FileZipIO::ReadU32()
{
	_READ_FROM_DATABUF(u32,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteS64(s64 sData)
{
	_WRITE_TO_DATABUF(s64,sData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


s64 FileZipIO::ReadS64()
{
	_READ_FROM_DATABUF(s64,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteU64(u64 uData)
{
	_WRITE_TO_DATABUF(u64,uData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


u64 FileZipIO::ReadU64()
{
	_READ_FROM_DATABUF(u64,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteVec2(const Vec2 &vData)
{
	_WRITE_TO_DATABUF(Vec2,vData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Vec2 FileZipIO::ReadVec2()
{
	_READ_FROM_DATABUF(Vec2,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteVec3(const Vec3 &vData)
{
	_WRITE_TO_DATABUF(Vec3,vData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Vec3 FileZipIO::ReadVec3()
{
	_READ_FROM_DATABUF(Vec3,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteVec3D(const Vec3D &vData)
{
	_WRITE_TO_DATABUF(Vec3D,vData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Vec3D FileZipIO::ReadVec3D()
{
	_READ_FROM_DATABUF(Vec3D,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteVec4(const Vec4 &qData)
{
	_WRITE_TO_DATABUF(Vec4,qData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Vec4 FileZipIO::ReadVec4()
{
	_READ_FROM_DATABUF(Vec4,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WritePlane(const Plane &qData)
{
	_WRITE_TO_DATABUF(Plane,qData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Plane FileZipIO::ReadPlane()
{
	_READ_FROM_DATABUF(Plane,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteQuaternion(const Quaternion &qData)
{
	_WRITE_TO_DATABUF(Quaternion,qData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Quaternion FileZipIO::ReadQuaternion()
{
	_READ_FROM_DATABUF(Quaternion,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteQuaternionD(const QuaternionD &qData)
{
	_WRITE_TO_DATABUF(QuaternionD,qData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


QuaternionD FileZipIO::ReadQuaternionD()
{
	_READ_FROM_DATABUF(QuaternionD,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteMat4x4(const Mat4x4 &matData)
{
	_WRITE_TO_DATABUF(Mat4x4,matData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Mat4x4 FileZipIO::ReadMat4x4()
{
	_READ_FROM_DATABUF(Mat4x4,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteMat4x3(const Mat4x3 &matData)
{
	_WRITE_TO_DATABUF(Mat4x3,matData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Mat4x3 FileZipIO::ReadMat4x3()
{
	_READ_FROM_DATABUF(Mat4x3,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteAABB(const AABB &bbData)
{
	_WRITE_TO_DATABUF(AABB,bbData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


AABB FileZipIO::ReadAABB()
{
	_READ_FROM_DATABUF(AABB,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteRay(const Ray &rayData)
{
	_WRITE_TO_DATABUF(Ray,rayData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Ray FileZipIO::ReadRay()
{
	_READ_FROM_DATABUF(Ray,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteSphere(const Sphere &sphData)
{
	_WRITE_TO_DATABUF(Sphere,sphData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Sphere FileZipIO::ReadSphere()
{
	_READ_FROM_DATABUF(Sphere,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteRect(const Rect &rcData)
{
	_WRITE_TO_DATABUF(Rect,rcData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Rect FileZipIO::ReadRect()
{
	_READ_FROM_DATABUF(Rect,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteViewport(const Viewport &vpData)
{
	_WRITE_TO_DATABUF(Viewport,vpData,m_vOriginalBuffer,m_sCurrOffsetPos);
}


Viewport FileZipIO::ReadViewport()
{
	_READ_FROM_DATABUF(Viewport,m_vOriginalBuffer,m_sCurrOffsetPos);
}


void FileZipIO::WriteBuffer(const Buffer &bufData)
{
	if(m_sCurrOffsetPos >=0){
		u32 uDataLen = bufData.GetLen();
		if(m_sCurrOffsetPos + sizeof(u32) + uDataLen > m_vOriginalBuffer.size()){
			m_vOriginalBuffer.resize(m_sCurrOffsetPos+ sizeof(u32)+uDataLen);
		}
		if(!m_vOriginalBuffer.empty()){
			::memcpy((BYTE*)&m_vOriginalBuffer[0]+m_sCurrOffsetPos,&uDataLen,sizeof(u32)); m_sCurrOffsetPos+=sizeof(u32);
			::memcpy((BYTE*)&m_vOriginalBuffer[0]+m_sCurrOffsetPos,bufData.GetData(),uDataLen);	m_sCurrOffsetPos+=uDataLen;
		}
	}else{
		throw 1;
	}
}


void FileZipIO::ReadBuffer(Buffer &bufData)
{

	if(m_sCurrOffsetPos>=0){
		cn::u32 uDataLen;
		if(m_sCurrOffsetPos + sizeof(u32)>m_vOriginalBuffer.size()){
			throw 1;
		}else{
			if(!m_vOriginalBuffer.empty()){
				::memcpy(&uDataLen,(BYTE*)&m_vOriginalBuffer[0]+m_sCurrOffsetPos,sizeof(u32));	
				m_sCurrOffsetPos+=sizeof(u32);
			}
		}
		if(m_sCurrOffsetPos + uDataLen>m_vOriginalBuffer.size()){
			throw 1;
		}else{
			bufData.Resize(uDataLen);
			if(!m_vOriginalBuffer.empty()){
				::memcpy(bufData.GetData(),(BYTE*)&m_vOriginalBuffer[0]+m_sCurrOffsetPos,uDataLen);
				m_sCurrOffsetPos+=uDataLen;
			}
		}
	}else{
		throw 1;
	}
}





}