
#include "Common/Type.h"
#include <string>

namespace cn
{


static SyncObj g_StrSyncObj; //��ʾ�����ַ���������ȫ�ַ�Χ���߳�ͬ������



//��ʾ��̬�ַ�����ʵ����
class StrImp : public Str
{
public:

	//���캯��
	StrImp(const std::string &strData);

	//��ȡ��ǰ���ַ�������ָ��
	virtual const char *GetCStr() const;

	//��ȡ��ǰ�ַ����ĳ���(������ĩβ���ַ�0)
	virtual cn::u32 GetLen() const;

	//��ʾ�ַ�������ǿ���ü�����1ʱ�Ļص�����
	static void OnStrPostRefDec(cn::BaseObj *lpObj);

protected:

	std::string m_strCurStr; //��ʾ��ǰ��̬�ַ���������
};


//��ʾȫ�����еľ�̬�ַ�������
std::map<std::string, Ref/*Str*/> g_mapGlobalStrMap; 


//���캯��
StrImp::StrImp(const std::string &strData)
: m_strCurStr(strData)
{
}


//��ȡ��ǰ���ַ�������ָ��
const char *StrImp::GetCStr() const
{
	return m_strCurStr.c_str();
}


//��ȡ��ǰ�ַ����ĳ���(������ĩβ���ַ�0)
cn::u32 StrImp::GetLen() const
{
	return m_strCurStr.length();
}


//��ʾ�ַ�������ǿ���ü�����1ʱ�Ļص�����
void StrImp::OnStrPostRefDec(cn::BaseObj *lpObj)
{
	if(lpObj && lpObj->GetRefCount()==1){ //���ַ��������������õĻ�
		StrImp *lpStr =(StrImp *)(lpObj);
		g_mapGlobalStrMap.erase(lpStr->m_strCurStr);
	}
}


//����һ���ַ�������pCStr��ʾC�����ַ���ָ��
Ref/*Str*/ Str::Create(const char *pCStr)
{
	static std::string strTempStr;
	static cn::Ref refTemp;

	if(!pCStr){ //��������ַ�����Ч�Ļ�
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


//����һ���ַ�������pStrFormat��ʾ�ַ����ĸ�ʽ������
Ref/*Str*/ Str::CreateEx(const char *pFormatStr, ...)
{
	//AutoSync CurAutoSync(&g_StrSyncObj); //�����߳�ͬ��

	static char pTempBuf[_MAX_STR_LEN+1];
	static std::string strTempStr;
	static cn::Ref refTemp;

	if(!pFormatStr){ //��������ַ�����Ч�Ļ�
		return Ref(NULL);
	}

	va_list arglist; //��ʾ����Ĳ����б�
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


//���캯��
Str::Str()
{
}








































}