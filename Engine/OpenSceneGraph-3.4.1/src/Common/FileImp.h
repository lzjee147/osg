#ifndef _FILEIMP_H
#define _FILEIMP_H


/*********************************************
*	定义单个文件压缩访问机制
*
*
**********************************************/
#include "../ZlibWrap/zlibwrap.h"


namespace cn
{



//表示文件访问模块的实现类
class FileIOImp : public FileIO
{
public:
	//构造函数(出错会throw)
	FileIOImp(const char *pFileName, const char *pMode);
	//析构函数
	virtual ~FileIOImp();

	//寻址到磁盘文件的某个位置上(出错会throw)
	virtual void Seek(SEEK_ORIG eSeekOrig, s64 sOffset);

	//获取当前访问位置(出错会throw)
	virtual s64 GetCurPos();

	//获取文件的总长度(出错会throw)
	virtual s64 GetLength();

	//将内部缓冲区中的数据全部清空并更新到文件中(出错会throw)
	virtual void Flush();

	//显示关闭磁盘文件(出错会throw)
	virtual void Close();


	/*以下为数据访问相关接口，这些接口出错会throw*/

	virtual void WriteFloat(float fData);
	virtual float ReadFloat();

	virtual void WriteDouble(double dData);
	virtual double ReadDouble();

	virtual void WriteString(const char *pStr, s32 sStrLen);
	virtual void ReadString(Buffer/*char*/ &bufData);

	virtual void WriteData(const void *pData, u32 uDataLen);
	virtual void ReadData(void *pData, u32 uDataLen);

	virtual void WriteS8(s8 sData);
	virtual s8 ReadS8();

	virtual void WriteU8(u8 uData);
	virtual u8 ReadU8();

	virtual void WriteS16(s16 sData);
	virtual s16 ReadS16();

	virtual void WriteU16(u16 uData);
	virtual u16 ReadU16();

	virtual void WriteS32(s32 sData);
	virtual s32 ReadS32();

	virtual void WriteU32(u32 uData);
	virtual u32 ReadU32();

	virtual void WriteS64(s64 sData);
	virtual s64 ReadS64();

	virtual void WriteU64(u64 uData);
	virtual u64 ReadU64();

	virtual void WriteVec2(const Vec2 &vData);
	virtual Vec2 ReadVec2();

	virtual void WriteVec3(const Vec3 &vData);
	virtual Vec3 ReadVec3();

	virtual void WriteVec3D(const Vec3D &vData);
	virtual Vec3D ReadVec3D();

	virtual void WriteVec4(const Vec4 &qData);
	virtual Vec4 ReadVec4();

	virtual void WritePlane(const Plane &qData);
	virtual Plane ReadPlane();

	virtual void WriteQuaternion(const Quaternion &qData);
	virtual Quaternion ReadQuaternion();

	virtual void WriteQuaternionD(const QuaternionD &qData);
	virtual QuaternionD ReadQuaternionD();

	virtual void WriteMat4x4(const Mat4x4 &matData);
	virtual Mat4x4 ReadMat4x4();

	virtual void WriteMat4x3(const Mat4x3 &matData);
	virtual Mat4x3 ReadMat4x3();

	virtual void WriteAABB(const AABB &bbData);
	virtual AABB ReadAABB();

	virtual void WriteRay(const Ray &rayData);
	virtual Ray ReadRay();

	virtual void WriteSphere(const Sphere &sphData);
	virtual Sphere ReadSphere();

	virtual void WriteRect(const Rect &rcData);
	virtual Rect ReadRect();

	virtual void WriteViewport(const Viewport &vpData);
	virtual Viewport ReadViewport();

	virtual void WriteBuffer(const Buffer &bufData);
	virtual void ReadBuffer(Buffer &bufData);

protected:
	FILE *m_lpFile; //表示文件流指针
};



class FileZipIO : public DiskIO
{
public:
	//构造函数，出错throw
	FileZipIO(const char* pFileName,const char* pMode);
	//析构函数
	~FileZipIO();

	//寻址到Zip解压出的内存缓冲区的某个位置上(出错会throw)
	virtual void Seek(SEEK_ORIG eSeekOrig, s64 sOffset);

	//获取当前访问位置(出错会throw)
	virtual s64 GetCurPos();

	//获取未压缩数据的缓冲区长度(出错会throw)
	virtual s64 GetLength();

	//将内部未压缩数据缓冲区中的数据全部清空并压缩后更新到zip文件中,
	//如果对同一个zip文件多次执行该函数，则新的buffer的内容会替换之前zip文件中的内容(出错会throw)
	virtual void Flush();

	//显式关闭Zip文件(出错会throw)
	virtual void Close();

	//以下为数据访问相关接口，这些接口出错会throw

	virtual void WriteFloat(float fData);
	virtual float ReadFloat();

	virtual void WriteDouble(double dData);
	virtual double ReadDouble();

	virtual void WriteString(const char *pStr, s32 sStrLen);
	virtual void ReadString(Buffer/*char*/ &bufData);

	virtual void WriteData(const void *pData, u32 uDataLen);
	virtual void ReadData(void *pData, u32 uDataLen);

	virtual void WriteS8(s8 sData);
	virtual s8 ReadS8();

	virtual void WriteU8(u8 uData);
	virtual u8 ReadU8();

	virtual void WriteS16(s16 sData);
	virtual s16 ReadS16();

	virtual void WriteU16(u16 uData);
	virtual u16 ReadU16();

	virtual void WriteS32(s32 sData);
	virtual s32 ReadS32();

	virtual void WriteU32(u32 uData);
	virtual u32 ReadU32();

	virtual void WriteS64(s64 sData);
	virtual s64 ReadS64();

	virtual void WriteU64(u64 uData);
	virtual u64 ReadU64();

	virtual void WriteVec2(const Vec2 &vData);
	virtual Vec2 ReadVec2();

	virtual void WriteVec3(const Vec3 &vData);
	virtual Vec3 ReadVec3();

	virtual void WriteVec3D(const Vec3D &vData);
	virtual Vec3D ReadVec3D();

	virtual void WriteVec4(const Vec4 &qData);
	virtual Vec4 ReadVec4();

	virtual void WritePlane(const Plane &qData);
	virtual Plane ReadPlane();

	virtual void WriteQuaternion(const Quaternion &qData);
	virtual Quaternion ReadQuaternion();

	virtual void WriteQuaternionD(const QuaternionD &qData);
	virtual QuaternionD ReadQuaternionD();

	virtual void WriteMat4x4(const Mat4x4 &matData);
	virtual Mat4x4 ReadMat4x4();

	virtual void WriteMat4x3(const Mat4x3 &matData);
	virtual Mat4x3 ReadMat4x3();

	virtual void WriteAABB(const AABB &bbData);
	virtual AABB ReadAABB();

	virtual void WriteRay(const Ray &rayData);
	virtual Ray ReadRay();

	virtual void WriteSphere(const Sphere &sphData);
	virtual Sphere ReadSphere();

	virtual void WriteRect(const Rect &rcData);
	virtual Rect ReadRect();

	virtual void WriteViewport(const Viewport &vpData);
	virtual Viewport ReadViewport();

	virtual void WriteBuffer(const Buffer &bufData);
	virtual void ReadBuffer(Buffer &bufData);

protected:
	std::vector<BYTE>	m_vOriginalBuffer;

	cn::u32			m_uZipFileOpenMode;		//zip文件句柄是以什么打开方式得到的 =0:解压读取 ; =1:压缩写入
	unzFile			m_hZipReadHandle;		//若用unzOpen()打开文件,该句柄实际为unzFile类型
	zipFile			m_hZipWriteHandle;		//若用zipOpen()打开文件，句柄实际为minizip中的zipFile类型

	cn::u32			m_sCurrOffsetPos;		//当前对缓冲区的操作位置

};



//表示资源包内的文件访问模块的实现类
class FileInPackIO : public FileIO
{
public:
	//构造函数(出错会throw)
	//refPackage：表示文件所属的资源包对象(该变量内部会被自动清空)
	//pPackRelPathMD5：表示文件在资源包内的相对路径的MD5值，若为NULL则模块内部自动计算
	//strPackRelPath：表示文件在资源包内的相对路径
	//pFileOrigData,uFileOrigDataLen：表示文件在资源包中的原始内容数据，若无效则模块内部自动获取原始内容
	FileInPackIO(const char *pFileName, const char *pMode, cn::Ref/*FilePackage*/ &refPackage, const u128 *pPackRelPathMD5, const std::string &strPackRelPath, 
		const void *pFileOrigData, cn::u32 uFileOrigDataLen);
	//析构函数
	virtual ~FileInPackIO();
	
	//重置文件访问模块
	virtual void Reset(const char *pFileName, const char *pMode, cn::Ref/*FilePackage*/ &refPackage, const u128 *pPackRelPathMD5, const std::string &strPackRelPath, 
		const void *pFileOrigData, cn::u32 uFileOrigDataLen);

	//从资源包中更新自身的数据
	virtual bool UpdateFromPack();

	//寻址到磁盘文件的某个位置上(出错会throw)
	virtual void Seek(SEEK_ORIG eSeekOrig, s64 sOffset);

	//获取当前访问位置(出错会throw)
	virtual s64 GetCurPos();

	//获取文件的总长度(出错会throw)
	virtual s64 GetLength();

	//将内部缓冲区中的数据全部清空并更新到文件中(出错会throw)
	virtual void Flush();

	//显示关闭磁盘文件(出错会throw)
	virtual void Close();


	/*以下为数据访问相关接口，这些接口出错会throw*/

	virtual void WriteFloat(float fData);
	virtual float ReadFloat();

	virtual void WriteDouble(double dData);
	virtual double ReadDouble();

	virtual void WriteString(const char *pStr, s32 sStrLen);
	virtual void ReadString(Buffer/*char*/ &bufData);

	virtual void WriteData(const void *pData, u32 uDataLen);
	virtual void ReadData(void *pData, u32 uDataLen);

	virtual void WriteS8(s8 sData);
	virtual s8 ReadS8();

	virtual void WriteU8(u8 uData);
	virtual u8 ReadU8();

	virtual void WriteS16(s16 sData);
	virtual s16 ReadS16();

	virtual void WriteU16(u16 uData);
	virtual u16 ReadU16();

	virtual void WriteS32(s32 sData);
	virtual s32 ReadS32();

	virtual void WriteU32(u32 uData);
	virtual u32 ReadU32();

	virtual void WriteS64(s64 sData);
	virtual s64 ReadS64();

	virtual void WriteU64(u64 uData);
	virtual u64 ReadU64();

	virtual void WriteVec2(const Vec2 &vData);
	virtual Vec2 ReadVec2();

	virtual void WriteVec3(const Vec3 &vData);
	virtual Vec3 ReadVec3();

	virtual void WriteVec3D(const Vec3D &vData);
	virtual Vec3D ReadVec3D();

	virtual void WriteVec4(const Vec4 &qData);
	virtual Vec4 ReadVec4();

	virtual void WritePlane(const Plane &qData);
	virtual Plane ReadPlane();

	virtual void WriteQuaternion(const Quaternion &qData);
	virtual Quaternion ReadQuaternion();

	virtual void WriteQuaternionD(const QuaternionD &qData);
	virtual QuaternionD ReadQuaternionD();

	virtual void WriteMat4x4(const Mat4x4 &matData);
	virtual Mat4x4 ReadMat4x4();

	virtual void WriteMat4x3(const Mat4x3 &matData);
	virtual Mat4x3 ReadMat4x3();

	virtual void WriteAABB(const AABB &bbData);
	virtual AABB ReadAABB();

	virtual void WriteRay(const Ray &rayData);
	virtual Ray ReadRay();

	virtual void WriteSphere(const Sphere &sphData);
	virtual Sphere ReadSphere();

	virtual void WriteRect(const Rect &rcData);
	virtual Rect ReadRect();

	virtual void WriteViewport(const Viewport &vpData);
	virtual Viewport ReadViewport();

	virtual void WriteBuffer(const Buffer &bufData);
	virtual void ReadBuffer(Buffer &bufData);

protected:

	std::string m_strMode; //表示文件的打开方式
	Ref/*FilePackage*/ m_refPackage; //表示文件所属的资源包对象
	FilePackage *m_lpPackage;
	bool m_bPathMD5Inited; //表示文件的包内相对路径MD5值是否已初始化
	u128 m_uPackRelPathMD5; //表示文件的包内相对路径MD5值
	std::string m_strPackRelPath; //表示文件的包内相对路径

	bool m_bValid; //表示文件是否处于有效状态
	cn::Buffer m_bufFileOrigData; //表示文件在资源包中的原始内容
	bool m_bNeedSaveToPack; //表示内存中的文件内容是否需要回写到资源包中
	std::vector<cn::u8> m_arrFileData; //表示文件的内容数据
	cn::s64 m_sFileDataLen; //表示文件的内容字节长度
	cn::s64 m_sFilePos; //表示文件当前的数据访问位置

	cn::u32 m_uStatCount;
	double m_dStatTime;
};



//表示内存访问对象的实现类
class MemIO : public DiskIO
{
public:
	//构造函数(出错会throw)
	MemIO(const void *pInitData, u32 uInitDatalen);
	//析构函数
	virtual ~MemIO();

	//寻址到磁盘文件的某个位置上(出错会throw)
	virtual void Seek(SEEK_ORIG eSeekOrig, s64 sOffset);

	//获取当前访问位置(出错会throw)
	virtual s64 GetCurPos();

	//获取文件的总长度(出错会throw)
	virtual s64 GetLength();

	//将内部缓冲区中的数据全部清空并更新到文件中(出错会throw)
	virtual void Flush();

	//显示关闭磁盘文件(出错会throw)
	virtual void Close();

	/*以下为数据访问相关接口，这些接口出错会throw*/

	virtual void WriteFloat(float fData);
	virtual float ReadFloat();

	virtual void WriteDouble(double dData);
	virtual double ReadDouble();

	virtual void WriteString(const char *pStr, s32 sStrLen);
	virtual void ReadString(Buffer/*char*/ &bufData);

	virtual void WriteData(const void *pData, u32 uDataLen);
	virtual void ReadData(void *pData, u32 uDataLen);

	virtual void WriteS8(s8 sData);
	virtual s8 ReadS8();

	virtual void WriteU8(u8 uData);
	virtual u8 ReadU8();

	virtual void WriteS16(s16 sData);
	virtual s16 ReadS16();

	virtual void WriteU16(u16 uData);
	virtual u16 ReadU16();

	virtual void WriteS32(s32 sData);
	virtual s32 ReadS32();

	virtual void WriteU32(u32 uData);
	virtual u32 ReadU32();

	virtual void WriteS64(s64 sData);
	virtual s64 ReadS64();

	virtual void WriteU64(u64 uData);
	virtual u64 ReadU64();

	virtual void WriteVec2(const Vec2 &vData);
	virtual Vec2 ReadVec2();

	virtual void WriteVec3(const Vec3 &vData);
	virtual Vec3 ReadVec3();

	virtual void WriteVec3D(const Vec3D &vData);
	virtual Vec3D ReadVec3D();

	virtual void WriteVec4(const Vec4 &qData);
	virtual Vec4 ReadVec4();

	virtual void WritePlane(const Plane &qData);
	virtual Plane ReadPlane();

	virtual void WriteQuaternion(const Quaternion &qData);
	virtual Quaternion ReadQuaternion();

	virtual void WriteQuaternionD(const QuaternionD &qData);
	virtual QuaternionD ReadQuaternionD();

	virtual void WriteMat4x4(const Mat4x4 &matData);
	virtual Mat4x4 ReadMat4x4();

	virtual void WriteMat4x3(const Mat4x3 &matData);
	virtual Mat4x3 ReadMat4x3();

	virtual void WriteAABB(const AABB &bbData);
	virtual AABB ReadAABB();

	virtual void WriteRay(const Ray &rayData);
	virtual Ray ReadRay();

	virtual void WriteSphere(const Sphere &sphData);
	virtual Sphere ReadSphere();

	virtual void WriteRect(const Rect &rcData);
	virtual Rect ReadRect();

	virtual void WriteViewport(const Viewport &vpData);
	virtual Viewport ReadViewport();

	virtual void WriteBuffer(const Buffer &bufData);
	virtual void ReadBuffer(Buffer &bufData);

protected:

	std::vector<cn::u8> m_arrMemData; //表示内存数据
	cn::s64 m_sMemDataLen; //表示内存数据的字节长度
	cn::s64 m_sMemPos; //表示内存数据的访问位置
};



//表示文件过滤模块(实现模块)
class FileFilterImp : public FileFilter
{
public:

	//表示一个字符索引节点
	struct CHAR_NODE
	{
		char m_uChar; //表示字符值
		u8 m_uChildNum; //表示节点的孩子节点数量
		u16 m_uRegionPos; //表示节点关联的区域信息在区域数组中的位置(==0xffff时表示没有关联任何区域信息)
		u32 m_uChildBasePos; //表示节点的孩子信息序列在总数组中的起始位置
	};

	//表示一个区域信息节点
	struct REGION_INFO
	{
		std::string m_strRegionID; //表示区域的标示名
		Ref/*ID_REGION2D_IMP*/ m_refRegion; //表示区域数据
		ID_REGION2D_IMP *m_lpRegion; //表示区域数据指针
	};

	//表示遍历字符节点树所需的堆栈结构
	struct CHAR_STACK
	{
		cn::s32 m_sCharBeginPos, m_sCharEndPos;
		cn::s32 m_sCurCharPos;
	};

	//表示临时的字符索引节点
	struct CHAR_NODE_SUB
	{
		char m_uChar; //表示字符值
		u16 m_uRegionPos; //表示节点关联的区域信息在区域数组中的位置(==0xffff时表示没有关联任何区域信息)
		std::list<CHAR_NODE_SUB>::iterator m_iterChildHead; //表示节点子序列的起始头节点
		std::list<CHAR_NODE_SUB>::iterator m_iterChildTail; //表示节点子序列的结束尾节点
		cn::u32 m_uSelfPos; //表示节点自身对应的索引位置
	};

	//表示一个过滤单元的附属结构
	struct FILTER_UNIT_SUB
	{
		std::string m_strUnitName; //表示过滤单元的名称
	};

public:

	//构造函数
	FileFilterImp();
	//析构函数
	virtual ~FileFilterImp();

	//开始添加新的过滤单元
	//返回值：true->可以开始添加，false->不允许添加
	virtual bool BeginAddFilterUnits();
	//添加一个过滤单元(必须位于BeginAddFilterUnits-EndAddFilterUnits对内)
	virtual bool AddFilterUnit(const FileFilter::FILTER_UNIT &cUnit);
	//添加一个过滤模块的数据到自身(必须位于BeginAddFilterUnits-EndAddFilterUnits对内)
	virtual bool AddFilter(const cn::WRef/*FileFilter*/ &refFilter);
	//结束添加新的过滤单元
	virtual void EndAddFilterUnits();

	/*以下方法只有在BeginAddFilterUnits-EndAddFilterUnits对外才有效*/

	//获取所有过滤单元
	virtual const FileFilter::FILTER_UNIT *GetAllFilterUnits(cn::u32 &uUnitNum);
	//删除所有过滤单元
	virtual void DelAllFilterUnits();
	//保存过滤数据到文件
	virtual bool Save(const char *pFullPath);
	//从文件加载过滤数据
	virtual bool Load(const char *pFullPath);
	//保存过滤数据到内存
	virtual bool SaveInMemory(Buffer &bufMem);
	//从内存加载过滤数据
	virtual bool LoadInMemory(Buffer &bufMem);

	//对文件进行过滤
	//pOrigResID：表示待过滤的原始完整文件资源ID，它可以是本地路径、URL链接等
	//ppFilePath：若文件筛选通过，则返回文件对应的磁盘路径；若筛选不通过，则返回空字符串；若ppFilePath==NULL则不返回任何信息
	//返回值：返回文件是否筛选通过
	virtual bool FilterFile(const char *pOrigResID, const char **ppFilePath =NULL);
	//对文件进行过滤
	//pFilterUnitName：表示文件资源所对应的过滤单元名称
	//sIDX, sIDZ：表示文件资源的资源索引，若不存在资源索引则置为-1即可
	//返回值：返回文件是否筛选通过
	virtual bool FilterFile(const char *pFilterUnitName, cn::s32 sIDX, cn::s32 sIDZ);

protected:

	//将正式存储信息转换为临时存储信息
	void ConvertFormalMemToTempMem();
	//将临时存储信息转换为正式存储信息
	void ConvertTempMemToFormalMem();

protected:

	cn::s32 m_sMaxUnitNameLen; //表示筛选单元的名称的最大长度
	cn::s32 m_sCharNodeNum; //表示字符索引节点数
	CHAR_NODE *m_lpCharNodes; //表示字符索引节点数组
	cn::s32 m_sRegionInfoNum; //表示区域信息总数
	REGION_INFO *m_lpRegionInfos; //表示区域信息数组

	bool m_bAdding; //表示是否正在进行添加操作

	std::map<std::string, std::string> m_mapTempUnitInfos; //表示临时中转的筛选单元信息 <筛选单元名称，对应区域的标示名>
	std::map<std::string, Ref/*ID_REGION2D*/> m_mapTempRegionInfos; //表示临时中转的区域信息 <区域的标示名，区域数据>	
};



//表示文件封包模块(实现模块)
class FilePackageImp : public FilePackage
{
public:

	//表示数据包文件的头部信息
	struct PACK_HEADER
	{
		cn::u32 m_uVersion; //表示文件封包结构的版本ID
		cn::u32 m_uWriting; //表示资源包是否正在进行写入修改
		cn::s64 m_sPackUnitSize; //表示文件封包中一个单元块的字节数
		cn::u64 m_uPackTimeStamp; //表示包数据的最大时间戳
		cn::s64 m_sFreeUnitNum; //表示空闲单元块的个数
		cn::s64 m_sFreeUnitsBaseUnit; //表示空闲单元链的头结点的单元块ID
		cn::s64 m_sFileLogNum; //表示包内文件更改日志的条数
		cn::s64 m_sMaxFileLogUnitNum; //表示包内存放文件更改日志的单元块的最大个数
		cn::s64 m_sFileNum; //表示当前包内的文件总个数

		cn::s64 m_sFreeDigestNum; //表示空闲摘要数据块的个数
		cn::s64 m_sFreeDigestsBasePos; //表示空闲摘要数据块链表的头结点的包地址
		cn::s64 m_sDigestEndPos; //表示摘要数据块的最后拓展写入地址

		cn::s64 m_sFileLogsBaseUnit; //表示包内文件更改日志的起始单元块ID
		cn::s64 m_sFileLogEndPos; //表示当前活动的文件更改日志的最后写入地址
		cn::s64 m_sFileInfosBaseUnit; //表示包内文件详细信息数据的起始单元块ID
		cn::s64 m_sFileInfoEndPos; //表示当前活动的文件详细信息数据的最后写入地址
		cn::s64 m_sFileDataEndPos; //表示当前活动的文件数据最后写入地址
	};

	//表示包内一个单元块的头部信息
	struct UNIT_HEADER
	{
		cn::s64 m_sCurUnitValidSize; //表示当前单元块的有效数据字节数(包括头部信息)
		cn::s64 m_sNextUnit; //表示与当前单元块存储相同类型数据的下一个单元块的ID(在存储文件数据的单元块中无效)
	};

	//表示包内一个文件的所有数据块组成的链表的链接指针(该链接指针位于每一个文件数据块的头部)
	struct FILE_DATA_LINK
	{
		cn::s64 m_sPrePos; //表示上一个数据块的包地址
		cn::s64 m_sNextPos; //表示下一个数据块的包地址
	};

	//表示包内一个文件的摘要信息
	struct FILE_DIGEST
	{
		FILE_DATA_LINK m_cDataLinkHead; //文件数据块链表的头结点(m_sPrePos永远等于 -(文件索引ID) )
		u128 m_uRelPathMD5; //表示文件在包内的规范化相对路径字符串的MD5值
		cn::s64 m_sFileInfoPos; //表示文件详细信息所存放的包地址
	};

	//表示内存中的文件摘要信息
	struct FILE_DIGEST_MEM
	{
		FILE_DIGEST m_cDiskInfo; //表示磁盘中的文件摘要信息
		cn::s64 m_sDiskInfoPos; //表示磁盘中文件摘要信息所存放的包地址

		//构造函数
		FILE_DIGEST_MEM();
	};

	//表示包内文件摘要信息的检索结构
	struct FILE_DIGEST_KEY
	{
		u128 m_uRelPathMD5; //表示文件在包内的规范化相对路径字符串的MD5值
		cn::u32 m_uDigestID; //表示摘要信息在FilePackageImp::m_arrMemFileDigests中的索引
	};

	//表示包内文件摘要信息的检索结构2
	struct FILE_DIGEST_KEY_2
	{
		cn::s64 m_sDiskInfoPos; //表示磁盘中文件摘要信息所存放的包地址
		cn::u32 m_uDigestID; //表示摘要信息在FilePackageImp::m_arrMemFileDigests中的索引
	};

	//表示包内一个文件的详细信息
	struct FILE_INFO
	{
		std::string m_strRelPath; //表示文件在包内的规范化相对路径
		cn::u64 m_uTimeStamp; //表示文件的最后修改时间戳
		cn::s64 m_sSize; //表示文件的字节数大小
	};

	//表示包内文件数据块的辅助结构
	struct FILE_DATA_ASSIST : public cn::BaseObj
	{
		cn::s64 m_sOrigDataPos; //表示数据块原有的包地址
		FILE_DATA_LINK m_cCurDataLink; //表示数据块当前的链接信息
		cn::s64 m_sDataSize; //表示数据块的字节数
		cn::u8 *m_pData; //表示数据块的内容
		std::vector<cn::u8> m_arrData;
	};

	//表示一个文件更改日志信息
	struct FILE_LOG
	{
		cn::u64 m_uTimeStamp; //表示该条日志发生的时间戳
		cn::u32 m_uEvent; //表示日志对应的事件ID(0->删除文件；1->添加文件；2->清空资源包内所有文件)
		cn::u128 m_uFileRelPathMD5; //表示目标文件对应的包内相对路径的MD5值
	};

	//表示存储文件更改日志信息的单元块的头部信息
	struct FILE_LOG_UNIT_HEADER
	{
		cn::s64 m_sCurUnitValidSize; //表示当前单元块的有效数据字节数(包括头部信息)
		cn::s64 m_sNextUnit; //表示存储日志信息的下一个单元块的ID
		cn::s64 m_sPreUnit; //表示存储日志信息的上一个单元块的ID
	};


public:

	//构造函数
	FilePackageImp(const char *pPackagePath, cn::s64 sDefaultPackUnitSize, cn::s64 sMaxFileLogUnitNum, bool bReadOnly);
	//析构函数
	virtual ~FilePackageImp();

	//获取包内单元块的字节大小
	virtual cn::s64 GetPackUnitSize();

	//计算资源包的统计信息
	virtual FilePackage::STAT_INFO ComputeStatInfo();

	//设置资源包的整体时间戳的自动更新状态
	virtual void SetAutoTimeStampEnable(bool bEnable);
	//获取资源包的整体时间戳的自动更新状态
	virtual bool GetAutoTimeStampEnable();
	//手动设置资源包的整体时间戳(当时间戳处于自动更新状态时，该函数无效)
	virtual void SetTimeStamp(cn::u64 uTimeStamp);
	//获取资源包的整体时间戳
	virtual cn::u64 GetTimeStamp();

	//清空包内所有数据
	//sNewPackUnitSize：表示包内一个基本存储单元的新的字节大小
	//sMaxFileLogUnitNum：表示包内存放文件更改日志的单元块的最大个数
	virtual void Clear(cn::s64 sNewPackUnitSize, cn::s64 sMaxFileLogUnitNum);

	//向包内添加一个文件内容
	//uRelNorPathMD5：表示文件在包内规范化相对路径(全部小写)字符串对应的MD5值
	//pRelNorPath：表示文件在包内的规范化相对路径
	//pFileData,sFileDataLen：表示文件内容在内存中的缓冲区
	//bCompactAdd：表示文件是否以紧凑方式存放与包中
	//pCustomTimeStamp：表示文件的自定义时间戳，若为NULL则内部自动计算文件的时间戳
	//返回值：true->添加成功；false->包内有相同路径的文件，添加不成功
	virtual bool AddAFile(u128 uRelNorPathMD5, const char *pRelNorPath, const cn::u8 *pFileData, cn::s64 sFileDataLen, bool bCompactAdd, const cn::u64 *pCustomTimeStamp);

	//从包中删除一系列文件
	//sDelFileNum：表示期望删除的文件总数
	//pRelNorPathMD5s：表示要删除的所有文件的规范化相对路径的MD5值
	//ppRelNorPaths：表示要删除的所有文件的规范化相对路径。若等于NULL，则只考虑文件相对路径的MD5值，相同MD5的文件都将从包中删除
	//lpFile：若不为NULL，则返回所有文件在包内的原始数据，若文件在包内不存在，则返回空数据
	virtual bool DelFiles(cn::s64 sDelFileNum, const cn::u128 *pRelNorPathMD5s, const char **ppRelNorPaths =NULL, cn::Buffer *lpFile =NULL);

	//查找包中是否存在指定MD5和相对路径的文件
	//返回值：true->存在；false->不存在
	virtual bool IsExist(u128 uRelNorPathMD5, const char *pRelNorPath);
	//查找包中是否存在指定MD5的文件
	//返回值：true->存在；false->不存在
	virtual bool IsExist(u128 uRelNorPathMD5);

	//获取包中文件的内容
	//bufFileData：返回包中文件的内容数据
	//uFileTimeStamp：返回包中文件的时间戳
	//返回值：true->获取成功；false->获取失败
	virtual bool GetFileData(u128 uRelNorPathMD5, const char *pRelNorPath, cn::Buffer &bufFileData, cn::u64 &uFileTimeStamp);

	//获取包中所有文件的属性信息
	//bufFileAttrs：返回所有文件的属性信息数组
	//bufFileRelPaths：返回文件在包内的规范化相对路径字符串序列，所有字符串子序列在总字符序列中顺序排列，每个子序列最后一个字符为'\0'
	//返回值：true->获取成功；false->获取失败
	virtual bool GetAllFileAttrs(cn::Buffer/*FilePackage::FILE_ATTR*/ &bufFileAttrs, cn::Buffer/*char*/ &bufFileRelPaths);

	//根据日志记录获取需要更新删除的文件的MD5信息
	//uOldPackTimeStamp：表示待比较的旧资源包的时间戳
	//bufInvalidFiles：返回旧资源包的时间戳到当前包内时间戳之间应变为无效删除的文件列表
	//返回值：true->旧资源包可从自身删除掉bufInvalidFiles指定的一系列无效文件；false->获取失败，旧资源包需要删除自身所有的数据
	virtual bool GetUpdateInvalidFiles(cn::u64 uOldPackTimeStamp, cn::Buffer/*cn::u128*/ &bufInvalidFiles);

protected:

	//获取一个MD5值在m_arrFileDigestKeys中对应的索引位置
	//uRelNorPathMD5：表示待查找的MD5值
	//sKeyLocNum：表示m_arrFileDigestKeys中与uRelNorPathMD5相等的关键字个数
	//返回值：若uRelNorPathMD5有相等的关键字的话则返回相等关键字的起始索引；若uRelNorPathMD5没有相等关键字的话则返回要插入的索引位置
	cn::s64 GetDigestKeyLocs(u128 uRelNorPathMD5, cn::s64 &sKeyLocNum);

	//将一个单元块加入空闲块链表
	void AddAFreeUnitID(cn::s64 sUnitID);
	//获取一个可用的空闲单元块ID
	cn::s64 GetUsableFreeUnitID();

	//将一个摘要数据块加入空闲摘要块链表
	void AddAFreeDigestPos(cn::s64 sDigestPos);
	//获取一个可用的空闲摘要块的包地址
	cn::s64 GetUsableFreeDigestPos();

	//向包中添加一系列文件更改日志
	void AddFileLogs(cn::s64 sLogNum, const FILE_LOG *lpLogs);
	//读取磁盘上的所有文件日志信息到内存中
	//arrFileLogs：表示存储文件日志信息的容器
	void LoadAllFileLogs(std::vector<FILE_LOG> &arrFileLogs);

	//读取磁盘上的所有文件摘要信息到内存中
	void LoadAllFileDigests();
	//读取磁盘上的所有文件详细信息到内存中
	//arrFileInfos：表示存储文件详细信息的容器
	//arrFileInfoUnits：返回存储文件详细信息的单元块列表
	void LoadAllFileInfos(std::vector<FILE_INFO> &arrFileInfos, std::vector<cn::s64> &arrFileInfoUnits);
	//将内存中的所有文件摘要信息和详细信息存入磁盘中
	//arrFileInfos：表示内存中新的文件详细信息
	//arrOldFileInfoUnits：表示磁盘中原有的老文件详细信息对应的单元块列表
	void SaveAllFileDigestAndInfos(const std::vector<FILE_INFO> &arrFileInfos, const std::vector<cn::s64> &arrOldFileInfoUnits);

protected:
	
	bool m_bReadOnly; //表示当前资源包是否为只读访问的

	cn::SyncObj *m_lpFileSync; //表示多线程互斥访问包文件数据的同步对象
	std::string m_strPackFilePath; //表示包文件的磁盘路径
	cn::Ref/*FileIOImp*/ m_refPackFile; //表示包文件对象
	FileIOImp *m_lpPackFile; //表示包文件指针

	bool m_bAutoTimeStampEnable; //表示资源包的整体时间戳的是否允许自动更新

	PACK_HEADER m_cPackHeader; //表示包数据的头部信息
	cn::s64 m_sUnitSize; //表示包单元块的字节数
	cn::s64 m_sFileLogNumPerUnit; //表示一个单元块所能存储的文件日志总数
	std::vector<cn::u8> m_arrEmptyUnit; //表示默认的空单元块数据
	std::vector<FILE_DIGEST_MEM> m_arrMemFileDigests; //表示包内现有文件的内存摘要信息
	std::vector<FILE_DIGEST_KEY> m_arrFileDigestKeys; //表示包内现有文件摘要信息的快速检索数组
};



//表示引擎资源服务器模块(测试模块)
class EngineResServerTest
{
public:

	//表示一个资源组的相关信息
	struct RES_GROUP
	{
		std::string m_strRootDir; //表示资源组的根目录
		std::map<std::string, cn::Ref/*cn::FilePackage*/> m_mapFilePackages; //表示当前资源组对应的资源包对象集合<资源包的磁盘路径(全部小写),资源包(若为空则表示该磁盘路径下没有任何资源包)>
	};

public:

	//构造函数
	EngineResServerTest();
	//析构函数
	virtual ~EngineResServerTest();

	//设置资源组的根目录
	virtual void SetResGroupRootDir(const char *pName, const char *pRootDir);
	//获取资源组的根目录
	virtual const char *GetResGroupRootDir(const char *pName);

	//对服务器请求资源数据
	//pURL：表示请求的URL链接
	//bufResData：返回引擎资源数据
	//返回值：返回资源数据是否获取成功
	virtual bool RequestEngineRes(const char *pURL, cn::Buffer &bufResData);

protected:

	//对服务器请求资源数据(内部版本)
	//pResGroupName：表示资源组标示名
	//pFilePath：表示文件路径信息
	//uFileTimeL，uFileTimeH：表示文件时间的低32位和高32位信息
	//pPackPath：表示资源包路径信息
	//uPackTimeL，uPackTimeH：表示资源包时间的低32位和高32位信息
	//bufResData：返回引擎资源数据
	//返回值：返回资源数据是否获取成功
	virtual bool RequestEngineResInternal(const char *pResGroupName, const char *pFilePath, cn::u32 uFileTimeL, cn::u32 uFileTimeH, 
		const char *pPackPath, cn::u32 uPackTimeL, cn::u32 uPackTimeH, cn::Buffer &bufResData);

protected:
	
	cn::SyncObj *m_lpServerSync; //表示多线程互斥访问服务器数据的同步对象
	std::map<std::string, RES_GROUP> m_mapResGroups; //表示所有的资源组数据<资源组标示名,资源组信息>
};




}


#endif