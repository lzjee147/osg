


namespace cn
{


#pragma warning(disable : 4996)
#pragma warning(disable : 4244)


static SyncObj g_FilePathSyncObj;				//�������ļ���Ŀ¼������ʵ�ͬ������

static char g_pEngineCWD[_MAX_PATH_LEN]=""; //��ʾ����ȫ��ʹ�õĹ���Ŀ¼
static bool g_bEngineCWDInited =false; //��ʾ����ȫ��ʹ�õĹ���Ŀ¼�Ƿ��ѳ�ʼ��

static char g_StrResCachePath[_MAX_PATH_LEN]="";					//Զ���ļ��ڱ��ر���ʱ�ĸ�·��
static bool g_bResCachePathInited =false; //��ʾԶ���ļ������·���Ƿ��ѳ�ʼ��

static DiskIO::ZIP_MODE g_eFileZipMode=	DiskIO::ZIP_MODE_NORMAL;	//��ǰʹ������ģʽ��ȡ�ļ� 
static cn::u64 g_uResCacheCapacity=(cn::u64)(2.0f *::powf(1024.0f,3.0f));		//��Դ����Ŀ¼����������,��λ���ֽ�
static cn::u64 g_uCurCacheSize = 0;			//��ǰ����Ŀ¼�����ļ���ʵ�ʴ�С
static float   g_fClearProportion =0.25f;	//������ʱ������������ͻ�����������֮��

static SyncObj g_cGolFileFilterSyncObj; //��ʾ����ȫ���ļ����˵�ͬ������
static cn::Buffer g_bufGolFileFilterMem; //��ʾȫ���ļ����˶�������ݴ洢��
static cn::u64 g_uGolFileFilterMemValidBatch =1; //��ʾȫ���ļ��������ݴ洢���Ӧ����Ч����

static SyncObj g_cGolFilePackageSyncObj; //��ʾ������ȫ�ַ�����󼯺Ͻ��л�����ʵ�ͬ������
static std::map<std::string, Ref/*FilePackage*/> m_mapGolFilePackages; //��ʾȫ����Դ�����󼯺�<��Դ���Ĵ���·��(ȫ��Сд),��Դ��(��Ϊ�����ʾ�ô���·����û���κ���Դ��)>

//static EngineResServerTest g_cResServerTest; //��ʾ�����õ�������Դ������

struct CacheFileInfo{	
	FILETIME ft;		//�ļ���������ʱ��
	cn::u32  filesize;	//�ļ��Ĵ�С
};

//��ȡ�ļ������ļ��Ĵ�С�͸��������ڲ�ʹ�ã�
static cn::u64 GetFolderSize(const char* pszPath, cn::u32 *uFiles, cn::u32 *uFolders,std::vector<CacheFileInfo>& vCacheFileInfo);
//�����ļ�������expiryDate����֮ǰ���ļ������ڲ�ʹ�ã�
static cn::u64 CleanUpExpiredFiles(const char* pszPath, FILETIME& expiryDate);


//��������ȫ��ʹ�õĹ���Ŀ¼
//��==NULL�򱣳�ԭ�й���Ŀ¼����
void DiskIO::SetEngineCWD(const char *pPath)
{
	if(pPath == NULL){
		return;
	}
	AutoSync CurAutoSync(&g_FilePathSyncObj); //�����߳�ͬ��
	strncpy_s(g_pEngineCWD, _MAX_PATH_LEN, pPath, strlen(pPath));
	g_bEngineCWDInited =true;
}


//��ȡ����ȫ��ʹ�õĹ���Ŀ¼
const char *DiskIO::GetEngineCWD()
{
	AutoSync CurAutoSync(&g_FilePathSyncObj); //�����߳�ͬ��
	DLL_TLS_DATA *lpDLLTLSData = cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	if(g_bEngineCWDInited == false){
		std::string strCWD;	char pStrBuf[MAX_PATH];
#if _DEBUG
		HMODULE hModule =::GetModuleHandle("Common_d.dll");
#else
		HMODULE hModule =::GetModuleHandle("Common.dll");
#endif
		if(hModule != NULL){
			cn::u32 uActualName =::GetModuleFileName(hModule, pStrBuf, MAX_PATH);
			if(uActualName > 0){
				strCWD =pStrBuf; cn::u32 uPos0 =strCWD.rfind('\\'); 
				if(uPos0 != std::string::npos){
					strCWD =strCWD.substr(0, uPos0);
				}else{::_getcwd(pStrBuf, MAX_PATH); strCWD =pStrBuf;}
			}else{::_getcwd(pStrBuf, MAX_PATH); strCWD =pStrBuf;}
		}else{::_getcwd(pStrBuf, MAX_PATH); strCWD =pStrBuf;}
		cn::DiskIO::SetEngineCWD(strCWD.c_str());	//��������Ĭ�ϵĹ���Ŀ¼
	}
	lpDLLTLSData->m_strEngineCWD = g_pEngineCWD;
	return lpDLLTLSData->m_strEngineCWD.c_str();
}


//����ȫ���ļ�����ģ��Ĺ�������
void DiskIO::SetGolFileFilterData(const Buffer &bufData)
{
	AutoSync cCurAutoSync(&g_cGolFileFilterSyncObj); //�����߳�ͬ��
	g_bufGolFileFilterMem =bufData;
	g_uGolFileFilterMemValidBatch++;
}


//ָ��Զ����Դ�����ش����ϻ���ʱ�Ļ���Ŀ¼
void DiskIO::SetURLResCachePath(const char* CachePath)
{
	if(CachePath == NULL){
		return;
	}
	AutoSync CurAutoSync(&g_FilePathSyncObj); //�����߳�ͬ��
	strncpy_s(g_StrResCachePath,_MAX_PATH_LEN,CachePath,strlen(CachePath));
	g_bResCachePathInited =true;
	
//g_cResServerTest.SetResGroupRootDir("machine01", "D:\\AppTest\\01\\test01\\");

	////��黺��Ŀ¼�Ĵ�С
	//cn::u32 ufileNum,uDirNum;
	//std::vector<CacheFileInfo> vCacheFileInfo;
	//cn::u64 uFolderSize = GetFolderSize(g_StrResCachePath,&ufileNum,&uDirNum,vCacheFileInfo);
	//if(uFolderSize != -1){
	//	g_uCurCacheSize = uFolderSize;
	//}
}


//��ȡԶ����Դ�����ش����ϻ���ʱ�Ļ���Ŀ¼
const char* DiskIO::GetURLResCachePath()
{
	AutoSync CurAutoSync(&g_FilePathSyncObj); //�����߳�ͬ��
	DLL_TLS_DATA *lpDLLTLSData = cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	if(g_bResCachePathInited == false){
		std::string strTmpDir = std::string(cn::DiskIO::GetEngineCWD())+"\\DownloadTemp\\";
		cn::DiskIO::SetURLResCachePath(strTmpDir.c_str());	//������Դ����λ�õ�Ĭ��ֵ
	}
	lpDLLTLSData->m_StrResCachePath = g_StrResCachePath;
	return lpDLLTLSData->m_StrResCachePath.c_str();
}

//���û����ļ���������ÿ��ִ��������������
void DiskIO::SetURLResCacheCapacity(float fCapacity,float fClearProportion)
{
	//AutoSync CurAutoSync(&g_FilePathSyncObj); //�����߳�ͬ��
	//g_uResCacheCapacity = (cn::u64)(fCapacity*::powf(1024.0f,3.0f));
	//g_fClearProportion  = fClearProportion;

	////��黺��Ŀ¼�Ĵ�С
	//cn::u32 ufileNum,uDirNum;
	//std::vector<CacheFileInfo> vCacheFileInfo;
	//cn::u64 uFolderSize = GetFolderSize(cn::DiskIO::GetURLResCachePath(),&ufileNum,&uDirNum,vCacheFileInfo);
	//if(uFolderSize != -1){
	//	g_uCurCacheSize = uFolderSize;
	//	if(g_uCurCacheSize > g_uResCacheCapacity){
	//		CleanUpURLResCacheDir();
	//	}
	//}
}


//��ȡ���ػ���Ŀ¼����������
float DiskIO::GetURLResCacheCapacity(float *pfClearProportion)
{
	AutoSync CurAutoSync(&g_FilePathSyncObj);
	if(pfClearProportion){
		*pfClearProportion = g_fClearProportion;
	}
	return g_uResCacheCapacity/::powf(1024.0f,3.0f);
}

//sort file times operation
bool earlyFileTime(const CacheFileInfo& cfi1,const CacheFileInfo& cfi2  )
{
	return ::CompareFileTime(&cfi1.ft,&cfi2.ft)<0;
}

//ǿ�����ȫ����Դ�����󼯺�
void DiskIO::ForceClearAllGolFilePackages()
{
	m_mapGolFilePackages.clear();
}


//ִ�л���Ŀ¼������
bool DiskIO::CleanUpURLResCacheDir()
{
	//AutoSync CurAutoSync(&g_FilePathSyncObj);
	//std::vector<CacheFileInfo> vCacheFileInfo;
	//cn::u32 uFiles,uFolders;
	//cn::u64 URLCacheFolderSize = GetFolderSize(cn::DiskIO::GetURLResCachePath(),&uFiles,&uFolders,vCacheFileInfo);
	//if(uFiles == 0){
	//	return true;
	//}
	//cn::u64 uNeedRemoveSize = URLCacheFolderSize - (cn::u64)(g_uResCacheCapacity*(1.0f - g_fClearProportion));
	//if(uNeedRemoveSize <= 0){
	//	return true;
	//}
	//std::sort(vCacheFileInfo.begin(),vCacheFileInfo.end(),earlyFileTime);
	//cn::u64 sizecount=0;
	//FILETIME expiryDate;
	//std::vector<CacheFileInfo>::iterator itor= vCacheFileInfo.begin();
	//for (;itor!=vCacheFileInfo.end();++itor){
	//	sizecount += itor->filesize;
	//	if(sizecount >= uNeedRemoveSize){
	//		break;
	//	}
	//}
	//if(itor == vCacheFileInfo.end()){
	//	expiryDate = (*(vCacheFileInfo.rbegin())).ft;
	//}else{
	//	expiryDate = itor->ft;
	//}

	//g_uCurCacheSize = CleanUpExpiredFiles(cn::DiskIO::GetURLResCachePath(),expiryDate);
	return true;
}


//��ȡ�ļ������ļ��Ĵ�С�͸������ڲ�ʹ�ã�
cn::u64 GetFolderSize(const char* pszPath, cn::u32 *uFiles, cn::u32 *uFolders,std::vector<CacheFileInfo>& vCacheFileInfo)
{
	if(FALSE == ::PathIsDirectory(pszPath)){
		return -1;
	}

	cn::u64    uSize = 0;
	HANDLE hFind = NULL;	
	WIN32_FIND_DATA fileinfo;

	std::string strFilePath =pszPath;
	if('\\'!=pszPath[strlen(pszPath)-1]) {strFilePath += "\\";}
	std::string strFileFilter = strFilePath + "*.*";
	hFind = FindFirstFile(strFileFilter.c_str(),&fileinfo);
	if(INVALID_HANDLE_VALUE == hFind){
		return -1;
	}else{
		do{
			if(fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				if (!strcmp(fileinfo.cFileName,".") || !strcmp(fileinfo.cFileName,"..")){
					//Do nothing for "." and ".." folders
				}else{
					std::string strtmp = strFilePath + fileinfo.cFileName + "\\";
					cn::u64 uRetSize = GetFolderSize(strtmp.c_str(),uFiles,uFolders,vCacheFileInfo);
					if(-1 != uRetSize){
						uSize = uSize + uRetSize;
					}				
					if(uFolders != NULL){
						++(*uFolders);
					}
				}
			}else{
				if(uFiles != NULL){
					++(*uFiles);
					CacheFileInfo info;
					info.filesize = fileinfo.nFileSizeLow;
					info.ft = fileinfo.ftLastAccessTime;
					vCacheFileInfo.push_back(info);
				}
			}
			uSize += (DWORD64)fileinfo.nFileSizeHigh * MAXDWORD + (DWORD64)fileinfo.nFileSizeLow;
		}while(FindNextFile(hFind,&fileinfo));
		FindClose(hFind);
	}	
	return uSize;
}


//�����ļ�������expiryDate����֮ǰ���ļ�
cn::u64 CleanUpExpiredFiles(const char* pszPath, FILETIME& expiryDate)
{
	if(FALSE == ::PathIsDirectory(pszPath)){
		return -1;
	}

	cn::u64    uSize = 0;
	HANDLE hFind = NULL;	
	WIN32_FIND_DATA fileinfo;

	std::string strFilePath =pszPath;
	if('\\'!=pszPath[strlen(pszPath)-1]){strFilePath += "\\";}
	std::string strFileFilter = strFilePath + "*.*";
	hFind = FindFirstFile(strFileFilter.c_str(),&fileinfo);
	if(INVALID_HANDLE_VALUE == hFind){
		return -1;
	}else{
		do{
			if(fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				if (!strcmp(fileinfo.cFileName,".") || !strcmp(fileinfo.cFileName,"..")){
					//Do nothing for "." and ".." folders
				}else{
					std::string strtmp = strFilePath + fileinfo.cFileName + "\\";
					cn::u64 uRetSize = CleanUpExpiredFiles(strtmp.c_str(),expiryDate);
					if(-1 != uRetSize){
						uSize = uSize + uRetSize;
						if(uRetSize == 0){
							::RemoveDirectory(strtmp.c_str());
						}
					}				
				}
			}else{
				if(::CompareFileTime(&fileinfo.ftLastAccessTime,&expiryDate)<0){
					std::string strtmp = strFilePath+ fileinfo.cFileName;
					::DeleteFile(strtmp.c_str());	
				}else{
					uSize += (DWORD64)fileinfo.nFileSizeHigh * MAXDWORD + (DWORD64)fileinfo.nFileSizeLow;
				}
			}		
		}while(FindNextFile(hFind,&fileinfo));
		FindClose(hFind);
	}	
	return uSize;
}


//�����ļ�ѹ���洢�ķ�ʽ
void DiskIO::SetFileZipMode(DiskIO::ZIP_MODE zm)
{
	g_eFileZipMode = zm;
}


//��ȡ�ļ�ѹ���洢�ķ�ʽ
DiskIO::ZIP_MODE DiskIO::GetFileZipMode()
{
	return g_eFileZipMode;
}


//��һ�������ļ�ѹ��Ϊһ��zip�ļ�
bool DiskIO::ZipExistFile(const char* pFileName,const char* pDestFile)
{
	std::string strDestfilePath;
	if(NULL == pDestFile){
		strDestfilePath = std::string(pFileName)+ ".zip";
	}else{
		strDestfilePath = pDestFile;
	}

	if(ZipCompress(pFileName,strDestfilePath.c_str())){
		return true;
	}else{
		return false;
	}
}


bool DiskIO::CompressBuffer(const void *pSrcBuf, cn::u32 uSrcBufLen, Buffer& DestBuf)
{
	if(uSrcBufLen <=0) return false;
	unsigned long  uBoundLength = compressBound(uSrcBufLen);	
	if(uBoundLength<=0) return false;
	cn::Buffer &bufTemp =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_buf_DiskIO_CompressBuffer_TempBuf;
	bufTemp(uBoundLength);
	cn::s32 sError = compress((Bytef *)(bufTemp.GetData()),&uBoundLength,(BYTE*)pSrcBuf,uSrcBufLen);
	if(sError != Z_OK){
		return false;
	}
	DestBuf.Resize(2*sizeof(cn::u32)+uBoundLength);			//ѹ������ڴ���ŵ�����Ϊ uSrcDataLength+uBoundLength+compressedData;
	cn::u8* pHandle = (cn::u8*)DestBuf.GetData();
	memcpy(pHandle,&uSrcBufLen,sizeof(cn::u32));				pHandle+=sizeof(cn::u32);
	memcpy(pHandle,&uBoundLength,sizeof(cn::u32));			pHandle+=sizeof(cn::u32);
	memcpy(pHandle,bufTemp.GetData(),uBoundLength);
	return true;
}


bool DiskIO::UncompressBuffer(const void *pSrcBuf, cn::u32 uSrcBufLen, Buffer& DestBuf)
{
	if(uSrcBufLen <=0) return false;
	cn::u8* pHandle = (cn::u8*)pSrcBuf;	
	unsigned long unzipSize = *(unsigned long*)pHandle;		pHandle+=sizeof(unsigned long);
	unsigned long zipedSize = *(unsigned long*)pHandle;		pHandle+=sizeof(unsigned long);
	if((uSrcBufLen-2*sizeof(ULONG)!= zipedSize)){
		return false;
	}
	DestBuf.Resize(unzipSize);		
	int sError = uncompress( (cn::u8*)DestBuf.GetData(),&unzipSize,pHandle,zipedSize);
	if(sError!= S_OK){
		return false;
	}
	return true;
}


//��ȡһ�����ݿ��Ӧ��MD5ֵ
u128 DiskIO::GetMD5FromBuffer(const void *pBuf, cn::u32 uBufLen)
{
	DLL_TLS_DATA *lpDLLTLSData = cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	HCRYPTPROV hContext =lpDLLTLSData->m_hMD5Context;	

	u128 uRet; uRet.u_0_63 =0; uRet.u_64_127 =0;
	HCRYPTPROV hHash =NULL;  
	if(::CryptCreateHash(hContext, CALG_MD5, 0, 0, &hHash) == FALSE){
		return uRet;
	}
	if(pBuf && uBufLen>0){
		::CryptHashData(hHash, (const BYTE *)(pBuf), uBufLen, 0);
	}
	DWORD dwHashDataLen =sizeof(u128);
	::CryptGetHashParam(hHash, HP_HASHVAL, (BYTE *)(&uRet), &dwHashDataLen, 0);
	::CryptDestroyHash(hHash);
	return uRet;
}


//��ȡһ�����ݿ��Ӧ��32λ��ϣֵ
//uHashType����ʾ��ϣֵ����ID����ЧֵΪ0~4
u32 DiskIO::GetHashValFromBuffer(const void *pBuf, cn::u32 uBufLen, cn::u32 uHashType)
{
	DLL_TLS_DATA *lpDLLTLSData = cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	cn::u32 i;

	uHashType =max(0, min(4, uHashType));
    unsigned long seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;  
	const cn::u8 *pSrc =(const cn::u8 *)(pBuf);
	for(i =0; i<uBufLen; i++){
        seed1 = lpDLLTLSData->m_pHashValTable[(uHashType << 8) + pSrc[i]] ^ (seed1 + seed2);  
        seed2 = pSrc[i] + seed1 + seed2 + (seed2 << 5) + 3;   
    }  
    return seed1;  
}


//����һ�����̷��ʶ���(�����throw)
Ref/*IDiskIO*/ DiskIO::Create(const char *pFilePath, const char *pMode, ZIP_MODE eZipMode, cn::SyncObj *lpDownloadingSync, cn::u32 *pDownloading, bool bUseGolMemData)
{
	cn::u32 i, j;

	if(NULL==pFilePath || NULL==pMode){
		throw 1;
	}

#if _USE_TIME_LIMIT
	{
		SYSTEMTIME cSysTime; ::GetSystemTime(&cSysTime);
		if(cSysTime.wYear != _TIME_LIMIT_YEAR){if(cSysTime.wYear > _TIME_LIMIT_YEAR){throw 1;}}
		else if(cSysTime.wMonth != _TIME_LIMIT_MONTH){if(cSysTime.wMonth > _TIME_LIMIT_MONTH){throw 1;}}
		else if(cSysTime.wDay != _TIME_LIMIT_DAY){if(cSysTime.wDay > _TIME_LIMIT_DAY){throw 1;}}
		else if(cSysTime.wHour != _TIME_LIMIT_HOUR){if(cSysTime.wHour > _TIME_LIMIT_HOUR){throw 1;}}
		else if(cSysTime.wMinute != _TIME_LIMIT_MINUTE){if(cSysTime.wMinute > _TIME_LIMIT_MINUTE){throw 1;}}
		else if(cSysTime.wSecond > WORD(float(::rand())/float(RAND_MAX)*59.9999f)){throw 1;}	
	}
#endif

	ZIP_MODE eFinalZipMode =(eZipMode == ZIP_MODE_UNKNOWN) ? DiskIO::GetFileZipMode() : eZipMode;	
	switch(eFinalZipMode)
	{
	case ZIP_MODE_NORMAL:
	case ZIP_MODE_SINGLE_ZIP:
	case ZIP_MODE_MIX:
		{
			cn::u32 uFilePathLen =::strlen(pFilePath);

			//�ж�pFilePath��·�����ͣ��ֱ���
			std::string strLocalPath; std::string strLocalPathZip; cn::u32 uIOType; std::string strFilePathZip;
			bool bNeedZip =(eFinalZipMode==ZIP_MODE_MIX || eFinalZipMode==ZIP_MODE_SINGLE_ZIP);
			bool bNeedUnzip =(eFinalZipMode==ZIP_MODE_MIX || eFinalZipMode==ZIP_MODE_NORMAL);
			if(TRUE == ::PathIsURL(pFilePath)){ //��URL·���Ļ�
				if(::strcmp(pMode, "r")!=0 && ::strcmp(pMode, "rt")!=0 && ::strcmp(pMode, "rb")!=0){
					throw 1; //��������·����ֻ����r,rt��rb���ַ���ģʽ
				}		
				DLL_TLS_DATA *lpDLLTLSData = cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
				cn::WebDownload *lpWebDownload = lpDLLTLSData->m_lpWdl;	
				cn::u64 uFileLen =0; cn::u32 uStartPos =0; cn::u32 uEndPos =0;
				strLocalPath =DiskIO::GetURLResCachePath(); strLocalPath +=DiskIO::GetURL_EngineDir(pFilePath, &uStartPos, &uEndPos);
				if(bNeedZip){strLocalPathZip =strLocalPath+".zip";}

				//���ļ���Դ����Ȩ�޹���
				{
					AutoSync cCurAutoSync(&g_cGolFileFilterSyncObj); //�����߳�ͬ��
					if(lpDLLTLSData->m_uGolFileFilterMemBatch != g_uGolFileFilterMemValidBatch){
						if(g_bufGolFileFilterMem.GetLen() > 0){
							lpDLLTLSData->m_refGolFileFilter =FileFilter::Create();
							FileFilter *lpFilter =(FileFilter *)(lpDLLTLSData->m_refGolFileFilter.Get());
							if(lpFilter){lpFilter->LoadInMemory(g_bufGolFileFilterMem);}
						}else{
							lpDLLTLSData->m_refGolFileFilter =Ref(NULL);
						}
						lpDLLTLSData->m_uGolFileFilterMemBatch =g_uGolFileFilterMemValidBatch;
					}
				}
				std::string strFilePathExt =DiskIO::GetURL_EngineParams(pFilePath);
				FileFilter *lpGolFilter =(FileFilter *)(lpDLLTLSData->m_refGolFileFilter.Get());
				if(lpGolFilter){
					if(lpGolFilter->FilterFile(strFilePathExt.c_str(), NULL) == false){ //���ļ�����ʧ�ܵĻ�
						throw 1;
					}
				}

				//������Դ
				if(bNeedZip && ::PathFileExists(strLocalPathZip.c_str())){
					uIOType =1;
				}else if(bNeedUnzip && ::PathFileExists(strLocalPath.c_str())){
					uIOType =0;
				}else{
					if(bNeedZip){strFilePathZip =std::string(pFilePath, uEndPos)+".zip"+std::string(pFilePath+uEndPos, uFilePathLen-uEndPos);}
					if(bNeedZip && lpWebDownload->DownloadFromURL(/*DiskIO::GetURL_EngineParams(strFilePathZip.c_str())*/strFilePathZip.c_str()/*��ʱ��ʱʹ�ã�����*/, strLocalPathZip.c_str(), &uFileLen)){	
						uIOType =1;
						g_uCurCacheSize += uFileLen; if(g_uCurCacheSize > g_uResCacheCapacity){DiskIO::CleanUpURLResCacheDir();}
					}else if(bNeedUnzip && lpWebDownload->DownloadFromURL(/*strFilePathExt.c_str()*/pFilePath/*��ʱ��ʱʹ�ã�����*/, strLocalPath.c_str(), &uFileLen)){	
						uIOType =0;
						g_uCurCacheSize += uFileLen; if(g_uCurCacheSize > g_uResCacheCapacity){DiskIO::CleanUpURLResCacheDir();}
					}else{
						throw 1;
					}
				}
			}else if(TRUE == PathIsUNC(pFilePath)){ //����UNC·���Ļ�
				throw 1;
			}else{ //���Ǳ���·���Ļ�
				strLocalPath =pFilePath;
				if(bNeedZip){strLocalPathZip =strLocalPath+".zip";}

				if(::strchr(pMode,'r') == NULL){ //��������ֻ�����r�����б�Ҫ�����ļ���
					DiskIO::CreatePathHierarchy(pFilePath);
					uIOType =0; //��д����ģʽ�£�ǿ��д���ѹ���ļ�
				}else{ //������ֻ����ǵĻ�
					if(bNeedZip && ::PathFileExists(strLocalPathZip.c_str())){
						uIOType =1;
					}else{
						uIOType =0;
					}
				}		
			}
			DiskIO *lpRetFile =NULL;
			if(uIOType == 0){
				lpRetFile = new FileIOImp(strLocalPath.c_str(), pMode);
			}else if(uIOType == 1){
				lpRetFile = new FileZipIO(strLocalPathZip.c_str(), pMode);
			}else{
				throw 1;
			}

			return lpRetFile;
		}
		break;
	case ZIP_MODE_PACKAGE:
		{


//DiskIO::SetURLResCacheCapacity(100.0f, 0.25f);
//DiskIO::SetURLResCachePath("D:\\AppTest\\01\\test01\\");
//pFilePath ="http://192.168.0.27:9000/RealEngineRes.aspx?dir=machine01&path=machine01\\huge.scenegroup";


			std::string strMode =pMode ? pMode : "";
			if(::PathIsURL(pFilePath) == TRUE){ //����URL·���Ļ�
				if(strMode.find('r')==std::string::npos || strMode.find("r+")!=std::string::npos){ //���ļ��������Բ�Ϊֻ���Ļ���������˳�
					throw 1;
				}	
				DLL_TLS_DATA *lpDLLTLSData = cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
				cn::WebDownload *lpWebDownload = lpDLLTLSData->m_lpWdl;	
				cn::u64 uFileLen =0; cn::u32 uStartPos =0; cn::u32 uEndPos =0;
				std::string strLocalPathPrefix =cn::Literal::ToLower(DiskIO::GetURLResCachePath()); std::string strLocalRelPath =DiskIO::GetURL_EngineDir(pFilePath, &uStartPos, &uEndPos);
				std::string strLocalFullPath =strLocalPathPrefix+strLocalRelPath;
				//std::string strFilePathExt =DiskIO::GetURL_EngineParams(pFilePath);
				cn::u32 uDirLevNum; const char *pFileName;
				const char **ppHierarchy =cn::DiskIO::GetPathHierarchy(strLocalRelPath.c_str(), uDirLevNum, pFileName);
				std::vector<std::string> arrHierarchy(uDirLevNum); std::string strFileName =pFileName; std::vector<std::string> arrPackPaths(uDirLevNum); 
				std::string strTemp =""; for(i =0; i<uDirLevNum; i++){arrHierarchy[i] =ppHierarchy[i]; strTemp +=arrHierarchy[i]; arrPackPaths[i] =strTemp+".pak"; strTemp +="\\";}
				bool bNeedQueryServer =true; //��ʾ�Ƿ���Ҫ��������ύ����
				cn::Ref refPackage; FilePackage *lpPackage =NULL; //��ʾ��Դ������
				std::string strPackPath; //��ʾ��Դ���ķ������ֲ�·��
				std::string strPackFullPath; //��ʾ��Դ���ڱ��ػ��������·��
				std::string strInPackRelPath; //��ʾ�ļ�����Դ���ڵ����·��
				cn::u128 uInPackRelPathMD5; //��ʾ�ļ�����Դ���ڵ����·����MD5ֵ
				if(uDirLevNum > 0){ //��Ŀ���ļ�Ϊ�༶Ŀ¼�ļ��Ļ�
					AutoSync cAutoSync(&g_cGolFilePackageSyncObj); //�������ȫ����Դ������
					for(i =0; i<uDirLevNum; i++){ //������Ŀ¼�������Դ���Ĵ�����
						strPackPath =arrPackPaths[i]; strPackFullPath =strLocalPathPrefix+strPackPath;
						std::map<std::string, Ref/*FilePackage*/>::iterator iterFind =m_mapGolFilePackages.find(strPackFullPath);
						if(iterFind != m_mapGolFilePackages.end()){
							lpPackage =(FilePackage *)(iterFind->second.Get());
							if(lpPackage){bNeedQueryServer =false; strInPackRelPath =""; for(j =i+1; j<uDirLevNum; j++){strInPackRelPath +=arrHierarchy[j]; strInPackRelPath +="\\";} strInPackRelPath +=strFileName; break;}
						}else if(::PathFileExists(strPackFullPath.c_str())){ //���״μ�⵽����Դ���Ļ�
							for(j =0; j<i; j++){m_mapGolFilePackages[strLocalPathPrefix+arrPackPaths[j]] =Ref(NULL);} //����Դ����ǰ׺·��ǿ����Ϊ��Ч��Դ��״̬
							m_mapGolFilePackages[strPackFullPath] =FilePackage::Create(strPackFullPath.c_str(), 1024*256, 0, false);
							lpPackage =(FilePackage *)(m_mapGolFilePackages[strPackFullPath].Get());
							if(lpPackage){ //����Դ����Ч�Ļ�
								lpPackage->SetAutoTimeStampEnable(false);
								strInPackRelPath =""; for(j =i+1; j<uDirLevNum; j++){strInPackRelPath +=arrHierarchy[j]; strInPackRelPath +="\\";} strInPackRelPath +=strFileName;
								break;
							}
						}
					}
					if(lpPackage == NULL){strPackPath =""; strPackFullPath =""; strInPackRelPath ="";}
					refPackage =lpPackage;
				}
				cn::Buffer &bufInPackFileData =lpDLLTLSData->m_bufInPackFileData; //��ʾ��Դ���ڵ��ļ����ݻ���
				cn::u64 uFileTimeStamp =0; //��ʾ��Դ�ļ���Ӧ��ʱ���(==0��ʾʱ���δ֪)
				cn::u64 uPackTimeStamp =0; //��ʾ��Դ����Ӧ��ʱ���(==0��ʾʱ���δ֪)
				if(lpPackage){ //�������ѻ�������Դ���Ļ�
					uInPackRelPathMD5 =DiskIO::GetMD5FromBuffer(strInPackRelPath.data(), strInPackRelPath.length());
					if(lpPackage->GetFileData(uInPackRelPathMD5, strInPackRelPath.c_str(), bufInPackFileData, uFileTimeStamp) == false){ //����Դ�ļ����ڱ��ػ������Դ���еĻ�
						bNeedQueryServer =true;
					}
					uPackTimeStamp =lpPackage->GetTimeStamp();
				}else if(::PathFileExists(strLocalFullPath.c_str())){ //�������ѻ�������Դ�ļ��Ļ�
					WIN32_FILE_ATTRIBUTE_DATA cAttrData;
					if(::GetFileAttributesEx(strLocalFullPath.c_str(), GetFileExInfoStandard, &cAttrData)){
						uFileTimeStamp =(cn::u64(cAttrData.ftLastWriteTime.dwLowDateTime)<<0)|(cn::u64(cAttrData.ftLastWriteTime.dwHighDateTime)<<32); //��ȡ������Դ�ļ���ʱ���
					}
				}
				if(bNeedQueryServer){ //����Ҫ��������ύ����Ļ�
#define _PACKAGE_SAFE_THROW if(lpPackage){AutoSync cAutoSync(&g_cGolFilePackageSyncObj); refPackage =NULL; refTemp.SetObj(NULL);} throw 1;
					std::string strFinalURL =cn::Literal::GetFormattedStr("%hs&filetimel=%u&filetimeh=%u&packpath=%hs&packtimel=%u&packtimeh=%u", pFilePath, 
						cn::u32((uFileTimeStamp>>0)&0xffffffff), cn::u32((uFileTimeStamp>>32)&0xffffffff), lpPackage ? strPackPath.c_str() : "unknown",
						cn::u32((uPackTimeStamp>>0)&0xffffffff), cn::u32((uPackTimeStamp>>32)&0xffffffff));
					cn::Buffer &bufRev =lpDLLTLSData->m_bufRev; //��ʾ�ӷ��������յ������ݿ�

					cn::WRef refTemp; if(lpPackage){AutoSync cAutoSync(&g_cGolFilePackageSyncObj); refTemp =refPackage; refPackage =NULL;}
					if(lpDownloadingSync){cn::AutoSync cAutoSync(lpDownloadingSync); if(pDownloading){*pDownloading =1;}}
					if(lpWebDownload->DownloadFromURL(strFinalURL.c_str(), &bufRev) == false){ //������������ʧ�ܵĻ���������˳�
						if(lpDownloadingSync){cn::AutoSync cAutoSync(lpDownloadingSync); if(pDownloading){*pDownloading =0;}}
						_PACKAGE_SAFE_THROW
					}
					if(lpDownloadingSync){cn::AutoSync cAutoSync(lpDownloadingSync); if(pDownloading){*pDownloading =0;}}
					if(lpPackage){AutoSync cAutoSync(&g_cGolFilePackageSyncObj); if(refTemp.Get()){refPackage =refTemp.Get(); refTemp.SetObj(NULL);}else{lpPackage =NULL;}}
//if(g_cResServerTest.RequestEngineRes(strFinalURL.c_str(), bufRev) == false){
//	_PACKAGE_SAFE_THROW
//}
					//�������������ص�����
					cn::u32 uRevLen =bufRev.GetLen();
					if(uRevLen == 0){_PACKAGE_SAFE_THROW}
					cn::u8 *pRevBuf =(cn::u8 *)(bufRev.GetData()); cn::u32 uRevBias =0;
					if(uRevBias+sizeof(cn::u32)>uRevLen){_PACKAGE_SAFE_THROW} cn::u32 uStrLen =*((cn::u32 *)(pRevBuf+uRevBias)); pRevBuf +=sizeof(cn::u32);
					std::string strNewPackPath(uStrLen, '0');
					if(uStrLen > 0){if(uRevBias+sizeof(char)*uStrLen>uRevLen){_PACKAGE_SAFE_THROW} ::memcpy((void *)(strNewPackPath.data()), pRevBuf+uRevBias, sizeof(char)*uStrLen); pRevBuf +=sizeof(char)*uStrLen;}
					if(strNewPackPath.length() > 0){ //������������Դ�ļ�����һ����Ч����Դ���Ļ�
						if(uRevBias+sizeof(cn::u64)>uRevLen){_PACKAGE_SAFE_THROW} cn::u64 uNewPackTimeStamp =*((cn::u64 *)(pRevBuf+uRevBias)); pRevBuf +=sizeof(cn::u64);
						if(uRevBias+sizeof(cn::u32)>uRevLen){_PACKAGE_SAFE_THROW} cn::u32 uNewInPackDelNum =*((cn::u32 *)(pRevBuf+uRevBias)); pRevBuf +=sizeof(cn::u32);
						cn::u128 *pNewDelMD5s =NULL;
						if(uNewInPackDelNum>0 && uNewInPackDelNum<0xffffffff){
							if(uRevBias+sizeof(cn::u128)*uNewInPackDelNum>uRevLen){_PACKAGE_SAFE_THROW} pNewDelMD5s =(cn::u128 *)(pRevBuf+uRevBias); pRevBuf +=sizeof(cn::u128)*uNewInPackDelNum;
						}
						if(strPackPath != strNewPackPath){ //����Դ�ļ���������Դ�������仯�Ļ�
							AutoSync cAutoSync(&g_cGolFilePackageSyncObj);
							if(lpPackage){refPackage =NULL; lpPackage =NULL; m_mapGolFilePackages[strPackFullPath] =NULL;}
							for(i =0; i<uDirLevNum; i++){ //������Ŀ¼�������Դ��
								strPackPath =arrPackPaths[i]; strPackFullPath =strLocalPathPrefix+strPackPath; cn::Ref &refCurPack =m_mapGolFilePackages[strPackFullPath];
								if(strPackPath != strNewPackPath){ //����ǰ·���Ӵ���Ϊ��Ч��Դ���Ļ�����ǿ����Ϊ��Ч
									refCurPack =NULL;
								}else if(refCurPack.Get()){ //���µ���Ч��Դ���Ѿ������Ļ�
									lpPackage =(FilePackage *)(refCurPack.Get());
									strInPackRelPath =""; for(j =i+1; j<uDirLevNum; j++){strInPackRelPath +=arrHierarchy[j]; strInPackRelPath +="\\";} strInPackRelPath +=strFileName; break;
								}else{ //���µ���Ч��Դ����δ�����Ļ�
									refCurPack =FilePackage::Create(strPackFullPath.c_str(), 1024*256, 0, false); lpPackage =(FilePackage *)(refCurPack.Get());
									if(lpPackage){ //����Դ����Ч�Ļ�
										lpPackage->SetAutoTimeStampEnable(false);
										strInPackRelPath =""; for(j =i+1; j<uDirLevNum; j++){strInPackRelPath +=arrHierarchy[j]; strInPackRelPath +="\\";} strInPackRelPath +=strFileName; break;
									}else{throw 1;}
								}
							}
							if(lpPackage == NULL){throw 1;} refPackage =lpPackage;
						}
						if(lpPackage->GetTimeStamp() != uNewPackTimeStamp){ //��������Դ����δ���µĻ�
							AutoSync cAutoSync(&g_cGolFilePackageSyncObj);
							if(lpPackage->GetTimeStamp() != uNewPackTimeStamp){ //���ٴμ�Ȿ����Դ����û�б������߳����ȸ��²�����ļ��Ļ�
								if(uNewInPackDelNum == 0xffffffff){ //����Ҫ��ձ�����Դ���Ļ�
									lpPackage->Clear(1024*256, 0);
								}else if(uNewInPackDelNum > 0){ //����Ҫɾ��������Դ���е��Ͼ��ļ��Ļ�
									lpPackage->DelFiles(uNewInPackDelNum, pNewDelMD5s, NULL, NULL);
								}
								lpPackage->SetTimeStamp(uNewPackTimeStamp); //���±�����Դ����ʱ���
							}
						}
						if(uRevBias+sizeof(cn::u64)>uRevLen){_PACKAGE_SAFE_THROW} cn::u64 uNewFileTimeStamp =*((cn::u64 *)(pRevBuf+uRevBias)); pRevBuf +=sizeof(cn::u64);
						if(uRevBias+sizeof(cn::u32)>uRevLen){_PACKAGE_SAFE_THROW} cn::u32 uNewFileDataLen =*((cn::u32 *)(pRevBuf+uRevBias)); pRevBuf +=sizeof(cn::u32);
						if(uNewFileDataLen < 0xffffffff){ //���ļ�������Ҫ���µĻ�
							bufInPackFileData.Resize(uNewFileDataLen);
							if(uNewFileDataLen > 0){if(uRevBias+sizeof(cn::u8)*uNewFileDataLen>uRevLen){_PACKAGE_SAFE_THROW} ::memcpy(bufInPackFileData.GetData(), pRevBuf+uRevBias, sizeof(cn::u8)*uNewFileDataLen); pRevBuf +=sizeof(cn::u8)*uNewFileDataLen;}
							uInPackRelPathMD5 =DiskIO::GetMD5FromBuffer(strInPackRelPath.data(), strInPackRelPath.length());
							lpPackage->AddAFile(uInPackRelPathMD5, strInPackRelPath.c_str(), (const cn::u8 *)(bufInPackFileData.GetData()), bufInPackFileData.GetLen(), true, &uNewFileTimeStamp);
						}
					}else{ //������������Դ�ļ��������κ���Դ���Ļ�
						if(lpPackage){AutoSync cAutoSync(&g_cGolFilePackageSyncObj); refPackage =NULL; lpPackage =NULL; m_mapGolFilePackages[strPackFullPath] =NULL;}
						if(uRevBias+sizeof(cn::u64)>uRevLen){throw 1;} cn::u64 uNewFileTimeStamp =*((cn::u64 *)(pRevBuf+uRevBias)); pRevBuf +=sizeof(cn::u64);
						if(uRevBias+sizeof(cn::u32)>uRevLen){throw 1;} cn::u32 uNewFileDataLen =*((cn::u32 *)(pRevBuf+uRevBias)); pRevBuf +=sizeof(cn::u32);
						if(uNewFileDataLen < 0xffffffff){ //���ļ�������Ҫ���µĻ�
							if(uNewFileDataLen>0 && uRevBias+sizeof(cn::u8)*uNewFileDataLen>uRevLen){throw 1;}
							DiskIO::CreatePathHierarchy(strLocalFullPath.c_str());
							::DeleteFile(strLocalFullPath.c_str());
							HANDLE hFile =::CreateFile(strLocalFullPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
							if(hFile != INVALID_HANDLE_VALUE){
								cn::u32 uWrittenDataLen =0;
								while(uWrittenDataLen < uNewFileDataLen){
									cn::u32 uCurLen; if(::WriteFile(hFile, pRevBuf+uRevBias, uNewFileDataLen-uWrittenDataLen, (DWORD *)(&uCurLen), NULL) == FALSE){::CloseHandle(hFile); throw 1;}
									uWrittenDataLen +=uCurLen; uRevBias +=uCurLen;
								}
								FILETIME cTempTime; cTempTime.dwLowDateTime =cn::u32((uNewFileTimeStamp>>0)&0xffffffff); cTempTime.dwHighDateTime =cn::u32((uNewFileTimeStamp>>32)&0xffffffff); 
								if(::SetFileTime(hFile, &cTempTime, &cTempTime, &cTempTime) == FALSE){throw 1;}
								::CloseHandle(hFile);
							}
						}
					}
				}
				Ref refRetFile; //��ʾҪ���ص��ļ����ʶ���
				if(lpPackage){ //��ʹ����Դ���ļ����ƵĻ�
					FileInPackIO *lpFileInPackIO =NULL;
					if(bUseGolMemData){
						refRetFile =lpDLLTLSData->m_refGolFileFilter; lpFileInPackIO =(FileInPackIO *)(refRetFile.Get());
						if(lpFileInPackIO){
							lpFileInPackIO->Reset(strLocalFullPath.c_str(), strMode.c_str(), refPackage, &uInPackRelPathMD5, strInPackRelPath, bufInPackFileData.GetData(), bufInPackFileData.GetLen());
						}else{
							lpFileInPackIO =new FileInPackIO(strLocalFullPath.c_str(), strMode.c_str(), refPackage, &uInPackRelPathMD5, strInPackRelPath, bufInPackFileData.GetData(), bufInPackFileData.GetLen());
							refRetFile =lpDLLTLSData->m_refGolFileFilter =Ref(lpFileInPackIO, NULL, NULL, NULL, NULL, NULL);
						}
					}else{						
						lpFileInPackIO =new FileInPackIO(strLocalFullPath.c_str(), strMode.c_str(), refPackage, &uInPackRelPathMD5, strInPackRelPath, bufInPackFileData.GetData(), bufInPackFileData.GetLen());
						refRetFile =Ref(lpFileInPackIO, NULL, NULL, NULL, NULL, NULL);
					}
					if(lpFileInPackIO->UpdateFromPack() == false){
						refRetFile =Ref(NULL); throw 1;
					}
				}else{ //��ʹ����ͨ�����ļ����ƵĻ�
					refRetFile =Ref(new FileIOImp(strLocalFullPath.c_str(), strMode.c_str()), NULL, NULL, NULL, NULL, NULL);
				}
				return refRetFile;

			}else if(TRUE == PathIsUNC(pFilePath)){ //����UNC·���Ļ�
				throw 1;
			}else{ //���Ǳ���·���Ļ�

				DLL_TLS_DATA *lpDLLTLSData = cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
				std::string strNorFilePath =cn::Literal::ToLower(pFilePath);
				cn::u32 uDirLevNum; const char *pFileName;
				const char **ppHierarchy =cn::DiskIO::GetPathHierarchy(pFilePath, uDirLevNum, pFileName);
				std::vector<std::string> arrHierarchy(uDirLevNum); std::string strFileName =pFileName; std::vector<std::string> arrPackPaths(uDirLevNum); 
				std::string strTemp =""; for(i =0; i<uDirLevNum; i++){arrHierarchy[i] =ppHierarchy[i]; strTemp +=arrHierarchy[i]; arrPackPaths[i] =strTemp+".pak"; strTemp +="\\";}
				Ref refRetFile; //��ʾҪ���ص��ļ����ʶ���
				if(uDirLevNum > 0){ //��Ŀ���ļ�Ϊ�༶Ŀ¼�ļ��Ļ�
					AutoSync cAutoSync(&g_cGolFilePackageSyncObj); //�������ȫ����Դ������

					FilePackage *lpPackage =NULL; std::string strPackRelPath;
					for(i =0; i<uDirLevNum; i++){ //������Ŀ¼�������Դ���Ĵ�����
						std::map<std::string, Ref/*FilePackage*/>::iterator iterFind =m_mapGolFilePackages.find(arrPackPaths[i]);
						if(iterFind != m_mapGolFilePackages.end()){
							lpPackage =(FilePackage *)(iterFind->second.Get());
							if(lpPackage){
								strPackRelPath =""; for(j =i+1; j<uDirLevNum; j++){strPackRelPath +=arrHierarchy[j]; strPackRelPath +="\\";} strPackRelPath +=strFileName; break;
							}
						}else if(::PathFileExists(arrPackPaths[i].c_str())){ //���״μ�⵽����Դ���Ļ�
							for(j =0; j<i; j++){m_mapGolFilePackages[arrPackPaths[j]] =Ref(NULL);} //����Դ����ǰ׺·��ǿ����Ϊ��Ч��Դ��״̬
							m_mapGolFilePackages[arrPackPaths[i]] =FilePackage::Create(arrPackPaths[i].c_str(), 1024*256, 0, true);
							lpPackage =(FilePackage *)(m_mapGolFilePackages[arrPackPaths[i]].Get());
							if(lpPackage){ //����Դ����Ч�Ļ�
								lpPackage->SetAutoTimeStampEnable(false);
								strPackRelPath =""; for(j =i+1; j<uDirLevNum; j++){strPackRelPath +=arrHierarchy[j]; strPackRelPath +="\\";} strPackRelPath +=strFileName;
								break;
							}
						}
					}
					if(lpPackage){
						cn::Ref refPackage =lpPackage; 
						if(bUseGolMemData){
							refRetFile =lpDLLTLSData->m_refGolFileFilter; FileInPackIO *lpFileInPackIO =(FileInPackIO *)(refRetFile.Get());
							if(lpFileInPackIO){
								lpFileInPackIO->Reset(strNorFilePath.c_str(), strMode.c_str(), refPackage, NULL, strPackRelPath, NULL, 0);
							}else{
								refRetFile =lpDLLTLSData->m_refGolFileFilter =Ref(new FileInPackIO(strNorFilePath.c_str(), strMode.c_str(), refPackage, NULL, strPackRelPath, NULL, 0), NULL, NULL, NULL, NULL, NULL);
							}
						}else{						
							refRetFile =Ref(new FileInPackIO(strNorFilePath.c_str(), strMode.c_str(), refPackage, NULL, strPackRelPath, NULL, 0), NULL, NULL, NULL, NULL, NULL);
						}
					}
				}
				if(refRetFile.Get()){ //��ʹ����Դ���ļ����ƵĻ�
					FileInPackIO *lpFileInPackIO =(FileInPackIO *)(refRetFile.Get());
					if(lpFileInPackIO->UpdateFromPack()){
						return refRetFile;
					}else{
						refRetFile =Ref(NULL); throw 1;
					}
				}else{ //��ʹ����ͨ�����ļ����ƵĻ�
					if(strMode.find('r')==std::string::npos){ //��������ֻ�����r�����б�Ҫ�����ļ���
						DiskIO::CreatePathHierarchy(strNorFilePath.c_str());
					}else if(::PathFileExists(strNorFilePath.c_str()) == FALSE){ //���ж���ǣ����ļ������ڵĻ�
						throw 1;
					}
					refRetFile =Ref(new FileIOImp(strNorFilePath.c_str(), pMode), NULL, NULL, NULL, NULL, NULL);
					return refRetFile;
				}
			}
		}
		break;
	default:
		throw 1;
	}
	return Ref(NULL);
}


//����һ���ڴ���ʶ���(�����throw)
//pInitData����ʾ�ڴ���ʶ���ĳ�ʼ�����׵�ַ
//uInitDatalen����ʾ�ڴ���ʶ���ĳ�ʼ���ݴ�С
Ref/*IDiskIO*/ DiskIO::CreateMemIO(const void *pInitData, u32 uInitDatalen)
{
	Ref refRetIO =Ref(new MemIO(pInitData, uInitDatalen));
	return refRetIO;
}


//ɾ��һ�������ļ�
bool DiskIO::Delete(const char *pFileName)
{
	return ::DeleteFile(pFileName)==TRUE;
}


//ɾ��һ�����ļ���
bool DiskIO::DeleteEmptyDir(const char *pDirName)
{
	return ::RemoveDirectory(pDirName)==TRUE;
}


//����һ���ļ�
bool DiskIO::Copy(const char *pOldFilePath, const char *pNewFilePath)
{
	return ::CopyFile(pOldFilePath, pNewFilePath, FALSE)==TRUE;
}


//��ȡһ��URL�ַ����������ض���·����Ϣ
//pURL����ʾԭʼURL�ַ���
//pStartPos������·���Ӵ���Ӧ��pURL�е���ʼ����
//pEndPos������·���Ӵ���Ӧ��pURL�е����һ����Ч����+1
//����ֵ������·���Ӵ�
const char *DiskIO::GetURL_EngineDir(const char *pURL, cn::u32 *pStartPos, cn::u32 *pEndPos)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	char *pTempBuf =lpDLLTLSData->m_pURLTempBuf;
	cn::u32 i, j;

	cn::u32 uState =0;
	cn::u32 uStart =-1;
	for(i =0, j =0; pURL[j]!='\0'; j++){
		char uCurChar =pURL[j];
		switch(uState)
		{
		case 0:
			if(uCurChar == '?'){uState =1;}
			break;
		case 1:
			if(uCurChar == 'p'){uState =2;}
			break;
		case 2:
			if(uCurChar == 'a'){uState =3;}else{uState =1;}
			break;
		case 3:
			if(uCurChar == 't'){uState =4;}else{uState =1;}
			break;
		case 4:
			if(uCurChar == 'h'){uState =5;}else{uState =1;}
			break;
		case 5:
			if(uCurChar == '='){uState =6;}else{uState =1;}
			break;
		case 6:
			if(uCurChar=='&' || uCurChar=='#'){
				goto _SEARTH_END;
			}else if(i < _MAX_STR_LEN){
				if(uStart > j){uStart =j;}
				pTempBuf[i++] =uCurChar;
			}else{
				goto _SEARTH_END;
			}
			break;
		default:
			break;
		}
	}
_SEARTH_END:
	pTempBuf[i] ='\0';
	if(i > 0){
		if(pStartPos){*pStartPos =uStart;} if(pEndPos){*pEndPos =uStart+i;}
	}else{
		if(pStartPos){*pStartPos =0;} if(pEndPos){*pEndPos =0;}
	}
	return pTempBuf;
}


//��һ��URL�ַ�����չΪ���������������ʽ��URL
const char *DiskIO::GetURL_EngineParams(const char *pURL)
{
	cn::u32 i, j, k;

	if(pURL == NULL){return "";}
	cn::u32 uOrigLen =::strlen(pURL);
	if(uOrigLen == 0){return "";}
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	
	//����������չ����
	const char *pRet ="";
	if(pURL[uOrigLen-1]!='&' || uOrigLen>_MAX_STR_LEN-100){ //��ԭʼURL��������չ��������Ļ�
		pRet =pURL;
	}else{
		char *pTempBuf =lpDLLTLSData->m_pURLTempBuf2;
		char pTempBuf2[_MAX_STR_LEN+1];
		cn::u32 uX_1, uX_2, uZ_1, uZ_2;
		cn::u32 uState =0;
		for(i =1, j =0; i<uOrigLen; i++){
			cn::u32 uCurPos =uOrigLen-i-1;
			char uCurChar =pURL[uCurPos];
			switch(uState)
			{
			case 0:
				pTempBuf2[j++] =uCurChar;
				if(uCurChar == '.'){uState =1;}
				break;
			case 1:
				pTempBuf2[j++] =uCurChar;
				if(uCurChar == ']'){
					uZ_2 =uCurPos; pTempBuf2[j++] ='d'; pTempBuf2[j++] ='%'; 
					uState =2;
				}
				break;
			case 2:
				if(uCurChar == '_'){
					uZ_1 =uCurPos+1; uX_2 =uCurPos; pTempBuf2[j++] ='_'; pTempBuf2[j++] ='d'; pTempBuf2[j++] ='%'; 
					uState =3;
				}
				break;
			case 3:
				if(uCurChar=='_' || uCurChar=='['){
					uX_1 =uCurPos+1; pTempBuf2[j++] =uCurChar;
					uState =4;
				}
				break;
			case 4:
				if(uCurChar=='\\' || uCurChar=='/' || uCurChar=='=' || uCurChar=='&' || uCurChar=='?'){
					uState =5; goto _SEARTH_END;
				}else{
					pTempBuf2[j++] =uCurChar;
				}
				break;
			default:
				break;
			}
		}
_SEARTH_END:
		if(uState != 5){ //��������չ����ʶ��ʧ�ܵĻ�
			pRet =pURL;
		}else{
			i =uOrigLen-i;
			::memcpy(pTempBuf, pURL, sizeof(char)*(i));
			::memcpy(pTempBuf+i, "&id=", sizeof(char)*(4)); i +=4;
			for(k =0; k<j; k++){pTempBuf[i++] =pTempBuf2[j-k-1];}
			::memcpy(pTempBuf+i, "&idx=", sizeof(char)*(5)); i +=5;
			::memcpy(pTempBuf+i, pURL+uX_1, sizeof(char)*(uX_2-uX_1)); i +=(uX_2-uX_1);
			::memcpy(pTempBuf+i, "&idz=", sizeof(char)*(5)); i +=5;
			::memcpy(pTempBuf+i, pURL+uZ_1, sizeof(char)*(uZ_2-uZ_1)); i +=(uZ_2-uZ_1);
			pTempBuf[i++] ='\0';
			pRet =pTempBuf;
		}
	}

	return pRet;
}


//��һ��URL���󷵻ص����ݷ���ָ����Buffer
bool DiskIO::RequestDataInServer(const char* pURL,cn::Buffer* pBuf)
{
	DLL_TLS_DATA *lpDLLTLSData = cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	cn::WebDownload *lpWebDownload = lpDLLTLSData->m_lpWdl;
	return lpWebDownload->DownloadFromURL(pURL,pBuf);
}

//��ȡһ������·�������в㼶�ֶ�
//pPath����ʾ·���ַ���
//uDirLevNum������·���г��ļ�����Ĳ㼶����
//pFileName������·���е��ļ���
//����ֵ�����ز㼶�ֶ�����(�㼶�ֶ��в������㼶��ָ�����'\\'��'/')
const char **DiskIO::GetPathHierarchy(const char *pPath, cn::u32 &uDirLevNum, const char *&pFileName)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	char *pTempBuf =lpDLLTLSData->m_pPathTempBuf;
	std::vector<const char *> &arrTemp2 =lpDLLTLSData->m_arrPathTemp2;
	cn::u32 i;

	uDirLevNum =0; pFileName ="";
	if(pPath == NULL){
		return NULL;
	}
	arrTemp2.clear();
	cn::u32 uLastSlashPos =-1;
	for(i =0; i<_MAX_STR_LEN && pPath[i]!='\0'; i++){
		if(pPath[i]=='\\' || pPath[i]=='/'){
			if(i > 0){
				if(uLastSlashPos < _MAX_STR_LEN){
					arrTemp2.push_back(pTempBuf+uLastSlashPos+1); uDirLevNum++;
				}else{
					arrTemp2.push_back(pTempBuf); uDirLevNum++;
				}
			}
			pTempBuf[i] ='\0';
			uLastSlashPos =i;
		}else{
			pTempBuf[i] =pPath[i];
		}
	}
	pTempBuf[i] ='\0';
	pFileName =((uLastSlashPos<_MAX_STR_LEN) ? pTempBuf+uLastSlashPos+1 : pTempBuf);
	return (uDirLevNum>0) ? &(arrTemp2[0]) : NULL;
}


//����һ������·���������ļ��в㼶
void DiskIO::CreatePathHierarchy(const char *pPath)
{
	cn::u32 i;

	if(pPath == NULL){
		return;
	}
	cn::u32 uDirLevNum; const char *pFileName;
	const char **ppHierarchy =cn::DiskIO::GetPathHierarchy(pPath, uDirLevNum, pFileName);
	std::string strCur ="";
	for(i =0; i<uDirLevNum; i++){
		strCur =cn::Literal::GetFormattedStr("%hs%hs\\", strCur.c_str(), ppHierarchy[i]);
		::CreateDirectory(strCur.c_str(), NULL);
	}
}


//���һ���ļ����������ļ������ļ���
bool DiskIO::CleanUpDir(const char* pszDir)
{
	if(FALSE == ::PathIsDirectory(pszDir)){
		return false;
	}
	HANDLE hFind = NULL;	WIN32_FIND_DATA fileinfo;	std::string strFilePath =pszDir;
	if('\\'!=pszDir[strlen(pszDir)-1]){strFilePath += "\\";}
	std::string strFileFilter = strFilePath + "*.*";
	hFind = FindFirstFile(strFileFilter.c_str(),&fileinfo);
	if(INVALID_HANDLE_VALUE == hFind){
		return false;
	}else{
		do{
			if(fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				if (!strcmp(fileinfo.cFileName,".") || !strcmp(fileinfo.cFileName,"..")){
					//Do nothing for "." and ".." folders
				}else{
					std::string strtmp = strFilePath + fileinfo.cFileName + "\\";
					CleanUpDir(strtmp.c_str());	::RemoveDirectory(strtmp.c_str());										
				}
			}else{
				std::string strtmp = strFilePath+ fileinfo.cFileName;
				::DeleteFile(strtmp.c_str());		
			}		
		}while(FindNextFile(hFind,&fileinfo));
		FindClose(hFind);
	}	
	return true;
}


//��ȡһ�����繲��·�������в㼶�ֶ�
//pPath����ʾ·���ַ���
//uDirLevNum������·���г��ļ�����Ĳ㼶����
//pFileName������·���е��ļ���
//����ֵ�����ز㼶�ֶ�����(�㼶�ֶ��в������㼶��ָ�����'\\'��'/')
const char **DiskIO::GetUNCHierarchy(const char *pUNCPath, cn::u32 &uDirLevNum, const char *&pFileName)
{
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	char *pTempBuf =lpDLLTLSData->m_pUNCTempBuf;
	std::vector<const char *> &arrTemp2 =lpDLLTLSData->m_arrUNCTemp2;
	cn::u32 i;

	uDirLevNum =0; pFileName ="";
	if(pUNCPath == NULL){
		return NULL;
	}
	arrTemp2.clear();
	cn::u32 uLastSlashPos =-1;
	cn::u32 uState =0;
	for(i =0; i<_MAX_STR_LEN && pUNCPath[i]!='\0'; i++){
		switch(uState)
		{
		case 0:
			pTempBuf[i] =pUNCPath[i];
			if(pUNCPath[i] == '\\'){uState =1;}else{return NULL;}
			break;
		case 1:
			pTempBuf[i] =pUNCPath[i];
			if(pUNCPath[i] == '\\'){uState =2;}else{return NULL;}
			break;
		case 2:
			{
				if(pUNCPath[i]=='\\' || pUNCPath[i]=='/'){
					if(uLastSlashPos < _MAX_STR_LEN){
						arrTemp2.push_back(pTempBuf+uLastSlashPos+1); uDirLevNum++;
					}else{
						arrTemp2.push_back(pTempBuf); uDirLevNum++;
					}
					pTempBuf[i] ='\0';
					uLastSlashPos =i;
				}else{
					pTempBuf[i] =pUNCPath[i];
				}
			}
			break;
		default:
			break;
		}
	}
	pTempBuf[i] ='\0';
	if(uLastSlashPos < _MAX_STR_LEN){
		pFileName =pTempBuf+uLastSlashPos+1;
	}else{
		pFileName =""; 
		arrTemp2.push_back(pTempBuf); uDirLevNum++;
	}
	return (uDirLevNum>0) ? &(arrTemp2[0]) : NULL;
}


//��ZIPѹ���ĵ�ģ�����һ��ZIP�ļ�
bool ZipIO::AddAZipFile(const char *pZipName)
{
	return false;
}


//�Ƴ������Ѽ��ص�ZIP�ļ�
void ZipIO::RemoveAllZipFiles()
{

}


//��ʾд��һ���ṹ�������ݵĺ�
#define _WRITE_TO_FILE(type,object,file) \
	if(file){ \
		::fwrite(&object, sizeof(type), 1, file); \
	}else{ \
		throw 1; \
	}

//��ʾ��ȡһ���ṹ�������ݵĺ�
#define _READ_FROM_FILE(type,file) \
	if(file){ \
		type TempRet; \
		::fread(&TempRet, sizeof(type), 1, file); \
		return TempRet; \
	}else{ \
		throw 1; \
	}




//static std::string ssss;



//���캯��(�����throw)
FileIOImp::FileIOImp(const char *pFileName, const char *pMode)
{
	m_lpFile =::fopen(pFileName, pMode);

//ssss =pMode;

	if(!m_lpFile){

//if(ssss=="w+b"){
//	int lskdjf =342;
//}

		throw 1;
	}
}


//��������
FileIOImp::~FileIOImp()
{

//	if(ssss=="w+b" && GetLength()==0){
//int lsdkjf =2424;
//	}

	Close();
}


//Ѱַ�������ļ���ĳ��λ����(�����throw)
void FileIOImp::Seek(DiskIO::SEEK_ORIG eSeekOrig, s64 sOffset)
{
	if(m_lpFile){
		int nOrig;
		switch(eSeekOrig)
		{
		case DiskIO::SEEK_ORIG_BEGIN:
			nOrig =SEEK_SET;
			break;
		case DiskIO::SEEK_ORIG_CUR:
			nOrig =SEEK_CUR;
			break;
		case DiskIO::SEEK_ORIG_END:
			nOrig =SEEK_END;
			break;
		default:
			break;
		}
		if(::_fseeki64(m_lpFile, sOffset, nOrig) != 0){
			throw 1;
		}
	}else{
		throw 1;
	}
}


//��ȡ��ǰ����λ��(�����throw)
s64 FileIOImp::GetCurPos()
{
	if(m_lpFile){
		s64 sTemp =::_ftelli64(m_lpFile);
		if(sTemp >= 0){
			return sTemp;
		}else{
			throw 1;
		}
	}else{
		throw 1;
	}
}


//��ȡ�ļ����ܳ���(�����throw)
s64 FileIOImp::GetLength()
{
	s64 sCurPos =GetCurPos();
	Seek(DiskIO::SEEK_ORIG_END, 0);
	s64 sLen =GetCurPos();
	Seek(DiskIO::SEEK_ORIG_BEGIN, sCurPos);
	return sLen;
}


//���ڲ��������е�����ȫ����ղ����µ��ļ���(�����throw)
void FileIOImp::Flush()
{
	if(m_lpFile){
		::fflush(m_lpFile);
	}else{
		throw 1;
	}
}


//��ʾ�رմ����ļ�(�����throw)
void FileIOImp::Close()
{
	if(m_lpFile){
		::fclose(m_lpFile);
		m_lpFile =NULL;
	}
}


void FileIOImp::WriteFloat(float fData)
{
	_WRITE_TO_FILE(float,fData,m_lpFile)
}


float FileIOImp::ReadFloat()
{
	_READ_FROM_FILE(float,m_lpFile)
}


void FileIOImp::WriteDouble(double dData)
{
	_WRITE_TO_FILE(double,dData,m_lpFile)
}


double FileIOImp::ReadDouble()
{
	_READ_FROM_FILE(double,m_lpFile)
}






//void FileIOImp::WriteString(const char *pStr, s32 sStrLen)
//{
//	if(m_lpFile){
//		::fwrite(&sStrLen, sizeof(s32), 1, m_lpFile);
//		::fwrite(pStr, sStrLen, 1, m_lpFile);
//	}else{ 
//		throw 1;
//	}
//}
//
//
//void FileIOImp::ReadString(Buffer/*char*/ &bufData)
//{
//	if(m_lpFile){
//		cn::u32 uStrLen;
//		::fread(&uStrLen, sizeof(u32), 1, m_lpFile);
//		bufData(uStrLen+1);
//		char *pStr =(char *)(bufData.GetData());
//		pStr[uStrLen] =0;
//		::fread(pStr, uStrLen, 1, m_lpFile);
//	}else{ 
//		throw 1;
//	}
//}






void FileIOImp::WriteString(const char *pStr, s32 sStrLen)
{
	if(m_lpFile){
		cn::s32 sNewStrLen =0;
		const WCHAR *pNewStr =cn::Literal::CharToWChar(pStr, sStrLen, &sNewStrLen);
		sNewStrLen =-sNewStrLen;
		::fwrite(&sNewStrLen, sizeof(s32), 1, m_lpFile);
		::fwrite(pNewStr, sizeof(WCHAR)*(-sNewStrLen), 1, m_lpFile);
	}else{ 
		throw 1;
	}
}


void FileIOImp::ReadString(Buffer/*char*/ &bufData)
{
	WCHAR *g_pTempStrBuf2 =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempStrBuf2;

	if(m_lpFile){
		cn::s32 sStrLen;
		::fread(&sStrLen, sizeof(s32), 1, m_lpFile);
		if(sStrLen >= 0){ //���ַ���ΪANSI�洢��ʽ�Ļ�

			bufData(sStrLen+1);
			char *pStr =(char *)(bufData.GetData());
			pStr[sStrLen] =0;
			::fread(pStr, sStrLen, 1, m_lpFile);

		}else{ //���ַ���ΪUnicode�洢��ʽ�Ļ�

			sStrLen =-sStrLen;
			if(sStrLen > _MAX_STR_LEN){ //���ַ������ȳ�����������˳�
				throw 1;
			}
			::fread(g_pTempStrBuf2, sizeof(WCHAR)*sStrLen, 1, m_lpFile);
			g_pTempStrBuf2[sStrLen] =0;
			
			cn::s32 sNewStrLen =0;
			const char *pNewStr =cn::Literal::WCharToChar(g_pTempStrBuf2, sStrLen, &sNewStrLen);
			bufData(sNewStrLen+1);
			char *pStr =(char *)(bufData.GetData());
			pStr[sNewStrLen] =0;
			::memcpy(pStr, pNewStr, sizeof(char)*sNewStrLen);

		}
	}else{ 
		throw 1;
	}
}



















void FileIOImp::WriteData(const void *pData, u32 uDataLen)
{
	if(m_lpFile){
		::fwrite(pData, uDataLen, 1, m_lpFile);
	}else{ 
		throw 1;
	}
}


void FileIOImp::ReadData(void *pData, u32 uDataLen)
{
	if(m_lpFile){
		::fread(pData, uDataLen, 1, m_lpFile);
	}else{ 
		throw 1;
	}
}


void FileIOImp::WriteS8(s8 sData)
{
	_WRITE_TO_FILE(s8,sData,m_lpFile)
}


s8 FileIOImp::ReadS8()
{
	_READ_FROM_FILE(s8,m_lpFile)
}


void FileIOImp::WriteU8(u8 uData)
{
	_WRITE_TO_FILE(u8,uData,m_lpFile)
}


u8 FileIOImp::ReadU8()
{
	_READ_FROM_FILE(u8,m_lpFile)
}


void FileIOImp::WriteS16(s16 sData)
{
	_WRITE_TO_FILE(s16,sData,m_lpFile)
}


s16 FileIOImp::ReadS16()
{
	_READ_FROM_FILE(s16,m_lpFile)
}


void FileIOImp::WriteU16(u16 uData)
{
	_WRITE_TO_FILE(u16,uData,m_lpFile)
}


u16 FileIOImp::ReadU16()
{
	_READ_FROM_FILE(u16,m_lpFile)
}


void FileIOImp::WriteS32(s32 sData)
{
	_WRITE_TO_FILE(s32,sData,m_lpFile)
}


s32 FileIOImp::ReadS32()
{
	_READ_FROM_FILE(s32,m_lpFile)
}


void FileIOImp::WriteU32(u32 uData)
{
	_WRITE_TO_FILE(u32,uData,m_lpFile)
}


u32 FileIOImp::ReadU32()
{
	_READ_FROM_FILE(u32,m_lpFile)
}


void FileIOImp::WriteS64(s64 sData)
{
	_WRITE_TO_FILE(s64,sData,m_lpFile)
}


s64 FileIOImp::ReadS64()
{
	_READ_FROM_FILE(s64,m_lpFile)
}


void FileIOImp::WriteU64(u64 uData)
{
	_WRITE_TO_FILE(u64,uData,m_lpFile)
}


u64 FileIOImp::ReadU64()
{
	_READ_FROM_FILE(u64,m_lpFile)
}


void FileIOImp::WriteVec2(const Vec2 &vData)
{
	_WRITE_TO_FILE(Vec2,vData,m_lpFile)
}


Vec2 FileIOImp::ReadVec2()
{
	_READ_FROM_FILE(Vec2,m_lpFile)
}


void FileIOImp::WriteVec3(const Vec3 &vData)
{
	_WRITE_TO_FILE(Vec3,vData,m_lpFile)
}


Vec3 FileIOImp::ReadVec3()
{
	_READ_FROM_FILE(Vec3,m_lpFile)
}


void FileIOImp::WriteVec3D(const Vec3D &vData)
{
	_WRITE_TO_FILE(Vec3D,vData,m_lpFile)
}


Vec3D FileIOImp::ReadVec3D()
{
	_READ_FROM_FILE(Vec3D,m_lpFile)
}


void FileIOImp::WriteVec4(const Vec4 &qData)
{
	_WRITE_TO_FILE(Vec4,qData,m_lpFile)
}


Vec4 FileIOImp::ReadVec4()
{
	_READ_FROM_FILE(Vec4,m_lpFile)
}


void FileIOImp::WritePlane(const Plane &qData)
{
	_WRITE_TO_FILE(Plane,qData,m_lpFile)
}


Plane FileIOImp::ReadPlane()
{
	_READ_FROM_FILE(Plane,m_lpFile)
}


void FileIOImp::WriteQuaternion(const Quaternion &qData)
{
	_WRITE_TO_FILE(Quaternion,qData,m_lpFile)
}


Quaternion FileIOImp::ReadQuaternion()
{
	_READ_FROM_FILE(Quaternion,m_lpFile)
}


void FileIOImp::WriteQuaternionD(const QuaternionD &qData)
{
	_WRITE_TO_FILE(QuaternionD,qData,m_lpFile)
}


QuaternionD FileIOImp::ReadQuaternionD()
{
	_READ_FROM_FILE(QuaternionD,m_lpFile)
}


void FileIOImp::WriteMat4x4(const Mat4x4 &matData)
{
	_WRITE_TO_FILE(Mat4x4,matData,m_lpFile)
}


Mat4x4 FileIOImp::ReadMat4x4()
{
	_READ_FROM_FILE(Mat4x4,m_lpFile)
}


void FileIOImp::WriteMat4x3(const Mat4x3 &matData)
{
	_WRITE_TO_FILE(Mat4x3,matData,m_lpFile)
}


Mat4x3 FileIOImp::ReadMat4x3()
{
	_READ_FROM_FILE(Mat4x3,m_lpFile)
}


void FileIOImp::WriteAABB(const AABB &bbData)
{
	_WRITE_TO_FILE(AABB,bbData,m_lpFile)
}


AABB FileIOImp::ReadAABB()
{
	_READ_FROM_FILE(AABB,m_lpFile)
}


void FileIOImp::WriteRay(const Ray &rayData)
{
	_WRITE_TO_FILE(Ray,rayData,m_lpFile)
}


Ray FileIOImp::ReadRay()
{
	_READ_FROM_FILE(Ray,m_lpFile)
}


void FileIOImp::WriteSphere(const Sphere &sphData)
{
	_WRITE_TO_FILE(Sphere,sphData,m_lpFile)
}


Sphere FileIOImp::ReadSphere()
{
	_READ_FROM_FILE(Sphere,m_lpFile)
}


void FileIOImp::WriteRect(const Rect &rcData)
{
	_WRITE_TO_FILE(Rect,rcData,m_lpFile)
}


Rect FileIOImp::ReadRect()
{
	_READ_FROM_FILE(Rect,m_lpFile)
}


void FileIOImp::WriteViewport(const Viewport &vpData)
{
	_WRITE_TO_FILE(Viewport,vpData,m_lpFile)
}


Viewport FileIOImp::ReadViewport()
{
	_READ_FROM_FILE(Viewport,m_lpFile)
}


void FileIOImp::WriteBuffer(const Buffer &bufData)
{
	if(m_lpFile){
		u32 uDataLen =bufData.GetLen();
		::fwrite(&uDataLen, sizeof(u32), 1, m_lpFile);
		::fwrite(bufData.GetData(), uDataLen, 1, m_lpFile);
	}else{ 
		throw 1;
	}
}


void FileIOImp::ReadBuffer(Buffer &bufData)
{
	if(m_lpFile){
		cn::u32 uDataLen;
		::fread(&uDataLen, sizeof(u32), 1, m_lpFile);
		bufData(uDataLen);
		::fread(bufData.GetData(), uDataLen, 1, m_lpFile);
	}else{ 
		throw 1;
	}
}


//��ʾд��һ���ṹ�������ݵĺ�
#define _WRITE_TO_FILEINPACK(type,object) \
	if(m_bValid){ \
		cn::s64 sEndPos =m_sFilePos+sizeof(type); \
		for(; m_sFileDataLen<sEndPos; m_sFileDataLen++){m_arrFileData.push_back(0);} \
		::memcpy(&(m_arrFileData[m_sFilePos]), &object, sizeof(type)); \
		m_sFilePos =sEndPos; \
	}else{ \
		throw 1; \
	}


//��ʾ��ȡһ���ṹ�������ݵĺ�
#define _READ_FROM_FILEINPACK(type) \
	if(m_bValid){ \
		if(m_sFilePos+sizeof(type) <= m_sFileDataLen){ \
			type &cRet =*((type *)(&(m_arrFileData[m_sFilePos]))); \
			m_sFilePos +=sizeof(type); \
			return cRet; \
		}else{ \
			throw 1; \
		} \
	}else{ \
		throw 1; \
	}


//���캯��(�����throw)
//refPackage����ʾ�ļ���������Դ������(�ñ����ڲ��ᱻ�Զ����)
//pPackRelPathMD5����ʾ�ļ�����Դ���ڵ����·����MD5ֵ����ΪNULL��ģ���ڲ��Զ�����
//strPackRelPath����ʾ�ļ�����Դ���ڵ����·��
//pFileOrigData,uFileOrigDataLen����ʾ�ļ�����Դ���е�ԭʼ�������ݣ�����Ч��ģ���ڲ��Զ���ȡԭʼ����
FileInPackIO::FileInPackIO(const char *pFileName, const char *pMode, cn::Ref/*FilePackage*/ &refPackage, const u128 *pPackRelPathMD5, const std::string &strPackRelPath, 
	const void *pFileOrigData, cn::u32 uFileOrigDataLen)
{
	AutoSync cAutoSync(&g_cGolFilePackageSyncObj); //�������ȫ����Դ������

	m_strMode =pMode;
	m_refPackage =refPackage; refPackage =NULL;
	m_lpPackage =(FilePackage *)(m_refPackage.Get());
	if(pPackRelPathMD5){m_bPathMD5Inited =true; m_uPackRelPathMD5 =*pPackRelPathMD5;}else{m_bPathMD5Inited =false;}
	m_strPackRelPath =strPackRelPath;
	m_bValid =false;
	m_bNeedSaveToPack =false;
	if(pFileOrigData && uFileOrigDataLen>0){m_bufFileOrigData(pFileOrigData, uFileOrigDataLen);}
	m_sFileDataLen =0; 
	m_sFilePos =0;

	m_uStatCount =0;
	m_dStatTime =0.0;
}


//��������
FileInPackIO::~FileInPackIO()
{
	Close();
}


//�����ļ�����ģ��
void FileInPackIO::Reset(const char *pFileName, const char *pMode, cn::Ref/*FilePackage*/ &refPackage, const u128 *pPackRelPathMD5, const std::string &strPackRelPath, 
	const void *pFileOrigData, cn::u32 uFileOrigDataLen)
{
	AutoSync cAutoSync(&g_cGolFilePackageSyncObj); //�������ȫ����Դ������

	m_strMode =pMode;
	m_refPackage =refPackage; refPackage =NULL;
	m_lpPackage =(FilePackage *)(m_refPackage.Get());
	if(pPackRelPathMD5){m_bPathMD5Inited =true; m_uPackRelPathMD5 =*pPackRelPathMD5;}else{m_bPathMD5Inited =false;}
	m_strPackRelPath =strPackRelPath;
	m_bValid =false;
	m_bNeedSaveToPack =false;
	if(pFileOrigData && uFileOrigDataLen>0){m_bufFileOrigData(pFileOrigData, uFileOrigDataLen);}else{m_bufFileOrigData.Resize(0);}
	m_arrFileData.clear();
	m_sFileDataLen =0; 
	m_sFilePos =0;
}


//����Դ���и������������
bool FileInPackIO::UpdateFromPack()
{
	cn::u64 uFileTimeStamp =0;
	m_bValid =false;
	m_bNeedSaveToPack =false;
	m_arrFileData.clear();
	m_sFileDataLen =0; 
	m_sFilePos =0;
	if(m_bPathMD5Inited == false){m_bPathMD5Inited =true; m_uPackRelPathMD5 =DiskIO::GetMD5FromBuffer(m_strPackRelPath.data(), m_strPackRelPath.length());}
	if(m_strMode.find('r')!=std::string::npos && m_strMode.find("r+")==std::string::npos){ //���ļ���������Ϊֻ���Ļ�
		if(m_bufFileOrigData.GetLen() == 0){ //���ļ�ԭʼ������Ч�Ļ��������Դ���ж�ȡ
			if(m_lpPackage->GetFileData(m_uPackRelPathMD5, m_strPackRelPath.c_str(), m_bufFileOrigData, uFileTimeStamp) == false){ //����Դ����û��ָ���ļ��Ļ�
				return false;
			}
		}
		m_bNeedSaveToPack =false; //����ڴ��е��ļ����ݲ����д����Դ����
	}else{ //���ļ���������Ϊ�ɶ�д�Ļ�
		bool bNeedGetOrigData =(m_bufFileOrigData.GetLen()==0) && (m_strMode.find("r+")!=std::string::npos || m_strMode.find('a')!=std::string::npos); //��ʾ�Ƿ���Ҫ��ȡ��Դ�����ļ���ԭ������
		const char *pPackRelPath =m_strPackRelPath.c_str();
		m_lpPackage->DelFiles(1, &m_uPackRelPathMD5, &pPackRelPath, bNeedGetOrigData ? &m_bufFileOrigData : NULL);
		m_bNeedSaveToPack =true; //����ڴ��е��ļ�������Ҫ��д����Դ����
	}
	if(m_bufFileOrigData.GetLen() > 0){ //���ļ���ʼ����������Ч�Ļ�
		cn::Buffer &bufUncompress =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_bufUncompress;
		if(DiskIO::UncompressBuffer(m_bufFileOrigData.GetData(), m_bufFileOrigData.GetLen(), bufUncompress)){
			m_sFileDataLen =bufUncompress.GetLen();
			
			cn::s64 sOrigCapacity =cn::s64(m_arrFileData.capacity()); cn::s64 sForceCapacity =-1;
			if(m_sFileDataLen < sOrigCapacity/2){
				m_uStatCount++;
				if(m_uStatCount > _MIN_MEM_RELEASE_COUNT){
					if(cn::IEventReceiver::GetSingleObj()->GetEventLevel() > 0){
						cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_INFO,
							cn::Literal::GetFormattedStr("FileInPackIO::UpdateFromPack = force resize Capacity[%u] StatCount[%u]", cn::u32(sOrigCapacity), m_uStatCount));
					}
					sForceCapacity =sOrigCapacity/2; m_uStatCount =0;
				}else if(m_uStatCount > _MIN_MEM_RELEASE_TIMESTAT_COUNT){
					double dCurTime =cn::Time::GetAbsTime(); double dInterval =dCurTime-m_dStatTime;
					if(dInterval > _MIN_MEM_RELEASE_TIMEOUT){
						if(cn::IEventReceiver::GetSingleObj()->GetEventLevel() > 0){
							cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_INFO,
								cn::Literal::GetFormattedStr("FileInPackIO::UpdateFromPack = force resize Capacity[%u] StatCount[%u]", cn::u32(sOrigCapacity), m_uStatCount));
						}
						sForceCapacity =sOrigCapacity/2; m_uStatCount =0;
					}			
				}else if(m_uStatCount == _MIN_MEM_RELEASE_TIMESTAT_COUNT){
					m_dStatTime =cn::Time::GetAbsTime();
				}
			}else{
				m_uStatCount =0;
			}
			if(sForceCapacity >= 0){
				{std::vector<cn::u8> arrTemp; m_arrFileData.swap(arrTemp);}
				m_arrFileData.resize(sForceCapacity);
			}

			m_arrFileData.resize(m_sFileDataLen); ::memcpy(&(m_arrFileData[0]), bufUncompress.GetData(), m_sFileDataLen);
		}
		m_bufFileOrigData.Resize(0);
	}
	m_bValid =true; //����ļ�����ģ�鴦����Ч״̬

	return true;
}


//Ѱַ�������ļ���ĳ��λ����(�����throw)
void FileInPackIO::Seek(SEEK_ORIG eSeekOrig, s64 sOffset)
{
	if(m_bValid){
		switch(eSeekOrig)
		{
		case DiskIO::SEEK_ORIG_BEGIN:
			m_sFilePos =sOffset;
			break;
		case DiskIO::SEEK_ORIG_CUR:
			m_sFilePos +=sOffset;
			break;
		case DiskIO::SEEK_ORIG_END:
			m_sFilePos =m_sFileDataLen+sOffset;
			break;
		default:
			break;
		}
		for(; m_sFileDataLen<m_sFilePos; m_sFileDataLen++){m_arrFileData.push_back(0);}
	}else{
		throw 1;
	}
}


//��ȡ��ǰ����λ��(�����throw)
s64 FileInPackIO::GetCurPos()
{
	if(m_bValid){
		return m_sFilePos;
	}else{
		throw 1;
	}
}


//��ȡ�ļ����ܳ���(�����throw)
s64 FileInPackIO::GetLength()
{
	if(m_bValid){
		return m_sFileDataLen;
	}else{
		throw 1;
	}
}


//���ڲ��������е�����ȫ����ղ����µ��ļ���(�����throw)
void FileInPackIO::Flush()
{

}


//��ʾ�رմ����ļ�(�����throw)
void FileInPackIO::Close()
{
	if(m_bValid && m_bNeedSaveToPack){
		if(m_bPathMD5Inited == false){m_bPathMD5Inited =true; m_uPackRelPathMD5 =DiskIO::GetMD5FromBuffer(m_strPackRelPath.data(), m_strPackRelPath.length());}
		m_lpPackage->AddAFile(m_uPackRelPathMD5, m_strPackRelPath.c_str(), (m_sFileDataLen>0) ? &(m_arrFileData[0]) : NULL, m_sFileDataLen, true, NULL);
	}
	{
		AutoSync cAutoSync(&g_cGolFilePackageSyncObj); //�������ȫ����Դ������
		m_refPackage =Ref(NULL);
		m_lpPackage =NULL;
	}
	m_bufFileOrigData.Resize(0);
	m_bNeedSaveToPack =false;
	m_arrFileData.clear();
	m_sFileDataLen =0;
	m_sFilePos =0;
	m_bValid =false; //����ļ�����ģ�鴦����Ч״̬
}


void FileInPackIO::WriteFloat(float fData)
{
	_WRITE_TO_FILEINPACK(float,fData)
}


float FileInPackIO::ReadFloat()
{
	_READ_FROM_FILEINPACK(float)
}


void FileInPackIO::WriteDouble(double dData)
{
	_WRITE_TO_FILEINPACK(double,dData)
}


double FileInPackIO::ReadDouble()
{
	_READ_FROM_FILEINPACK(double)
}


void FileInPackIO::WriteString(const char *pStr, s32 sStrLen)
{
	if(m_bValid){
		cn::s32 sNewStrLen =0;
		const WCHAR *pNewStr =cn::Literal::CharToWChar(pStr, sStrLen, &sNewStrLen);
		sNewStrLen =-sNewStrLen;
		cn::s64 sEndPos =m_sFilePos+sizeof(s32)+sizeof(WCHAR)*(-sNewStrLen);
		for(; m_sFileDataLen<sEndPos; m_sFileDataLen++){m_arrFileData.push_back(0);} 
		::memcpy(&(m_arrFileData[m_sFilePos]), &sNewStrLen, sizeof(s32)); m_sFilePos +=sizeof(s32);
		::memcpy(&(m_arrFileData[m_sFilePos]), pNewStr, sizeof(WCHAR)*(-sNewStrLen)); m_sFilePos +=sizeof(WCHAR)*(-sNewStrLen);
	}else{
		throw 1;
	}
}


void FileInPackIO::ReadString(Buffer/*char*/ &bufData)
{
	WCHAR *g_pTempStrBuf2 =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempStrBuf2;

	if(m_bValid){
		cn::s32 sStrLen;
		if(m_sFilePos+sizeof(s32) <= m_sFileDataLen){
			sStrLen =*((s32 *)(&(m_arrFileData[m_sFilePos]))); m_sFilePos +=sizeof(s32);
		}else{throw 1;}
		if(sStrLen >= 0){ //���ַ���ΪANSI�洢��ʽ�Ļ�

			bufData(sStrLen+1);
			char *pStr =(char *)(bufData.GetData());
			pStr[sStrLen] =0;
			if(m_sFilePos+sStrLen <= m_sFileDataLen){
				::memcpy(pStr, &(m_arrFileData[m_sFilePos]), sStrLen); m_sFilePos +=sStrLen;
			}else{throw 1;}

		}else{ //���ַ���ΪUnicode�洢��ʽ�Ļ�

			sStrLen =-sStrLen;
			if(sStrLen > _MAX_STR_LEN){ //���ַ������ȳ�����������˳�
				throw 1;
			}
			if(m_sFilePos+sizeof(WCHAR)*sStrLen <= m_sFileDataLen){
				::memcpy(g_pTempStrBuf2, &(m_arrFileData[m_sFilePos]), sizeof(WCHAR)*sStrLen); m_sFilePos +=sizeof(WCHAR)*sStrLen;
			}else{throw 1;}
			g_pTempStrBuf2[sStrLen] =0;
			
			cn::s32 sNewStrLen =0;
			const char *pNewStr =cn::Literal::WCharToChar(g_pTempStrBuf2, sStrLen, &sNewStrLen);
			bufData(sNewStrLen+1);
			char *pStr =(char *)(bufData.GetData());
			pStr[sNewStrLen] =0;
			::memcpy(pStr, pNewStr, sizeof(char)*sNewStrLen);

		}
	}else{ 
		throw 1;
	}
}


void FileInPackIO::WriteData(const void *pData, u32 uDataLen)
{
	if(m_bValid){
		cn::s64 sEndPos =m_sFilePos+uDataLen;
		for(; m_sFileDataLen<sEndPos; m_sFileDataLen++){m_arrFileData.push_back(0);} 
		::memcpy(&(m_arrFileData[m_sFilePos]), pData, uDataLen); m_sFilePos +=uDataLen;
	}else{
		throw 1;
	}
}


void FileInPackIO::ReadData(void *pData, u32 uDataLen)
{
	if(m_bValid){
		if(m_sFilePos+uDataLen <= m_sFileDataLen){
			::memcpy(pData, &(m_arrFileData[m_sFilePos]), uDataLen); m_sFilePos +=uDataLen;
		}else{throw 1;}
	}else{ 
		throw 1;
	}
}


void FileInPackIO::WriteS8(s8 sData)
{
	_WRITE_TO_FILEINPACK(s8,sData)
}


s8 FileInPackIO::ReadS8()
{
	_READ_FROM_FILEINPACK(s8)
}


void FileInPackIO::WriteU8(u8 uData)
{
	_WRITE_TO_FILEINPACK(u8,uData)
}


u8 FileInPackIO::ReadU8()
{
	_READ_FROM_FILEINPACK(u8)
}


void FileInPackIO::WriteS16(s16 sData)
{
	_WRITE_TO_FILEINPACK(s16,sData)
}


s16 FileInPackIO::ReadS16()
{
	_READ_FROM_FILEINPACK(s16)
}


void FileInPackIO::WriteU16(u16 uData)
{
	_WRITE_TO_FILEINPACK(u16,uData)
}


u16 FileInPackIO::ReadU16()
{
	_READ_FROM_FILEINPACK(u16)
}


void FileInPackIO::WriteS32(s32 sData)
{
	_WRITE_TO_FILEINPACK(s32,sData)
}


s32 FileInPackIO::ReadS32()
{
	_READ_FROM_FILEINPACK(s32)
}


void FileInPackIO::WriteU32(u32 uData)
{
	_WRITE_TO_FILEINPACK(u32,uData)
}


u32 FileInPackIO::ReadU32()
{
	_READ_FROM_FILEINPACK(u32)
}


void FileInPackIO::WriteS64(s64 sData)
{
	_WRITE_TO_FILEINPACK(s64,sData)
}


s64 FileInPackIO::ReadS64()
{
	_READ_FROM_FILEINPACK(s64)
}


void FileInPackIO::WriteU64(u64 uData)
{
	_WRITE_TO_FILEINPACK(u64,uData)
}


u64 FileInPackIO::ReadU64()
{
	_READ_FROM_FILEINPACK(u64)
}


void FileInPackIO::WriteVec2(const Vec2 &vData)
{
	_WRITE_TO_FILEINPACK(Vec2,vData)
}


Vec2 FileInPackIO::ReadVec2()
{
	_READ_FROM_FILEINPACK(Vec2)
}


void FileInPackIO::WriteVec3(const Vec3 &vData)
{
	_WRITE_TO_FILEINPACK(Vec3,vData)
}


Vec3 FileInPackIO::ReadVec3()
{
	_READ_FROM_FILEINPACK(Vec3)
}


void FileInPackIO::WriteVec3D(const Vec3D &vData)
{
	_WRITE_TO_FILEINPACK(Vec3D,vData)
}


Vec3D FileInPackIO::ReadVec3D()
{
	_READ_FROM_FILEINPACK(Vec3D)
}


void FileInPackIO::WriteVec4(const Vec4 &qData)
{
	_WRITE_TO_FILEINPACK(Vec4,qData)
}


Vec4 FileInPackIO::ReadVec4()
{
	_READ_FROM_FILEINPACK(Vec4)
}


void FileInPackIO::WritePlane(const Plane &qData)
{
	_WRITE_TO_FILEINPACK(Plane,qData)
}


Plane FileInPackIO::ReadPlane()
{
	_READ_FROM_FILEINPACK(Plane)
}


void FileInPackIO::WriteQuaternion(const Quaternion &qData)
{
	_WRITE_TO_FILEINPACK(Quaternion,qData)
}


Quaternion FileInPackIO::ReadQuaternion()
{
	_READ_FROM_FILEINPACK(Quaternion)
}


void FileInPackIO::WriteQuaternionD(const QuaternionD &qData)
{
	_WRITE_TO_FILEINPACK(QuaternionD,qData)
}


QuaternionD FileInPackIO::ReadQuaternionD()
{
	_READ_FROM_FILEINPACK(QuaternionD)
}


void FileInPackIO::WriteMat4x4(const Mat4x4 &matData)
{
	_WRITE_TO_FILEINPACK(Mat4x4,matData)
}


Mat4x4 FileInPackIO::ReadMat4x4()
{
	_READ_FROM_FILEINPACK(Mat4x4)
}


void FileInPackIO::WriteMat4x3(const Mat4x3 &matData)
{
	_WRITE_TO_FILEINPACK(Mat4x3,matData)
}


Mat4x3 FileInPackIO::ReadMat4x3()
{
	_READ_FROM_FILEINPACK(Mat4x3)
}


void FileInPackIO::WriteAABB(const AABB &bbData)
{
	_WRITE_TO_FILEINPACK(AABB,bbData)
}


AABB FileInPackIO::ReadAABB()
{
	_READ_FROM_FILEINPACK(AABB)
}


void FileInPackIO::WriteRay(const Ray &rayData)
{
	_WRITE_TO_FILEINPACK(Ray,rayData)
}


Ray FileInPackIO::ReadRay()
{
	_READ_FROM_FILEINPACK(Ray)
}


void FileInPackIO::WriteSphere(const Sphere &sphData)
{
	_WRITE_TO_FILEINPACK(Sphere,sphData)
}


Sphere FileInPackIO::ReadSphere()
{
	_READ_FROM_FILEINPACK(Sphere)
}


void FileInPackIO::WriteRect(const Rect &rcData)
{
	_WRITE_TO_FILEINPACK(Rect,rcData)
}


Rect FileInPackIO::ReadRect()
{
	_READ_FROM_FILEINPACK(Rect)
}


void FileInPackIO::WriteViewport(const Viewport &vpData)
{
	_WRITE_TO_FILEINPACK(Viewport,vpData)
}


Viewport FileInPackIO::ReadViewport()
{
	_READ_FROM_FILEINPACK(Viewport)
}


void FileInPackIO::WriteBuffer(const Buffer &bufData)
{
	if(m_bValid){
		u32 uDataLen =bufData.GetLen();
		cn::s64 sEndPos =m_sFilePos+sizeof(u32)+uDataLen;
		for(; m_sFileDataLen<sEndPos; m_sFileDataLen++){m_arrFileData.push_back(0);} 
		::memcpy(&(m_arrFileData[m_sFilePos]), &uDataLen, sizeof(u32)); m_sFilePos +=sizeof(u32);
		::memcpy(&(m_arrFileData[m_sFilePos]), bufData.GetData(), uDataLen); m_sFilePos +=uDataLen;
	}else{
		throw 1;
	}
}


void FileInPackIO::ReadBuffer(Buffer &bufData)
{
	if(m_bValid){
		cn::u32 uDataLen;
		if(m_sFilePos+sizeof(u32) <= m_sFileDataLen){
			uDataLen =*((u32 *)(&(m_arrFileData[m_sFilePos]))); m_sFilePos +=sizeof(u32);
		}else{throw 1;}
		bufData(uDataLen);
		if(m_sFilePos+uDataLen <= m_sFileDataLen){
			::memcpy(bufData.GetData(), &(m_arrFileData[m_sFilePos]), uDataLen); m_sFilePos +=uDataLen;
		}else{throw 1;}
	}else{ 
		throw 1;
	}
}


//��ʾд��һ���ṹ�������ݵĺ�
#define _WRITE_TO_MEMIO(type,object) \
	cn::s64 sEndPos =m_sMemPos+sizeof(type); \
	for(; m_sMemDataLen<sEndPos; m_sMemDataLen++){m_arrMemData.push_back(0);} \
	::memcpy(&(m_arrMemData[m_sMemPos]), &object, sizeof(type)); \
	m_sMemPos =sEndPos;


//��ʾ��ȡһ���ṹ�������ݵĺ�
#define _READ_FROM_MEMIO(type) \
	if(m_sMemPos+sizeof(type) <= m_sMemDataLen){ \
		type &cRet =*((type *)(&(m_arrMemData[m_sMemPos]))); \
		m_sMemPos +=sizeof(type); \
		return cRet; \
	}else{ \
		throw 1; \
	}


//���캯��(�����throw)
MemIO::MemIO(const void *pInitData, u32 uInitDatalen)
{
	m_arrMemData.resize(uInitDatalen, 0);
	m_sMemDataLen =uInitDatalen; 
	m_sMemPos =0;
	if(uInitDatalen>0 && pInitData){
		::memcpy(&(m_arrMemData[0]), pInitData, uInitDatalen);
	}
}


//��������
MemIO::~MemIO()
{
	Close();
}


//Ѱַ�������ļ���ĳ��λ����(�����throw)
void MemIO::Seek(SEEK_ORIG eSeekOrig, s64 sOffset)
{
	switch(eSeekOrig)
	{
	case DiskIO::SEEK_ORIG_BEGIN:
		m_sMemPos =sOffset;
		break;
	case DiskIO::SEEK_ORIG_CUR:
		m_sMemPos +=sOffset;
		break;
	case DiskIO::SEEK_ORIG_END:
		m_sMemPos =m_sMemDataLen+sOffset;
		break;
	default:
		break;
	}
	for(; m_sMemDataLen<m_sMemPos; m_sMemDataLen++){m_arrMemData.push_back(0);}
}


//��ȡ��ǰ����λ��(�����throw)
s64 MemIO::GetCurPos()
{
	return m_sMemPos;
}


//��ȡ�ļ����ܳ���(�����throw)
s64 MemIO::GetLength()
{
	return m_sMemDataLen;
}


//���ڲ��������е�����ȫ����ղ����µ��ļ���(�����throw)
void MemIO::Flush()
{

}


//��ʾ�رմ����ļ�(�����throw)
void MemIO::Close()
{
	m_arrMemData.clear();
	m_sMemDataLen =0;
	m_sMemPos =0;
}


void MemIO::WriteFloat(float fData)
{
	_WRITE_TO_MEMIO(float,fData)
}


float MemIO::ReadFloat()
{
	_READ_FROM_MEMIO(float)
}


void MemIO::WriteDouble(double dData)
{
	_WRITE_TO_MEMIO(double,dData)
}


double MemIO::ReadDouble()
{
	_READ_FROM_MEMIO(double)
}


void MemIO::WriteString(const char *pStr, s32 sStrLen)
{
	cn::s32 sNewStrLen =0;
	const WCHAR *pNewStr =cn::Literal::CharToWChar(pStr, sStrLen, &sNewStrLen);
	sNewStrLen =-sNewStrLen;
	cn::s64 sEndPos =m_sMemPos+sizeof(s32)+sizeof(WCHAR)*(-sNewStrLen);
	for(; m_sMemDataLen<sEndPos; m_sMemDataLen++){m_arrMemData.push_back(0);} 
	::memcpy(&(m_arrMemData[m_sMemPos]), &sNewStrLen, sizeof(s32)); m_sMemPos +=sizeof(s32);
	::memcpy(&(m_arrMemData[m_sMemPos]), pNewStr, sizeof(WCHAR)*(-sNewStrLen)); m_sMemPos +=sizeof(WCHAR)*(-sNewStrLen);
}


void MemIO::ReadString(Buffer/*char*/ &bufData)
{
	WCHAR *g_pTempStrBuf2 =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempStrBuf2;

	cn::s32 sStrLen;
	if(m_sMemPos+sizeof(s32) <= m_sMemDataLen){
		sStrLen =*((s32 *)(&(m_arrMemData[m_sMemPos]))); m_sMemPos +=sizeof(s32);
	}else{throw 1;}
	if(sStrLen >= 0){ //���ַ���ΪANSI�洢��ʽ�Ļ�

		bufData(sStrLen+1);
		char *pStr =(char *)(bufData.GetData());
		pStr[sStrLen] =0;
		if(m_sMemPos+sStrLen <= m_sMemDataLen){
			::memcpy(pStr, &(m_arrMemData[m_sMemPos]), sStrLen); m_sMemPos +=sStrLen;
		}else{throw 1;}

	}else{ //���ַ���ΪUnicode�洢��ʽ�Ļ�

		sStrLen =-sStrLen;
		if(sStrLen > _MAX_STR_LEN){ //���ַ������ȳ�����������˳�
			throw 1;
		}
		if(m_sMemPos+sizeof(WCHAR)*sStrLen <= m_sMemDataLen){
			::memcpy(g_pTempStrBuf2, &(m_arrMemData[m_sMemPos]), sizeof(WCHAR)*sStrLen); m_sMemPos +=sizeof(WCHAR)*sStrLen;
		}else{throw 1;}
		g_pTempStrBuf2[sStrLen] =0;
		
		cn::s32 sNewStrLen =0;
		const char *pNewStr =cn::Literal::WCharToChar(g_pTempStrBuf2, sStrLen, &sNewStrLen);
		bufData(sNewStrLen+1);
		char *pStr =(char *)(bufData.GetData());
		pStr[sNewStrLen] =0;
		::memcpy(pStr, pNewStr, sizeof(char)*sNewStrLen);

	}
}


void MemIO::WriteData(const void *pData, u32 uDataLen)
{
	cn::s64 sEndPos =m_sMemPos+uDataLen;
	for(; m_sMemDataLen<sEndPos; m_sMemDataLen++){m_arrMemData.push_back(0);} 
	::memcpy(&(m_arrMemData[m_sMemPos]), pData, uDataLen); m_sMemPos +=uDataLen;
}


void MemIO::ReadData(void *pData, u32 uDataLen)
{
	if(m_sMemPos+uDataLen <= m_sMemDataLen){
		::memcpy(pData, &(m_arrMemData[m_sMemPos]), uDataLen); m_sMemPos +=uDataLen;
	}else{throw 1;}
}


void MemIO::WriteS8(s8 sData)
{
	_WRITE_TO_MEMIO(s8,sData)
}


s8 MemIO::ReadS8()
{
	_READ_FROM_MEMIO(s8)
}


void MemIO::WriteU8(u8 uData)
{
	_WRITE_TO_MEMIO(u8,uData)
}


u8 MemIO::ReadU8()
{
	_READ_FROM_MEMIO(u8)
}


void MemIO::WriteS16(s16 sData)
{
	_WRITE_TO_MEMIO(s16,sData)
}


s16 MemIO::ReadS16()
{
	_READ_FROM_MEMIO(s16)
}


void MemIO::WriteU16(u16 uData)
{
	_WRITE_TO_MEMIO(u16,uData)
}


u16 MemIO::ReadU16()
{
	_READ_FROM_MEMIO(u16)
}


void MemIO::WriteS32(s32 sData)
{
	_WRITE_TO_MEMIO(s32,sData)
}


s32 MemIO::ReadS32()
{
	_READ_FROM_MEMIO(s32)
}


void MemIO::WriteU32(u32 uData)
{
	_WRITE_TO_MEMIO(u32,uData)
}


u32 MemIO::ReadU32()
{
	_READ_FROM_MEMIO(u32)
}


void MemIO::WriteS64(s64 sData)
{
	_WRITE_TO_MEMIO(s64,sData)
}


s64 MemIO::ReadS64()
{
	_READ_FROM_MEMIO(s64)
}


void MemIO::WriteU64(u64 uData)
{
	_WRITE_TO_MEMIO(u64,uData)
}


u64 MemIO::ReadU64()
{
	_READ_FROM_MEMIO(u64)
}


void MemIO::WriteVec2(const Vec2 &vData)
{
	_WRITE_TO_MEMIO(Vec2,vData)
}


Vec2 MemIO::ReadVec2()
{
	_READ_FROM_MEMIO(Vec2)
}


void MemIO::WriteVec3(const Vec3 &vData)
{
	_WRITE_TO_MEMIO(Vec3,vData)
}


Vec3 MemIO::ReadVec3()
{
	_READ_FROM_MEMIO(Vec3)
}


void MemIO::WriteVec3D(const Vec3D &vData)
{
	_WRITE_TO_MEMIO(Vec3D,vData)
}


Vec3D MemIO::ReadVec3D()
{
	_READ_FROM_MEMIO(Vec3D)
}


void MemIO::WriteVec4(const Vec4 &qData)
{
	_WRITE_TO_MEMIO(Vec4,qData)
}


Vec4 MemIO::ReadVec4()
{
	_READ_FROM_MEMIO(Vec4)
}


void MemIO::WritePlane(const Plane &qData)
{
	_WRITE_TO_MEMIO(Plane,qData)
}


Plane MemIO::ReadPlane()
{
	_READ_FROM_MEMIO(Plane)
}


void MemIO::WriteQuaternion(const Quaternion &qData)
{
	_WRITE_TO_MEMIO(Quaternion,qData)
}


Quaternion MemIO::ReadQuaternion()
{
	_READ_FROM_MEMIO(Quaternion)
}


void MemIO::WriteQuaternionD(const QuaternionD &qData)
{
	_WRITE_TO_MEMIO(QuaternionD,qData)
}


QuaternionD MemIO::ReadQuaternionD()
{
	_READ_FROM_MEMIO(QuaternionD)
}


void MemIO::WriteMat4x4(const Mat4x4 &matData)
{
	_WRITE_TO_MEMIO(Mat4x4,matData)
}


Mat4x4 MemIO::ReadMat4x4()
{
	_READ_FROM_MEMIO(Mat4x4)
}


void MemIO::WriteMat4x3(const Mat4x3 &matData)
{
	_WRITE_TO_MEMIO(Mat4x3,matData)
}


Mat4x3 MemIO::ReadMat4x3()
{
	_READ_FROM_MEMIO(Mat4x3)
}


void MemIO::WriteAABB(const AABB &bbData)
{
	_WRITE_TO_MEMIO(AABB,bbData)
}


AABB MemIO::ReadAABB()
{
	_READ_FROM_MEMIO(AABB)
}


void MemIO::WriteRay(const Ray &rayData)
{
	_WRITE_TO_MEMIO(Ray,rayData)
}


Ray MemIO::ReadRay()
{
	_READ_FROM_MEMIO(Ray)
}


void MemIO::WriteSphere(const Sphere &sphData)
{
	_WRITE_TO_MEMIO(Sphere,sphData)
}


Sphere MemIO::ReadSphere()
{
	_READ_FROM_MEMIO(Sphere)
}


void MemIO::WriteRect(const Rect &rcData)
{
	_WRITE_TO_MEMIO(Rect,rcData)
}


Rect MemIO::ReadRect()
{
	_READ_FROM_MEMIO(Rect)
}


void MemIO::WriteViewport(const Viewport &vpData)
{
	_WRITE_TO_MEMIO(Viewport,vpData)
}


Viewport MemIO::ReadViewport()
{
	_READ_FROM_MEMIO(Viewport)
}


void MemIO::WriteBuffer(const Buffer &bufData)
{
	u32 uDataLen =bufData.GetLen();
	cn::s64 sEndPos =m_sMemPos+sizeof(u32)+uDataLen;
	for(; m_sMemDataLen<sEndPos; m_sMemDataLen++){m_arrMemData.push_back(0);} 
	::memcpy(&(m_arrMemData[m_sMemPos]), &uDataLen, sizeof(u32)); m_sMemPos +=sizeof(u32);
	::memcpy(&(m_arrMemData[m_sMemPos]), bufData.GetData(), uDataLen); m_sMemPos +=uDataLen;
}


void MemIO::ReadBuffer(Buffer &bufData)
{
	cn::u32 uDataLen;
	if(m_sMemPos+sizeof(u32) <= m_sMemDataLen){
		uDataLen =*((u32 *)(&(m_arrMemData[m_sMemPos]))); m_sMemPos +=sizeof(u32);
	}else{throw 1;}
	bufData(uDataLen);
	if(m_sMemPos+uDataLen <= m_sMemDataLen){
		::memcpy(bufData.GetData(), &(m_arrMemData[m_sMemPos]), uDataLen); m_sMemPos +=uDataLen;
	}else{throw 1;}
}


//����һ����άID�������
Ref/*FileFilter*/ FileFilter::Create()
{
	return cn::Ref(new FileFilterImp());
}


//���캯��
FileFilter::FileFilter()
{

}


//��������
FileFilter::~FileFilter()
{

}


//���캯��
FileFilterImp::FileFilterImp()
: FileFilter()
{
	m_sMaxUnitNameLen =0;
	m_sCharNodeNum =0;
	m_lpCharNodes =NULL;
	m_sRegionInfoNum =0;
	m_lpRegionInfos =NULL;

	m_bAdding =false;
}


//��������
FileFilterImp::~FileFilterImp()
{
	SAFE_DELETE_ARRAY(m_lpCharNodes);
	SAFE_DELETE_ARRAY(m_lpRegionInfos);
}


//��ʼ����µĹ��˵�Ԫ
//����ֵ��true->���Կ�ʼ��ӣ�false->���������
bool FileFilterImp::BeginAddFilterUnits()
{
	if(m_bAdding){
		return false;
	}
	ConvertFormalMemToTempMem();
	m_bAdding =true;

	return true;
}


//���һ�����˵�Ԫ(����λ��BeginAddFilterUnits-EndAddFilterUnits����)
bool FileFilterImp::AddFilterUnit(const FileFilter::FILTER_UNIT &cUnit)
{
	if(m_bAdding == false){
		return false;
	}
	std::string strFinalRegionID =cUnit.m_pRegionID ? cUnit.m_pRegionID : "";
	m_mapTempUnitInfos[cUnit.m_pUnitName] =strFinalRegionID;
	ID_REGION2D *lpSrcRegion =(ID_REGION2D *)(cUnit.m_refRegion.Get());
	std::pair<std::map<std::string, Ref/*ID_REGION2D*/>::iterator, bool> pairIns =m_mapTempRegionInfos.insert(std::make_pair(strFinalRegionID, Ref(NULL)));
	if(pairIns.second){
		if(lpSrcRegion){
			pairIns.first->second =lpSrcRegion->Clone();
		}
	}else{
		ID_REGION2D *lpDestRegion =(ID_REGION2D *)(pairIns.first->second.Get());
		if(lpDestRegion){
			if(lpSrcRegion){
				if(lpDestRegion->BeginAddColSegs()){
					lpDestRegion->AddIDRegion2D(lpSrcRegion);
					lpDestRegion->EndAddColSegs();
				}
			}else{
				pairIns.first->second =Ref(NULL);
			}
		}
	}
	return true;
}


//���һ������ģ������ݵ�����(����λ��BeginAddFilterUnits-EndAddFilterUnits����)
bool FileFilterImp::AddFilter(const cn::WRef/*FileFilter*/ &refFilter)
{
	if(m_bAdding == false){
		return false;
	}
	FileFilterImp *lpSrcFilter =(FileFilterImp *)(refFilter.Get());
	if(lpSrcFilter==NULL || lpSrcFilter->m_bAdding){
		return false;
	}
	if(lpSrcFilter->m_sCharNodeNum > 1){
		DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
		std::vector<BYTE> &arrTemp1 =lpDLLTLSData->m_arrTempFilter1;
		std::vector<BYTE> &arrTemp2 =lpDLLTLSData->m_arrTempFilter2;
		if(arrTemp1.size() < lpSrcFilter->m_sMaxUnitNameLen*sizeof(char)){arrTemp1.resize(lpSrcFilter->m_sMaxUnitNameLen*sizeof(char));} char *pTempStrBuf =(char *)(&(arrTemp1[0]));
		if(arrTemp2.size() < lpSrcFilter->m_sMaxUnitNameLen*sizeof(CHAR_STACK)){arrTemp2.resize(lpSrcFilter->m_sMaxUnitNameLen*sizeof(CHAR_STACK));} CHAR_STACK *lpTempStack =(CHAR_STACK *)(&(arrTemp2[0])); cn::s32 sCurStackSize =0;

		CHAR_NODE &cHeadChar =lpSrcFilter->m_lpCharNodes[0];		
		if(cHeadChar.m_uChildNum > 0){
			CHAR_STACK &cFirstStack =lpTempStack[sCurStackSize++];
			cFirstStack.m_sCharBeginPos =cHeadChar.m_uChildBasePos; cFirstStack.m_sCharEndPos =cHeadChar.m_uChildBasePos+cHeadChar.m_uChildNum; cFirstStack.m_sCurCharPos =cFirstStack.m_sCharBeginPos-1;
			while(sCurStackSize > 0){
				CHAR_STACK &cCurStack =lpTempStack[sCurStackSize-1];
				cCurStack.m_sCurCharPos++;
				if(cCurStack.m_sCurCharPos < cCurStack.m_sCharEndPos){
					CHAR_NODE &cCurChar =lpSrcFilter->m_lpCharNodes[cCurStack.m_sCurCharPos];
					pTempStrBuf[sCurStackSize-1] =cCurChar.m_uChar;
					if(cCurChar.m_uRegionPos < 0xffff){ //���ҵ�һ��ɸѡ��Ԫ���Ļ�
						REGION_INFO &cCurRegion =lpSrcFilter->m_lpRegionInfos[cCurChar.m_uRegionPos];

						m_mapTempUnitInfos[std::string(pTempStrBuf, sCurStackSize)] =cCurRegion.m_strRegionID;
						std::pair<std::map<std::string, Ref/*ID_REGION2D*/>::iterator, bool> pairIns =m_mapTempRegionInfos.insert(std::make_pair(cCurRegion.m_strRegionID, Ref(NULL)));
						if(pairIns.second){
							if(cCurRegion.m_lpRegion){
								pairIns.first->second =cCurRegion.m_lpRegion->Clone();
							}
						}else{
							ID_REGION2D *lpDestRegion =(ID_REGION2D *)(pairIns.first->second.Get());
							if(lpDestRegion){
								if(cCurRegion.m_lpRegion){
									if(lpDestRegion->BeginAddColSegs()){
										lpDestRegion->AddIDRegion2D(cCurRegion.m_lpRegion);
										lpDestRegion->EndAddColSegs();
									}
								}else{
									pairIns.first->second =Ref(NULL);
								}
							}
						}
					}
					if(cCurChar.m_uChildNum > 0){
						CHAR_STACK &cNewStack =lpTempStack[sCurStackSize++];
						cNewStack.m_sCharBeginPos =cCurChar.m_uChildBasePos; cNewStack.m_sCharEndPos =cCurChar.m_uChildBasePos+cCurChar.m_uChildNum; cNewStack.m_sCurCharPos =cNewStack.m_sCharBeginPos-1;
					}
				}else{
					sCurStackSize--;
				}
			}
		}
	}

	return true;
}


//��������µĹ��˵�Ԫ
void FileFilterImp::EndAddFilterUnits()
{
	if(m_bAdding == false){
		return;
	}
	ConvertTempMemToFormalMem();
	m_bAdding =false;
}


//��ȡ���й��˵�Ԫ
const FileFilter::FILTER_UNIT *FileFilterImp::GetAllFilterUnits(cn::u32 &uUnitNum)
{
	cn::u32 i;
	FileFilter::FILTER_UNIT cTempUnit;
	FileFilterImp::FILTER_UNIT_SUB cTempUnitSub;

	if(m_bAdding || m_sCharNodeNum<=0){
		uUnitNum =0; return NULL;
	}
	CHAR_NODE &cHeadChar =m_lpCharNodes[0];	
	if(cHeadChar.m_uChildNum == 0){
		uUnitNum =0; return NULL;
	}
	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	std::vector<BYTE> &arrTemp1 =lpDLLTLSData->m_arrTempFilter1;
	std::vector<BYTE> &arrTemp2 =lpDLLTLSData->m_arrTempFilter2;
	std::vector<FileFilter::FILTER_UNIT> &arrRet =lpDLLTLSData->m_arrTempFilter3;
	std::vector<FileFilterImp::FILTER_UNIT_SUB> &arrRetSub =lpDLLTLSData->m_arrTempFilter4;
	if(arrTemp1.size() < m_sMaxUnitNameLen*sizeof(char)){arrTemp1.resize(m_sMaxUnitNameLen*sizeof(char));} char *pTempStrBuf =(char *)(&(arrTemp1[0]));
	if(arrTemp2.size() < m_sMaxUnitNameLen*sizeof(CHAR_STACK)){arrTemp2.resize(m_sMaxUnitNameLen*sizeof(CHAR_STACK));} CHAR_STACK *lpTempStack =(CHAR_STACK *)(&(arrTemp2[0])); cn::s32 sCurStackSize =0;
	arrRet.clear(); arrRetSub.clear();
	CHAR_STACK &cFirstStack =lpTempStack[sCurStackSize++];
	cFirstStack.m_sCharBeginPos =cHeadChar.m_uChildBasePos; cFirstStack.m_sCharEndPos =cHeadChar.m_uChildBasePos+cHeadChar.m_uChildNum; cFirstStack.m_sCurCharPos =cFirstStack.m_sCharBeginPos-1;
	while(sCurStackSize > 0){
		CHAR_STACK &cCurStack =lpTempStack[sCurStackSize-1];
		cCurStack.m_sCurCharPos++;
		if(cCurStack.m_sCurCharPos < cCurStack.m_sCharEndPos){
			CHAR_NODE &cCurChar =m_lpCharNodes[cCurStack.m_sCurCharPos];
			pTempStrBuf[sCurStackSize-1] =cCurChar.m_uChar;
			if(cCurChar.m_uRegionPos < 0xffff){ //���ҵ�һ��ɸѡ��Ԫ���Ļ�
				REGION_INFO &cCurRegion =m_lpRegionInfos[cCurChar.m_uRegionPos];
				cTempUnitSub.m_strUnitName =std::string(pTempStrBuf, sCurStackSize);
				cTempUnit.m_pRegionID =cCurRegion.m_strRegionID.c_str();
				cTempUnit.m_refRegion =cCurRegion.m_refRegion;
				arrRet.push_back(cTempUnit); arrRetSub.push_back(cTempUnitSub);
			}
			if(cCurChar.m_uChildNum > 0){
				CHAR_STACK &cNewStack =lpTempStack[sCurStackSize++];
				cNewStack.m_sCharBeginPos =cCurChar.m_uChildBasePos; cNewStack.m_sCharEndPos =cCurChar.m_uChildBasePos+cCurChar.m_uChildNum; cNewStack.m_sCurCharPos =cNewStack.m_sCharBeginPos-1;
			}
		}else{
			sCurStackSize--;
		}
	}
	uUnitNum =arrRet.size();
	if(uUnitNum > 0){
		for(i =0; i<uUnitNum; i++){arrRet[i].m_pUnitName =arrRetSub[i].m_strUnitName.c_str();}
		return &(arrRet[0]);
	}else{
		return NULL;
	}
}


//ɾ�����й��˵�Ԫ
void FileFilterImp::DelAllFilterUnits()
{
	if(m_bAdding){
		return;
	}
	m_sMaxUnitNameLen =0;
	m_sCharNodeNum =0;
	SAFE_DELETE_ARRAY(m_lpCharNodes);
	m_sRegionInfoNum =0;
	SAFE_DELETE_ARRAY(m_lpRegionInfos);
}


//����������ݵ��ļ�
bool FileFilterImp::Save(const char *pFullPath)
{
	cn::s32 i;

	if(m_bAdding){
		return false;
	}
	try
	{
		Ref/*IDiskIO*/ refDiskIO =DiskIO::Create(pFullPath, "w+b"); //����Դ��Ӧ�Ĵ�������
		DiskIO *lpCurIO =(DiskIO *)(refDiskIO.Get());
		lpCurIO->Seek(DiskIO::SEEK_ORIG_BEGIN, 0);

		lpCurIO->WriteS32(m_sMaxUnitNameLen);
		lpCurIO->WriteS32(m_sCharNodeNum);
		if(m_sCharNodeNum > 0){
			lpCurIO->WriteData(m_lpCharNodes, sizeof(CHAR_NODE)*m_sCharNodeNum);
		}
		lpCurIO->WriteS32(m_sRegionInfoNum);
		if(m_sRegionInfoNum > 0){
			for(i =0; i<m_sRegionInfoNum; i++){
				cn::s32 sCurStrLen =m_lpRegionInfos[i].m_strRegionID.length();
				lpCurIO->WriteS32(sCurStrLen);
				if(sCurStrLen > 0){lpCurIO->WriteData(m_lpRegionInfos[i].m_strRegionID.data(), sizeof(char)*sCurStrLen);}
				ID_REGION2D_IMP *lpCurRegion =m_lpRegionInfos[i].m_lpRegion;
				cn::s32 sRegionState =(lpCurRegion ? 1 : 0);
				lpCurIO->WriteS32(sRegionState);
				if(sRegionState > 0){
					lpCurIO->WriteS32(lpCurRegion->m_sColNum);
					if(lpCurRegion->m_sColNum > 0){
						lpCurIO->WriteData(lpCurRegion->m_lpColInfos, sizeof(ID_REGION2D_IMP::COL_EXT)*lpCurRegion->m_sColNum);
					}
					lpCurIO->WriteS32(lpCurRegion->m_sSegNum);
					if(lpCurRegion->m_sSegNum > 0){
						lpCurIO->WriteData(lpCurRegion->m_lpSegInfos, sizeof(ID_REGION2D_IMP::COL_SEG_EXT)*lpCurRegion->m_sSegNum);
					}
				}
			}
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}


//���ļ����ع�������
bool FileFilterImp::Load(const char *pFullPath)
{
	cn::s32 i;

	if(m_bAdding){
		return false;
	}
	try
	{
		m_sMaxUnitNameLen =0;
		m_sCharNodeNum =0;
		SAFE_DELETE_ARRAY(m_lpCharNodes);
		m_sRegionInfoNum =0;
		SAFE_DELETE_ARRAY(m_lpRegionInfos);

		Ref/*IDiskIO*/ refDiskIO =DiskIO::Create(pFullPath, "rb"); //����Դ��Ӧ�Ĵ�������
		DiskIO *lpCurIO =(DiskIO *)(refDiskIO.Get());
		lpCurIO->Seek(DiskIO::SEEK_ORIG_BEGIN, 0);

		m_sMaxUnitNameLen =lpCurIO->ReadS32();
		m_sCharNodeNum =lpCurIO->ReadS32();
		if(m_sCharNodeNum > 0){
			m_lpCharNodes =new CHAR_NODE[m_sCharNodeNum];
			lpCurIO->ReadData(m_lpCharNodes, sizeof(CHAR_NODE)*m_sCharNodeNum);
		}
		m_sRegionInfoNum =lpCurIO->ReadS32();
		if(m_sRegionInfoNum > 0){
			m_lpRegionInfos =new REGION_INFO[m_sRegionInfoNum];
			for(i =0; i<m_sRegionInfoNum; i++){
				cn::Buffer bufID; lpCurIO->ReadString(bufID);
				m_lpRegionInfos[i].m_strRegionID =(const char *)(bufID.GetData());
				cn::s32 sRegionState =lpCurIO->ReadS32();
				if(sRegionState > 0){
					m_lpRegionInfos[i].m_refRegion =ID_REGION2D::Create();
					m_lpRegionInfos[i].m_lpRegion =(ID_REGION2D_IMP *)(m_lpRegionInfos[i].m_refRegion.Get());
					ID_REGION2D_IMP *lpCurRegion =m_lpRegionInfos[i].m_lpRegion;
					lpCurRegion->m_sColNum =lpCurIO->ReadS32();
					if(lpCurRegion->m_sColNum > 0){
						lpCurRegion->m_lpColInfos =new ID_REGION2D_IMP::COL_EXT[lpCurRegion->m_sColNum];
						lpCurIO->ReadData(lpCurRegion->m_lpColInfos, sizeof(ID_REGION2D_IMP::COL_EXT)*lpCurRegion->m_sColNum);
					}
					lpCurRegion->m_sSegNum =lpCurIO->ReadS32();
					if(lpCurRegion->m_sSegNum > 0){
						lpCurRegion->m_lpSegInfos =new ID_REGION2D_IMP::COL_SEG_EXT[lpCurRegion->m_sSegNum];
						lpCurIO->ReadData(lpCurRegion->m_lpSegInfos, sizeof(ID_REGION2D_IMP::COL_SEG_EXT)*lpCurRegion->m_sSegNum);
					}
				}else{
					m_lpRegionInfos[i].m_refRegion =Ref(NULL);
					m_lpRegionInfos[i].m_lpRegion =NULL;
				}
			}
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}


//����������ݵ��ڴ�
bool FileFilterImp::SaveInMemory(Buffer &bufMem)
{
	cn::s32 i;

	if(m_bAdding){
		return false;
	}
	try
	{
		std::vector<cn::u8> arrMem; cn::u32 uCurOffset =0; cn::u32 uDataSize =0;
		uDataSize =sizeof(cn::s32); arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), &m_sMaxUnitNameLen, uDataSize); uCurOffset +=uDataSize;
		uDataSize =sizeof(cn::s32); arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), &m_sCharNodeNum, uDataSize); uCurOffset +=uDataSize;
		if(m_sCharNodeNum > 0){
			uDataSize =sizeof(CHAR_NODE)*m_sCharNodeNum; arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), m_lpCharNodes, uDataSize); uCurOffset +=uDataSize;
		}
		uDataSize =sizeof(cn::s32); arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), &m_sRegionInfoNum, uDataSize); uCurOffset +=uDataSize;
		if(m_sRegionInfoNum > 0){
			for(i =0; i<m_sRegionInfoNum; i++){
				cn::s32 sCurStrLen =m_lpRegionInfos[i].m_strRegionID.length();
				uDataSize =sizeof(cn::s32); arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), &sCurStrLen, uDataSize); uCurOffset +=uDataSize;
				if(sCurStrLen > 0){
					uDataSize =sizeof(char)*sCurStrLen; arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), m_lpRegionInfos[i].m_strRegionID.data(), uDataSize); uCurOffset +=uDataSize;
				}
				ID_REGION2D_IMP *lpCurRegion =m_lpRegionInfos[i].m_lpRegion;
				cn::s32 sRegionState =(lpCurRegion ? 1 : 0);
				uDataSize =sizeof(cn::s32); arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), &sRegionState, uDataSize); uCurOffset +=uDataSize;
				if(sRegionState > 0){
					uDataSize =sizeof(cn::s32); arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), &(lpCurRegion->m_sColNum), uDataSize); uCurOffset +=uDataSize;
					if(lpCurRegion->m_sColNum > 0){
						uDataSize =sizeof(ID_REGION2D_IMP::COL_EXT)*lpCurRegion->m_sColNum; arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), lpCurRegion->m_lpColInfos, uDataSize); uCurOffset +=uDataSize;
					}
					uDataSize =sizeof(cn::s32); arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), &(lpCurRegion->m_sSegNum), uDataSize); uCurOffset +=uDataSize;
					if(lpCurRegion->m_sSegNum > 0){
						uDataSize =sizeof(ID_REGION2D_IMP::COL_SEG_EXT)*lpCurRegion->m_sSegNum; arrMem.resize(uCurOffset+uDataSize); ::memcpy(&(arrMem[uCurOffset]), lpCurRegion->m_lpSegInfos, uDataSize); uCurOffset +=uDataSize;
					}
				}
			}
		}

		bufMem.Resize(uCurOffset);
		::memcpy(bufMem.GetData(), &(arrMem[0]), uCurOffset);
		return true;
	}
	catch(...)
	{
		return false;
	}
}


//���ڴ���ع�������
bool FileFilterImp::LoadInMemory(Buffer &bufMem)
{
	cn::s32 i;

	if(m_bAdding){
		return false;
	}
	try
	{
		m_sMaxUnitNameLen =0;
		m_sCharNodeNum =0;
		SAFE_DELETE_ARRAY(m_lpCharNodes);
		m_sRegionInfoNum =0;
		SAFE_DELETE_ARRAY(m_lpRegionInfos);

		cn::u8 *pMem =(cn::u8 *)(bufMem.GetData()); cn::u32 uMemSize =bufMem.GetLen(); cn::u32 uCurOffset =0; cn::u32 uDataSize =0;
		uDataSize =sizeof(cn::s32); if(uCurOffset+uDataSize>uMemSize){throw 1;} ::memcpy(&m_sMaxUnitNameLen, pMem+uCurOffset, uDataSize); uCurOffset +=uDataSize;
		uDataSize =sizeof(cn::s32); if(uCurOffset+uDataSize>uMemSize){throw 1;} ::memcpy(&m_sCharNodeNum, pMem+uCurOffset, uDataSize); uCurOffset +=uDataSize;
		if(m_sCharNodeNum > 0){
			m_lpCharNodes =new CHAR_NODE[m_sCharNodeNum];
			uDataSize =sizeof(CHAR_NODE)*m_sCharNodeNum; if(uCurOffset+uDataSize>uMemSize){throw 1;} ::memcpy(m_lpCharNodes, pMem+uCurOffset, uDataSize); uCurOffset +=uDataSize;
		}
		uDataSize =sizeof(cn::s32); if(uCurOffset+uDataSize>uMemSize){throw 1;} ::memcpy(&m_sRegionInfoNum, pMem+uCurOffset, uDataSize); uCurOffset +=uDataSize;
		if(m_sRegionInfoNum > 0){
			m_lpRegionInfos =new REGION_INFO[m_sRegionInfoNum];
			for(i =0; i<m_sRegionInfoNum; i++){
				cn::s32 sCurStrLen =0;
				uDataSize =sizeof(cn::s32); if(uCurOffset+uDataSize>uMemSize){throw 1;} ::memcpy(&sCurStrLen, pMem+uCurOffset, uDataSize); uCurOffset +=uDataSize;
				if(sCurStrLen > 0){
					uDataSize =sizeof(char)*sCurStrLen; if(uCurOffset+uDataSize>uMemSize){throw 1;}
					m_lpRegionInfos[i].m_strRegionID =std::string((const char *)(pMem+uCurOffset), sCurStrLen);
					uCurOffset +=uDataSize;
				}else{
					m_lpRegionInfos[i].m_strRegionID ="";
				}
				cn::s32 sRegionState =0;
				uDataSize =sizeof(cn::s32); if(uCurOffset+uDataSize>uMemSize){throw 1;} ::memcpy(&sRegionState, pMem+uCurOffset, uDataSize); uCurOffset +=uDataSize;
				if(sRegionState > 0){
					m_lpRegionInfos[i].m_refRegion =ID_REGION2D::Create();
					m_lpRegionInfos[i].m_lpRegion =(ID_REGION2D_IMP *)(m_lpRegionInfos[i].m_refRegion.Get());
					ID_REGION2D_IMP *lpCurRegion =m_lpRegionInfos[i].m_lpRegion;
					uDataSize =sizeof(cn::s32); if(uCurOffset+uDataSize>uMemSize){throw 1;} ::memcpy(&(lpCurRegion->m_sColNum), pMem+uCurOffset, uDataSize); uCurOffset +=uDataSize;
					if(lpCurRegion->m_sColNum > 0){
						lpCurRegion->m_lpColInfos =new ID_REGION2D_IMP::COL_EXT[lpCurRegion->m_sColNum];
						uDataSize =sizeof(ID_REGION2D_IMP::COL_EXT)*lpCurRegion->m_sColNum; if(uCurOffset+uDataSize>uMemSize){throw 1;} ::memcpy(lpCurRegion->m_lpColInfos, pMem+uCurOffset, uDataSize); uCurOffset +=uDataSize;
					}
					uDataSize =sizeof(cn::s32); if(uCurOffset+uDataSize>uMemSize){throw 1;} ::memcpy(&(lpCurRegion->m_sSegNum), pMem+uCurOffset, uDataSize); uCurOffset +=uDataSize;
					if(lpCurRegion->m_sSegNum > 0){
						lpCurRegion->m_lpSegInfos =new ID_REGION2D_IMP::COL_SEG_EXT[lpCurRegion->m_sSegNum];
						uDataSize =sizeof(ID_REGION2D_IMP::COL_SEG_EXT)*lpCurRegion->m_sSegNum; if(uCurOffset+uDataSize>uMemSize){throw 1;} ::memcpy(lpCurRegion->m_lpSegInfos, pMem+uCurOffset, uDataSize); uCurOffset +=uDataSize;
					}
				}else{
					m_lpRegionInfos[i].m_refRegion =Ref(NULL);
					m_lpRegionInfos[i].m_lpRegion =NULL;
				}
			}
		}

		return true;
	}
	catch(...)
	{
		return false;
	}
}


//���ļ����й���
//pOrigResID����ʾ�����˵�ԭʼ�����ļ���ԴID���������Ǳ���·����URL���ӵ�
//ppFilePath�����ļ�ɸѡͨ�����򷵻��ļ���Ӧ�Ĵ���·������ɸѡ��ͨ�����򷵻ؿ��ַ�������ppFilePath==NULL�򲻷����κ���Ϣ
//����ֵ�������ļ��Ƿ�ɸѡͨ��
bool FileFilterImp::FilterFile(const char *pOrigResID, const char **ppFilePath)
{
	cn::s32 i;

	if(ppFilePath){*ppFilePath ="";}
	if(m_bAdding || pOrigResID==NULL){
		return false;
	}

	//������ԴID
	cn::s32 sPathStart =-1, sPathEnd =-1, sIDStart =-1, sIDEnd =-1, sIDX =-1, sIDZ =-1;
	cn::u32 uState =0;
	for(i =0; pOrigResID[i]!='\0'; i++){
		char uCurChar =pOrigResID[i];
		switch(uState)
		{
		case 0:
			if(uCurChar=='h'){uState =1;}else{goto _SEARTH_END;}
			break;
		case 1:
			if(uCurChar=='t'){uState =2;}else{goto _SEARTH_END;}
			break;
		case 2:
			if(uCurChar=='t'){uState =3;}else{goto _SEARTH_END;}
			break;
		case 3:
			if(uCurChar=='p'){uState =4;}else{goto _SEARTH_END;}
			break;
		case 4:
			if(uCurChar=='?'){uState =6;}
			break;
		case 5:
			if(uCurChar=='&' || uCurChar=='#'){uState =6;}
			break;
		case 6: //��ʼ���Ҳ�����
			if(uCurChar=='p'){uState =7;}else if(uCurChar=='i'){uState =11;}else{i--; uState =5;} break;
		case 7:
			if(uCurChar=='a'){uState =8;}else{i--; uState =5;} break;
		case 8:
			if(uCurChar=='t'){uState =9;}else{i--; uState =5;} break;
		case 9:
			if(uCurChar=='h'){uState =10;}else{i--; uState =5;} break;
		case 10:
			if(uCurChar=='='){uState =100; sPathStart =i+1; /*�ҵ�path����*/}else{i--; uState =5;} break;
		case 11:
			if(uCurChar=='d'){uState =12;}else{i--; uState =5;} break;
		case 12:
			if(uCurChar=='='){uState =200; sIDStart =i+1; /*�ҵ�id����*/}else if(uCurChar=='x'){uState =13;}else if(uCurChar=='z'){uState =14;}else{i--; uState =5;} break;
		case 13:
			if(uCurChar=='='){uState =300; sIDX =0; /*�ҵ�idx����*/}else{i--; uState =5;} break;
		case 14:
			if(uCurChar=='='){uState =400; sIDZ =0; /*�ҵ�idz����*/}else{i--; uState =5;} break;

		case 100:
			if(uCurChar=='&' || uCurChar=='#'){uState =6;}else{sPathEnd =i+1;} break;
		case 200:
			if(uCurChar=='&' || uCurChar=='#'){uState =6;}else{sIDEnd =i+1;} break;
		case 300:
			if(uCurChar=='&' || uCurChar=='#'){uState =6;}else if(uCurChar>='0' && uCurChar<='9'){sIDX =sIDX*10+(uCurChar-'0');} break;
		case 400:
			if(uCurChar=='&' || uCurChar=='#'){uState =6;}else if(uCurChar>='0' && uCurChar<='9'){sIDZ =sIDZ*10+(uCurChar-'0');} break;
		default:
			break;
		}
	}
_SEARTH_END:
	cn::u32 uResIDType =0;
	const char *pSrcNamePtr =NULL; cn::s32 sSrcNameLen =0;
	if(uState < 4){ //����ԴIDΪ��ͨ����·���Ļ�
		uResIDType =0; pSrcNamePtr =pOrigResID; sSrcNameLen =::strlen(pOrigResID);
	}else if(sPathStart<sPathEnd && sIDStart<sIDEnd && sIDX>=0 && sIDZ>=0){ //����ԴIDΪ��������Ϣ��URL·���Ļ�
		uResIDType =1; pSrcNamePtr =pOrigResID+sIDStart; sSrcNameLen =sIDEnd-sIDStart;
	}else if(sPathStart < sPathEnd){ //����ԴIDΪ��ͨURL·���Ļ�
		uResIDType =2; pSrcNamePtr =pOrigResID+sPathStart; sSrcNameLen =sPathEnd-sPathStart;
	}
	if(sSrcNameLen <= 0){
		return false;
	}

	//�����ļ�ɸѡ
	if(m_sCharNodeNum > 1){
		CHAR_NODE &cHeadChar =m_lpCharNodes[0];		
		if(cHeadChar.m_uChildNum > 0){
			cn::s32 sCurSeqStart =cHeadChar.m_uChildBasePos; cn::s32 sCurSeqEnd =sCurSeqStart+cHeadChar.m_uChildNum;
			for(i =0; i<sSrcNameLen; i++){
				char uSrcChar =pSrcNamePtr[i];
				while(sCurSeqStart < sCurSeqEnd){
					cn::s32 sSeqMid =(sCurSeqStart+sCurSeqEnd)/2;
					CHAR_NODE &cCurChar =m_lpCharNodes[sSeqMid];
					if(cCurChar.m_uChar == uSrcChar){
						if(cCurChar.m_uRegionPos < 0xffff){ //���ҵ�Դ�ַ�����ǰ׺���Ļ�
							REGION_INFO &cCurRegion =m_lpRegionInfos[cCurChar.m_uRegionPos];
							if(cCurRegion.m_lpRegion){ //����Ҫ��һ�����ɸѡ��Ԫ��������Ϣ�Ļ�
								if(uResIDType!=1 || cCurRegion.m_lpRegion->IsInside(sIDX, sIDZ)==false){
									return false;
								}
							}
							goto _SEARTH_END2;
						}else if(cCurChar.m_uChildNum > 0){
							sCurSeqStart =cCurChar.m_uChildBasePos; sCurSeqEnd =sCurSeqStart+cCurChar.m_uChildNum;
							break;
						}else{
							return false;
						}
					}else if(cCurChar.m_uChar > uSrcChar){
						sCurSeqEnd =sSeqMid;
					}else{
						sCurSeqStart =sSeqMid+1;
					}
				}
				if(sCurSeqStart >= sCurSeqEnd){
					return false;
				}
			}
		}else{
			return false;
		}
	}else{
		return false;
	}
_SEARTH_END2:
	if(ppFilePath){ //����Ҫ����ļ�·���Ļ�
		DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
		std::string &strRet =lpDLLTLSData->m_strTempFilter5;
		if(uResIDType == 0){
			strRet =pOrigResID;
		}else if(uResIDType == 1){
			std::string strTemp(pSrcNamePtr, sSrcNameLen);
			strRet =std::string(pOrigResID+sPathStart, sPathEnd-sPathStart)+cn::Literal::GetFormattedStr(strTemp.c_str(), sIDX, sIDZ);
		}else{
			strRet =std::string(pSrcNamePtr, sSrcNameLen);
		}
		*ppFilePath =strRet.c_str();
	}
	return true;	
}


//���ļ����й���
//pFilterUnitName����ʾ�ļ���Դ����Ӧ�Ĺ��˵�Ԫ����
//sIDX, sIDZ����ʾ�ļ���Դ����Դ����������������Դ��������Ϊ-1����
//����ֵ�������ļ��Ƿ�ɸѡͨ��
bool FileFilterImp::FilterFile(const char *pFilterUnitName, cn::s32 sIDX, cn::s32 sIDZ)
{
	cn::s32 i;

	if(m_bAdding || pFilterUnitName==NULL){
		return false;
	}

	//�����ļ�ɸѡ
	cn::s32 sSrcNameLen =::strlen(pFilterUnitName);
	if(m_sCharNodeNum > 1){
		CHAR_NODE &cHeadChar =m_lpCharNodes[0];		
		if(cHeadChar.m_uChildNum > 0){
			cn::s32 sCurSeqStart =cHeadChar.m_uChildBasePos; cn::s32 sCurSeqEnd =sCurSeqStart+cHeadChar.m_uChildNum;
			for(i =0; i<sSrcNameLen; i++){
				char uSrcChar =pFilterUnitName[i];
				while(sCurSeqStart < sCurSeqEnd){
					cn::s32 sSeqMid =(sCurSeqStart+sCurSeqEnd)/2;
					CHAR_NODE &cCurChar =m_lpCharNodes[sSeqMid];
					if(cCurChar.m_uChar == uSrcChar){
						if(cCurChar.m_uRegionPos < 0xffff){ //���ҵ�Դ�ַ�����ǰ׺���Ļ�
							REGION_INFO &cCurRegion =m_lpRegionInfos[cCurChar.m_uRegionPos];
							if(cCurRegion.m_lpRegion){ //����Ҫ��һ�����ɸѡ��Ԫ��������Ϣ�Ļ�
								if(cCurRegion.m_lpRegion->IsInside(sIDX, sIDZ)==false){
									return false;
								}
							}
							return true;
						}else if(cCurChar.m_uChildNum > 0){
							sCurSeqStart =cCurChar.m_uChildBasePos; sCurSeqEnd =sCurSeqStart+cCurChar.m_uChildNum;
							break;
						}else{
							return false;
						}
					}else if(cCurChar.m_uChar > uSrcChar){
						sCurSeqEnd =sSeqMid;
					}else{
						sCurSeqStart =sSeqMid+1;
					}
				}
				if(sCurSeqStart >= sCurSeqEnd){
					return false;
				}
			}
		}else{
			return false;
		}
	}else{
		return false;
	}
	return false;	
}


//����ʽ�洢��Ϣת��Ϊ��ʱ�洢��Ϣ
void FileFilterImp::ConvertFormalMemToTempMem()
{
	m_mapTempUnitInfos.clear();
	m_mapTempRegionInfos.clear();

	if(m_sCharNodeNum > 1){
		DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
		std::vector<BYTE> &arrTemp1 =lpDLLTLSData->m_arrTempFilter1;
		std::vector<BYTE> &arrTemp2 =lpDLLTLSData->m_arrTempFilter2;
		if(arrTemp1.size() < m_sMaxUnitNameLen*sizeof(char)){arrTemp1.resize(m_sMaxUnitNameLen*sizeof(char));} char *pTempStrBuf =(char *)(&(arrTemp1[0]));
		if(arrTemp2.size() < m_sMaxUnitNameLen*sizeof(CHAR_STACK)){arrTemp2.resize(m_sMaxUnitNameLen*sizeof(CHAR_STACK));} CHAR_STACK *lpTempStack =(CHAR_STACK *)(&(arrTemp2[0])); cn::s32 sCurStackSize =0;

		CHAR_NODE &cHeadChar =m_lpCharNodes[0];		
		if(cHeadChar.m_uChildNum > 0){
			CHAR_STACK &cFirstStack =lpTempStack[sCurStackSize++];
			cFirstStack.m_sCharBeginPos =cHeadChar.m_uChildBasePos; cFirstStack.m_sCharEndPos =cHeadChar.m_uChildBasePos+cHeadChar.m_uChildNum; cFirstStack.m_sCurCharPos =cFirstStack.m_sCharBeginPos-1;
			while(sCurStackSize > 0){
				CHAR_STACK &cCurStack =lpTempStack[sCurStackSize-1];
				cCurStack.m_sCurCharPos++;
				if(cCurStack.m_sCurCharPos < cCurStack.m_sCharEndPos){
					CHAR_NODE &cCurChar =m_lpCharNodes[cCurStack.m_sCurCharPos];
					pTempStrBuf[sCurStackSize-1] =cCurChar.m_uChar;
					if(cCurChar.m_uRegionPos < 0xffff){ //���ҵ�һ��ɸѡ��Ԫ���Ļ�
						REGION_INFO &cCurRegion =m_lpRegionInfos[cCurChar.m_uRegionPos];
						m_mapTempUnitInfos[std::string(pTempStrBuf, sCurStackSize)] =cCurRegion.m_strRegionID;
						m_mapTempRegionInfos[cCurRegion.m_strRegionID] =cCurRegion.m_refRegion;			
					}
					if(cCurChar.m_uChildNum > 0){
						CHAR_STACK &cNewStack =lpTempStack[sCurStackSize++];
						cNewStack.m_sCharBeginPos =cCurChar.m_uChildBasePos; cNewStack.m_sCharEndPos =cCurChar.m_uChildBasePos+cCurChar.m_uChildNum; cNewStack.m_sCurCharPos =cNewStack.m_sCharBeginPos-1;
					}
				}else{
					sCurStackSize--;
				}
			}
		}
	}

	m_sMaxUnitNameLen =0;
	m_sCharNodeNum =0;
	SAFE_DELETE_ARRAY(m_lpCharNodes);
	m_sRegionInfoNum =0;
	SAFE_DELETE_ARRAY(m_lpRegionInfos);
}


//����ʱ�洢��Ϣת��Ϊ��ʽ�洢��Ϣ
void FileFilterImp::ConvertTempMemToFormalMem()
{
	cn::s32 i;

	std::list<CHAR_NODE_SUB>::iterator iterPreChar, iterCurChar, iterChildEnd;
	CHAR_NODE_SUB cTempSubCharNode;
	REGION_INFO cTempRegionInfo;

	m_sMaxUnitNameLen =0;
	m_sCharNodeNum =0;
	SAFE_DELETE_ARRAY(m_lpCharNodes);
	m_sRegionInfoNum =0;
	SAFE_DELETE_ARRAY(m_lpRegionInfos);

	std::vector<REGION_INFO> arrRegionInfos;
	std::map<std::string, cn::u16> mapRegionPos;
	for(std::map<std::string, Ref/*ID_REGION2D*/>::iterator iter1 =m_mapTempRegionInfos.begin(); iter1!=m_mapTempRegionInfos.end(); iter1++){
		if(m_sRegionInfoNum >= 65535){
			break;
		}
		cTempRegionInfo.m_strRegionID =iter1->first; cTempRegionInfo.m_refRegion =iter1->second; cTempRegionInfo.m_lpRegion =(ID_REGION2D_IMP *)(cTempRegionInfo.m_refRegion.Get());
		arrRegionInfos.push_back(cTempRegionInfo); mapRegionPos[cTempRegionInfo.m_strRegionID] =cn::u16(m_sRegionInfoNum);
		m_sRegionInfoNum++;
	}
	if(m_sRegionInfoNum > 0){
		m_lpRegionInfos =new REGION_INFO[m_sRegionInfoNum];
		for(i =0; i<m_sRegionInfoNum; i++){
			m_lpRegionInfos[i].m_strRegionID =arrRegionInfos[i].m_strRegionID;
			m_lpRegionInfos[i].m_refRegion =arrRegionInfos[i].m_refRegion;
			m_lpRegionInfos[i].m_lpRegion =arrRegionInfos[i].m_lpRegion;
		}
	}
	std::list<CHAR_NODE_SUB> lstTempSubNodes;
	cTempSubCharNode.m_uChar ='\0'; cTempSubCharNode.m_uRegionPos =0xffff; cTempSubCharNode.m_iterChildHead =lstTempSubNodes.end(); cTempSubCharNode.m_iterChildTail =lstTempSubNodes.end();
	lstTempSubNodes.insert(lstTempSubNodes.end(), cTempSubCharNode);
	for(std::map<std::string, std::string>::iterator iter2 =m_mapTempUnitInfos.begin(); iter2!=m_mapTempUnitInfos.end(); iter2++){
		std::map<std::string, cn::u16>::iterator iterFind =mapRegionPos.find(iter2->second);
		if(iterFind == mapRegionPos.end()){continue;}
		const std::string &strUnitName =iter2->first; 
		cn::s32 sUnitNameLen =strUnitName.length(); if(sUnitNameLen == 0){continue;}
		const char *pUnitName =strUnitName.data();
		m_sMaxUnitNameLen =max(m_sMaxUnitNameLen, sUnitNameLen);
		iterPreChar =lstTempSubNodes.begin();
		for(i =0; i<sUnitNameLen; i++){
			CHAR_NODE_SUB &cPreChar =(*iterPreChar);
			if(cPreChar.m_iterChildHead != lstTempSubNodes.end()){
				iterChildEnd =cPreChar.m_iterChildTail; iterChildEnd++;
				for(iterCurChar =cPreChar.m_iterChildHead; iterCurChar!=iterChildEnd; iterCurChar++){
					char uRefer =iterCurChar->m_uChar;
					if(pUnitName[i] < uRefer){
						cTempSubCharNode.m_uChar =pUnitName[i]; cTempSubCharNode.m_uRegionPos =(i==sUnitNameLen-1) ? iterFind->second : 0xffff; cTempSubCharNode.m_iterChildHead =cTempSubCharNode.m_iterChildTail =lstTempSubNodes.end();
						iterPreChar =lstTempSubNodes.insert(iterCurChar, cTempSubCharNode);
						if(iterCurChar == cPreChar.m_iterChildHead){cPreChar.m_iterChildHead =iterPreChar;}
						iterCurChar =iterPreChar;
						break;
					}else if(pUnitName[i] == uRefer){
						iterPreChar =iterCurChar; if(i==sUnitNameLen-1){iterCurChar->m_uRegionPos =iterFind->second;}
						break;
					}
				}
				if(iterCurChar == iterChildEnd){
					cTempSubCharNode.m_uChar =pUnitName[i]; cTempSubCharNode.m_uRegionPos =(i==sUnitNameLen-1) ? iterFind->second : 0xffff; cTempSubCharNode.m_iterChildHead =cTempSubCharNode.m_iterChildTail =lstTempSubNodes.end();
					iterPreChar =lstTempSubNodes.insert(iterCurChar, cTempSubCharNode);
					cPreChar.m_iterChildTail =iterPreChar;
					iterCurChar =iterPreChar;
				}
			}else{
				cTempSubCharNode.m_uChar =pUnitName[i]; cTempSubCharNode.m_uRegionPos =(i==sUnitNameLen-1) ? iterFind->second : 0xffff; cTempSubCharNode.m_iterChildHead =cTempSubCharNode.m_iterChildTail =lstTempSubNodes.end();
				iterCurChar =lstTempSubNodes.insert(lstTempSubNodes.end(), cTempSubCharNode);
				cPreChar.m_iterChildHead =cPreChar.m_iterChildTail =iterCurChar; iterPreChar =iterCurChar;
			}
		}
	}
	i =0; for(std::list<CHAR_NODE_SUB>::iterator iter3 =lstTempSubNodes.begin(); iter3!=lstTempSubNodes.end(); iter3++){iter3->m_uSelfPos =(i++);}
	m_sCharNodeNum =lstTempSubNodes.size();
	m_lpCharNodes =new CHAR_NODE[m_sCharNodeNum];
	i =0; for(std::list<CHAR_NODE_SUB>::iterator iter4 =lstTempSubNodes.begin(); iter4!=lstTempSubNodes.end(); iter4++){
		CHAR_NODE_SUB &cCurSubNode =(*iter4); CHAR_NODE &cCurNode =m_lpCharNodes[i++];
		cCurNode.m_uChar =cCurSubNode.m_uChar; cCurNode.m_uRegionPos =cCurSubNode.m_uRegionPos;
		if(cCurSubNode.m_iterChildHead != lstTempSubNodes.end()){
			cCurNode.m_uChildBasePos =cCurSubNode.m_iterChildHead->m_uSelfPos;
			cCurNode.m_uChildNum =cn::u8(cCurSubNode.m_iterChildTail->m_uSelfPos-cCurNode.m_uChildBasePos+1);
		}else{
			cCurNode.m_uChildBasePos =0xffffffff; cCurNode.m_uChildNum =0;
		}		
	}

	std::map<std::string, std::string> mapTemp1; std::map<std::string, Ref/*ID_REGION2D*/> mapTemp2;
	m_mapTempUnitInfos.swap(mapTemp1); m_mapTempRegionInfos.swap(mapTemp2);
}


//����һ���ļ����ģ�����
//pPackagePath����ʾ�������Ӧ�Ĵ���·��
//sDefaultPackUnitSize����ʾ����һ�������洢��Ԫ��Ĭ���ֽڴ�С(�����Ѵ��ڣ�����ð��ڵĵ�Ԫ���С)
//sMaxFileLogUnitNum����ʾ���ڴ���ļ�������־�ĵ�Ԫ���������(�����Ѵ��ڣ�����ð��ڵ�������)
//bReadOnly����ʾ�Ƿ���ֻ����ʽ����Դ��
Ref/*FilePackage*/ FilePackage::Create(const char *pPackagePath, cn::s64 sDefaultPackUnitSize, cn::s64 sMaxFileLogUnitNum, bool bReadOnly)
{
	try
	{
		return cn::Ref(new FilePackageImp(pPackagePath, sDefaultPackUnitSize, sMaxFileLogUnitNum, bReadOnly));
	}
	catch(...)
	{
		cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
			"RealEngine::FilePackage::Create = failed to create the pack \"%hs\"", pPackagePath ? pPackagePath : ""));
		return cn::Ref(NULL);
	}
}


//���캯��
FilePackage::FilePackage(const char *pPackagePath, cn::s64 sDefaultPackUnitSize, cn::s64 sMaxFileLogUnitNum, bool bReadOnly)
{

}


//��������
FilePackage::~FilePackage()
{

}


//���ļ�ժҪ���ݵ�����ȽϺ���
bool FileDigestKeyCmp(const FilePackageImp::FILE_DIGEST_KEY &cKey1, const FilePackageImp::FILE_DIGEST_KEY &cKey2)
{
	if(cKey1.m_uRelPathMD5.u_64_127 != cKey2.m_uRelPathMD5.u_64_127){
		return cKey1.m_uRelPathMD5.u_64_127 < cKey2.m_uRelPathMD5.u_64_127;
	}else{
		return cKey1.m_uRelPathMD5.u_0_63 < cKey2.m_uRelPathMD5.u_0_63;
	}
}


//���ļ�ժҪ���ݵ�����ȽϺ���2
bool FileDigestKeyCmp2(const FilePackageImp::FILE_DIGEST_KEY_2 &cKey1, const FilePackageImp::FILE_DIGEST_KEY_2 &cKey2)
{
	return cKey1.m_sDiskInfoPos < cKey2.m_sDiskInfoPos;
}


//���캯��
FilePackageImp::FILE_DIGEST_MEM::FILE_DIGEST_MEM()
{
	m_cDiskInfo.m_sFileInfoPos =-1;
	m_sDiskInfoPos =-1;
}


//���캯��
FilePackageImp::FilePackageImp(const char *pPackagePath, cn::s64 sDefaultPackUnitSize, cn::s64 sMaxFileLogUnitNum, bool bReadOnly)
: FilePackage(pPackagePath, sDefaultPackUnitSize, sMaxFileLogUnitNum, bReadOnly)
{
	UNIT_HEADER cUnitHeader;

	m_bReadOnly =bReadOnly;

	m_strPackFilePath =pPackagePath ? pPackagePath : "";
	if(m_strPackFilePath.length() == 0){throw 1;}
	m_bAutoTimeStampEnable =true;
	bool bNeedGenPack =true;
	if(::PathFileExists(m_strPackFilePath.c_str())){ //�����ļ��Ѵ��ڵĻ�
		m_lpPackFile =new FileIOImp(m_strPackFilePath.c_str(), m_bReadOnly ? "rb" : "r+b"); m_refPackFile =cn::Ref(m_lpPackFile, NULL, NULL, NULL, NULL, NULL);
		cn::s64 sPackFileLen =m_lpPackFile->GetLength(); m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, 0);
		m_lpPackFile->ReadData(&cUnitHeader, sizeof(UNIT_HEADER)); //��ȡ��0����Ԫ���ͷ����Ϣ
		m_lpPackFile->ReadData(&m_cPackHeader, sizeof(PACK_HEADER)); //��ȡ���ļ���ͷ����Ϣ
		if(m_cPackHeader.m_uWriting == 0){ //�����ļ�û��δ��ɵ��޸ĵĻ�
			bNeedGenPack =false;
			if(m_bReadOnly == false){
				m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sizeof(UNIT_HEADER)+sizeof(cn::u32));
				m_cPackHeader.m_uWriting =0xffffffff; m_lpPackFile->WriteU32(m_cPackHeader.m_uWriting); m_lpPackFile->Flush();
				m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sizeof(UNIT_HEADER)+sizeof(PACK_HEADER));				
			}
			m_sUnitSize =m_cPackHeader.m_sPackUnitSize;
			m_sFileLogNumPerUnit =(m_sUnitSize-sizeof(FILE_LOG_UNIT_HEADER)-sizeof(cn::s64))/sizeof(FILE_LOG);
			m_arrEmptyUnit.resize(m_sUnitSize, 0); 
			if(m_cPackHeader.m_uVersion != _FILE_PACKAGE_VERSION){
				cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
					"RealEngine::FilePackageImp::FilePackageImp = \"%hs\" package version invalid", m_strPackFilePath.c_str()));
				throw 1;
			}
			if(m_sUnitSize<(sizeof(UNIT_HEADER)+sizeof(PACK_HEADER)+sizeof(cn::s64)+sizeof(FILE_DIGEST)*1) || 
				sPackFileLen%m_sUnitSize!=0 || sPackFileLen/m_sUnitSize<1)
			{
				cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
					"RealEngine::FilePackageImp::FilePackageImp = \"%hs\" package size is not mult to unit size", m_strPackFilePath.c_str()));
				throw 1;
			}
			if(m_cPackHeader.m_sFreeUnitNum<0 || (m_cPackHeader.m_sFreeUnitNum==0 && m_cPackHeader.m_sFreeUnitsBaseUnit>=0) || (m_cPackHeader.m_sFreeUnitNum>0 && m_cPackHeader.m_sFreeUnitsBaseUnit<=0)){
				cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
					"RealEngine::FilePackageImp::FilePackageImp = \"%hs\" free unit info invalid", m_strPackFilePath.c_str()));
				throw 1;
			}
			if(m_cPackHeader.m_sFileLogNum<0 || m_cPackHeader.m_sMaxFileLogUnitNum<0 || (m_cPackHeader.m_sFileLogNum==0 && m_cPackHeader.m_sFileLogsBaseUnit>=0) || (m_cPackHeader.m_sFileLogNum>0 && m_cPackHeader.m_sFileLogsBaseUnit<=0)){
				cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
					"RealEngine::FilePackageImp::FilePackageImp = \"%hs\" file log info invalid", m_strPackFilePath.c_str()));
				throw 1;
			}
			if(m_cPackHeader.m_sFileNum<0 || (m_cPackHeader.m_sFileNum==0 && m_cPackHeader.m_sFileInfosBaseUnit>=0) || (m_cPackHeader.m_sFileNum>0 && m_cPackHeader.m_sFileInfosBaseUnit<=0)){
				cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
					"RealEngine::FilePackageImp::FilePackageImp = \"%hs\" file info invalid", m_strPackFilePath.c_str()));
				throw 1;
			}
			if(m_cPackHeader.m_sFreeDigestNum<0 || (m_cPackHeader.m_sFreeDigestNum==0 && m_cPackHeader.m_sFreeDigestsBasePos>=0) || (m_cPackHeader.m_sFreeDigestNum>0 && m_cPackHeader.m_sFreeDigestsBasePos<=0)){
				cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
					"RealEngine::FilePackageImp::FilePackageImp = \"%hs\" free digest info invalid", m_strPackFilePath.c_str()));
				throw 1;
			}
			LoadAllFileDigests(); //�������е��ļ�ժҪ��Ϣ��ȡ���ڴ�
		}else{
			m_refPackFile =NULL; m_lpPackFile =NULL;
		}
	}
	if(bNeedGenPack){ //�����ļ������ڵĻ�
		cn::DiskIO::CreatePathHierarchy(m_strPackFilePath.c_str());
		m_lpPackFile =new FileIOImp(m_strPackFilePath.c_str(), "w+b"); m_refPackFile =cn::Ref(m_lpPackFile, NULL, NULL, NULL, NULL, NULL);
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, 0);
		cUnitHeader.m_sCurUnitValidSize =sizeof(UNIT_HEADER)+sizeof(PACK_HEADER)+sizeof(cn::s64);
		cUnitHeader.m_sNextUnit =-1;
		m_cPackHeader.m_uVersion =_FILE_PACKAGE_VERSION;
		m_cPackHeader.m_uWriting =m_bReadOnly ? 0 : 0xffffffff;
		m_cPackHeader.m_sPackUnitSize =sDefaultPackUnitSize;
		m_cPackHeader.m_uPackTimeStamp =0;
		m_cPackHeader.m_sFreeUnitNum =0;
		m_cPackHeader.m_sFreeUnitsBaseUnit =-1;
		m_cPackHeader.m_sFileLogNum =0;
		m_cPackHeader.m_sMaxFileLogUnitNum =max(0, sMaxFileLogUnitNum);
		m_cPackHeader.m_sFileNum =0;
		m_cPackHeader.m_sFreeDigestNum =0;
		m_cPackHeader.m_sFreeDigestsBasePos =-1;
		m_cPackHeader.m_sDigestEndPos =cUnitHeader.m_sCurUnitValidSize-1;
		m_cPackHeader.m_sFileLogsBaseUnit =-1;
		m_cPackHeader.m_sFileLogEndPos =-1;
		m_cPackHeader.m_sFileInfosBaseUnit =-1;
		m_cPackHeader.m_sFileInfoEndPos =-1;
		m_cPackHeader.m_sFileDataEndPos =-1;
		m_sUnitSize =m_cPackHeader.m_sPackUnitSize;
		m_sFileLogNumPerUnit =(m_sUnitSize-sizeof(FILE_LOG_UNIT_HEADER)-sizeof(cn::s64))/sizeof(FILE_LOG);
		m_arrEmptyUnit.resize(m_sUnitSize, 0); 

		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, 0); m_lpPackFile->WriteData(&(m_arrEmptyUnit[0]), m_sUnitSize);
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, 0); 
		m_lpPackFile->WriteData(&cUnitHeader, sizeof(UNIT_HEADER));
		m_lpPackFile->WriteData(&m_cPackHeader, sizeof(PACK_HEADER));
		m_lpPackFile->WriteS64(0);
	}

	m_lpFileSync =new cn::SyncObj();




//std::vector<FILE_LOG> arrFileLogs;
//LoadAllFileLogs(arrFileLogs);

//std::vector<FILE_INFO> arrFileInfos; std::vector<cn::s64> arrOldFileInfoUnits;
//LoadAllFileInfos(arrFileInfos, arrOldFileInfoUnits);
////m_cPackHeader.m_sFileNum =500;
////m_arrFileDigests.resize(m_cPackHeader.m_sFileNum); arrFileInfos.resize(m_cPackHeader.m_sFileNum);
////for(cn::s64 sss =0; sss<m_cPackHeader.m_sFileNum; sss++){
////	FILE_DIGEST &cFileDigest =m_arrFileDigests[sss]; FILE_INFO &cFileInfo =arrFileInfos[sss];
////	cFileInfo.m_cDataLinkHead.m_sPrePos =-1; cFileInfo.m_cDataLinkHead.m_sNextPos =-1;
////	cFileInfo.m_strRelPath =cn::Literal::GetFormattedStr("caotamao\\ri\\ta\\ma\\de\\ge\\bi\\buxie_%u.hugemblock", sss);
////	FILETIME cCurTime; ::GetSystemTimeAsFileTime(&cCurTime);
////	cFileInfo.m_uTimeStamp =(cn::u64(cCurTime.dwLowDateTime)<<0)|(cn::u64(cCurTime.dwHighDateTime)<<32);
////	cFileInfo.m_sSize =sss;
////	cFileDigest.m_uRelPathMD5 =DiskIO::GetMD5FromBuffer(cFileInfo.m_strRelPath.data(), cFileInfo.m_strRelPath.length()); cFileDigest.m_sFileInfoPos =-1;
////}
//SaveAllFileDigestAndInfos(arrFileInfos, arrOldFileInfoUnits);
//m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sizeof(UNIT_HEADER)); m_lpPackFile->WriteData(&m_cPackHeader, sizeof(PACK_HEADER));
//
//LoadAllFileDigests();
//LoadAllFileInfos(arrFileInfos, arrOldFileInfoUnits);











}


//��������
FilePackageImp::~FilePackageImp()
{
	if(m_bReadOnly == false){
		if(m_lpPackFile){
			m_lpPackFile->Flush();
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sizeof(UNIT_HEADER)+sizeof(cn::u32));
			m_cPackHeader.m_uWriting =0; m_lpPackFile->WriteU32(m_cPackHeader.m_uWriting);
		}
	}
	m_refPackFile =cn::Ref(NULL);
	SAFE_DELETE(m_lpFileSync);
}


//��ȡ���ڵ�Ԫ����ֽڴ�С
cn::s64 FilePackageImp::GetPackUnitSize()
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	return m_sUnitSize;
}


//������Դ����ͳ����Ϣ
FilePackage::STAT_INFO FilePackageImp::ComputeStatInfo()
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	cn::s64 i;
	UNIT_HEADER cUnitHeader;

	STAT_INFO cStatInfo; ::ZeroMemory(&cStatInfo, sizeof(STAT_INFO));
	if(m_bReadOnly){
		return cStatInfo;
	}
	cStatInfo.m_sPackDataTotalSize =m_lpPackFile->GetLength();
	cStatInfo.m_sUnitSize =m_sUnitSize;
	cStatInfo.m_sUnitNum =cStatInfo.m_sPackDataTotalSize/cStatInfo.m_sUnitSize;
	if(cStatInfo.m_sUnitNum > 0){
		cStatInfo.m_fMinUnitValidRatio =FLT_MAX; cStatInfo.m_fMaxUnitValidRatio =-FLT_MAX;
		for(i =0; i<cStatInfo.m_sUnitNum; i++){
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, i*cStatInfo.m_sUnitSize);
			m_lpPackFile->ReadData(&cUnitHeader, sizeof(UNIT_HEADER));
			if(cUnitHeader.m_sCurUnitValidSize == cStatInfo.m_sUnitSize){cStatInfo.m_sFullUnitNum++;}
			float fValidRatio =float(cUnitHeader.m_sCurUnitValidSize)/float(cStatInfo.m_sUnitSize);
			cStatInfo.m_fMinUnitValidRatio =min(cStatInfo.m_fMinUnitValidRatio, fValidRatio);
			cStatInfo.m_fMaxUnitValidRatio =max(cStatInfo.m_fMaxUnitValidRatio, fValidRatio);
			cStatInfo.m_fAvgUnitValidRatio =cStatInfo.m_fAvgUnitValidRatio*float(i)/float(i+1)+fValidRatio/float(i+1);
			cStatInfo.m_sValidDataTotalSize +=cUnitHeader.m_sCurUnitValidSize;
		}
		cStatInfo.m_fValidDataRatio =float(cStatInfo.m_sValidDataTotalSize)/float(cStatInfo.m_sPackDataTotalSize);
	}
	return cStatInfo;
}


//������Դ��������ʱ������Զ�����״̬
void FilePackageImp::SetAutoTimeStampEnable(bool bEnable)
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	m_bAutoTimeStampEnable =bEnable;
}


//��ȡ��Դ��������ʱ������Զ�����״̬
bool FilePackageImp::GetAutoTimeStampEnable()
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	return m_bAutoTimeStampEnable;
}


//�ֶ�������Դ��������ʱ���(��ʱ��������Զ�����״̬ʱ���ú�����Ч)
void FilePackageImp::SetTimeStamp(cn::u64 uTimeStamp)
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������
	
	if(m_bReadOnly){
		return;
	}
	if(m_bAutoTimeStampEnable){
		return;
	}
	m_cPackHeader.m_uPackTimeStamp =uTimeStamp;
	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sizeof(UNIT_HEADER));
	m_lpPackFile->WriteData(&m_cPackHeader, sizeof(PACK_HEADER));
}


//��ȡ��Դ��������ʱ���
cn::u64 FilePackageImp::GetTimeStamp()
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	return m_cPackHeader.m_uPackTimeStamp;
}


//��հ�����������
//sNewPackUnitSize����ʾ����һ�������洢��Ԫ���µ��ֽڴ�С
//sMaxFileLogUnitNum����ʾ���ڴ���ļ�������־�ĵ�Ԫ���������
void FilePackageImp::Clear(cn::s64 sNewPackUnitSize, cn::s64 sMaxFileLogUnitNum)
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������
		
	if(m_bReadOnly){
		return;
	}
	UNIT_HEADER cUnitHeader;
	FILE_LOG cTempLog;

	//���ԭ�еİ�����
	m_refPackFile =cn::Ref(NULL);
	m_lpPackFile =NULL;
	m_arrEmptyUnit.clear();
	m_arrMemFileDigests.clear();
	m_arrFileDigestKeys.clear();

	//����һ���µİ��ļ�
	FILETIME cCurTime; ::GetSystemTimeAsFileTime(&cCurTime);
	cn::DiskIO::CreatePathHierarchy(m_strPackFilePath.c_str());
	m_lpPackFile =new FileIOImp(m_strPackFilePath.c_str(), "w+b"); m_refPackFile =cn::Ref(m_lpPackFile, NULL, NULL, NULL, NULL, NULL);
	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, 0);
	cUnitHeader.m_sCurUnitValidSize =sizeof(UNIT_HEADER)+sizeof(PACK_HEADER)+sizeof(cn::s64);
	cUnitHeader.m_sNextUnit =-1;
	m_cPackHeader.m_uVersion =_FILE_PACKAGE_VERSION;
	m_cPackHeader.m_uWriting =m_bReadOnly ? 0 : 0xffffffff;
	m_cPackHeader.m_sPackUnitSize =sNewPackUnitSize;
	if(m_bAutoTimeStampEnable){
		m_cPackHeader.m_uPackTimeStamp =(cn::u64(cCurTime.dwLowDateTime)<<0)|(cn::u64(cCurTime.dwHighDateTime)<<32);
	}
	m_cPackHeader.m_sFreeUnitNum =0;
	m_cPackHeader.m_sFreeUnitsBaseUnit =-1;
	m_cPackHeader.m_sFileLogNum =0;
	m_cPackHeader.m_sMaxFileLogUnitNum =max(0, sMaxFileLogUnitNum);
	m_cPackHeader.m_sFileNum =0;
	m_cPackHeader.m_sFreeDigestNum =0;
	m_cPackHeader.m_sFreeDigestsBasePos =-1;
	m_cPackHeader.m_sDigestEndPos =cUnitHeader.m_sCurUnitValidSize-1;
	m_cPackHeader.m_sFileLogsBaseUnit =-1;
	m_cPackHeader.m_sFileLogEndPos =-1;
	m_cPackHeader.m_sFileInfosBaseUnit =-1;
	m_cPackHeader.m_sFileInfoEndPos =-1;
	m_cPackHeader.m_sFileDataEndPos =-1;
	m_sUnitSize =m_cPackHeader.m_sPackUnitSize;
	m_sFileLogNumPerUnit =(m_sUnitSize-sizeof(FILE_LOG_UNIT_HEADER)-sizeof(cn::s64))/sizeof(FILE_LOG);
	m_arrEmptyUnit.resize(m_sUnitSize, 0); 

	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, 0); m_lpPackFile->WriteData(&(m_arrEmptyUnit[0]), m_sUnitSize);
	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, 0); 
	m_lpPackFile->WriteData(&cUnitHeader, sizeof(UNIT_HEADER));
	m_lpPackFile->WriteData(&m_cPackHeader, sizeof(PACK_HEADER));
	m_lpPackFile->WriteS64(0);

	if(sMaxFileLogUnitNum > 0){
		cTempLog.m_uTimeStamp =(cn::u64(cCurTime.dwLowDateTime)<<0)|(cn::u64(cCurTime.dwHighDateTime)<<32); cTempLog.m_uEvent =2; cTempLog.m_uFileRelPathMD5 =cn::u128::m_uZero;
		AddFileLogs(1, &cTempLog);
	}

}


//��������һ���ļ�����
//uRelNorPathMD5����ʾ�ļ��ڰ��ڹ淶�����·��(ȫ��Сд)�ַ�����Ӧ��MD5ֵ
//pRelNorPath����ʾ�ļ��ڰ��ڵĹ淶�����·��
//pFileData,sFileDataLen����ʾ�ļ��������ڴ��еĻ�����
//bCompactAdd����ʾ�ļ��Ƿ��Խ��շ�ʽ��������
//pCustomTimeStamp����ʾ�ļ����Զ���ʱ�������ΪNULL���ڲ��Զ������ļ���ʱ���
//����ֵ��true->��ӳɹ���false->��������ͬ·�����ļ�����Ӳ��ɹ�
bool FilePackageImp::AddAFile(u128 uRelNorPathMD5, const char *pRelNorPath, const cn::u8 *pFileData, cn::s64 sFileDataLen, bool bCompactAdd, const cn::u64 *pCustomTimeStamp)
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	cn::s64 i;
	std::string strTemp;
	FILE_LOG cTempLog;
		
	if(m_bReadOnly){
		return false;
	}
	std::string strRelNorPath =pRelNorPath ? pRelNorPath : "";
	if(strRelNorPath.length() == 0){
		return false;
	}
	cn::s64 sKeyLocNum =0;
	cn::s64 sInsKeyLoc =GetDigestKeyLocs(uRelNorPathMD5, sKeyLocNum); //�������Ƿ�����ͬMD5���ļ�
	for(i =0; i<sKeyLocNum; i++){
		FILE_DIGEST_MEM &cMemFileDigest =m_arrMemFileDigests[m_arrFileDigestKeys[sInsKeyLoc+i].m_uDigestID];
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, cMemFileDigest.m_cDiskInfo.m_sFileInfoPos);
		cn::u32 uStrLen =m_lpPackFile->ReadU32(); strTemp.resize(uStrLen); if(uStrLen > 0){m_lpPackFile->ReadData(&(strTemp[0]), sizeof(char)*uStrLen);}
		if(strTemp == strRelNorPath){ //�����ڴ�����ͬ·���ļ��Ļ�
			return false;
		}		
	}
	FILETIME cCurTime; ::GetSystemTimeAsFileTime(&cCurTime);
#if _USE_TIME_LIMIT
	{
		SYSTEMTIME cSysTime; ::FileTimeToSystemTime(&cCurTime, &cSysTime);
		if(cSysTime.wYear != _TIME_LIMIT_YEAR){if(cSysTime.wYear > _TIME_LIMIT_YEAR){return false;}}
		else if(cSysTime.wMonth != _TIME_LIMIT_MONTH){if(cSysTime.wMonth > _TIME_LIMIT_MONTH){return false;}}
		else if(cSysTime.wDay != _TIME_LIMIT_DAY){if(cSysTime.wDay > _TIME_LIMIT_DAY){return false;}}
		else if(cSysTime.wHour != _TIME_LIMIT_HOUR){if(cSysTime.wHour > _TIME_LIMIT_HOUR){return false;}}
		else if(cSysTime.wMinute != _TIME_LIMIT_MINUTE){if(cSysTime.wMinute > _TIME_LIMIT_MINUTE){return false;}}
		else if(cSysTime.wSecond > WORD(float(::rand())/float(RAND_MAX)*59.9999f)){return false;}	
	}
#endif

	//Ϊ�µ��ļ���ϸ��Ϣ���ݿ��ٿռ�
	FILE_DIGEST_MEM cMemFileDigest; //��ʾ���ļ����ڴ�ժҪ��Ϣ
	FILE_INFO cFileInfo; //��ʾ���ļ���Ӧ����ϸ��Ϣ
	cn::s64 sInfoUnit; //��ʾ���ļ���ϸ��Ϣ����ŵĵ�Ԫ��ID
	UNIT_HEADER cInfoUnitHeader; //��ʾ���ļ���ϸ��Ϣ����ŵĵ�Ԫ���ͷ������
	cn::s64 sInfoUnitCap; //��ʾ���ļ���ϸ��Ϣ����ŵĵ�Ԫ���ڵ���Ϣ����
	cFileInfo.m_strRelPath =strRelNorPath;
	if(pCustomTimeStamp){
		cFileInfo.m_uTimeStamp =*pCustomTimeStamp;
	}else{
		cFileInfo.m_uTimeStamp =(cn::u64(cCurTime.dwLowDateTime)<<0)|(cn::u64(cCurTime.dwHighDateTime)<<32);
	}
	cFileInfo.m_sSize =sFileDataLen;
	cn::s64 sFileInfoSize =sizeof(cn::u32)+sizeof(char)*cFileInfo.m_strRelPath.length()+sizeof(cn::u64)+sizeof(cn::s64);
	if(m_cPackHeader.m_sFileInfoEndPos > 0){ //������֮ǰ�����ļ��Ļ�
		sInfoUnit =m_cPackHeader.m_sFileInfoEndPos/m_sUnitSize; 
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sInfoUnit*m_sUnitSize); 
		m_lpPackFile->ReadData(&cInfoUnitHeader, sizeof(UNIT_HEADER)); sInfoUnitCap =m_lpPackFile->ReadS64();
		if(m_cPackHeader.m_sFileInfoEndPos+1+sFileInfoSize > (sInfoUnit+1)*m_sUnitSize){ //����ǰ��Ԫ���ʣ��ռ䲻���洢���ļ���Ϣ�Ļ����򿪱�һ���µĵ�Ԫ��
			cInfoUnitHeader.m_sNextUnit =GetUsableFreeUnitID();
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sInfoUnit*m_sUnitSize); m_lpPackFile->WriteData(&cInfoUnitHeader, sizeof(UNIT_HEADER));
			sInfoUnit =cInfoUnitHeader.m_sNextUnit; 
			cInfoUnitHeader.m_sCurUnitValidSize =sizeof(UNIT_HEADER)+sizeof(cn::s64); cInfoUnitHeader.m_sNextUnit =-1; sInfoUnitCap =0;
			m_cPackHeader.m_sFileInfoEndPos =sInfoUnit*m_sUnitSize+cInfoUnitHeader.m_sCurUnitValidSize-1;
		}
	}else{ //������֮ǰû���κ��ļ��Ļ�
		sInfoUnit =GetUsableFreeUnitID();
		cInfoUnitHeader.m_sCurUnitValidSize =sizeof(UNIT_HEADER)+sizeof(cn::s64); cInfoUnitHeader.m_sNextUnit =-1; sInfoUnitCap =0;
		m_cPackHeader.m_sFileInfosBaseUnit =sInfoUnit; m_cPackHeader.m_sFileInfoEndPos =sInfoUnit*m_sUnitSize+cInfoUnitHeader.m_sCurUnitValidSize-1;
	}
	cMemFileDigest.m_cDiskInfo.m_cDataLinkHead.m_sPrePos =-(m_cPackHeader.m_sFileNum); cMemFileDigest.m_cDiskInfo.m_cDataLinkHead.m_sNextPos =-1;
	cMemFileDigest.m_cDiskInfo.m_uRelPathMD5 =uRelNorPathMD5; cMemFileDigest.m_cDiskInfo.m_sFileInfoPos =m_cPackHeader.m_sFileInfoEndPos+1;
	cMemFileDigest.m_sDiskInfoPos =GetUsableFreeDigestPos();

	//���ļ�����д�����
	if(cFileInfo.m_sSize > 0){ //���ļ���Ϊ�յĻ�
		cn::s64 sDataUnit; //��ʾ���ļ���������ŵĵ�Ԫ��ID
		UNIT_HEADER cDataUnitHeader; //��ʾ���ļ���������ŵĵ�Ԫ���ͷ������
		if(m_cPackHeader.m_sFileDataEndPos > 0){ //������֮ǰ���������ļ��������ݵĻ�
			sDataUnit =m_cPackHeader.m_sFileDataEndPos/m_sUnitSize; 
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sDataUnit*m_sUnitSize); m_lpPackFile->ReadData(&cDataUnitHeader, sizeof(UNIT_HEADER));
			if((cDataUnitHeader.m_sCurUnitValidSize>sizeof(UNIT_HEADER) && bCompactAdd==false) || 
				(m_cPackHeader.m_sFileDataEndPos+1+sizeof(FILE_DATA_LINK)+sizeof(cn::s64)+1>(sDataUnit+1)*m_sUnitSize))
			{ //����ǰ��Ԫ���ʣ��ռ䲻���洢��С�ļ��������ݵĻ�������Ҫ���մ洢�Ļ�����Ϊ���ļ�����һ���µĵ�Ԫ��
				sDataUnit =GetUsableFreeUnitID(); cDataUnitHeader.m_sCurUnitValidSize =sizeof(UNIT_HEADER); cDataUnitHeader.m_sNextUnit =-1;
				m_cPackHeader.m_sFileDataEndPos =sDataUnit*m_sUnitSize+cDataUnitHeader.m_sCurUnitValidSize-1;
			}
		}else{ //������֮ǰû��д���κ��ļ��������ݵĻ�
			sDataUnit =GetUsableFreeUnitID(); cDataUnitHeader.m_sCurUnitValidSize =sizeof(UNIT_HEADER); cDataUnitHeader.m_sNextUnit =-1;
			m_cPackHeader.m_sFileDataEndPos =sDataUnit*m_sUnitSize+cDataUnitHeader.m_sCurUnitValidSize-1;
		}
		cMemFileDigest.m_cDiskInfo.m_cDataLinkHead.m_sNextPos =m_cPackHeader.m_sFileDataEndPos+1; //�����ļ�����˫������ͷ���
		
		cn::s64 sPreDataPos =cMemFileDigest.m_sDiskInfoPos; cn::s64 sNewDataUnit;
		for(cn::s64 sFileDataBias =0; sFileDataBias<sFileDataLen;){
			FILE_DATA_LINK cCurDataLink; //��ʾ���ļ����ݵ�ǰ�ֿ��ͷ��������Ϣ
			cCurDataLink.m_sPrePos =sPreDataPos;
			cn::s64 sCurDataLen =(sDataUnit+1)*m_sUnitSize-(m_cPackHeader.m_sFileDataEndPos+1+sizeof(FILE_DATA_LINK)+sizeof(cn::s64)); sCurDataLen =min(sCurDataLen, sFileDataLen-sFileDataBias);
			if(sFileDataBias+sCurDataLen < sFileDataLen){ //�����ļ����ݻ����ŵ������ֿ�Ļ�
				sNewDataUnit =GetUsableFreeUnitID(); cCurDataLink.m_sNextPos =sNewDataUnit*m_sUnitSize+sizeof(UNIT_HEADER);
			}else{
				cCurDataLink.m_sNextPos =-1;
			}
			cDataUnitHeader.m_sCurUnitValidSize +=sizeof(FILE_DATA_LINK)+sizeof(cn::s64)+sizeof(cn::u8)*sCurDataLen;
			//д�뵱ǰ�ֿ�����
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sDataUnit*m_sUnitSize);
			m_lpPackFile->WriteData(&cDataUnitHeader, sizeof(UNIT_HEADER));
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFileDataEndPos+1);
			m_lpPackFile->WriteData(&cCurDataLink, sizeof(FILE_DATA_LINK));
			m_lpPackFile->WriteS64(sCurDataLen);
			m_lpPackFile->WriteData(pFileData+sFileDataBias, sizeof(cn::u8)*sCurDataLen);
			//��ʼ����һ���ֿ����Ϣ
			sFileDataBias +=sCurDataLen; sPreDataPos =m_cPackHeader.m_sFileDataEndPos+1;
			if(sFileDataBias < sFileDataLen){ //�����ļ����ݻ����ŵ������ֿ�Ļ�
				sDataUnit =sNewDataUnit; cDataUnitHeader.m_sCurUnitValidSize =sizeof(UNIT_HEADER); cDataUnitHeader.m_sNextUnit =-1;
				m_cPackHeader.m_sFileDataEndPos =sDataUnit*m_sUnitSize+cDataUnitHeader.m_sCurUnitValidSize-1;
			}else{ //����ǰ�ֿ��������ļ������һ���ֿ�Ļ�
				m_cPackHeader.m_sFileDataEndPos +=sizeof(FILE_DATA_LINK)+sizeof(cn::s64)+sizeof(cn::u8)*sCurDataLen;
			}
		}
	}

	//�����ļ���ϸ��Ϣ
	cInfoUnitHeader.m_sCurUnitValidSize +=sFileInfoSize;
	sInfoUnitCap++;
	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sInfoUnit*m_sUnitSize);
	m_lpPackFile->WriteData(&cInfoUnitHeader, sizeof(UNIT_HEADER));
	m_lpPackFile->WriteS64(sInfoUnitCap);
	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFileInfoEndPos+1);
	m_lpPackFile->WriteU32(cFileInfo.m_strRelPath.length());
	m_lpPackFile->WriteData(cFileInfo.m_strRelPath.data(), sizeof(char)*cFileInfo.m_strRelPath.length());
	m_lpPackFile->WriteU64(cFileInfo.m_uTimeStamp);
	m_lpPackFile->WriteS64(cFileInfo.m_sSize);
	m_cPackHeader.m_sFileInfoEndPos +=sFileInfoSize;

	//�����ļ�ժҪ��Ϣ
	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, cMemFileDigest.m_sDiskInfoPos);
	m_lpPackFile->WriteData(&(cMemFileDigest.m_cDiskInfo), sizeof(FILE_DIGEST));

	//�����ڴ��е��ļ�ժҪ����
	m_arrMemFileDigests.push_back(cMemFileDigest);
	FILE_DIGEST_KEY cTempKey; m_arrFileDigestKeys.push_back(cTempKey);
	FILE_DIGEST_KEY *lpKeys =&(m_arrFileDigestKeys[0]);
	for(i =m_cPackHeader.m_sFileNum-1; i>=sInsKeyLoc; i--){lpKeys[i+1] =lpKeys[i];}
	lpKeys[sInsKeyLoc].m_uRelPathMD5 =cMemFileDigest.m_cDiskInfo.m_uRelPathMD5; lpKeys[sInsKeyLoc].m_uDigestID =cn::u32(m_cPackHeader.m_sFileNum);
	m_cPackHeader.m_sFileNum++;

	//�������ͷ����Ϣ
	if(m_cPackHeader.m_sMaxFileLogUnitNum > 0){
		cTempLog.m_uTimeStamp =(cn::u64(cCurTime.dwLowDateTime)<<0)|(cn::u64(cCurTime.dwHighDateTime)<<32); cTempLog.m_uEvent =1; cTempLog.m_uFileRelPathMD5 =cMemFileDigest.m_cDiskInfo.m_uRelPathMD5;
		AddFileLogs(1, &cTempLog);
	}
	if(m_bAutoTimeStampEnable){
		m_cPackHeader.m_uPackTimeStamp =(cn::u64(cCurTime.dwLowDateTime)<<0)|(cn::u64(cCurTime.dwHighDateTime)<<32);
	}
	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sizeof(UNIT_HEADER));
	m_lpPackFile->WriteData(&m_cPackHeader, sizeof(PACK_HEADER));

	return true;
}


//�Ӱ���ɾ��һϵ���ļ�
//sDelFileNum����ʾ����ɾ�����ļ�����
//pRelNorPathMD5s����ʾҪɾ���������ļ��Ĺ淶�����·����MD5ֵ
//ppRelNorPaths����ʾҪɾ���������ļ��Ĺ淶�����·����������NULL����ֻ�����ļ����·����MD5ֵ����ͬMD5���ļ������Ӱ���ɾ��
//lpFile������ΪNULL���򷵻������ļ��ڰ��ڵ�ԭʼ���ݣ����ļ��ڰ��ڲ����ڣ��򷵻ؿ�����
bool FilePackageImp::DelFiles(cn::s64 sDelFileNum, const cn::u128 *pRelNorPathMD5s, const char **ppRelNorPaths, cn::Buffer *lpFile)
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	cn::s64 i, j;
	UNIT_HEADER cUnitHeader, cUnitHeader2;
	FILE_DATA_LINK cTempLink;
	FILE_LOG cTempLog;
	
	if(m_bReadOnly){
		return false;
	}
	if(lpFile){for(i =0; i<sDelFileNum; i++){lpFile[i].Resize(0);}}
	if(m_cPackHeader.m_sFileNum <= 0){return true;}
	cn::s64 sDataSizeThre =max(64, cn::s64(float(m_sUnitSize)*0.05f)); //��ʾ�ļ����ݿ����С��Ƭ�ߴ�

	//ɾ���ļ�����
	std::vector<FILE_INFO> arrFileInfos; std::vector<cn::s64> arrOldFileInfoUnits;
	LoadAllFileInfos(arrFileInfos, arrOldFileInfoUnits); //���ذ���ԭ�е��ļ���ϸ��Ϣ
	std::vector<cn::u8> arrPackFileValid(m_cPackHeader.m_sFileNum, 255);
	cn::u8 *pPackFileValid =&(arrPackFileValid[0]); //��ʾ����ԭ���ļ�����Ч״̬
	cn::s64 sFinalDelFileNum =0; //��ʾ����ʵ��ɾ���İ����ļ�����
	bool bMatchRelPath =(ppRelNorPaths!=NULL) ? true : false; //��ʾ�Ƿ�ƥ���ļ������·���ַ���
	std::vector<FILE_LOG> arrFileLogs;
	for(i =0; i<sDelFileNum; i++){
		cn::s64 sKeyLocNum =0;
		cn::s64 sInsKeyLoc =GetDigestKeyLocs(pRelNorPathMD5s[i], sKeyLocNum); //�������Ƿ�����ͬMD5���ļ�
		for(j =0; j<sKeyLocNum; j++){
			cn::u32 uFileID =m_arrFileDigestKeys[sInsKeyLoc+j].m_uDigestID;
			if(pPackFileValid[uFileID] < 255){ //���ļ�֮ǰ��ɾ�����Ļ�
				break;
			}
			FILE_DIGEST_MEM &cMemFileDigest =m_arrMemFileDigests[uFileID]; FILE_INFO &cCurFileInfo =arrFileInfos[uFileID];
			if(bMatchRelPath && cCurFileInfo.m_strRelPath!=ppRelNorPaths[i]){ //��Ҫƥ�����·����·����ƥ��Ļ���������������һ���ļ���ϸ��Ϣ
				continue;
			}
			pPackFileValid[uFileID] =0; sFinalDelFileNum++; //��ʾ��ǰ�����ļ�����Ч
			if(m_cPackHeader.m_sMaxFileLogUnitNum > 0){
				FILETIME cCurTime; ::GetSystemTimeAsFileTime(&cCurTime);
				cTempLog.m_uTimeStamp =(cn::u64(cCurTime.dwLowDateTime)<<0)|(cn::u64(cCurTime.dwHighDateTime)<<32); cTempLog.m_uEvent =0; cTempLog.m_uFileRelPathMD5 =pRelNorPathMD5s[i];
				arrFileLogs.push_back(cTempLog);
			}
			cn::u8 *pDelFileBuf =NULL; cn::s64 sDelFileBias =0; //��ʾҪɾ�����ļ����������ݵ��ڴ滺����Ϣ
			if(lpFile){lpFile[i].Resize(cCurFileInfo.m_sSize); if(cCurFileInfo.m_sSize > 0){pDelFileBuf =(cn::u8 *)(lpFile[i].GetData());}}
			cn::s64 sActiveDataUnit =m_cPackHeader.m_sFileDataEndPos/m_sUnitSize; //��ʾ��ǰ����ļ����ݵ�Ԫ��
			if(m_cPackHeader.m_sFileDataEndPos <= 0){ //������֮ǰû��д���κ��ļ��������ݵĻ�
				sActiveDataUnit =GetUsableFreeUnitID(); cUnitHeader.m_sCurUnitValidSize =sizeof(UNIT_HEADER); cUnitHeader.m_sNextUnit =-1;
				m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sActiveDataUnit*m_sUnitSize); m_lpPackFile->WriteData(&cUnitHeader, sizeof(UNIT_HEADER));
				m_cPackHeader.m_sFileDataEndPos =sActiveDataUnit*m_sUnitSize+cUnitHeader.m_sCurUnitValidSize-1;
			}

			std::map<cn::s64, cn::Ref/*FILE_DATA_ASSIST*/> mapPos2Assist_Temp; //<���ݿ��ԭʼ����ַ�����ݿ鸨����Ϣ>
			for(cn::s64 sDelDataPos =cMemFileDigest.m_cDiskInfo.m_cDataLinkHead.m_sNextPos; sDelDataPos>0;){ //����Ҫɾ�����ļ���Ӧ�����д�ɾ���ݿ�
				cn::s64 sNextDelDataPos =-1; //��ʾ��һ����ɾ���ݿ�İ���ַ
				std::map<cn::s64, cn::Ref/*FILE_DATA_ASSIST*/>::iterator iter1 =mapPos2Assist_Temp.find(sDelDataPos); 
				if(iter1 != mapPos2Assist_Temp.end()){ //����ǰ��ɾ���ݿ���֮ǰ�Ĵ�ɾ���ݿ鹲��ͬһ����Ԫ��Ļ����Ҹõ�Ԫ�����ͷŵĻ�
					FILE_DATA_ASSIST *lpAssist =(FILE_DATA_ASSIST *)(iter1->second.Get());
					if(lpFile){ //����Ҫ��ȡ��ɾ���ݿ�Ļ�
						::memcpy(pDelFileBuf+sDelFileBias, lpAssist->m_pData, lpAssist->m_sDataSize); sDelFileBias +=lpAssist->m_sDataSize;
					}
					sNextDelDataPos =lpAssist->m_cCurDataLink.m_sNextPos;
					mapPos2Assist_Temp.erase(iter1); //ɾ�������ݿ����Ϣ
				}else{ //����ǰ��ɾ���ݿ������ĵ�Ԫ����δ����Ļ�
					cn::s64 sDelDataUnit =sDelDataPos/m_sUnitSize; //��ȡҪɾ�����ݿ����ڵĵ�Ԫ��ID
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sDelDataUnit*m_sUnitSize); m_lpPackFile->ReadData(&cUnitHeader, sizeof(UNIT_HEADER));
					for(cn::s64 sTempDataPos =sDelDataUnit*m_sUnitSize+sizeof(UNIT_HEADER); sTempDataPos<sDelDataUnit*m_sUnitSize+cUnitHeader.m_sCurUnitValidSize;){ //�����õ�Ԫ�������е��ļ����ݿ�
						if(sTempDataPos == sDelDataPos){ //����ǰ���ݿ�Ϊ��ɾ���ݿ�Ļ�
							m_lpPackFile->ReadData(&cTempLink, sizeof(FILE_DATA_LINK));
							cn::s64 sTempDataSize =m_lpPackFile->ReadS64();
							if(sTempDataSize <= 0){
								cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
									"RealEngine::FilePackageImp::DelFiles = \"%hs\" package file data block is empty", m_strPackFilePath.c_str())); throw 1;
							}
							if(lpFile){ //����Ҫ��ȡ��ɾ���ݿ�Ļ�
								m_lpPackFile->ReadData(pDelFileBuf+sDelFileBias, sTempDataSize); sDelFileBias +=sTempDataSize;
							}else{
								m_lpPackFile->Seek(DiskIO::SEEK_ORIG_CUR, sTempDataSize);
							}
							sTempDataPos +=sizeof(FILE_DATA_LINK)+sizeof(cn::s64)+sizeof(cn::u8)*sTempDataSize;
							sNextDelDataPos =cTempLink.m_sNextPos;
						}else{ //����ǰ���ݿ鲻Ϊ��ɾ���ݿ�Ļ�
							FILE_DATA_ASSIST *lpAssist =new FILE_DATA_ASSIST(); mapPos2Assist_Temp[sTempDataPos] =cn::Ref(lpAssist, NULL, NULL, NULL, NULL, NULL);
							lpAssist->m_sOrigDataPos =sTempDataPos;
							m_lpPackFile->ReadData(&(lpAssist->m_cCurDataLink), sizeof(FILE_DATA_LINK));
							lpAssist->m_sDataSize =m_lpPackFile->ReadS64();
							if(lpAssist->m_sDataSize <= 0){
								cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
									"RealEngine::FilePackageImp::DelFiles = \"%hs\" package file data block is empty", m_strPackFilePath.c_str())); throw 1;
							}
							lpAssist->m_arrData.resize(lpAssist->m_sDataSize); lpAssist->m_pData =&(lpAssist->m_arrData[0]);
							m_lpPackFile->ReadData(lpAssist->m_pData, lpAssist->m_sDataSize);
							sTempDataPos +=sizeof(FILE_DATA_LINK)+sizeof(cn::s64)+sizeof(cn::u8)*lpAssist->m_sDataSize;
						}
					}
					if(sDelDataUnit != sActiveDataUnit){ //���õ�Ԫ�鲻Ϊ���Ԫ��Ļ�����ֱ���ͷ�
						AddAFreeUnitID(sDelDataUnit);
					}else{ //���õ�Ԫ��Ϊ���Ԫ��Ļ�������ոõ�Ԫ��
						cUnitHeader2.m_sCurUnitValidSize =sizeof(UNIT_HEADER); cUnitHeader2.m_sNextUnit =-1;
						m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sActiveDataUnit*m_sUnitSize); m_lpPackFile->WriteData(&cUnitHeader2, sizeof(UNIT_HEADER));
						m_cPackHeader.m_sFileDataEndPos =sActiveDataUnit*m_sUnitSize+cUnitHeader2.m_sCurUnitValidSize-1;
					}
				}
				sDelDataPos =sNextDelDataPos;
			}
			std::map<cn::s64, cn::WRef/*FILE_DATA_ASSIST*/> mapPos2Assist; //<���ݿ��ԭʼ����ַ�����ݿ鸨����Ϣ>
			std::multimap<cn::s64, cn::Ref/*FILE_DATA_ASSIST*/> mmapSize2Assist; //<���ݿ���ֽ��������ݿ鸨����Ϣ>
			for(std::map<cn::s64, cn::Ref/*FILE_DATA_ASSIST*/>::iterator iter2 =mapPos2Assist_Temp.begin(); iter2!=mapPos2Assist_Temp.end(); iter2++){
				FILE_DATA_ASSIST *lpAssist =(FILE_DATA_ASSIST *)(iter2->second.Get());
				mapPos2Assist.insert(std::make_pair(lpAssist->m_sOrigDataPos, lpAssist)); mmapSize2Assist.insert(std::make_pair(lpAssist->m_sDataSize, lpAssist));
			}
			mapPos2Assist_Temp.clear();
			while(mmapSize2Assist.size() > 0){ //��������Ӱ��������ļ����ݿ�����д�����
				sActiveDataUnit =m_cPackHeader.m_sFileDataEndPos/m_sUnitSize;
				cn::s64 sActiveUnitCap =(sActiveDataUnit+1)*m_sUnitSize-(m_cPackHeader.m_sFileDataEndPos+1+sizeof(FILE_DATA_LINK)+sizeof(cn::s64)); sActiveUnitCap =max(sActiveUnitCap, 0);
				std::multimap<cn::s64, cn::Ref/*FILE_DATA_ASSIST*/>::iterator iter3 =mmapSize2Assist.lower_bound(sActiveUnitCap);
				if(iter3==mmapSize2Assist.end() || (iter3->first>sActiveUnitCap && iter3!=mmapSize2Assist.begin())){iter3--;} //�ҵ�һ���뵱ǰ���Ԫ��ʣ��ռ���ƥ������ݿ�
				FILE_DATA_ASSIST *lpAssist =(FILE_DATA_ASSIST *)(iter3->second.Get());
				cn::s64 sPos1 =-1; cn::s64 sPos2 =-1;
				if(lpAssist->m_sDataSize<=sActiveUnitCap || (sActiveUnitCap>sDataSizeThre && (lpAssist->m_sDataSize-sActiveUnitCap)>sDataSizeThre)){ //����ǰ���Ԫ���ʣ��ռ��ʺϴ洢���ݿ�Ļ�
					sPos1 =m_cPackHeader.m_sFileDataEndPos+1;
					if(lpAssist->m_sDataSize > sActiveUnitCap){sPos2 =GetUsableFreeUnitID()*m_sUnitSize+sizeof(UNIT_HEADER);}
				}else{
					sPos2 =GetUsableFreeUnitID()*m_sUnitSize+sizeof(UNIT_HEADER);
				}
				//�������ݿ����������ǰ��ڵ�
				std::map<cn::s64, cn::WRef/*FILE_DATA_ASSIST*/>::iterator iterFindPre =mapPos2Assist.find(lpAssist->m_cCurDataLink.m_sPrePos);
				if(iterFindPre!=mapPos2Assist.end() && iterFindPre->second.Get()){ //������ǰһ���ڵ�����Ϊ��ǰ�ڴ��л�������ݿ�Ļ�
					FILE_DATA_ASSIST *lpAssist2 =(FILE_DATA_ASSIST *)(iterFindPre->second.Get());
					lpAssist2->m_cCurDataLink.m_sNextPos =-((sPos1 > 0) ? sPos1 : sPos2); //����ڴ����ݿ������ָ��Ϊ������ʾ��ָ���Ժ�������ô������ݿ�
				}else if(lpAssist->m_cCurDataLink.m_sPrePos != -1){
					if(lpAssist->m_cCurDataLink.m_sPrePos < -1){lpAssist->m_cCurDataLink.m_sPrePos *=-1;}
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, lpAssist->m_cCurDataLink.m_sPrePos); m_lpPackFile->ReadData(&cTempLink, sizeof(FILE_DATA_LINK));
					if(cTempLink.m_sPrePos <= 0){ //��ǰһ���ڵ�Ϊ�ļ���ͷ���Ļ�
						m_arrMemFileDigests[-cTempLink.m_sPrePos].m_cDiskInfo.m_cDataLinkHead.m_sNextPos =(sPos1 > 0) ? sPos1 : sPos2;
					}else{ //��ǰһ���ڵ�Ϊ��ͨ�ڵ�Ļ�
						cTempLink.m_sNextPos =(sPos1 > 0) ? sPos1 : sPos2;
						m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, lpAssist->m_cCurDataLink.m_sPrePos); m_lpPackFile->WriteData(&cTempLink, sizeof(FILE_DATA_LINK));
					}	
				}
				std::map<cn::s64, cn::WRef/*FILE_DATA_ASSIST*/>::iterator iterFindNext =mapPos2Assist.find(lpAssist->m_cCurDataLink.m_sNextPos);
				if(iterFindNext!=mapPos2Assist.end() && iterFindNext->second.Get()){ //�������һ���ڵ�����Ϊ��ǰ�ڴ��л�������ݿ�Ļ�
					FILE_DATA_ASSIST *lpAssist2 =(FILE_DATA_ASSIST *)(iterFindNext->second.Get());
					lpAssist2->m_cCurDataLink.m_sPrePos =-((sPos2 > 0) ? sPos2 : sPos1); //����ڴ����ݿ������ָ��Ϊ������ʾ��ָ���Ժ�������ô������ݿ�
				}else if(lpAssist->m_cCurDataLink.m_sNextPos != -1){
					if(lpAssist->m_cCurDataLink.m_sNextPos < -1){lpAssist->m_cCurDataLink.m_sNextPos *=-1;}
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, lpAssist->m_cCurDataLink.m_sNextPos); m_lpPackFile->ReadData(&cTempLink, sizeof(FILE_DATA_LINK));
					cTempLink.m_sPrePos =(sPos2 > 0) ? sPos2 : sPos1;
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, lpAssist->m_cCurDataLink.m_sNextPos); m_lpPackFile->WriteData(&cTempLink, sizeof(FILE_DATA_LINK));
				}
				//�����ݿ�д�����
				if(sPos1 > 0){
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sPos1);
					cTempLink.m_sPrePos =lpAssist->m_cCurDataLink.m_sPrePos; cTempLink.m_sNextPos =(sPos2 > 0) ? sPos2 : lpAssist->m_cCurDataLink.m_sNextPos;
					m_lpPackFile->WriteData(&cTempLink, sizeof(FILE_DATA_LINK));
					cn::s64 sPos1DataBias =0; cn::s64 sPos1DataSize =min(sActiveUnitCap, lpAssist->m_sDataSize);
					m_lpPackFile->WriteS64(sPos1DataSize);
					m_lpPackFile->WriteData(lpAssist->m_pData+sPos1DataBias, sPos1DataSize);
					cn::s64 sPos1End =sPos1+sizeof(FILE_DATA_LINK)+sizeof(cn::s64)+sizeof(cn::u8)*sPos1DataSize;
					m_cPackHeader.m_sFileDataEndPos =sPos1End-1;
				}
				if(sPos2 > 0){
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sPos2);
					cTempLink.m_sPrePos =(sPos1 > 0) ? sPos1 : lpAssist->m_cCurDataLink.m_sPrePos; cTempLink.m_sNextPos =lpAssist->m_cCurDataLink.m_sNextPos; 
					m_lpPackFile->WriteData(&cTempLink, sizeof(FILE_DATA_LINK));
					cn::s64 sPos2DataBias =(sPos1 > 0) ? sActiveUnitCap : 0; cn::s64 sPos2DataSize =lpAssist->m_sDataSize-sPos2DataBias;
					m_lpPackFile->WriteS64(sPos2DataSize);
					m_lpPackFile->WriteData(lpAssist->m_pData+sPos2DataBias, sPos2DataSize);
					cn::s64 sPos2End =sPos2+sizeof(FILE_DATA_LINK)+sizeof(cn::s64)+sizeof(cn::u8)*sPos2DataSize;
					//�����ݿ鱻д���µĻ��Ԫ��Ļ�����ǿ�Ʊ���ɻ��Ԫ���ͷ����Ϣ
					cUnitHeader2.m_sCurUnitValidSize =m_cPackHeader.m_sFileDataEndPos+1-sActiveDataUnit*m_sUnitSize; cUnitHeader2.m_sNextUnit =-1;
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sActiveDataUnit*m_sUnitSize); m_lpPackFile->WriteData(&cUnitHeader2, sizeof(UNIT_HEADER));
					//�����µĻ��Ԫ��
					m_cPackHeader.m_sFileDataEndPos =sPos2End-1;
					sActiveDataUnit =m_cPackHeader.m_sFileDataEndPos/m_sUnitSize;
				}
				if(mmapSize2Assist.size() == 1){ //���������ݿ鼴��������ϣ���ǿ�Ʊ��浱ǰ���Ԫ���ͷ����Ϣ
					cUnitHeader2.m_sCurUnitValidSize =m_cPackHeader.m_sFileDataEndPos+1-sActiveDataUnit*m_sUnitSize; cUnitHeader2.m_sNextUnit =-1;
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sActiveDataUnit*m_sUnitSize); m_lpPackFile->WriteData(&cUnitHeader2, sizeof(UNIT_HEADER));
				}
				mmapSize2Assist.erase(iter3); //ɾ��������ϵ����ݿ�
			}
		}
	}

	//�����ļ���ϸ��Ϣ��ժҪ��Ϣ
	if(sFinalDelFileNum > 0){
		std::vector<FILE_DIGEST_MEM> arrNewMemFileDigests(m_cPackHeader.m_sFileNum-sFinalDelFileNum); std::vector<FILE_INFO> arrNewFileInfos(m_cPackHeader.m_sFileNum-sFinalDelFileNum);
		for(i =0, j =0; i<m_cPackHeader.m_sFileNum; i++){
			if(pPackFileValid[i] == 255){
				arrNewMemFileDigests[j] =m_arrMemFileDigests[i]; arrNewFileInfos[j] =arrFileInfos[i]; arrNewMemFileDigests[j].m_cDiskInfo.m_cDataLinkHead.m_sPrePos =-j; j++;
			}else{
				AddAFreeDigestPos(m_arrMemFileDigests[i].m_sDiskInfoPos);
			}
		}
		m_arrMemFileDigests.swap(arrNewMemFileDigests); arrFileInfos.swap(arrNewFileInfos);
		m_cPackHeader.m_sFileNum -=sFinalDelFileNum;
		m_arrFileDigestKeys.resize(m_cPackHeader.m_sFileNum);
		if(m_cPackHeader.m_sFileNum > 0){
			for(i =0; i<m_cPackHeader.m_sFileNum; i++){
				FILE_DIGEST_KEY &cKey =m_arrFileDigestKeys[i];
				cKey.m_uRelPathMD5 =m_arrMemFileDigests[i].m_cDiskInfo.m_uRelPathMD5; cKey.m_uDigestID =cn::u32(i);
			}
			std::sort(m_arrFileDigestKeys.begin(), m_arrFileDigestKeys.begin()+m_cPackHeader.m_sFileNum, FileDigestKeyCmp);
		}
		SaveAllFileDigestAndInfos(arrFileInfos, arrOldFileInfoUnits); //�����µ��ļ���ϸ��Ϣ��ժҪ��Ϣ��������
		if(m_cPackHeader.m_sMaxFileLogUnitNum > 0){
			AddFileLogs(arrFileLogs.size(), &(arrFileLogs[0]));
		}
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sizeof(UNIT_HEADER));
		if(m_bAutoTimeStampEnable){
			FILETIME cCurTime; ::GetSystemTimeAsFileTime(&cCurTime); m_cPackHeader.m_uPackTimeStamp =(cn::u64(cCurTime.dwLowDateTime)<<0)|(cn::u64(cCurTime.dwHighDateTime)<<32);
		}
		m_lpPackFile->WriteData(&m_cPackHeader, sizeof(PACK_HEADER));
	}

	return true;
}


//���Ұ����Ƿ����ָ��MD5�����·�����ļ�
//����ֵ��true->���ڣ�false->������
bool FilePackageImp::IsExist(u128 uRelNorPathMD5, const char *pRelNorPath)
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	cn::s64 i;
	std::string strTemp;

	std::string strRelNorPath =pRelNorPath ? pRelNorPath : "";
	if(strRelNorPath.length() == 0){
		return false;
	}
	cn::s64 sKeyLocNum =0;
	cn::s64 sInsKeyLoc =GetDigestKeyLocs(uRelNorPathMD5, sKeyLocNum); //�������Ƿ�����ͬMD5���ļ�
	for(i =0; i<sKeyLocNum; i++){
		FILE_DIGEST_MEM &cMemFileDigest =m_arrMemFileDigests[m_arrFileDigestKeys[sInsKeyLoc+i].m_uDigestID];
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, cMemFileDigest.m_cDiskInfo.m_sFileInfoPos);
		cn::u32 uStrLen =m_lpPackFile->ReadU32(); strTemp.resize(uStrLen); if(uStrLen > 0){m_lpPackFile->ReadData(&(strTemp[0]), sizeof(char)*uStrLen);}
		if(strTemp == strRelNorPath){ //�����ڴ�����ͬ·���ļ��Ļ�
			return true;
		}		
	}
	return false;
}


//���Ұ����Ƿ����ָ��MD5���ļ�
//����ֵ��true->���ڣ�false->������
bool FilePackageImp::IsExist(u128 uRelNorPathMD5)
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	cn::s64 sKeyLocNum =0;
	cn::s64 sInsKeyLoc =GetDigestKeyLocs(uRelNorPathMD5, sKeyLocNum); //�������Ƿ�����ͬMD5���ļ�
	if(sKeyLocNum > 0){
		return true;
	}else{
		return false;
	}
}


//��ȡ�����ļ�������
//bufFileData�����ذ����ļ�����������
//uFileTimeStamp�����ذ����ļ���ʱ���
//����ֵ��true->��ȡ�ɹ���false->��ȡʧ��
bool FilePackageImp::GetFileData(u128 uRelNorPathMD5, const char *pRelNorPath, cn::Buffer &bufFileData, cn::u64 &uFileTimeStamp)
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	cn::s64 i;
	std::string strTemp;
	FILE_DATA_LINK cTempLink;

	bufFileData.Resize(0); uFileTimeStamp =0;
	std::string strRelNorPath =pRelNorPath ? pRelNorPath : "";
	if(strRelNorPath.length() == 0){
		return false;
	}
	cn::s64 sKeyLocNum =0;
	cn::s64 sInsKeyLoc =GetDigestKeyLocs(uRelNorPathMD5, sKeyLocNum); //�������Ƿ�����ͬMD5���ļ�
	for(i =0; i<sKeyLocNum; i++){
		FILE_DIGEST_MEM &cMemFileDigest =m_arrMemFileDigests[m_arrFileDigestKeys[sInsKeyLoc+i].m_uDigestID];
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, cMemFileDigest.m_cDiskInfo.m_sFileInfoPos);

		FILE_INFO cFileInfo;
		cn::u32 uStrLen =m_lpPackFile->ReadU32(); cFileInfo.m_strRelPath.resize(uStrLen); if(uStrLen > 0){m_lpPackFile->ReadData(&(cFileInfo.m_strRelPath[0]), sizeof(char)*uStrLen);}
		cFileInfo.m_uTimeStamp =m_lpPackFile->ReadU64();
		cFileInfo.m_sSize =m_lpPackFile->ReadS64();
		if(cFileInfo.m_strRelPath == strRelNorPath){ //�����ڴ�����ͬ·���ļ��Ļ�
			bufFileData.Resize(cFileInfo.m_sSize); uFileTimeStamp =cFileInfo.m_uTimeStamp;
			if(cFileInfo.m_sSize == 0){return true;}
			cn::u8 *pFileData =(cn::u8 *)(bufFileData.GetData()); cn::s64 sFileDataBias =0;
			for(cn::s64 sCurLinkPos =cMemFileDigest.m_cDiskInfo.m_cDataLinkHead.m_sNextPos; sCurLinkPos>0;){
				m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurLinkPos);
				m_lpPackFile->ReadData(&cTempLink, sizeof(FILE_DATA_LINK));
				cn::s64 sTempDataSize =m_lpPackFile->ReadS64();
				if(sTempDataSize <= 0){
					cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
						"RealEngine::FilePackageImp::GetFileData = \"%hs\" package file data block is empty", m_strPackFilePath.c_str()));
					bufFileData.Resize(0); uFileTimeStamp =0; return false;
				}
				m_lpPackFile->ReadData(pFileData+sFileDataBias, sTempDataSize); sFileDataBias +=sTempDataSize;
				sCurLinkPos =cTempLink.m_sNextPos;
			}
			return true;
		}
	}
	return false;
}


//��ȡ���������ļ���������Ϣ
//bufFileAttrs�����������ļ���������Ϣ����
//bufFileRelPaths�������ļ��ڰ��ڵĹ淶�����·���ַ������У������ַ��������������ַ�������˳�����У�ÿ�����������һ���ַ�Ϊ'\0'
//����ֵ��true->��ȡ�ɹ���false->��ȡʧ��
bool FilePackageImp::GetAllFileAttrs(cn::Buffer/*FilePackage::FILE_ATTR*/ &bufFileAttrs, cn::Buffer/*char*/ &bufFileRelPaths)
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	cn::s64 i; cn::u32 j;

	bufFileAttrs.Resize(0); bufFileRelPaths.Resize(0);
	std::vector<FILE_INFO> arrFileInfos; std::vector<cn::s64> arrOldFileInfoUnits;
	LoadAllFileInfos(arrFileInfos, arrOldFileInfoUnits); //���ذ���ԭ�е��ļ���ϸ��Ϣ
	bufFileAttrs.Resize(sizeof(FILE_ATTR)*m_cPackHeader.m_sFileNum); FILE_ATTR *lpFileAttrs =(FILE_ATTR *)(bufFileAttrs.GetData());
	cn::u32 uStrTotalLen =0;
	for(i =0; i<m_cPackHeader.m_sFileNum; i++){
		FILE_DIGEST_MEM &cMemFileDigest =m_arrMemFileDigests[i]; FILE_INFO &cCurFileInfo =arrFileInfos[i];
		lpFileAttrs[i].m_uRelPathMD5 =cMemFileDigest.m_cDiskInfo.m_uRelPathMD5;
		lpFileAttrs[i].m_uTimeStamp =cCurFileInfo.m_uTimeStamp;
		lpFileAttrs[i].m_sSize =cCurFileInfo.m_sSize;
		uStrTotalLen +=cCurFileInfo.m_strRelPath.length()+1;
	}
	bufFileRelPaths.Resize(sizeof(char)*uStrTotalLen); char *pFileRelPaths =(char *)(bufFileRelPaths.GetData());
	cn::u32 uCurStrBias =0;
	for(i =0; i<m_cPackHeader.m_sFileNum; i++){
		FILE_INFO &cCurFileInfo =arrFileInfos[i];
		const char *pRelPath =cCurFileInfo.m_strRelPath.c_str(); cn::u32 uRelPathLen =cCurFileInfo.m_strRelPath.length();
		for(j =0; j<=uRelPathLen; j++){pFileRelPaths[uCurStrBias++] =pRelPath[j];}
	}

	return true;
}


//������־��¼��ȡ��Ҫ����ɾ�����ļ���MD5��Ϣ
//uOldPackTimeStamp����ʾ���Ƚϵľ���Դ����ʱ���
//bufInvalidFiles�����ؾ���Դ����ʱ�������ǰ����ʱ���֮��Ӧ��Ϊ��Чɾ�����ļ��б�
//����ֵ��true->����Դ���ɴ�����ɾ����bufInvalidFilesָ����һϵ����Ч�ļ���false->��ȡʧ�ܣ�����Դ����Ҫɾ���������е�����
bool FilePackageImp::GetUpdateInvalidFiles(cn::u64 uOldPackTimeStamp, cn::Buffer/*cn::u128*/ &bufInvalidFiles)
{
	cn::AutoSync cFileAutoSync(m_lpFileSync); //������ʰ����������

	FILE_LOG_UNIT_HEADER cUnitHeader;
	FILE_LOG cTempLog;

	cn::s64 i;

	bufInvalidFiles.Resize(0);
	if(m_cPackHeader.m_uPackTimeStamp == uOldPackTimeStamp){return true;}
	if(m_cPackHeader.m_sFileLogNum <= 0){return false;}
	std::set<cn::u128> setDelFiles; std::vector<FILE_LOG> arrUnitFileLogs(m_sFileLogNumPerUnit); FILE_LOG *lpUnitFileLogs =&(arrUnitFileLogs[0]);

	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFileLogsBaseUnit*m_sUnitSize+sizeof(FILE_LOG_UNIT_HEADER)+sizeof(cn::s64));
	m_lpPackFile->ReadData(&cTempLog, sizeof(FILE_LOG));
	if(uOldPackTimeStamp < cTempLog.m_uTimeStamp){ //����ʱ���������־��¼��Χ�ڵĻ�
		return false;
	}
	bool bMeetLogTail =false;
	cn::s64 sCurUnit =m_cPackHeader.m_sFileLogEndPos/m_sUnitSize;
	for(; sCurUnit>0;){
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurUnit*m_sUnitSize);
		m_lpPackFile->ReadData(&cUnitHeader, sizeof(FILE_LOG_UNIT_HEADER));
		cn::s64 sCurSegSize =m_lpPackFile->ReadS64(); //��ȡ��ǰ��Ԫ��������ļ���־��Ϣ����
		m_lpPackFile->ReadData(lpUnitFileLogs, sizeof(FILE_LOG)*sCurSegSize);
		if(bMeetLogTail==false && sCurSegSize>0){
			bMeetLogTail =true;
			if(uOldPackTimeStamp >= lpUnitFileLogs[sCurSegSize-1].m_uTimeStamp){ //����ʱ���������־��¼��Χ�ڵĻ�
				return false;
			}
		}
		for(i =sCurSegSize-1; i>=0; i--){
			if(uOldPackTimeStamp >= lpUnitFileLogs[i].m_uTimeStamp){ //����־��¼�ѱȾ�ʱ������ϵĻ�
				goto _LOG_SEARCH_END;
			}else if(lpUnitFileLogs[i].m_uEvent == 2){ //����־��¼Ϊ�����Դ���Ļ������践����Ч�ļ��б�
				return false;
			}else if(lpUnitFileLogs[i].m_uEvent == 0){ //����־��¼Ϊɾ���ļ��Ļ�
				setDelFiles.insert(lpUnitFileLogs[i].m_uFileRelPathMD5);
			}
		}
		sCurUnit =cUnitHeader.m_sPreUnit;
	}	
_LOG_SEARCH_END:
	bufInvalidFiles.Resize(sizeof(cn::u128)*setDelFiles.size());
	cn::u128 *pInvalidFiles =(cn::u128 *)(bufInvalidFiles.GetData()); i =0;
	for(std::set<cn::u128>::iterator iter1 =setDelFiles.begin(); iter1!=setDelFiles.end(); iter1++){
		pInvalidFiles[i++] =*iter1;
	}
	return true;
}


//��ȡһ��MD5ֵ��m_arrFileDigestKeys�ж�Ӧ������λ��
//uRelNorPathMD5����ʾ�����ҵ�MD5ֵ
//sKeyLocNum����ʾm_arrFileDigestKeys����uRelNorPathMD5��ȵĹؼ��ָ���
//����ֵ����uRelNorPathMD5����ȵĹؼ��ֵĻ��򷵻���ȹؼ��ֵ���ʼ��������uRelNorPathMD5û����ȹؼ��ֵĻ��򷵻�Ҫ���������λ��
cn::s64 FilePackageImp::GetDigestKeyLocs(u128 uRelNorPathMD5, cn::s64 &sKeyLocNum)
{
	cn::s64 i, j, sMid;

	sKeyLocNum =0;
	if(m_cPackHeader.m_sFileNum <= 0){return 0;}
	FILE_DIGEST_KEY *lpKeys =&(m_arrFileDigestKeys[0]);
	for(i =0, j =m_cPackHeader.m_sFileNum-1; i<=j;){
		sMid =(i+j)/2;
		if(lpKeys[sMid].m_uRelPathMD5 == uRelNorPathMD5){
			break;
		}else if(lpKeys[sMid].m_uRelPathMD5 < uRelNorPathMD5){
			i =sMid+1;
		}else{
			j =sMid-1;
		}
	}
	if(i <= j){ //���ҵ���ȵĹؼ��ֵĻ�
		for(j =sMid+1; j<m_cPackHeader.m_sFileNum; j++){if(lpKeys[j].m_uRelPathMD5 != uRelNorPathMD5){break;} sKeyLocNum++;}
		for(i =sMid; i>=0; i--){if(lpKeys[i].m_uRelPathMD5 != uRelNorPathMD5){break;} sKeyLocNum++;}
		return i+1;
	}else{ //��δ�ҵ���ȵĹؼ��ֵĻ�
		return i;
	}
}


//��һ����Ԫ�������п�����
void FilePackageImp::AddAFreeUnitID(cn::s64 sUnitID)
{
	UNIT_HEADER cUnitHeader;

	cUnitHeader.m_sCurUnitValidSize =0; cUnitHeader.m_sNextUnit =m_cPackHeader.m_sFreeUnitsBaseUnit;
	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sUnitID*m_sUnitSize); m_lpPackFile->WriteData(&cUnitHeader, sizeof(UNIT_HEADER));
	m_cPackHeader.m_sFreeUnitsBaseUnit =sUnitID; m_cPackHeader.m_sFreeUnitNum++;
}


//��ȡһ�����õĿ��е�Ԫ��ID
cn::s64 FilePackageImp::GetUsableFreeUnitID()
{
	UNIT_HEADER cUnitHeader;

	cn::s64 sRet;
	if(m_cPackHeader.m_sFreeUnitsBaseUnit > 0){ //���п��õĿ��п�Ļ�
		sRet =m_cPackHeader.m_sFreeUnitsBaseUnit;	
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFreeUnitsBaseUnit*m_sUnitSize); m_lpPackFile->ReadData(&cUnitHeader, sizeof(UNIT_HEADER));
		m_cPackHeader.m_sFreeUnitsBaseUnit =cUnitHeader.m_sNextUnit; m_cPackHeader.m_sFreeUnitNum--;
	}else{ //��û�п��õĿ��п�Ļ�����Ӱ��ļ�ĩβ�����¿ռ�
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_END, 0);
		sRet =m_lpPackFile->GetLength()/m_sUnitSize;
		m_lpPackFile->WriteData(&(m_arrEmptyUnit[0]), m_sUnitSize);
	}
	return sRet;
}


//��һ��ժҪ���ݿ�������ժҪ������
void FilePackageImp::AddAFreeDigestPos(cn::s64 sDigestPos)
{
	FILE_DIGEST cTempDigest;

	cTempDigest.m_cDataLinkHead.m_sPrePos =-1; cTempDigest.m_cDataLinkHead.m_sNextPos =m_cPackHeader.m_sFreeDigestsBasePos;
	cTempDigest.m_uRelPathMD5 =cn::u128::m_uZero; cTempDigest.m_sFileInfoPos =-1;
	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sDigestPos); m_lpPackFile->WriteData(&cTempDigest, sizeof(FILE_DIGEST));
	m_cPackHeader.m_sFreeDigestsBasePos =sDigestPos; m_cPackHeader.m_sFreeDigestNum++;
}


//��ȡһ�����õĿ���ժҪ��İ���ַ
cn::s64 FilePackageImp::GetUsableFreeDigestPos()
{
	FILE_DIGEST cTempDigest;

	cn::s64 sRet;
	if(m_cPackHeader.m_sFreeDigestsBasePos > 0){ //���п��õĿ���ժҪ��Ļ�
		sRet =m_cPackHeader.m_sFreeDigestsBasePos;
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFreeDigestsBasePos); m_lpPackFile->ReadData(&cTempDigest, sizeof(FILE_DIGEST));
		m_cPackHeader.m_sFreeDigestsBasePos =cTempDigest.m_cDataLinkHead.m_sNextPos; m_cPackHeader.m_sFreeDigestNum--;
	}else{ //��û�п��õĿ���ժҪ��Ļ�
		cn::s64 sCurUnit =m_cPackHeader.m_sDigestEndPos/m_sUnitSize;
		UNIT_HEADER cCurUnitHeader; cn::s64 sCurUnitCap; 
		if(m_cPackHeader.m_sDigestEndPos+sizeof(FILE_DIGEST) < (sCurUnit+1)*m_sUnitSize){ //����ǰ��Ԫ�黹�ɿ���ժҪ��Ϣ�ռ�Ļ�
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurUnit*m_sUnitSize); m_lpPackFile->ReadData(&cCurUnitHeader, sizeof(UNIT_HEADER));
			if(sCurUnit == 0){m_lpPackFile->Seek(DiskIO::SEEK_ORIG_CUR, sizeof(PACK_HEADER));} sCurUnitCap =m_lpPackFile->ReadS64();
		}else{ //����ǰ��Ԫ��ռ䲻��Ļ�
			cn::s64 sNewUnit =GetUsableFreeUnitID();
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurUnit*m_sUnitSize); m_lpPackFile->ReadData(&cCurUnitHeader, sizeof(UNIT_HEADER));
			cCurUnitHeader.m_sNextUnit =sNewUnit;
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurUnit*m_sUnitSize); m_lpPackFile->WriteData(&cCurUnitHeader, sizeof(UNIT_HEADER));
			sCurUnit =sNewUnit; cCurUnitHeader.m_sCurUnitValidSize =sizeof(UNIT_HEADER)+sizeof(cn::s64); cCurUnitHeader.m_sNextUnit =-1; sCurUnitCap =0;
			m_cPackHeader.m_sDigestEndPos =sCurUnit*m_sUnitSize+cCurUnitHeader.m_sCurUnitValidSize-1;
		}
		sRet =m_cPackHeader.m_sDigestEndPos+1;
		cCurUnitHeader.m_sCurUnitValidSize +=sizeof(FILE_DIGEST); sCurUnitCap++;
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurUnit*m_sUnitSize); m_lpPackFile->WriteData(&cCurUnitHeader, sizeof(UNIT_HEADER));
		if(sCurUnit == 0){m_lpPackFile->Seek(DiskIO::SEEK_ORIG_CUR, sizeof(PACK_HEADER));} m_lpPackFile->WriteS64(sCurUnitCap);
		m_cPackHeader.m_sDigestEndPos +=sizeof(FILE_DIGEST);
	}
	return sRet;
}


//��������һϵ���ļ�������־
void FilePackageImp::AddFileLogs(cn::s64 sLogNum, const FILE_LOG *lpLogs)
{
	FILE_LOG_UNIT_HEADER cTempUnitHeader;

	if(m_cPackHeader.m_sMaxFileLogUnitNum<=0 || sLogNum<=0){
		return;
	}

	cn::s64 sLogUnit; cn::s64 sLogUnitCap; FILE_LOG_UNIT_HEADER cLogUnitHeader;
	if(m_cPackHeader.m_sFileLogEndPos > 0){ //������֮ǰ����־���ݵĻ�
		sLogUnit =m_cPackHeader.m_sFileLogEndPos/m_sUnitSize;
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sLogUnit*m_sUnitSize); 
		m_lpPackFile->ReadData(&cLogUnitHeader, sizeof(FILE_LOG_UNIT_HEADER)); sLogUnitCap =m_lpPackFile->ReadS64();
		if(sLogUnitCap >= m_sFileLogNumPerUnit){ //����ǰ��Ԫ���ʣ��ռ䲻���洢���ļ���־�Ļ����򿪱�һ���µĵ�Ԫ��
			if(m_cPackHeader.m_sFileLogNum/m_sFileLogNumPerUnit >= m_cPackHeader.m_sMaxFileLogUnitNum+1){
				cLogUnitHeader.m_sNextUnit =m_cPackHeader.m_sFileLogsBaseUnit;
				m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFileLogsBaseUnit*m_sUnitSize); m_lpPackFile->ReadData(&cTempUnitHeader, sizeof(FILE_LOG_UNIT_HEADER));
				m_cPackHeader.m_sFileLogsBaseUnit =cTempUnitHeader.m_sNextUnit;
				if(m_cPackHeader.m_sFileLogsBaseUnit != sLogUnit){
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFileLogsBaseUnit*m_sUnitSize); m_lpPackFile->ReadData(&cTempUnitHeader, sizeof(FILE_LOG_UNIT_HEADER));
					cTempUnitHeader.m_sPreUnit =-1;
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFileLogsBaseUnit*m_sUnitSize); m_lpPackFile->WriteData(&cTempUnitHeader, sizeof(FILE_LOG_UNIT_HEADER));
				}else{
					cLogUnitHeader.m_sPreUnit =-1;
				}
				m_cPackHeader.m_sFileLogNum -=m_sFileLogNumPerUnit;
			}else{
				cLogUnitHeader.m_sNextUnit =GetUsableFreeUnitID();
			}
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sLogUnit*m_sUnitSize); m_lpPackFile->WriteData(&cLogUnitHeader, sizeof(FILE_LOG_UNIT_HEADER));
			cLogUnitHeader.m_sCurUnitValidSize =sizeof(FILE_LOG_UNIT_HEADER)+sizeof(cn::s64); 
			cLogUnitHeader.m_sPreUnit =sLogUnit; sLogUnit =cLogUnitHeader.m_sNextUnit; cLogUnitHeader.m_sNextUnit =-1; sLogUnitCap =0;
			m_cPackHeader.m_sFileLogEndPos =sLogUnit*m_sUnitSize+cLogUnitHeader.m_sCurUnitValidSize-1;
		}
	}else{ //������֮ǰû����־���ݵĻ�
		sLogUnit =GetUsableFreeUnitID();
		cLogUnitHeader.m_sCurUnitValidSize =sizeof(FILE_LOG_UNIT_HEADER)+sizeof(cn::s64); cLogUnitHeader.m_sPreUnit =-1; cLogUnitHeader.m_sNextUnit =-1; sLogUnitCap =0;
		m_cPackHeader.m_sFileLogsBaseUnit =sLogUnit; m_cPackHeader.m_sFileLogEndPos =sLogUnit*m_sUnitSize+cLogUnitHeader.m_sCurUnitValidSize-1;
	}
	cn::s64 sNewLogUnit;
	for(cn::s64 sLogBias =0; sLogBias<sLogNum;){
		cn::s64 sCurLogLen =m_sFileLogNumPerUnit-sLogUnitCap; sCurLogLen =min(sCurLogLen, sLogNum-sLogBias);
		if(sLogBias+sCurLogLen < sLogNum){ //�����ļ���־�����ŵ�������Ԫ��Ļ�
			if((m_cPackHeader.m_sFileLogNum+sCurLogLen)/m_sFileLogNumPerUnit >= m_cPackHeader.m_sMaxFileLogUnitNum+1){
				sNewLogUnit =m_cPackHeader.m_sFileLogsBaseUnit;
				m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFileLogsBaseUnit*m_sUnitSize); m_lpPackFile->ReadData(&cTempUnitHeader, sizeof(FILE_LOG_UNIT_HEADER));
				m_cPackHeader.m_sFileLogsBaseUnit =cTempUnitHeader.m_sNextUnit;
				if(m_cPackHeader.m_sFileLogsBaseUnit != sLogUnit){
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFileLogsBaseUnit*m_sUnitSize); m_lpPackFile->ReadData(&cTempUnitHeader, sizeof(FILE_LOG_UNIT_HEADER));
					cTempUnitHeader.m_sPreUnit =-1;
					m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFileLogsBaseUnit*m_sUnitSize); m_lpPackFile->WriteData(&cTempUnitHeader, sizeof(FILE_LOG_UNIT_HEADER));
				}else{
					cLogUnitHeader.m_sPreUnit =-1;
				}
				m_cPackHeader.m_sFileLogNum -=m_sFileLogNumPerUnit;
			}else{
				sNewLogUnit =GetUsableFreeUnitID();
			}
		}else{
			sNewLogUnit =-1;
		}
		cLogUnitHeader.m_sCurUnitValidSize +=sizeof(FILE_LOG)*sCurLogLen; cLogUnitHeader.m_sNextUnit =sNewLogUnit; sLogUnitCap +=sCurLogLen;
		//д�뵱ǰ��Ԫ������
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sLogUnit*m_sUnitSize);
		m_lpPackFile->WriteData(&cLogUnitHeader, sizeof(FILE_LOG_UNIT_HEADER));
		m_lpPackFile->WriteS64(sLogUnitCap);
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, m_cPackHeader.m_sFileLogEndPos+1);
		m_lpPackFile->WriteData(lpLogs+sLogBias, sizeof(FILE_LOG)*sCurLogLen);
		//��ʼ����һ����Ԫ�����Ϣ
		sLogBias +=sCurLogLen; m_cPackHeader.m_sFileLogNum +=sCurLogLen;
		if(sLogBias < sLogNum){ //�����ļ����ݻ����ŵ������ֿ�Ļ�
			cLogUnitHeader.m_sCurUnitValidSize =sizeof(FILE_LOG_UNIT_HEADER)+sizeof(cn::s64); cLogUnitHeader.m_sPreUnit =sLogUnit; sLogUnit =sNewLogUnit; cLogUnitHeader.m_sNextUnit =-1; sLogUnitCap =0;
			m_cPackHeader.m_sFileLogEndPos =sLogUnit*m_sUnitSize+cLogUnitHeader.m_sCurUnitValidSize-1;
		}else{ //����ǰ�ֿ��������ļ������һ���ֿ�Ļ�
			m_cPackHeader.m_sFileLogEndPos +=sizeof(FILE_LOG)*sCurLogLen;
		}
	}
}


//��ȡ�����ϵ������ļ���־��Ϣ���ڴ���
//arrFileLogs����ʾ�洢�ļ���־��Ϣ������
void FilePackageImp::LoadAllFileLogs(std::vector<FILE_LOG> &arrFileLogs)
{
	FILE_LOG_UNIT_HEADER cUnitHeader;

	arrFileLogs.clear(); arrFileLogs.resize(m_cPackHeader.m_sFileLogNum);
	if(m_cPackHeader.m_sFileLogNum <= 0){return;}
	cn::s64 sCurFileLog =0; cn::s64 sCurUnit =m_cPackHeader.m_sFileLogsBaseUnit;
	for(; sCurFileLog<m_cPackHeader.m_sFileLogNum;){
		if(sCurUnit < 0){
			cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
				"RealEngine::FilePackageImp::LoadAllFileLogs = \"%hs\" package unit link invalid", m_strPackFilePath.c_str()));
			throw 1;
		}
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurUnit*m_sUnitSize);
		m_lpPackFile->ReadData(&cUnitHeader, sizeof(FILE_LOG_UNIT_HEADER));
		cn::s64 sCurSegSize =m_lpPackFile->ReadS64(); //��ȡ��ǰ��Ԫ��������ļ���־��Ϣ����
		cn::s64 sReadNum =min(sCurSegSize, m_cPackHeader.m_sFileLogNum-sCurFileLog); //��ȡ��ǰ��Ԫ��Ҫ��ȡ���ļ���־��Ϣ����
		m_lpPackFile->ReadData(&(arrFileLogs[sCurFileLog]), sizeof(FILE_LOG)*sReadNum);
		sCurFileLog +=sReadNum; sCurUnit =cUnitHeader.m_sNextUnit;
	}
}


//��ȡ�����ϵ������ļ�ժҪ��Ϣ���ڴ���
void FilePackageImp::LoadAllFileDigests()
{
	cn::s64 i;
	UNIT_HEADER cUnitHeader;

	m_arrMemFileDigests.clear(); m_arrMemFileDigests.resize(m_cPackHeader.m_sFileNum);
	m_arrFileDigestKeys.clear(); m_arrFileDigestKeys.resize(m_cPackHeader.m_sFileNum);
	if(m_cPackHeader.m_sFileNum <= 0){return;}
	std::vector<FILE_DIGEST> arrTempBuf(m_sUnitSize/sizeof(FILE_DIGEST)); FILE_DIGEST *lpTempBuf =&(arrTempBuf[0]);
	cn::s64 sCurUnit =0;
	for(; sCurUnit>=0;){
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurUnit*m_sUnitSize); 
		m_lpPackFile->ReadData(&cUnitHeader, sizeof(UNIT_HEADER));
		if(sCurUnit == 0){m_lpPackFile->Seek(DiskIO::SEEK_ORIG_CUR, sizeof(PACK_HEADER));}
		cn::s64 sCurSegSize =m_lpPackFile->ReadS64(); //��ȡ��ǰ��Ԫ��������ļ�ժҪ��Ϣ����
		m_lpPackFile->ReadData(lpTempBuf, sizeof(FILE_DIGEST)*sCurSegSize); //��ȡ��ǰ��Ԫ����ļ�ժҪ��Ϣ
		cn::s64 sCurPos =(sCurUnit == 0) ? sizeof(UNIT_HEADER)+sizeof(PACK_HEADER)+sizeof(cn::s64) : sCurUnit*m_sUnitSize+sizeof(UNIT_HEADER)+sizeof(cn::s64);
		for(i =0; i<sCurSegSize; i++, sCurPos +=sizeof(FILE_DIGEST)){
			if(lpTempBuf[i].m_sFileInfoPos > 0){
				FILE_DIGEST_MEM &cMemDigest =m_arrMemFileDigests[-lpTempBuf[i].m_cDataLinkHead.m_sPrePos];
				cMemDigest.m_cDiskInfo =lpTempBuf[i]; cMemDigest.m_sDiskInfoPos =sCurPos;
			}
		}
		sCurUnit =cUnitHeader.m_sNextUnit;
	}
	for(i =0; i<m_cPackHeader.m_sFileNum; i++){
		FILE_DIGEST_KEY &cKey =m_arrFileDigestKeys[i];
		cKey.m_uRelPathMD5 =m_arrMemFileDigests[i].m_cDiskInfo.m_uRelPathMD5; cKey.m_uDigestID =cn::u32(i);
	}
	std::sort(m_arrFileDigestKeys.begin(), m_arrFileDigestKeys.begin()+m_cPackHeader.m_sFileNum, FileDigestKeyCmp);
}


//��ȡ�����ϵ������ļ���ϸ��Ϣ���ڴ���
//arrFileInfos����ʾ�洢�ļ���ϸ��Ϣ������
//arrFileInfoUnits�����ش洢�ļ���ϸ��Ϣ�ĵ�Ԫ���б�
void FilePackageImp::LoadAllFileInfos(std::vector<FILE_INFO> &arrFileInfos, std::vector<cn::s64> &arrFileInfoUnits)
{
	cn::s64 i;
	UNIT_HEADER cUnitHeader;

	arrFileInfos.clear(); arrFileInfos.resize(m_cPackHeader.m_sFileNum);
	arrFileInfoUnits.clear();
	if(m_cPackHeader.m_sFileNum <= 0){return;}
	cn::s64 sCurFile =0; cn::s64 sCurUnit =m_cPackHeader.m_sFileInfosBaseUnit;
	for(; sCurFile<m_cPackHeader.m_sFileNum;){
		if(sCurUnit < 0){
			cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
				"RealEngine::FilePackageImp::LoadAllFileInfos = \"%hs\" package unit link invalid", m_strPackFilePath.c_str()));
			throw 1;
		}
		arrFileInfoUnits.push_back(sCurUnit);
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurUnit*m_sUnitSize);
		m_lpPackFile->ReadData(&cUnitHeader, sizeof(UNIT_HEADER));
		cn::s64 sCurSegSize =m_lpPackFile->ReadS64(); //��ȡ��ǰ��Ԫ��������ļ���ϸ��Ϣ����
		cn::s64 sReadNum =min(sCurSegSize, m_cPackHeader.m_sFileNum-sCurFile); //��ȡ��ǰ��Ԫ��Ҫ��ȡ���ļ���ϸ��Ϣ����
		for(i =0; i<sReadNum; i++){
			FILE_INFO &cFileInfo =arrFileInfos[sCurFile+i];
			cn::u32 uStrLen =m_lpPackFile->ReadU32(); cFileInfo.m_strRelPath.resize(uStrLen);
			if(uStrLen > 0){m_lpPackFile->ReadData(&(cFileInfo.m_strRelPath[0]), sizeof(char)*uStrLen);}
			cFileInfo.m_uTimeStamp =m_lpPackFile->ReadU64();
			cFileInfo.m_sSize =m_lpPackFile->ReadS64();
		}
		sCurFile +=sReadNum; sCurUnit =cUnitHeader.m_sNextUnit;
	}
}


//���ڴ��е������ļ�ժҪ��Ϣ����ϸ��Ϣ���������
//arrFileInfos����ʾ�ڴ����µ��ļ���ϸ��Ϣ
//arrOldFileInfoUnits����ʾ������ԭ�е����ļ���ϸ��Ϣ��Ӧ�ĵ�Ԫ���б�
void FilePackageImp::SaveAllFileDigestAndInfos(const std::vector<FILE_INFO> &arrFileInfos, const std::vector<cn::s64> &arrOldFileInfoUnits)
{
	cn::s64 i;
	UNIT_HEADER cUnitHeader;
	
	if(m_cPackHeader.m_sFileNum!=m_arrMemFileDigests.size() || m_arrMemFileDigests.size()!=arrFileInfos.size()){
		cn::IEventReceiver::GetSingleObj()->ReceiveEvent(cn::IEventReceiver::EVENT_TYPE_ERROR, cn::Literal::GetFormattedStr(
			"RealEngine::FilePackageImp::SaveAllFileDigestAndInfos = \"%hs\" file digest and info num doesnt equal", m_strPackFilePath.c_str()));
		throw 1;
	}

	//���ڴ��е����ļ���ϸ��Ϣд�����
	cn::s64 sOldFileInfoUnitLoc =0; //��ʾ��ǰ��ʹ�õ��ϵ�Ԫ����������ƫ��
	m_cPackHeader.m_sFileInfosBaseUnit =-1; m_cPackHeader.m_sFileInfoEndPos =-1;
	if(m_cPackHeader.m_sFileNum > 0){ //�����ļ���ϸ��Ϣ��Ϊ�յĻ�
		cn::s64 sCurFile =0; cn::s64 sCurUnit =(arrOldFileInfoUnits.size() > 0) ? arrOldFileInfoUnits[sOldFileInfoUnitLoc++] : GetUsableFreeUnitID(); 
		for(; sCurFile<m_cPackHeader.m_sFileNum;){
			cn::s64 sCurPos =sCurUnit*m_sUnitSize+sizeof(UNIT_HEADER)+sizeof(cn::s64); cn::s64 sMaxPos =(sCurUnit+1)*m_sUnitSize;
			if(sCurFile == 0){m_cPackHeader.m_sFileInfosBaseUnit =sCurUnit;}
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurPos);
			cn::s64 sWriteNum =0; //��ʾ��ǰ��Ԫ���ڿ�д����ļ���ϸ��Ϣ����
			for(; sCurFile<m_cPackHeader.m_sFileNum; sCurFile++){
				const FILE_INFO &cFileInfo =arrFileInfos[sCurFile]; cn::s64 sFileInfoSize =sizeof(cn::u32)+sizeof(char)*cFileInfo.m_strRelPath.length()+sizeof(cn::u64)+sizeof(cn::s64);
				if(sCurPos+sFileInfoSize > sMaxPos){ //����ǰ�ļ���ϸ��Ϣ�ѳ�����Ԫ��ĩβ�Ļ�
					break;
				}
				m_lpPackFile->WriteU32(cFileInfo.m_strRelPath.length());
				m_lpPackFile->WriteData(cFileInfo.m_strRelPath.data(), sizeof(char)*cFileInfo.m_strRelPath.length());
				m_lpPackFile->WriteU64(cFileInfo.m_uTimeStamp);
				m_lpPackFile->WriteS64(cFileInfo.m_sSize);
				m_arrMemFileDigests[sCurFile].m_cDiskInfo.m_sFileInfoPos =sCurPos; 
				sCurPos +=sFileInfoSize; sWriteNum++;
			}
			if(sCurFile < m_cPackHeader.m_sFileNum){ //������Ҫ�µ�Ԫ��Ļ�
				if(sOldFileInfoUnitLoc < arrOldFileInfoUnits.size()){ //������ϸ��Ϣ���е�Ԫ����õĻ�
					cUnitHeader.m_sNextUnit =arrOldFileInfoUnits[sOldFileInfoUnitLoc++];
				}else{ //������ϸ��Ϣû�е�Ԫ����ã���Ӱ����пռ��л�ȡ��Ԫ��
					cUnitHeader.m_sNextUnit =GetUsableFreeUnitID();
				}
			}else{ //����ϸ��Ϣ�Ѵ�����Ļ�
				cUnitHeader.m_sNextUnit =-1; m_cPackHeader.m_sFileInfoEndPos =sCurPos-1;
			}
			cUnitHeader.m_sCurUnitValidSize =sCurPos-sCurUnit*m_sUnitSize;
			m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, sCurUnit*m_sUnitSize); 
			m_lpPackFile->WriteData(&cUnitHeader, sizeof(UNIT_HEADER)); m_lpPackFile->WriteS64(sWriteNum);
			sCurUnit =cUnitHeader.m_sNextUnit;
		}
	}

	//������ļ���ϸ��Ϣʣ��ĵ�Ԫ��
	for(; sOldFileInfoUnitLoc < arrOldFileInfoUnits.size(); sOldFileInfoUnitLoc++){
		AddAFreeUnitID(arrOldFileInfoUnits[sOldFileInfoUnitLoc]); //�����ļ���ϸ��Ϣ���ϵ�Ԫ�������п�������
	}


	//��ȡ���������ļ�ժҪ��Ӧ�ĵ�Ԫ����Ϣ
	m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, 0); m_lpPackFile->ReadData(&cUnitHeader, sizeof(UNIT_HEADER));
	cn::s64 sOldDigestHeadUnit =cUnitHeader.m_sNextUnit; //��ȡ��ժҪ��Ϣ�ĵڶ�����Ԫ��
	//cn::s64 sOldDigestTailUnit =(m_cPackHeader.m_sFileDigestEndPos > 0) ? m_cPackHeader.m_sFileDigestEndPos/m_sUnitSize : -1; //��ȡ��ժҪ��Ϣ��β��Ԫ��

	//���ڴ��е����ļ�ժҪ��Ϣд�����
	std::vector<FILE_DIGEST_KEY_2> arrFileDigestKeys2(m_cPackHeader.m_sFileNum);
	for(i =0; i<m_cPackHeader.m_sFileNum; i++){
		FILE_DIGEST_KEY_2 &cKey =arrFileDigestKeys2[i];
		cKey.m_sDiskInfoPos =m_arrMemFileDigests[i].m_sDiskInfoPos; cKey.m_uDigestID =cn::u32(i);
	}
	std::sort(arrFileDigestKeys2.begin(), arrFileDigestKeys2.begin()+m_cPackHeader.m_sFileNum, FileDigestKeyCmp2);
	for(i =0; i<m_cPackHeader.m_sFileNum; i++){
		FILE_DIGEST_MEM &cMemDigest =m_arrMemFileDigests[arrFileDigestKeys2[i].m_uDigestID];
		m_lpPackFile->Seek(DiskIO::SEEK_ORIG_BEGIN, cMemDigest.m_sDiskInfoPos);
		m_lpPackFile->WriteData(&(cMemDigest.m_cDiskInfo), sizeof(FILE_DIGEST));
	}
}


//���캯��
EngineResServerTest::EngineResServerTest()
{
	m_lpServerSync =new cn::SyncObj();

}


//��������
EngineResServerTest::~EngineResServerTest()
{
	SAFE_DELETE(m_lpServerSync);
}


//������Դ��ĸ�Ŀ¼
void EngineResServerTest::SetResGroupRootDir(const char *pName, const char *pRootDir)
{
	AutoSync cAutoSync(m_lpServerSync); //������ʷ���������

	std::string strName =cn::Literal::ToLower(pName);
	m_mapResGroups[strName].m_strRootDir =cn::Literal::ToLower(pRootDir);
}


//��ȡ��Դ��ĸ�Ŀ¼
const char *EngineResServerTest::GetResGroupRootDir(const char *pName)
{
	AutoSync cAutoSync(m_lpServerSync); //������ʷ���������

	std::string strName =cn::Literal::ToLower(pName);
	std::map<std::string, RES_GROUP>::iterator iter1 =m_mapResGroups.find(strName);
	if(iter1 != m_mapResGroups.end()){
		return iter1->second.m_strRootDir.c_str();
	}else{
		return "";
	}
}


//�Է�����������Դ����
//pURL����ʾ�����URL����
//bufResData������������Դ����
//����ֵ��������Դ�����Ƿ��ȡ�ɹ�
bool EngineResServerTest::RequestEngineRes(const char *pURL, cn::Buffer &bufResData)
{
	if(pURL == NULL){
		return false;
	}

	DLL_TLS_DATA *lpDLLTLSData =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj();
	char *pTempBuf =lpDLLTLSData->m_pURLTempBuf;
	char *pTempBuf2 =lpDLLTLSData->m_pURLTempBuf2;
	cn::u32 i;

	const char *pParamVal[7] ={NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	cn::u32 uState =0; cn::u32 uPtr0 =0; cn::u32 uPtr1 =0;
	for(i =0; pURL[i]!='\0'; i++){
		char uCurChar =pURL[i];
		switch(uState)
		{
		case 0:
			if(uCurChar == '?'){uState =1;}
			break;
		case 1:
			if(uCurChar == '='){
				pTempBuf[uPtr0] ='\0';
				if(::strcmp(pTempBuf, "dir") == 0){uState =2; pParamVal[0] =pTempBuf2+uPtr1;}
				else if(::strcmp(pTempBuf, "path") == 0){uState =2; pParamVal[1] =pTempBuf2+uPtr1;}
				else if(::strcmp(pTempBuf, "filetimel") == 0){uState =2; pParamVal[2] =pTempBuf2+uPtr1;}
				else if(::strcmp(pTempBuf, "filetimeh") == 0){uState =2; pParamVal[3] =pTempBuf2+uPtr1;}
				else if(::strcmp(pTempBuf, "packpath") == 0){uState =2; pParamVal[4] =pTempBuf2+uPtr1;}
				else if(::strcmp(pTempBuf, "packtimel") == 0){uState =2; pParamVal[5] =pTempBuf2+uPtr1;}
				else if(::strcmp(pTempBuf, "packtimeh") == 0){uState =2; pParamVal[6] =pTempBuf2+uPtr1;}
				uPtr0 =0;
			}else{pTempBuf[uPtr0++] =uCurChar;}
			break;
		case 2:
			if(uCurChar=='&' || uCurChar=='#'){
				pTempBuf2[uPtr1++] ='\0'; uState =1;
			}else if(i<_MAX_STR_LEN){
				pTempBuf2[uPtr1++] =uCurChar;
			}else{
				pTempBuf2[uPtr1++] ='\0'; goto _SEARTH_END;
			}
			break;
		default:
			break;
		}
	}
_SEARTH_END:
	pTempBuf2[uPtr1++] ='\0';
	cn::u32 uFileTimeL =cn::Literal::ToU32(pParamVal[2]); cn::u32 uFileTimeH =cn::Literal::ToU32(pParamVal[3]);
	cn::u32 uPackTimeL =cn::Literal::ToU32(pParamVal[5]); cn::u32 uPackTimeH =cn::Literal::ToU32(pParamVal[6]);
	return RequestEngineResInternal(pParamVal[0], pParamVal[1], uFileTimeL, uFileTimeH, pParamVal[4], uPackTimeL, uPackTimeH, bufResData);
	return true;
}


//�Է�����������Դ����(�ڲ��汾)
//pResGroupName����ʾ��Դ���ʾ��
//pFilePath����ʾ�ļ�·����Ϣ
//uFileTimeL��uFileTimeH����ʾ�ļ�ʱ��ĵ�32λ�͸�32λ��Ϣ
//pPackPath����ʾ��Դ��·����Ϣ
//uPackTimeL��uPackTimeH����ʾ��Դ��ʱ��ĵ�32λ�͸�32λ��Ϣ
//bufResData������������Դ����
//����ֵ��������Դ�����Ƿ��ȡ�ɹ�
bool EngineResServerTest::RequestEngineResInternal(const char *pResGroupName, const char *pFilePath, cn::u32 uFileTimeL, cn::u32 uFileTimeH, 
	const char *pPackPath, cn::u32 uPackTimeL, cn::u32 uPackTimeH, cn::Buffer &bufResData)
{
	AutoSync cAutoSync(m_lpServerSync); //������ʷ���������

	cn::u32 i, j;

	bufResData.Resize(0);
	std::string strResGroupName =cn::Literal::ToLower(pResGroupName);
	std::string strOrigFilePath =cn::Literal::ToLower(pFilePath);
	if(strResGroupName.length()==0 || strOrigFilePath.length()==0){
		return false;
	}
	std::map<std::string, RES_GROUP>::iterator iter1 =m_mapResGroups.find(strResGroupName);
	if(iter1 == m_mapResGroups.end()){
		return false;
	}
	RES_GROUP &cResGroup =iter1->second;
	cn::u64 uOrigFileTime =(cn::u64(uFileTimeL)<<0)|(cn::u64(uFileTimeH)<<32);
	std::string strOrigPackPath =cn::Literal::ToLower(pPackPath);
	if(strOrigPackPath == "unknown"){strOrigPackPath ="";}
	cn::u64 uOrigPackTime =(cn::u64(uPackTimeL)<<0)|(cn::u64(uPackTimeH)<<32);

	cn::u32 uResDataLen =0; //��ʾ��Դ���ݵ��ܴ�С
	std::string strFileFullPath =cResGroup.m_strRootDir+strOrigFilePath;
	cn::u32 uDirLevNum; const char *pFileName;
	const char **ppHierarchy =cn::DiskIO::GetPathHierarchy(strOrigFilePath.c_str(), uDirLevNum, pFileName);
	std::vector<std::string> arrPackPaths(uDirLevNum);
	std::string strTemp =""; for(i =0; i<uDirLevNum; i++){strTemp +=ppHierarchy[i]; arrPackPaths[i] =strTemp+".pak"; strTemp +="\\";}
	FilePackage *lpPackage =NULL; //��ʾ��Դ������
	std::string strPackPath; //��ʾ��Դ���ľֲ�·��
	std::string strPackFullPath; //��ʾ��Դ��������·��
	std::string strInPackRelPath; //��ʾ�ļ�����Դ���ڵ����·��
	if(uDirLevNum > 0){ //��Ŀ���ļ�Ϊ�༶Ŀ¼�ļ��Ļ�
		for(i =0; i<uDirLevNum; i++){ //������Ŀ¼�������Դ���Ĵ�����
			strPackPath =arrPackPaths[i]; strPackFullPath =cResGroup.m_strRootDir+strPackPath;
			std::map<std::string, Ref/*FilePackage*/>::iterator iterFind =cResGroup.m_mapFilePackages.find(strPackFullPath);
			if(iterFind != cResGroup.m_mapFilePackages.end()){
				lpPackage =(FilePackage *)(iterFind->second.Get());
				if(lpPackage){strInPackRelPath =""; for(j =i+1; j<uDirLevNum; j++){strInPackRelPath +=ppHierarchy[j]; strInPackRelPath +="\\";} strInPackRelPath +=pFileName; break;}
			}else if(::PathFileExists(strPackFullPath.c_str())){ //���״μ�⵽����Դ���Ļ�
				for(j =0; j<i; j++){cResGroup.m_mapFilePackages[cResGroup.m_strRootDir+arrPackPaths[j]] =NULL;} //����Դ����ǰ׺·��ǿ����Ϊ��Ч��Դ��״̬
				cResGroup.m_mapFilePackages[strPackFullPath] =FilePackage::Create(strPackFullPath.c_str(), 1024*256, 100, true);
				lpPackage =(FilePackage *)(cResGroup.m_mapFilePackages[strPackFullPath].Get());
				if(lpPackage){ //����Դ����Ч�Ļ�
					strInPackRelPath =""; for(j =i+1; j<uDirLevNum; j++){strInPackRelPath +=ppHierarchy[j]; strInPackRelPath +="\\";} strInPackRelPath +=pFileName;
					break;
				}
			}
		}
	}
	if(lpPackage){ //����Դ�����ڵĻ�
		uResDataLen +=sizeof(cn::u32)+sizeof(char)*strPackPath.length()+sizeof(cn::u64)+sizeof(cn::u32);
		cn::u128 uInPackRelPathMD5 =cn::DiskIO::GetMD5FromBuffer(strInPackRelPath.data(), strInPackRelPath.length());
		cn::u64 uPackTime =lpPackage->GetTimeStamp();
		cn::Buffer bufFileData; cn::u64 uFileTime;
		if(lpPackage->GetFileData(uInPackRelPathMD5, strInPackRelPath.c_str(), bufFileData, uFileTime) == false){
			return false;
		}
		cn::u32 uDelFileNum; cn::Buffer bufDelFiles;
		if(uOrigPackTime != uPackTime){
			if(lpPackage->GetUpdateInvalidFiles(uOrigPackTime, bufDelFiles)){
				uDelFileNum =bufDelFiles.GetLen()/sizeof(cn::u128); uResDataLen +=sizeof(cn::u128)*uDelFileNum;
			}else{
				uDelFileNum =0xffffffff; uResDataLen +=sizeof(cn::u128)*0; //��տͻ��˵���Դ��
			}
		}else{
			uDelFileNum =0; //��ʾ����Ҫ���¿ͻ��˵���Դ��
		}
		uResDataLen +=sizeof(cn::u64)+sizeof(cn::u32);
		cn::u32 uFileDataLen;
		if(uOrigFileTime!=uFileTime || strOrigPackPath.length()==0 || uOrigPackTime!=uPackTime){
			uFileDataLen =bufFileData.GetLen();
			uResDataLen +=sizeof(cn::u8)*uFileDataLen;
		}else{
			uFileDataLen =0xffffffff; //��ʾ�ļ����ݲ���Ҫ����
		}
		
		bufResData.Resize(uResDataLen); cn::u8 *pCurData =(cn::u8 *)(bufResData.GetData());
		*((cn::u32 *)(pCurData)) =strPackPath.length(); pCurData +=sizeof(cn::u32);
		::memcpy(pCurData, strPackPath.data(), sizeof(char)*strPackPath.length()); pCurData +=sizeof(char)*strPackPath.length();
		*((cn::u64 *)(pCurData)) =uPackTime; pCurData +=sizeof(cn::u64);
		*((cn::u32 *)(pCurData)) =uDelFileNum; pCurData +=sizeof(cn::u32);
		if(uDelFileNum>0 && uDelFileNum<0xffffffff){::memcpy(pCurData, bufDelFiles.GetData(), sizeof(cn::u128)*uDelFileNum); pCurData +=sizeof(cn::u128)*uDelFileNum;}
		*((cn::u64 *)(pCurData)) =uFileTime; pCurData +=sizeof(cn::u64);
		*((cn::u32 *)(pCurData)) =uFileDataLen; pCurData +=sizeof(cn::u32);
		if(uFileDataLen>0 && uFileDataLen<0xffffffff){::memcpy(pCurData, bufFileData.GetData(), sizeof(cn::u8)*uFileDataLen); pCurData +=sizeof(cn::u8)*uFileDataLen;}
	
	}else{ //����Դ�������ڵĻ�
		try
		{
			cn::Ref/*cn::IDiskIO*/ refDiskIO =cn::DiskIO::Create(strFileFullPath.c_str(), "rb", cn::DiskIO::ZIP_MODE_NORMAL); //����Դ��Ӧ�Ĵ�������
			cn::DiskIO *lpCurIO =(cn::DiskIO *)(refDiskIO.Get());
			cn::Buffer bufFileData; cn::u64 uFileTime;
			cn::s64 sFileLen =lpCurIO->GetLength(); //��õ�ǰ�ļ������ֽ���
			WIN32_FILE_ATTRIBUTE_DATA cAttrData;
			if(::GetFileAttributesEx(strFileFullPath.c_str(), GetFileExInfoStandard, &cAttrData) == FALSE){
				throw 1;
			}
			uFileTime =(cn::u64(cAttrData.ftLastWriteTime.dwLowDateTime)<<0)|(cn::u64(cAttrData.ftLastWriteTime.dwHighDateTime)<<32); //��ȡ������Դ�ļ���ʱ���
			
			uResDataLen +=sizeof(cn::u32)+sizeof(cn::u64)+sizeof(cn::u32);
			cn::u32 uFileDataLen;
			if(uOrigFileTime!=uFileTime || strOrigPackPath.length()>0){
				uFileDataLen =sFileLen; uResDataLen +=sizeof(cn::u8)*uFileDataLen;
				bufFileData.Resize(uFileDataLen);
				if(uFileDataLen > 0){lpCurIO->Seek(cn::DiskIO::SEEK_ORIG_BEGIN, 0); lpCurIO->ReadData(bufFileData.GetData(), bufFileData.GetLen());}
			}else{
				uFileDataLen =0xffffffff; //��ʾ�ļ����ݲ���Ҫ����
			}
			
			bufResData.Resize(uResDataLen); cn::u8 *pCurData =(cn::u8 *)(bufResData.GetData());
			*((cn::u32 *)(pCurData)) =0; pCurData +=sizeof(cn::u32);
			*((cn::u64 *)(pCurData)) =uFileTime; pCurData +=sizeof(cn::u64);
			*((cn::u32 *)(pCurData)) =uFileDataLen; pCurData +=sizeof(cn::u32);
			if(uFileDataLen>0 && uFileDataLen<0xffffffff){::memcpy(pCurData, bufFileData.GetData(), sizeof(cn::u8)*uFileDataLen); pCurData +=sizeof(cn::u8)*uFileDataLen;}
		}
		catch(...)
		{
			return false;
		}
	}
	return true;
}




}