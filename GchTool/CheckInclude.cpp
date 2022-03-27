
/*
 * 分析目录下cpp包含.h 包含频率
 * analyseInc.txt 供 ReplaceGCH.sh 使用 
 * analyseInc2.txt 便于我们查看头文件包含频率
 *
 * 使用方法: ./CheckInclude CheckDir FindDirA FindDirB ...
 * v2: 递归查找, 按照编译单元统计(cpp)
 */

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <cstdlib>
#include <string.h>
#include <map>
#include <algorithm>
#include <filesystem>
#include <variant>

using namespace std;

const string INCLUDE = "#include";
const string SPACE = " ";
set<string> findDirSet;

void FindInclude(string header, set<string>& recordedSet, map<string, int>& countMap);
void ReadFile(string filePath, std::set<string>& recordedSet, std::map<string, int>& countMap);

string GetHeader(string lineStr)
{
	if(lineStr.find(INCLUDE) != string::npos)
	{
		// 只包含#include <> ""
		auto pos = lineStr.find("\"");
		if(pos != string::npos)
		{
			lineStr = lineStr.substr(pos);
			pos = lineStr.rfind("\"");
			lineStr = lineStr.substr(0, pos+1);
		}
		else
		{
			pos = lineStr.find("<");
			lineStr = lineStr.substr(pos);
			pos = lineStr.find(">");
			lineStr = lineStr.substr(0, pos+1);
		}
		return lineStr;
	}
	else 
		return "";
}

string GetFilePath(string header)
{
	auto str = header;
	auto pos = str.find("\"");
	string result;
	// 项目头文件
	if(pos != string::npos)
	{
		str = str.substr(pos + 1);
		pos = str.rfind("\"");
		if(pos != string::npos)
			str = str.substr(0, pos);
		else 
		{
			cout << "get file path error: " << header << endl; 
			return result;
		}
		for(auto dir : findDirSet)
		{
			filesystem::path p{dir + "/" + str};	
			if(filesystem::exists(p) && filesystem::is_regular_file(p))
			{
				result = dir + "/" + str;
				break;
			}
		}
	}
	return result;
}

/*
 * recordedSet 本编译单元已经记录过
 * 本编译单元递归查找头文件
 * header: "a.h" 或 <iostream>
 */
void FindInclude(string header, set<string>& recordedSet, map<string, int>& countMap)
{
	if(!recordedSet.insert(header).second)
		return;
	cout << "FindInclude:" << header << endl;

	auto filePath = GetFilePath(header);
	// header: "a.h"
	if(filePath.size())
	{
		ReadFile(filePath, recordedSet, countMap);
	}
}

void ReadFile(string filePath, std::set<string>& recordedSet, std::map<string, int>& countMap)
{
	ifstream in(filePath);
	if(in.is_open())
	{
		string lineStr;
		while(getline(in, lineStr))
		{
			if(lineStr.find(".cpp") != string::npos || lineStr.find("//") != string::npos)\
				continue;
			auto header = GetHeader(lineStr); // "a.h"
			if(header.size() && !recordedSet.contains(header))
			{
				auto include = INCLUDE + SPACE + header; // #include "a.h" 
				countMap[include] += 1;
				//cout << "add_A:" << include << " from file:" << filePath << " count:" << countMap[include] << endl;
				FindInclude(header, recordedSet, countMap);
			}
		}
		in.close ();
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "请输入路径 ./CheckInclude CheckDir [FindDirA] [FindDirB] ..." << endl;
		return 0;
	}
	string checkDir = argv[1];
	if(checkDir.ends_with("/"))
		checkDir = checkDir.substr(0, checkDir.size() - 1);
	findDirSet.insert(checkDir);
	for(int i = 2; i < argc; i++)
	{
		string dir = argv[i];
		if(dir.ends_with("/"))
			dir = dir.substr(0, dir.size() - 1);
		findDirSet.insert(dir);
	}

	string execStr = "ls ";
	execStr += checkDir;
	execStr += "/*.cpp > cppset";
	system(execStr.c_str());

	ifstream in("cppset");
	string fileNameStr;
	vector<string> fileVec;

	while(getline(in, fileNameStr))
		fileVec.push_back(fileNameStr);
	in.close();

	map<string, int> countMap;
	for(const auto& fileName : fileVec)	// a.cpp a2.cpp
	{
		cout << "\n---" << fileName << "---" << endl;
		set<string> recordedSet;
		ReadFile(fileName, recordedSet, countMap);
	}

	vector<pair<string,int>> countVec(countMap.begin(), countMap.end());
	sort(countVec.begin(), countVec.end(), [](const auto& k1, const auto& k2) { return k1.second > k2.second; });
	ofstream out("analyseInc.txt");
	ofstream out2("analyseInc2.txt");
	for(const auto& item : countVec)
		out << item.first << endl;
	for(const auto& item : countVec)
		out2 << item.first <<"频率: " << item.second << endl;
	out.close();
	out2.close();
	system("rm -f cppset");
	return 0;
}
