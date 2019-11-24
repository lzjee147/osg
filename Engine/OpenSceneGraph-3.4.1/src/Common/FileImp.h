#ifndef _FILEIMP_H
#define _FILEIMP_H


/*********************************************
*	���嵥���ļ�ѹ�����ʻ���
*
*
**********************************************/
#include "../ZlibWrap/zlibwrap.h"


namespace cn
{



//��ʾ�ļ�����ģ���ʵ����
class FileIOImp : public FileIO
{
public:
	//���캯��(�����throw)
	FileIOImp(const char *pFileName, const char *pMode);
	//��������
	virtual ~FileIOImp();

	//Ѱַ�������ļ���ĳ��λ����(�����throw)
	virtual void Seek(SEEK_ORIG eSeekOrig, s64 sOffset);

	//��ȡ��ǰ����λ��(�����throw)
	virtual s64 GetCurPos();

	//��ȡ�ļ����ܳ���(�����throw)
	virtual s64 GetLength();

	//���ڲ��������е�����ȫ����ղ����µ��ļ���(�����throw)
	virtual void Flush();

	//��ʾ�رմ����ļ�(�����throw)
	virtual void Close();


	/*����Ϊ���ݷ�����ؽӿڣ���Щ�ӿڳ����throw*/

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
	FILE *m_lpFile; //��ʾ�ļ���ָ��
};



class FileZipIO : public DiskIO
{
public:
	//���캯��������throw
	FileZipIO(const char* pFileName,const char* pMode);
	//��������
	~FileZipIO();

	//Ѱַ��Zip��ѹ�����ڴ滺������ĳ��λ����(�����throw)
	virtual void Seek(SEEK_ORIG eSeekOrig, s64 sOffset);

	//��ȡ��ǰ����λ��(�����throw)
	virtual s64 GetCurPos();

	//��ȡδѹ�����ݵĻ���������(�����throw)
	virtual s64 GetLength();

	//���ڲ�δѹ�����ݻ������е�����ȫ����ղ�ѹ������µ�zip�ļ���,
	//�����ͬһ��zip�ļ����ִ�иú��������µ�buffer�����ݻ��滻֮ǰzip�ļ��е�����(�����throw)
	virtual void Flush();

	//��ʽ�ر�Zip�ļ�(�����throw)
	virtual void Close();

	//����Ϊ���ݷ�����ؽӿڣ���Щ�ӿڳ����throw

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

	cn::u32			m_uZipFileOpenMode;		//zip�ļ��������ʲô�򿪷�ʽ�õ��� =0:��ѹ��ȡ ; =1:ѹ��д��
	unzFile			m_hZipReadHandle;		//����unzOpen()���ļ�,�þ��ʵ��ΪunzFile����
	zipFile			m_hZipWriteHandle;		//����zipOpen()���ļ������ʵ��Ϊminizip�е�zipFile����

	cn::u32			m_sCurrOffsetPos;		//��ǰ�Ի������Ĳ���λ��

};



//��ʾ��Դ���ڵ��ļ�����ģ���ʵ����
class FileInPackIO : public FileIO
{
public:
	//���캯��(�����throw)
	//refPackage����ʾ�ļ���������Դ������(�ñ����ڲ��ᱻ�Զ����)
	//pPackRelPathMD5����ʾ�ļ�����Դ���ڵ����·����MD5ֵ����ΪNULL��ģ���ڲ��Զ�����
	//strPackRelPath����ʾ�ļ�����Դ���ڵ����·��
	//pFileOrigData,uFileOrigDataLen����ʾ�ļ�����Դ���е�ԭʼ�������ݣ�����Ч��ģ���ڲ��Զ���ȡԭʼ����
	FileInPackIO(const char *pFileName, const char *pMode, cn::Ref/*FilePackage*/ &refPackage, const u128 *pPackRelPathMD5, const std::string &strPackRelPath, 
		const void *pFileOrigData, cn::u32 uFileOrigDataLen);
	//��������
	virtual ~FileInPackIO();
	
	//�����ļ�����ģ��
	virtual void Reset(const char *pFileName, const char *pMode, cn::Ref/*FilePackage*/ &refPackage, const u128 *pPackRelPathMD5, const std::string &strPackRelPath, 
		const void *pFileOrigData, cn::u32 uFileOrigDataLen);

	//����Դ���и������������
	virtual bool UpdateFromPack();

	//Ѱַ�������ļ���ĳ��λ����(�����throw)
	virtual void Seek(SEEK_ORIG eSeekOrig, s64 sOffset);

	//��ȡ��ǰ����λ��(�����throw)
	virtual s64 GetCurPos();

	//��ȡ�ļ����ܳ���(�����throw)
	virtual s64 GetLength();

	//���ڲ��������е�����ȫ����ղ����µ��ļ���(�����throw)
	virtual void Flush();

	//��ʾ�رմ����ļ�(�����throw)
	virtual void Close();


	/*����Ϊ���ݷ�����ؽӿڣ���Щ�ӿڳ����throw*/

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

	std::string m_strMode; //��ʾ�ļ��Ĵ򿪷�ʽ
	Ref/*FilePackage*/ m_refPackage; //��ʾ�ļ���������Դ������
	FilePackage *m_lpPackage;
	bool m_bPathMD5Inited; //��ʾ�ļ��İ������·��MD5ֵ�Ƿ��ѳ�ʼ��
	u128 m_uPackRelPathMD5; //��ʾ�ļ��İ������·��MD5ֵ
	std::string m_strPackRelPath; //��ʾ�ļ��İ������·��

	bool m_bValid; //��ʾ�ļ��Ƿ�����Ч״̬
	cn::Buffer m_bufFileOrigData; //��ʾ�ļ�����Դ���е�ԭʼ����
	bool m_bNeedSaveToPack; //��ʾ�ڴ��е��ļ������Ƿ���Ҫ��д����Դ����
	std::vector<cn::u8> m_arrFileData; //��ʾ�ļ�����������
	cn::s64 m_sFileDataLen; //��ʾ�ļ��������ֽڳ���
	cn::s64 m_sFilePos; //��ʾ�ļ���ǰ�����ݷ���λ��

	cn::u32 m_uStatCount;
	double m_dStatTime;
};



//��ʾ�ڴ���ʶ����ʵ����
class MemIO : public DiskIO
{
public:
	//���캯��(�����throw)
	MemIO(const void *pInitData, u32 uInitDatalen);
	//��������
	virtual ~MemIO();

	//Ѱַ�������ļ���ĳ��λ����(�����throw)
	virtual void Seek(SEEK_ORIG eSeekOrig, s64 sOffset);

	//��ȡ��ǰ����λ��(�����throw)
	virtual s64 GetCurPos();

	//��ȡ�ļ����ܳ���(�����throw)
	virtual s64 GetLength();

	//���ڲ��������е�����ȫ����ղ����µ��ļ���(�����throw)
	virtual void Flush();

	//��ʾ�رմ����ļ�(�����throw)
	virtual void Close();

	/*����Ϊ���ݷ�����ؽӿڣ���Щ�ӿڳ����throw*/

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

	std::vector<cn::u8> m_arrMemData; //��ʾ�ڴ�����
	cn::s64 m_sMemDataLen; //��ʾ�ڴ����ݵ��ֽڳ���
	cn::s64 m_sMemPos; //��ʾ�ڴ����ݵķ���λ��
};



//��ʾ�ļ�����ģ��(ʵ��ģ��)
class FileFilterImp : public FileFilter
{
public:

	//��ʾһ���ַ������ڵ�
	struct CHAR_NODE
	{
		char m_uChar; //��ʾ�ַ�ֵ
		u8 m_uChildNum; //��ʾ�ڵ�ĺ��ӽڵ�����
		u16 m_uRegionPos; //��ʾ�ڵ������������Ϣ�����������е�λ��(==0xffffʱ��ʾû�й����κ�������Ϣ)
		u32 m_uChildBasePos; //��ʾ�ڵ�ĺ�����Ϣ�������������е���ʼλ��
	};

	//��ʾһ��������Ϣ�ڵ�
	struct REGION_INFO
	{
		std::string m_strRegionID; //��ʾ����ı�ʾ��
		Ref/*ID_REGION2D_IMP*/ m_refRegion; //��ʾ��������
		ID_REGION2D_IMP *m_lpRegion; //��ʾ��������ָ��
	};

	//��ʾ�����ַ��ڵ�������Ķ�ջ�ṹ
	struct CHAR_STACK
	{
		cn::s32 m_sCharBeginPos, m_sCharEndPos;
		cn::s32 m_sCurCharPos;
	};

	//��ʾ��ʱ���ַ������ڵ�
	struct CHAR_NODE_SUB
	{
		char m_uChar; //��ʾ�ַ�ֵ
		u16 m_uRegionPos; //��ʾ�ڵ������������Ϣ�����������е�λ��(==0xffffʱ��ʾû�й����κ�������Ϣ)
		std::list<CHAR_NODE_SUB>::iterator m_iterChildHead; //��ʾ�ڵ������е���ʼͷ�ڵ�
		std::list<CHAR_NODE_SUB>::iterator m_iterChildTail; //��ʾ�ڵ������еĽ���β�ڵ�
		cn::u32 m_uSelfPos; //��ʾ�ڵ������Ӧ������λ��
	};

	//��ʾһ�����˵�Ԫ�ĸ����ṹ
	struct FILTER_UNIT_SUB
	{
		std::string m_strUnitName; //��ʾ���˵�Ԫ������
	};

public:

	//���캯��
	FileFilterImp();
	//��������
	virtual ~FileFilterImp();

	//��ʼ����µĹ��˵�Ԫ
	//����ֵ��true->���Կ�ʼ��ӣ�false->���������
	virtual bool BeginAddFilterUnits();
	//���һ�����˵�Ԫ(����λ��BeginAddFilterUnits-EndAddFilterUnits����)
	virtual bool AddFilterUnit(const FileFilter::FILTER_UNIT &cUnit);
	//���һ������ģ������ݵ�����(����λ��BeginAddFilterUnits-EndAddFilterUnits����)
	virtual bool AddFilter(const cn::WRef/*FileFilter*/ &refFilter);
	//��������µĹ��˵�Ԫ
	virtual void EndAddFilterUnits();

	/*���·���ֻ����BeginAddFilterUnits-EndAddFilterUnits�������Ч*/

	//��ȡ���й��˵�Ԫ
	virtual const FileFilter::FILTER_UNIT *GetAllFilterUnits(cn::u32 &uUnitNum);
	//ɾ�����й��˵�Ԫ
	virtual void DelAllFilterUnits();
	//����������ݵ��ļ�
	virtual bool Save(const char *pFullPath);
	//���ļ����ع�������
	virtual bool Load(const char *pFullPath);
	//����������ݵ��ڴ�
	virtual bool SaveInMemory(Buffer &bufMem);
	//���ڴ���ع�������
	virtual bool LoadInMemory(Buffer &bufMem);

	//���ļ����й���
	//pOrigResID����ʾ�����˵�ԭʼ�����ļ���ԴID���������Ǳ���·����URL���ӵ�
	//ppFilePath�����ļ�ɸѡͨ�����򷵻��ļ���Ӧ�Ĵ���·������ɸѡ��ͨ�����򷵻ؿ��ַ�������ppFilePath==NULL�򲻷����κ���Ϣ
	//����ֵ�������ļ��Ƿ�ɸѡͨ��
	virtual bool FilterFile(const char *pOrigResID, const char **ppFilePath =NULL);
	//���ļ����й���
	//pFilterUnitName����ʾ�ļ���Դ����Ӧ�Ĺ��˵�Ԫ����
	//sIDX, sIDZ����ʾ�ļ���Դ����Դ����������������Դ��������Ϊ-1����
	//����ֵ�������ļ��Ƿ�ɸѡͨ��
	virtual bool FilterFile(const char *pFilterUnitName, cn::s32 sIDX, cn::s32 sIDZ);

protected:

	//����ʽ�洢��Ϣת��Ϊ��ʱ�洢��Ϣ
	void ConvertFormalMemToTempMem();
	//����ʱ�洢��Ϣת��Ϊ��ʽ�洢��Ϣ
	void ConvertTempMemToFormalMem();

protected:

	cn::s32 m_sMaxUnitNameLen; //��ʾɸѡ��Ԫ�����Ƶ���󳤶�
	cn::s32 m_sCharNodeNum; //��ʾ�ַ������ڵ���
	CHAR_NODE *m_lpCharNodes; //��ʾ�ַ������ڵ�����
	cn::s32 m_sRegionInfoNum; //��ʾ������Ϣ����
	REGION_INFO *m_lpRegionInfos; //��ʾ������Ϣ����

	bool m_bAdding; //��ʾ�Ƿ����ڽ�����Ӳ���

	std::map<std::string, std::string> m_mapTempUnitInfos; //��ʾ��ʱ��ת��ɸѡ��Ԫ��Ϣ <ɸѡ��Ԫ���ƣ���Ӧ����ı�ʾ��>
	std::map<std::string, Ref/*ID_REGION2D*/> m_mapTempRegionInfos; //��ʾ��ʱ��ת��������Ϣ <����ı�ʾ������������>	
};



//��ʾ�ļ����ģ��(ʵ��ģ��)
class FilePackageImp : public FilePackage
{
public:

	//��ʾ���ݰ��ļ���ͷ����Ϣ
	struct PACK_HEADER
	{
		cn::u32 m_uVersion; //��ʾ�ļ�����ṹ�İ汾ID
		cn::u32 m_uWriting; //��ʾ��Դ���Ƿ����ڽ���д���޸�
		cn::s64 m_sPackUnitSize; //��ʾ�ļ������һ����Ԫ����ֽ���
		cn::u64 m_uPackTimeStamp; //��ʾ�����ݵ����ʱ���
		cn::s64 m_sFreeUnitNum; //��ʾ���е�Ԫ��ĸ���
		cn::s64 m_sFreeUnitsBaseUnit; //��ʾ���е�Ԫ����ͷ���ĵ�Ԫ��ID
		cn::s64 m_sFileLogNum; //��ʾ�����ļ�������־������
		cn::s64 m_sMaxFileLogUnitNum; //��ʾ���ڴ���ļ�������־�ĵ�Ԫ���������
		cn::s64 m_sFileNum; //��ʾ��ǰ���ڵ��ļ��ܸ���

		cn::s64 m_sFreeDigestNum; //��ʾ����ժҪ���ݿ�ĸ���
		cn::s64 m_sFreeDigestsBasePos; //��ʾ����ժҪ���ݿ������ͷ���İ���ַ
		cn::s64 m_sDigestEndPos; //��ʾժҪ���ݿ�������չд���ַ

		cn::s64 m_sFileLogsBaseUnit; //��ʾ�����ļ�������־����ʼ��Ԫ��ID
		cn::s64 m_sFileLogEndPos; //��ʾ��ǰ����ļ�������־�����д���ַ
		cn::s64 m_sFileInfosBaseUnit; //��ʾ�����ļ���ϸ��Ϣ���ݵ���ʼ��Ԫ��ID
		cn::s64 m_sFileInfoEndPos; //��ʾ��ǰ����ļ���ϸ��Ϣ���ݵ����д���ַ
		cn::s64 m_sFileDataEndPos; //��ʾ��ǰ����ļ��������д���ַ
	};

	//��ʾ����һ����Ԫ���ͷ����Ϣ
	struct UNIT_HEADER
	{
		cn::s64 m_sCurUnitValidSize; //��ʾ��ǰ��Ԫ�����Ч�����ֽ���(����ͷ����Ϣ)
		cn::s64 m_sNextUnit; //��ʾ�뵱ǰ��Ԫ��洢��ͬ�������ݵ���һ����Ԫ���ID(�ڴ洢�ļ����ݵĵ�Ԫ������Ч)
	};

	//��ʾ����һ���ļ����������ݿ���ɵ����������ָ��(������ָ��λ��ÿһ���ļ����ݿ��ͷ��)
	struct FILE_DATA_LINK
	{
		cn::s64 m_sPrePos; //��ʾ��һ�����ݿ�İ���ַ
		cn::s64 m_sNextPos; //��ʾ��һ�����ݿ�İ���ַ
	};

	//��ʾ����һ���ļ���ժҪ��Ϣ
	struct FILE_DIGEST
	{
		FILE_DATA_LINK m_cDataLinkHead; //�ļ����ݿ������ͷ���(m_sPrePos��Զ���� -(�ļ�����ID) )
		u128 m_uRelPathMD5; //��ʾ�ļ��ڰ��ڵĹ淶�����·���ַ�����MD5ֵ
		cn::s64 m_sFileInfoPos; //��ʾ�ļ���ϸ��Ϣ����ŵİ���ַ
	};

	//��ʾ�ڴ��е��ļ�ժҪ��Ϣ
	struct FILE_DIGEST_MEM
	{
		FILE_DIGEST m_cDiskInfo; //��ʾ�����е��ļ�ժҪ��Ϣ
		cn::s64 m_sDiskInfoPos; //��ʾ�������ļ�ժҪ��Ϣ����ŵİ���ַ

		//���캯��
		FILE_DIGEST_MEM();
	};

	//��ʾ�����ļ�ժҪ��Ϣ�ļ����ṹ
	struct FILE_DIGEST_KEY
	{
		u128 m_uRelPathMD5; //��ʾ�ļ��ڰ��ڵĹ淶�����·���ַ�����MD5ֵ
		cn::u32 m_uDigestID; //��ʾժҪ��Ϣ��FilePackageImp::m_arrMemFileDigests�е�����
	};

	//��ʾ�����ļ�ժҪ��Ϣ�ļ����ṹ2
	struct FILE_DIGEST_KEY_2
	{
		cn::s64 m_sDiskInfoPos; //��ʾ�������ļ�ժҪ��Ϣ����ŵİ���ַ
		cn::u32 m_uDigestID; //��ʾժҪ��Ϣ��FilePackageImp::m_arrMemFileDigests�е�����
	};

	//��ʾ����һ���ļ�����ϸ��Ϣ
	struct FILE_INFO
	{
		std::string m_strRelPath; //��ʾ�ļ��ڰ��ڵĹ淶�����·��
		cn::u64 m_uTimeStamp; //��ʾ�ļ�������޸�ʱ���
		cn::s64 m_sSize; //��ʾ�ļ����ֽ�����С
	};

	//��ʾ�����ļ����ݿ�ĸ����ṹ
	struct FILE_DATA_ASSIST : public cn::BaseObj
	{
		cn::s64 m_sOrigDataPos; //��ʾ���ݿ�ԭ�еİ���ַ
		FILE_DATA_LINK m_cCurDataLink; //��ʾ���ݿ鵱ǰ��������Ϣ
		cn::s64 m_sDataSize; //��ʾ���ݿ���ֽ���
		cn::u8 *m_pData; //��ʾ���ݿ������
		std::vector<cn::u8> m_arrData;
	};

	//��ʾһ���ļ�������־��Ϣ
	struct FILE_LOG
	{
		cn::u64 m_uTimeStamp; //��ʾ������־������ʱ���
		cn::u32 m_uEvent; //��ʾ��־��Ӧ���¼�ID(0->ɾ���ļ���1->����ļ���2->�����Դ���������ļ�)
		cn::u128 m_uFileRelPathMD5; //��ʾĿ���ļ���Ӧ�İ������·����MD5ֵ
	};

	//��ʾ�洢�ļ�������־��Ϣ�ĵ�Ԫ���ͷ����Ϣ
	struct FILE_LOG_UNIT_HEADER
	{
		cn::s64 m_sCurUnitValidSize; //��ʾ��ǰ��Ԫ�����Ч�����ֽ���(����ͷ����Ϣ)
		cn::s64 m_sNextUnit; //��ʾ�洢��־��Ϣ����һ����Ԫ���ID
		cn::s64 m_sPreUnit; //��ʾ�洢��־��Ϣ����һ����Ԫ���ID
	};


public:

	//���캯��
	FilePackageImp(const char *pPackagePath, cn::s64 sDefaultPackUnitSize, cn::s64 sMaxFileLogUnitNum, bool bReadOnly);
	//��������
	virtual ~FilePackageImp();

	//��ȡ���ڵ�Ԫ����ֽڴ�С
	virtual cn::s64 GetPackUnitSize();

	//������Դ����ͳ����Ϣ
	virtual FilePackage::STAT_INFO ComputeStatInfo();

	//������Դ��������ʱ������Զ�����״̬
	virtual void SetAutoTimeStampEnable(bool bEnable);
	//��ȡ��Դ��������ʱ������Զ�����״̬
	virtual bool GetAutoTimeStampEnable();
	//�ֶ�������Դ��������ʱ���(��ʱ��������Զ�����״̬ʱ���ú�����Ч)
	virtual void SetTimeStamp(cn::u64 uTimeStamp);
	//��ȡ��Դ��������ʱ���
	virtual cn::u64 GetTimeStamp();

	//��հ�����������
	//sNewPackUnitSize����ʾ����һ�������洢��Ԫ���µ��ֽڴ�С
	//sMaxFileLogUnitNum����ʾ���ڴ���ļ�������־�ĵ�Ԫ���������
	virtual void Clear(cn::s64 sNewPackUnitSize, cn::s64 sMaxFileLogUnitNum);

	//��������һ���ļ�����
	//uRelNorPathMD5����ʾ�ļ��ڰ��ڹ淶�����·��(ȫ��Сд)�ַ�����Ӧ��MD5ֵ
	//pRelNorPath����ʾ�ļ��ڰ��ڵĹ淶�����·��
	//pFileData,sFileDataLen����ʾ�ļ��������ڴ��еĻ�����
	//bCompactAdd����ʾ�ļ��Ƿ��Խ��շ�ʽ��������
	//pCustomTimeStamp����ʾ�ļ����Զ���ʱ�������ΪNULL���ڲ��Զ������ļ���ʱ���
	//����ֵ��true->��ӳɹ���false->��������ͬ·�����ļ�����Ӳ��ɹ�
	virtual bool AddAFile(u128 uRelNorPathMD5, const char *pRelNorPath, const cn::u8 *pFileData, cn::s64 sFileDataLen, bool bCompactAdd, const cn::u64 *pCustomTimeStamp);

	//�Ӱ���ɾ��һϵ���ļ�
	//sDelFileNum����ʾ����ɾ�����ļ�����
	//pRelNorPathMD5s����ʾҪɾ���������ļ��Ĺ淶�����·����MD5ֵ
	//ppRelNorPaths����ʾҪɾ���������ļ��Ĺ淶�����·����������NULL����ֻ�����ļ����·����MD5ֵ����ͬMD5���ļ������Ӱ���ɾ��
	//lpFile������ΪNULL���򷵻������ļ��ڰ��ڵ�ԭʼ���ݣ����ļ��ڰ��ڲ����ڣ��򷵻ؿ�����
	virtual bool DelFiles(cn::s64 sDelFileNum, const cn::u128 *pRelNorPathMD5s, const char **ppRelNorPaths =NULL, cn::Buffer *lpFile =NULL);

	//���Ұ����Ƿ����ָ��MD5�����·�����ļ�
	//����ֵ��true->���ڣ�false->������
	virtual bool IsExist(u128 uRelNorPathMD5, const char *pRelNorPath);
	//���Ұ����Ƿ����ָ��MD5���ļ�
	//����ֵ��true->���ڣ�false->������
	virtual bool IsExist(u128 uRelNorPathMD5);

	//��ȡ�����ļ�������
	//bufFileData�����ذ����ļ�����������
	//uFileTimeStamp�����ذ����ļ���ʱ���
	//����ֵ��true->��ȡ�ɹ���false->��ȡʧ��
	virtual bool GetFileData(u128 uRelNorPathMD5, const char *pRelNorPath, cn::Buffer &bufFileData, cn::u64 &uFileTimeStamp);

	//��ȡ���������ļ���������Ϣ
	//bufFileAttrs�����������ļ���������Ϣ����
	//bufFileRelPaths�������ļ��ڰ��ڵĹ淶�����·���ַ������У������ַ��������������ַ�������˳�����У�ÿ�����������һ���ַ�Ϊ'\0'
	//����ֵ��true->��ȡ�ɹ���false->��ȡʧ��
	virtual bool GetAllFileAttrs(cn::Buffer/*FilePackage::FILE_ATTR*/ &bufFileAttrs, cn::Buffer/*char*/ &bufFileRelPaths);

	//������־��¼��ȡ��Ҫ����ɾ�����ļ���MD5��Ϣ
	//uOldPackTimeStamp����ʾ���Ƚϵľ���Դ����ʱ���
	//bufInvalidFiles�����ؾ���Դ����ʱ�������ǰ����ʱ���֮��Ӧ��Ϊ��Чɾ�����ļ��б�
	//����ֵ��true->����Դ���ɴ�����ɾ����bufInvalidFilesָ����һϵ����Ч�ļ���false->��ȡʧ�ܣ�����Դ����Ҫɾ���������е�����
	virtual bool GetUpdateInvalidFiles(cn::u64 uOldPackTimeStamp, cn::Buffer/*cn::u128*/ &bufInvalidFiles);

protected:

	//��ȡһ��MD5ֵ��m_arrFileDigestKeys�ж�Ӧ������λ��
	//uRelNorPathMD5����ʾ�����ҵ�MD5ֵ
	//sKeyLocNum����ʾm_arrFileDigestKeys����uRelNorPathMD5��ȵĹؼ��ָ���
	//����ֵ����uRelNorPathMD5����ȵĹؼ��ֵĻ��򷵻���ȹؼ��ֵ���ʼ��������uRelNorPathMD5û����ȹؼ��ֵĻ��򷵻�Ҫ���������λ��
	cn::s64 GetDigestKeyLocs(u128 uRelNorPathMD5, cn::s64 &sKeyLocNum);

	//��һ����Ԫ�������п�����
	void AddAFreeUnitID(cn::s64 sUnitID);
	//��ȡһ�����õĿ��е�Ԫ��ID
	cn::s64 GetUsableFreeUnitID();

	//��һ��ժҪ���ݿ�������ժҪ������
	void AddAFreeDigestPos(cn::s64 sDigestPos);
	//��ȡһ�����õĿ���ժҪ��İ���ַ
	cn::s64 GetUsableFreeDigestPos();

	//��������һϵ���ļ�������־
	void AddFileLogs(cn::s64 sLogNum, const FILE_LOG *lpLogs);
	//��ȡ�����ϵ������ļ���־��Ϣ���ڴ���
	//arrFileLogs����ʾ�洢�ļ���־��Ϣ������
	void LoadAllFileLogs(std::vector<FILE_LOG> &arrFileLogs);

	//��ȡ�����ϵ������ļ�ժҪ��Ϣ���ڴ���
	void LoadAllFileDigests();
	//��ȡ�����ϵ������ļ���ϸ��Ϣ���ڴ���
	//arrFileInfos����ʾ�洢�ļ���ϸ��Ϣ������
	//arrFileInfoUnits�����ش洢�ļ���ϸ��Ϣ�ĵ�Ԫ���б�
	void LoadAllFileInfos(std::vector<FILE_INFO> &arrFileInfos, std::vector<cn::s64> &arrFileInfoUnits);
	//���ڴ��е������ļ�ժҪ��Ϣ����ϸ��Ϣ���������
	//arrFileInfos����ʾ�ڴ����µ��ļ���ϸ��Ϣ
	//arrOldFileInfoUnits����ʾ������ԭ�е����ļ���ϸ��Ϣ��Ӧ�ĵ�Ԫ���б�
	void SaveAllFileDigestAndInfos(const std::vector<FILE_INFO> &arrFileInfos, const std::vector<cn::s64> &arrOldFileInfoUnits);

protected:
	
	bool m_bReadOnly; //��ʾ��ǰ��Դ���Ƿ�Ϊֻ�����ʵ�

	cn::SyncObj *m_lpFileSync; //��ʾ���̻߳�����ʰ��ļ����ݵ�ͬ������
	std::string m_strPackFilePath; //��ʾ���ļ��Ĵ���·��
	cn::Ref/*FileIOImp*/ m_refPackFile; //��ʾ���ļ�����
	FileIOImp *m_lpPackFile; //��ʾ���ļ�ָ��

	bool m_bAutoTimeStampEnable; //��ʾ��Դ��������ʱ������Ƿ������Զ�����

	PACK_HEADER m_cPackHeader; //��ʾ�����ݵ�ͷ����Ϣ
	cn::s64 m_sUnitSize; //��ʾ����Ԫ����ֽ���
	cn::s64 m_sFileLogNumPerUnit; //��ʾһ����Ԫ�����ܴ洢���ļ���־����
	std::vector<cn::u8> m_arrEmptyUnit; //��ʾĬ�ϵĿյ�Ԫ������
	std::vector<FILE_DIGEST_MEM> m_arrMemFileDigests; //��ʾ���������ļ����ڴ�ժҪ��Ϣ
	std::vector<FILE_DIGEST_KEY> m_arrFileDigestKeys; //��ʾ���������ļ�ժҪ��Ϣ�Ŀ��ټ�������
};



//��ʾ������Դ������ģ��(����ģ��)
class EngineResServerTest
{
public:

	//��ʾһ����Դ��������Ϣ
	struct RES_GROUP
	{
		std::string m_strRootDir; //��ʾ��Դ��ĸ�Ŀ¼
		std::map<std::string, cn::Ref/*cn::FilePackage*/> m_mapFilePackages; //��ʾ��ǰ��Դ���Ӧ����Դ�����󼯺�<��Դ���Ĵ���·��(ȫ��Сд),��Դ��(��Ϊ�����ʾ�ô���·����û���κ���Դ��)>
	};

public:

	//���캯��
	EngineResServerTest();
	//��������
	virtual ~EngineResServerTest();

	//������Դ��ĸ�Ŀ¼
	virtual void SetResGroupRootDir(const char *pName, const char *pRootDir);
	//��ȡ��Դ��ĸ�Ŀ¼
	virtual const char *GetResGroupRootDir(const char *pName);

	//�Է�����������Դ����
	//pURL����ʾ�����URL����
	//bufResData������������Դ����
	//����ֵ��������Դ�����Ƿ��ȡ�ɹ�
	virtual bool RequestEngineRes(const char *pURL, cn::Buffer &bufResData);

protected:

	//�Է�����������Դ����(�ڲ��汾)
	//pResGroupName����ʾ��Դ���ʾ��
	//pFilePath����ʾ�ļ�·����Ϣ
	//uFileTimeL��uFileTimeH����ʾ�ļ�ʱ��ĵ�32λ�͸�32λ��Ϣ
	//pPackPath����ʾ��Դ��·����Ϣ
	//uPackTimeL��uPackTimeH����ʾ��Դ��ʱ��ĵ�32λ�͸�32λ��Ϣ
	//bufResData������������Դ����
	//����ֵ��������Դ�����Ƿ��ȡ�ɹ�
	virtual bool RequestEngineResInternal(const char *pResGroupName, const char *pFilePath, cn::u32 uFileTimeL, cn::u32 uFileTimeH, 
		const char *pPackPath, cn::u32 uPackTimeL, cn::u32 uPackTimeH, cn::Buffer &bufResData);

protected:
	
	cn::SyncObj *m_lpServerSync; //��ʾ���̻߳�����ʷ��������ݵ�ͬ������
	std::map<std::string, RES_GROUP> m_mapResGroups; //��ʾ���е���Դ������<��Դ���ʾ��,��Դ����Ϣ>
};




}


#endif