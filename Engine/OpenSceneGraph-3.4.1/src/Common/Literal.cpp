

namespace cn
{


//SyncObj g_LiteralSyncObj; //表示用于文字处理的全局范围的线程同步对象




u32 Literal::ToU32(const char *pText, u32 uDefaultVal)
{
	if(pText){
		return (u32)(::_atoi64(pText));
	}else{
		return uDefaultVal;
	}
}


s32 Literal::ToS32(const char *pText, s32 sDefaultVal)
{
	if(pText){
		return (s32)(::atoi(pText));
	}else{
		return sDefaultVal;
	}
}


u64 Literal::ToU64(const char *pText, u64 uDefaultVal)
{
	if(pText){
		return (u64)(::_atoi64(pText));
	}else{
		return uDefaultVal;
	}
}


s64 Literal::ToS64(const char *pText, s64 sDefaultVal)
{
	if(pText){
		return (s64)(::_atoi64(pText));
	}else{
		return sDefaultVal;
	}
}


float Literal::ToFloat(const char *pText, float fDefaultVal)
{
	if(pText){
		return float(::atof(pText));
	}else{
		return fDefaultVal;
	}
}


double Literal::ToDouble(const char *pText, double dDefaultVal)
{
	if(pText){
		return ::atof(pText);
	}else{
		return dDefaultVal;
	}
}


void Literal::ToVec2(const char *pText, Vec2 &ret, const Vec2 &vDefaultVal)
{
	//AutoSync AutoSyncObj(&g_LiteralSyncObj);

	std::stringstream strstream;
	if(pText){
		strstream<<pText;
		strstream>>ret.x>>ret.y;
	}else{
		ret =vDefaultVal;
	}
}


void Literal::ToVec3(const char *pText, Vec3 &ret, const Vec3 &vDefaultVal)
{
	//AutoSync AutoSyncObj(&g_LiteralSyncObj);

	std::stringstream strstream;
	if(pText){
		strstream<<pText;
		strstream>>ret.x>>ret.y>>ret.z;
	}else{
		ret =vDefaultVal;
	}
}


void Literal::ToVec4(const char *pText, Vec4 &ret, const Vec4 &qDefaultVal)
{
	//AutoSync AutoSyncObj(&g_LiteralSyncObj);

	std::stringstream strstream;
	if(pText){
		strstream<<pText;
		strstream>>ret.x>>ret.y>>ret.z>>ret.w;
	}else{
		ret =qDefaultVal;
	}
}


void Literal::ToPlane(const char *pText, Plane &ret, const Plane &qDefaultVal)
{
	//AutoSync AutoSyncObj(&g_LiteralSyncObj);

	std::stringstream strstream;
	if(pText){
		strstream<<pText;
		strstream>>ret.a>>ret.b>>ret.c>>ret.d;
	}else{
		ret =qDefaultVal;
	}
}


void Literal::ToQuaternion(const char *pText, Quaternion &ret, const Quaternion &qDefaultVal)
{
	//AutoSync AutoSyncObj(&g_LiteralSyncObj);

	std::stringstream strstream;
	if(pText){
		strstream<<pText;
		strstream>>ret.x>>ret.y>>ret.z>>ret.w;
	}else{
		ret =qDefaultVal;
	}
}


void Literal::ToRect(const char *pText, Rect &ret, const Rect &qDefaultVal)
{
	//AutoSync AutoSyncObj(&g_LiteralSyncObj);

	std::stringstream strstream;
	if(pText){
		strstream<<pText;
		strstream>>ret.m_fLeft>>ret.m_fTop>>ret.m_fRight>>ret.m_fBottom;
	}else{
		ret =qDefaultVal;
	}
}

void Literal::ToRectInt(const char *pText, RectInt &ret, const RectInt &qDefaultVal)
{
	std::stringstream strstream;
	if(pText){
		strstream<<pText;
		strstream>>ret.m_sLeft>>ret.m_sTop>>ret.m_sRight>>ret.m_sBottom;
	}else{
		ret =qDefaultVal;
	}
}


void Literal::ToMat4x4(const char *pText, Mat4x4 &ret, const Mat4x4 &matDefaultVal)
{
	//AutoSync AutoSyncObj(&g_LiteralSyncObj);

	std::stringstream strstream;
	if(pText){
		strstream<<pText;
		strstream>>ret._11>>ret._12>>ret._13>>ret._14
				 >>ret._21>>ret._22>>ret._23>>ret._24
				 >>ret._31>>ret._32>>ret._33>>ret._34
				 >>ret._41>>ret._42>>ret._43>>ret._44;
	}else{
		ret =matDefaultVal;
	}
}


void Literal::ToMat4x3(const char *pText, Mat4x3 &ret, const Mat4x3 &matDefaultVal)
{
	//AutoSync AutoSyncObj(&g_LiteralSyncObj);

	std::stringstream strstream;
	if(pText){
		strstream<<pText;
		strstream>>ret._11>>ret._12>>ret._13
				 >>ret._21>>ret._22>>ret._23
				 >>ret._31>>ret._32>>ret._33
				 >>ret._41>>ret._42>>ret._43;
	}else{
		ret =matDefaultVal;
	}
}


const char *Literal::GetFormattedStr(const char *pFormatStr, ...)
{
	char *g_pTempBuf =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempBuf;

	if(!pFormatStr){ //若输入的字符串无效的话
		return "";
	}

	va_list arglist; //表示输入的参数列表
	va_start(arglist, pFormatStr);
	::vsnprintf(g_pTempBuf, _MAX_STR_LEN, pFormatStr, arglist);
	return g_pTempBuf;
}


const char *Literal::ToLower(const char *pText)
{	
	char *g_pTempBuf =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempBuf;

	cn::u32 i;
	if(!pText){
		return "";
	}
	for(i =0; i<_MAX_STR_LEN && pText[i]!=0; i++){
		if(pText[i]>=65 && pText[i]<=90){
			g_pTempBuf[i] =pText[i]+32; //将大写字母转换为小写字母
		}else{
			g_pTempBuf[i] =pText[i];
		}
	}
	g_pTempBuf[i] =0;
	return g_pTempBuf;
}


const char *Literal::ToUpper(const char *pText)
{
	char *g_pTempBuf =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempBuf;

	cn::u32 i;
	if(!pText){
		return "";
	}
	for(i =0; i<_MAX_STR_LEN && pText[i]!=0; i++){
		if(pText[i]>=97 && pText[i]<=122){
			g_pTempBuf[i] =pText[i]-32; //将小写字母转换为大写字母
		}else{
			g_pTempBuf[i] =pText[i];
		}
	}
	g_pTempBuf[i] =0;
	return g_pTempBuf;
}


const char *Literal::WCharToChar(const WCHAR *pText, cn::s32 sTextLen, cn::s32 *pNewTextLen ,cn::Buffer *lpDestBuf /*=NULL*/)
{	
	//cn::u32 i, j;
	if(!pText){
		return "";
	}
	if(sTextLen < 0){
		sTextLen =::wcslen(pText);
	}

	char *pTempBuf = NULL;
	cn::s32 sCharacterNum = 0;

	if (NULL == lpDestBuf){
		pTempBuf =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempBuf;
		sCharacterNum =::WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, pText, sTextLen, pTempBuf, _MAX_STR_LEN, NULL, NULL);

	}else{
		cn::s32 sWantedBufSize =::WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,pText,sTextLen,NULL,0,NULL,NULL);
		if (sWantedBufSize > 0){			
			lpDestBuf->Resize(sWantedBufSize + 1);		
			pTempBuf =(char*)lpDestBuf->GetData();
			sCharacterNum =::WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,pText,sTextLen,pTempBuf,sWantedBufSize,NULL,NULL);
		}	
	}
	if(sCharacterNum > 0){
		pTempBuf[sCharacterNum] =0;
	}
	if(pNewTextLen){
		*pNewTextLen =sCharacterNum;
	}
	return pTempBuf;
}


const WCHAR *Literal::CharToWChar(const char *pText, cn::s32 sTextLen, cn::s32 *pNewTextLen ,cn::Buffer *lpDestBuf /*=NULL*/)
{
	//cn::u32 i, j;
	if(!pText){
		return L"";
	}

	if(sTextLen < 0){
		sTextLen =::strlen(pText);
	}

	WCHAR *pTempBuf2 =NULL;
	cn::s32 sCharacterNum = 0;

	if(NULL == lpDestBuf){
		pTempBuf2 =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempBuf2;
		sCharacterNum =::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pText, sTextLen, pTempBuf2, _MAX_STR_LEN);

	}else{		
		cn::s32 sWantedBufSize =::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pText,sTextLen,NULL,0);
		if (sWantedBufSize > 0){
			lpDestBuf->Resize((sWantedBufSize+1)*sizeof(WCHAR));
			pTempBuf2 =(WCHAR*)lpDestBuf->GetData();
			sCharacterNum =::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pText, sTextLen, pTempBuf2, sWantedBufSize);
		}
	}
	if (sCharacterNum > 0){
		pTempBuf2[sCharacterNum] = 0;
	}
	if(pNewTextLen){
		*pNewTextLen =sCharacterNum;
	}
	return pTempBuf2;
}

const char *Literal::Utf8ToChar(const char *pText, cn::s32 sTextLen /*=-1*/, cn::s32 *pNewTextLen /*=NULL*/, cn::Buffer *lpDestBuf /*= NULL*/)
{
	//把utf-8编码转到ANSI编码，需要utf-16做中转
	if (NULL == pText || strlen(pText) == 0 ){
		if(pNewTextLen) pNewTextLen=0;
		if(lpDestBuf) lpDestBuf->Resize(0);
		return "";
	}
	if (sTextLen < 0){
		sTextLen = ::strlen(pText);
	}
	
	char *pTempBuf =NULL;

	cn::s32 utf16BufLength =MultiByteToWideChar(CP_UTF8,0,pText,sTextLen,NULL,0);

	if (utf16BufLength > 0)
	{
		std::vector<WCHAR> midBuffer(utf16BufLength+1);
		cn::s32 utf16CharCount = MultiByteToWideChar(CP_UTF8,0,pText,sTextLen,&midBuffer[0],utf16BufLength);
		midBuffer[utf16CharCount] = 0;
	
		cn::s32 sCharacterNum = 0;
		
		if (NULL == lpDestBuf){
			pTempBuf =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempBuf;
			sCharacterNum =::WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, &midBuffer[0], -1, pTempBuf, _MAX_STR_LEN, NULL, NULL);
		}else{
			cn::s32 sWantedBufSize =::WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,&midBuffer[0],-1,NULL,0,NULL,NULL);
			if (sWantedBufSize > 0){
				lpDestBuf->Resize(sWantedBufSize + 1);		
				pTempBuf =(char*)lpDestBuf->GetData();
				sCharacterNum =::WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,&midBuffer[0], -1,pTempBuf,sWantedBufSize,NULL,NULL);
			}
		}
		
		if(sCharacterNum > 0){
			pTempBuf[sCharacterNum] =0;
		}
		if(pNewTextLen){
			*pNewTextLen =sCharacterNum;
		}		
	}else{
		pTempBuf = "";
	}
	return pTempBuf;
}

const char *Literal::CharToUtf8(const char *pText, cn::s32 sTextLen /*=-1*/, cn::s32 *pNewTextLen /*=NULL*/, cn::Buffer *lpDestBuf /*= NULL*/)
{//把ANSI编码转到utf-8编码，需要utf-16做中转
	if (NULL == pText || strlen(pText) == 0 ){
		if(pNewTextLen) pNewTextLen=0;
		if(lpDestBuf) lpDestBuf->Resize(0);
		return "";
	}
	if (sTextLen < 0){
		sTextLen = ::strlen(pText);
	}

	char *pTempBuf =NULL;

	cn::s32 utf16BufLength = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pText,sTextLen,NULL,0); 

	if (utf16BufLength > 0){
		std::vector<WCHAR> midBuffer(utf16BufLength+1);
		cn::s32 utf16CharCount = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pText,sTextLen,&midBuffer[0],utf16BufLength);
		midBuffer[utf16CharCount] = 0;

		cn::s32 sCharacterNum = 0;


		if (NULL == lpDestBuf){
			pTempBuf =cn::TLSSingleton<DLL_TLS_DATA>::GetSingleObj()->m_g_pTempBuf;
			sCharacterNum =::WideCharToMultiByte(CP_UTF8, 0, &midBuffer[0], -1, pTempBuf, _MAX_STR_LEN, NULL, NULL);
		}else{
			cn::s32 sWantedBufSize =::WideCharToMultiByte(CP_UTF8,0,&midBuffer[0],-1,NULL,0,NULL,NULL);
			if (sWantedBufSize > 0){
				lpDestBuf->Resize(sWantedBufSize + 1);		
				pTempBuf =(char*)lpDestBuf->GetData();
				sCharacterNum =::WideCharToMultiByte(CP_UTF8,0,&midBuffer[0], -1,pTempBuf,sWantedBufSize,NULL,NULL);
			}
		}
		
		if(sCharacterNum > 0){
			pTempBuf[sCharacterNum] =0;
		}
		if(pNewTextLen){
			*pNewTextLen =sCharacterNum;
		}		
	}else{
		pTempBuf = "";
	}
	return pTempBuf;
}


//比较包含通配符的字符串pWildcardStr与普通字符串pStr是否匹配
bool Literal::MatchWildcardStr(const char *pWildcardStr, const char *pStr)
{
	if(pWildcardStr==NULL || pStr==NULL){
		return false;
	}
	while(pWildcardStr[0] != '\0'){
		switch(pWildcardStr[0])
		{
		case '*':
			{
				while(pWildcardStr[1] == '*'){pWildcardStr++;}
				if(pWildcardStr[1] == '\0'){
					return true;
				}else{
					for(; pStr[0]!='\0'; pStr++){
						if(Literal::MatchWildcardStr(pWildcardStr+1, pStr)){
							return true;
						}
					}
					return false;
				}
			}
			break;
		case '?':
			{
				if(pStr[0] != '\0'){
					pWildcardStr++; pStr++;
				}else{
					return false;
				}
			}
			break;
		default:
			{
				if(pWildcardStr[0] == pStr[0]){
					pWildcardStr++; pStr++;
				}else{
					return false;
				}
			}
			break;
		}
	}
	if(pStr[0] == '\0'){
		return true;
	}else{
		return false;
	}
}

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool is_base64(unsigned char c) {return (isalnum(c) || (c == '+') || (c == '/'));}
//base64编码
void Literal::Base64_Encode(const cn::u8* bytes_to_encode, cn::u32 ulen,cn::Buffer *lpDestBuf)
{
	/*
 11111100 0xFC
 00000011 0x3
 11110000 0xF0
 00001111 0xF
 11000000 0xC0
 00111111 0x3F
 */
	std::string ret; int i=0, j=0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	if(bytes_to_encode == NULL || ulen<=0 || lpDestBuf==NULL){
		return;
	}

	while (ulen--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;
			for(i = 0; (i <4) ; i++){
				ret += base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}
	if (i){
		for(j = i; j < 3; j++){
			char_array_3[j] = '\0';
		}
		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;
		for (j = 0; (j < i + 1); j++){
			ret += base64_chars[char_array_4[j]];
		}
		while((i++ < 3)){
			ret += '=';
		}
	}
	lpDestBuf->Resize(ret.length());
	char* pData = (char*)lpDestBuf->GetData();
	for (i=0;i<ret.length();++i){pData[i] = ret.at(i);}
}

//base64解码
void Literal::Base64_Decode(const char* str_to_decode,cn::u32 ulen,cn::Buffer *lpDestBuf)
{
	int in_len = ulen;
	int i = 0 , j=0, in_=0;

	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && ( str_to_decode[in_] != '=') && is_base64(str_to_decode[in_])) {
		char_array_4[i++] = str_to_decode[in_]; in_++;
		if (i ==4) {
			for (i = 0; i <4; i++){
				char_array_4[i] = base64_chars.find(char_array_4[i]);
			}
			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
			for (i = 0; (i < 3); i++){
				ret += char_array_3[i];
			}
			i = 0;
		}
	}
	if (i) {
		for (j = i; j <4; j++){
			char_array_4[j] = 0;
		}
		for (j = 0; j <4; j++){
			char_array_4[j] = base64_chars.find(char_array_4[j]);
		}
		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
		for (j = 0; (j < i - 1); j++) {ret += char_array_3[j];}
	}
	lpDestBuf->Resize(ret.length());		
	cn::u8 * pData = (cn::u8*)lpDestBuf->GetData();
	for(i=0;i<ret.length();++i){	pData[i]=ret.at(i);	}
}





}