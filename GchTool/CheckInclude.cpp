
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

using namespace std;

const string INCLUDE = "#include";
const string SPACE = " ";

set<string> findDirSet;
map<string, int> countMap;
map<string, string> headerSourceMap;

string MergeInclude(string header);
void FindInclude(string header, set<string>& recordedSet, string fromFile);
void ReadFile(string filePath, set<string>& recordedSet, string fromFile);

string MergeInclude(string header)
{
	return INCLUDE + SPACE + header; // #include "a.h"
}

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
			if(pos != string::npos)
				lineStr = lineStr.substr(0, pos+1);
		}
		else
		{
			pos = lineStr.find("<");
			if(pos != string::npos)
			{
				lineStr = lineStr.substr(pos);
				pos = lineStr.find(">");
				if(pos != string::npos)
					lineStr = lineStr.substr(0, pos+1);
			}
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
void FindInclude(string header, set<string>& recordedSet, string fromFile)
{
	if(!recordedSet.insert(header).second)
		return;
	headerSourceMap[MergeInclude(header)] = fromFile;
	// cout << "FindInclude:" << header << endl;

	auto filePath = GetFilePath(header);
	// header: "a.h"
	if(filePath.size())
	{
		fromFile = filePath;
		ReadFile(filePath, recordedSet, fromFile);
	}
}

void ReadFile(string filePath, set<string>& recordedSet, string fromFile)
{
	ifstream in(filePath);
	if(in.is_open())
	{
		string lineStr;
		while(getline(in, lineStr))
		{
			if(lineStr.find(".cpp") != string::npos || lineStr.find("//#") != string::npos)
				continue;
			auto header = GetHeader(lineStr); // "a.h"
			if(header.size() && !recordedSet.contains(header))
			{
				auto include = MergeInclude(header); 
				countMap[include] += 1;
				//cout << "add_A:" << include << " from file:" << filePath << " count:" << countMap[include] << endl;
				FindInclude(header, recordedSet, fromFile);
			}
		}
		in.close();
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "请输入路径 ./CheckInclude CheckDir [FindDirA] [FindDirB] ..." << endl;
		return 0;
	}
	cout << "开始" << endl;
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

	int cppCount = 0;
	for(const auto& fileName : fileVec)	// a.cpp a2.cpp
	{
		++cppCount;
		// cout << "\n---" << fileName << "---" << endl;
		set<string> recordedSet;
		ReadFile(fileName, recordedSet, fileName);
	}

	vector<pair<string,int>> countVec(countMap.begin(), countMap.end());
	sort(countVec.begin(), countVec.end(), [](const auto& k1, const auto& k2) { return k1.second > k2.second; });
	filesystem::path checkDirPath{checkDir};
	string outFile1 = "analyseInc.txt-" + checkDirPath.filename().string();
	string outFile2 = "analyseInc2.txt-" + checkDirPath.filename().string();
	ofstream out(outFile1);
	ofstream out2(outFile2);
	for(const auto& item : countVec)
		out << item.first << endl;
	out2 <<"目录下 cpp 数目: " << cppCount << "\n" << endl;
	for(const auto& item : countVec)
	{
		out2 << item.first <<"\t\tCOUNT:" << item.second << "\t\tRATE:" << (float)item.second / cppCount << "\t\tFrom:" << headerSourceMap[item.first] << endl;
	}
	out.close();
	out2.close();
	system("rm -f cppset");
	cout << "完成，生成 " << outFile1 << "    " << outFile2 << endl; 
	return 0;
}
