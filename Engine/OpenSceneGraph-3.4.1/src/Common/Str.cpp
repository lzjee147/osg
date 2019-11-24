
#include "Common/Type.h"
#include <string>

namespace cn
{


static SyncObj g_StrSyncObj; //表示用于字符串创建的全局范围的线程同步对象



//表示静态字符串的实现类
class StrImp : public Str
{
public:

	//构造函数
	StrImp(const std::string &strData);

	//获取当前的字符串内容指针
	virtual const char *GetCStr() const;

	//获取当前字符串的长度(不包含末尾的字符0)
	virtual cn::u32 GetLen() const;

	//表示字符串对象强引用计数减1时的回调函数
	static void OnStrPostRefDec(cn::BaseObj *lpObj);

protected:

	std::string m_strCurStr; //表示当前静态字符串的内容
};


//表示全局所有的静态字符串集合
std::map<std::string, Ref/*Str*/> g_mapGlobalStrMap; 


//构造函数
StrImp::StrImp(const std::string &strData)
: m_strCurStr(strData)
{
}


//获取当前的字符串内容指针
const char *StrImp::GetCStr() const
{
	return m_strCurStr.c_str();
}


//获取当前字符串的长度(不包含末尾的字符0)
cn::u32 StrImp::GetLen() const
{
	return m_strCurStr.length();
}


//表示字符串对象强引用计数减1时的回调函数
void StrImp::OnStrPostRefDec(cn::BaseObj *lpObj)
{
	if(lpObj && lpObj->GetRefCount()==1){ //若字符串对象无人引用的话
		StrImp *lpStr =(StrImp *)(lpObj);
		g_mapGlobalStrMap.erase(lpStr->m_strCurStr);
	}
}


//创建一个字符串对象，pCStr表示C风格的字符串指针
Ref/*Str*/ Str::Create(const char *pCStr)
{
	static std::string strTempStr;
	static cn::Ref refTemp;

	if(!pCStr){ //若输入的字符串无效的话
		return Ref(NULL);
	}

	strTempStr =pCStr;

	std::pair<std::map<std::string, Ref/*Str*/>::iterator, bool> InsResult =g_mapGlobalStrMap.insert(
		std::make_pair(strTempStr, refTemp));
	if(InsResult.second){
		cn::Ref refTemp2(new StrImp(strTempStr), NULL, StrImp::OnStrPostRefDec, NULL, NULL, NULL);
		InsResult.first->second =refTemp2;
		return InsResult.first->second;
	}else{
		return InsResult.first->second;
	}
}


//创建一个字符串对象，pStrFormat表示字符串的格式化内容
Ref/*Str*/ Str::CreateEx(const char *pFormatStr, ...)
{
	//AutoSync CurAutoSync(&g_StrSyncObj); //进行线程同步

	static char pTempBuf[_MAX_STR_LEN+1];
	static std::string strTempStr;
	static cn::Ref refTemp;

	if(!pFormatStr){ //若输入的字符串无效的话
		return Ref(NULL);
	}

	va_list arglist; //表示输入的参数列表
	va_start(arglist, pFormatStr);
	u32 uSize =::vsnprintf(pTempBuf, _MAX_STR_LEN, pFormatStr, arglist);
	strTempStr =pTempBuf;

	std::pair<std::map<std::string, Ref/*Str*/>::iterator, bool> InsResult =g_mapGlobalStrMap.insert(
		std::make_pair(strTempStr, refTemp));
	if(InsResult.second){
		cn::Ref refTemp2(new StrImp(strTempStr), NULL, StrImp::OnStrPostRefDec, NULL, NULL, NULL);
		InsResult.first->second =refTemp2;
		return InsResult.first->second;
	}else{
		return InsResult.first->second;
	}

	
}


//构造函数
Str::Str()
{
}








































}