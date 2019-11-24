
#include "Common/Ref.h"

namespace cn
{



//等于运算符重载
bool operator == (const Ref &src1, const WRef &src2)
{
	return src1.m_lpRefKernel == src2.m_lpRefKernel;
}

//不等于运算符重载
bool operator != (const Ref &src1, const WRef &src2)
{
	return src1.m_lpRefKernel != src2.m_lpRefKernel;
}

//小于运算符重载
bool operator < (const Ref &src1, const WRef &src2)
{
	return src1.m_lpRefKernel < src2.m_lpRefKernel;
}

//大于运算符重载
bool operator > (const Ref &src1, const WRef &src2)
{
	return src1.m_lpRefKernel > src2.m_lpRefKernel;
}

//等于运算符重载
bool operator == (const WRef &src1, const Ref &src2)
{
	return src1.m_lpRefKernel == src2.m_lpRefKernel;
}

//不等于运算符重载
bool operator != (const WRef &src1, const Ref &src2)
{
	return src1.m_lpRefKernel != src2.m_lpRefKernel;
}

//小于运算符重载
bool operator < (const WRef &src1, const Ref &src2)
{
	return src1.m_lpRefKernel < src2.m_lpRefKernel;
}

//大于运算符重载
bool operator > (const WRef &src1, const Ref &src2)
{
	return src1.m_lpRefKernel > src2.m_lpRefKernel;
}

//等于运算符重载
bool operator == (const Ref &src1, const Ref &src2)
{
	return src1.m_lpRefKernel == src2.m_lpRefKernel;
}

//不等于运算符重载
bool operator != (const Ref &src1, const Ref &src2)
{
	return src1.m_lpRefKernel != src2.m_lpRefKernel;
}

//小于运算符重载
bool operator < (const Ref &src1, const Ref &src2)
{
	return src1.m_lpRefKernel < src2.m_lpRefKernel;
}

//大于运算符重载
bool operator > (const Ref &src1, const Ref &src2)
{
	return src1.m_lpRefKernel > src2.m_lpRefKernel;
}

//等于运算符重载
bool operator == (const WRef &src1, const WRef &src2)
{
	return src1.m_lpRefKernel == src2.m_lpRefKernel;
}

//不等于运算符重载
bool operator != (const WRef &src1, const WRef &src2)
{
	return src1.m_lpRefKernel != src2.m_lpRefKernel;
}

//小于运算符重载
bool operator < (const WRef &src1, const WRef &src2)
{
	return src1.m_lpRefKernel < src2.m_lpRefKernel;
}

//大于运算符重载
bool operator > (const WRef &src1, const WRef &src2)
{
	return src1.m_lpRefKernel > src2.m_lpRefKernel;
}



//创建引用对象内核
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

//释放引用对象内核
void DeallocRefKernel(RefKernel *lpRefKernel)
{
	delete lpRefKernel;
}

//数据对象的默认释放函数
void DeallocObj(BaseObj *lpObj)
{
	delete lpObj;
}


//构造函数
IEventReceiver::IEventReceiver()
{
	m_lpEventLevelSyncObj =new cn::SyncObj(); m_uEventLevel =0;
}


//析构函数
IEventReceiver::~IEventReceiver()
{
	SAFE_DELETE(m_lpEventLevelSyncObj);
}


//设置事件处理等级
//uLevel：0表示最低等级，事件处理量最少；值越大，事件处理量越多
void IEventReceiver::SetEventLevel(DWORD uLevel)
{
	cn::AutoSync cCurAutoSync(m_lpEventLevelSyncObj); m_uEventLevel =uLevel;
}


//获取事件处理等级
DWORD IEventReceiver::GetEventLevel()
{
	cn::AutoSync cCurAutoSync(m_lpEventLevelSyncObj); return m_uEventLevel;
}































}