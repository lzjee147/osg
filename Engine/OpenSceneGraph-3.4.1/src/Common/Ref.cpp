
#include "Common/Ref.h"

namespace cn
{



//�������������
bool operator == (const Ref &src1, const WRef &src2)
{
	return src1.m_lpRefKernel == src2.m_lpRefKernel;
}

//���������������
bool operator != (const Ref &src1, const WRef &src2)
{
	return src1.m_lpRefKernel != src2.m_lpRefKernel;
}

//С�����������
bool operator < (const Ref &src1, const WRef &src2)
{
	return src1.m_lpRefKernel < src2.m_lpRefKernel;
}

//�������������
bool operator > (const Ref &src1, const WRef &src2)
{
	return src1.m_lpRefKernel > src2.m_lpRefKernel;
}

//�������������
bool operator == (const WRef &src1, const Ref &src2)
{
	return src1.m_lpRefKernel == src2.m_lpRefKernel;
}

//���������������
bool operator != (const WRef &src1, const Ref &src2)
{
	return src1.m_lpRefKernel != src2.m_lpRefKernel;
}

//С�����������
bool operator < (const WRef &src1, const Ref &src2)
{
	return src1.m_lpRefKernel < src2.m_lpRefKernel;
}

//�������������
bool operator > (const WRef &src1, const Ref &src2)
{
	return src1.m_lpRefKernel > src2.m_lpRefKernel;
}

//�������������
bool operator == (const Ref &src1, const Ref &src2)
{
	return src1.m_lpRefKernel == src2.m_lpRefKernel;
}

//���������������
bool operator != (const Ref &src1, const Ref &src2)
{
	return src1.m_lpRefKernel != src2.m_lpRefKernel;
}

//С�����������
bool operator < (const Ref &src1, const Ref &src2)
{
	return src1.m_lpRefKernel < src2.m_lpRefKernel;
}

//�������������
bool operator > (const Ref &src1, const Ref &src2)
{
	return src1.m_lpRefKernel > src2.m_lpRefKernel;
}

//�������������
bool operator == (const WRef &src1, const WRef &src2)
{
	return src1.m_lpRefKernel == src2.m_lpRefKernel;
}

//���������������
bool operator != (const WRef &src1, const WRef &src2)
{
	return src1.m_lpRefKernel != src2.m_lpRefKernel;
}

//С�����������
bool operator < (const WRef &src1, const WRef &src2)
{
	return src1.m_lpRefKernel < src2.m_lpRefKernel;
}

//�������������
bool operator > (const WRef &src1, const WRef &src2)
{
	return src1.m_lpRefKernel > src2.m_lpRefKernel;
}



//�������ö����ں�
RefKernel *AllocRefKernel(cn::BaseObj *lpObj,
									void *pOnPostRefInc,
									void *pOnPostRefDec,
									void *pOnPostWRefInc,
									void *pOnPostWRefDec,
									void *pObjDeallocator)
{
	return new RefKernel(lpObj,
		(RefKernel::CallbackObj)(pOnPostRefInc),
		(RefKernel::CallbackObj)(pOnPostRefDec),
		(RefKernel::CallbackObj)(pOnPostWRefInc),
		(RefKernel::CallbackObj)(pOnPostWRefDec),
		(RefKernel::CallbackObj)(pObjDeallocator));
}

//�ͷ����ö����ں�
void DeallocRefKernel(RefKernel *lpRefKernel)
{
	delete lpRefKernel;
}

//���ݶ����Ĭ���ͷź���
void DeallocObj(BaseObj *lpObj)
{
	delete lpObj;
}


//���캯��
IEventReceiver::IEventReceiver()
{
	m_lpEventLevelSyncObj =new cn::SyncObj(); m_uEventLevel =0;
}


//��������
IEventReceiver::~IEventReceiver()
{
	SAFE_DELETE(m_lpEventLevelSyncObj);
}


//�����¼�����ȼ�
//uLevel��0��ʾ��͵ȼ����¼����������٣�ֵԽ���¼�������Խ��
void IEventReceiver::SetEventLevel(DWORD uLevel)
{
	cn::AutoSync cCurAutoSync(m_lpEventLevelSyncObj); m_uEventLevel =uLevel;
}


//��ȡ�¼�����ȼ�
DWORD IEventReceiver::GetEventLevel()
{
	cn::AutoSync cCurAutoSync(m_lpEventLevelSyncObj); return m_uEventLevel;
}































}