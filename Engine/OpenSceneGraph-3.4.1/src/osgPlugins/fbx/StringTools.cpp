#include "StringTools.h"


StringTools::StringTools()
{
}


StringTools::~StringTools()
{
}


string StringTools::FbxStr2StdStr(FbxString fbxStr)
{
	string retStr = "";

	char * newStr = NULL;
	FbxUTF8ToAnsi(fbxStr.Buffer(), newStr);
	if (NULL != newStr)
	{
		retStr = newStr;
		delete[] newStr;
	}

	return retStr;
}