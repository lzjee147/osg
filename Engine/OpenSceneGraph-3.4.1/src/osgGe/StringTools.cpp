#include "OsgGe/StringTools.h"
#include <io.h> 
//#include "shlwapi.h"
//#pragma comment(lib,"shlwapi.lib") //linux_lzj

using namespace osg;


void StringTools::SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

string StringTools::GetTheNameFromPath(string& filepath)
{
	if (filepath.empty())
	{
		return "";
	}
	std::size_t pos = filepath.find_last_of("\\");

	if (pos == string::npos)
	{
		pos = filepath.find_last_of("//");
	}

	std::string _filename = filepath.substr(pos + 1);
	int lendth = _filename.length();
	_filename = _filename.substr(0, lendth - 4);
	
	return _filename;
}

string StringTools::GetFilePathWithoutName(string& filepath)
{
	if (filepath.empty())
	{
		return "";
	}
	std::size_t pos = filepath.find_last_of("\\");
	string _filepathnoname = filepath.substr(0, pos);

	return _filepathnoname;
}

string StringTools::ReplaceSpaces(string &str, const string& oldstr, const string& newstr)
{
	while (true) {
		string::size_type   pos(0);
		if ((pos = str.find(oldstr)) != string::npos)
			str.replace(pos, oldstr.length(), newstr);
		else   break;
	}
	return   str;
}

void StringTools::GetAllFormatFiles(string path, vector<string>& files, string format)
{
	//文件句柄    
	long   hFile = 0;
	//文件信息    
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
					GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

void StringTools::Makedirpath(string _filepath)
{
	//if (!PathIsDirectory(_filepath.c_str()))
	//{
	//	CreateDirectory(_filepath.c_str(), NULL);
	//}
}

std::string StringTools::WString2String(const std::wstring& ws)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
	char *chDest = new char[nDestSize];
	memset(chDest,0,nDestSize);
	wcstombs(chDest,wchSrc,nDestSize);
	std::string strResult = chDest;
	delete []chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
}

std::wstring StringTools::String2WString(const std::string& s)
{
	std::string strLocale = setlocale(LC_ALL, ""); 
	const char* chSrc = s.c_str();
	size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
	wchar_t* wchDest = new wchar_t[nDestSize];
	wmemset(wchDest, 0, nDestSize);
	mbstowcs(wchDest,chSrc,nDestSize);
	std::wstring wstrResult = wchDest;
	delete []wchDest;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
}
