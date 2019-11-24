#pragma once
#include <string>
#include <vector>
#include <osgGe/Export>

using namespace std;
namespace osg {
class OSGGE_EXPORT StringTools
{
public:

static void SplitString(const string& s, vector<string>& v, const string& c);

static string GetTheNameFromPath(string & filepath);

static string GetFilePathWithoutName(string & filepath);

static string ReplaceSpaces(string & str, const string & oldstr, const string & newstr);

static void GetAllFormatFiles(string path, vector<string>& files, string format);

static void Makedirpath(string _filepath);

static string WString2String(const std::wstring& ws);

static std::wstring String2WString(const std::string& s);
};

}


