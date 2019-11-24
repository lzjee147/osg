
#include "PreDefine.h"




namespace cn
{

	extern std::map<std::string, cn::Ref/*Str*/> g_mapGlobalStrMap; 

}


BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpReserved )
{
	switch( fdwReason ) 
	{ 
	case DLL_PROCESS_ATTACH:
		{
			cn::Time::GetAbsTime();
			cn::Time::GetRelTime();
			cn::WebDownload::InitWebDownload();
			cn::TLSSingleton<DLL_TLS_DATA>::InitTLSIndex();
			cn::TLSSingleton<DLL_TLS_DATA>::InitTLSMem();
		}
		break;

	case DLL_THREAD_ATTACH:
		{
			cn::TLSSingleton<DLL_TLS_DATA>::InitTLSMem();
		}
		break;

	case DLL_THREAD_DETACH:
		{
			cn::TLSSingleton<DLL_TLS_DATA>::ReleaseTLSMem();
		}
		break;

	case DLL_PROCESS_DETACH:
		{
			std::map<std::string, cn::Ref/*Str*/> mapTemp;
			cn::g_mapGlobalStrMap.swap(mapTemp);

			cn::TLSSingleton<DLL_TLS_DATA>::ReleaseTLSMem();
			cn::TLSSingleton<DLL_TLS_DATA>::ReleaseTLSIndex();
			cn::WebDownload::CleanupWebDownload();
//::MessageBox(NULL, "Common DLL detach", "", MB_OK);
		}
		break;
	}
	return TRUE;
}