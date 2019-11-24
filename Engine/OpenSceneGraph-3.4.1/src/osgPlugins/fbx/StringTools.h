#pragma once
#include <string>
#include "fbxsdk.h"

using namespace std;

class StringTools
{
public:
	StringTools();
	~StringTools();
public:
	static string FbxStr2StdStr(FbxString fbxStr);
};

