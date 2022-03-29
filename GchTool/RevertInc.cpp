/* 
 * Copyright (C) 2022 Meepo All Rights Reserved.
 * 
 * File Name     : RevertInc.cpp
 * Author        : Meepo
 * Creation Date : 2022-03-20
 * INFO          : 将 include预编译头文件的cpp 还原 
 *
 */

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <regex>

using namespace std;

const string INCLUDE = "#include";

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		cout << "请输入路径和预编译头文件名: ./RevertInc TestRevertTool/dirA inc.h [unified]" << endl;
		return 1;
	}
	string invokePath = filesystem::path{argv[0]}.parent_path();
	string dir = argv[1];
	string incFileName = argv[2];
	string unifiedPrefix;	// 处理联合编译单元 一个大的cpp包含多个cpp
	if(argc == 4)
		unifiedPrefix = argv[3];

	ifstream incIs{dir + "/" + incFileName};
	if(!incIs.is_open())
	{
		cout << incFileName << " 不存在" << endl;
		return 1;
	}
	vector<string> incContentVec;
	string includeStr;
	cout << "inc.h 包含:"<< endl;
	while(getline(incIs, includeStr))
	{
		if(includeStr.find(INCLUDE) == string::npos)
			continue;
		cout << includeStr << endl;
		incContentVec.push_back(includeStr);
	}
	incIs.close();

	const filesystem::path path{dir};
	if(!unifiedPrefix.empty())
	{
		cout << "========== 处理联合编译单元 ==========" << endl;
		string exec = invokePath + "/InsertInc";
		regex reg(".*\"(.*)\"");
		for(auto const& dirEntry : filesystem::directory_iterator{path})
		{
			string fileName = dirEntry.path(); // TestRevertTool/dirA/unified_file.cpp
			if(fileName.find(unifiedPrefix) == string::npos || fileName.find(".swp") != string::npos)
				continue;

			set<string> unifiedFileSet;
			ifstream is{fileName};
			string line;
			bool findInc = false;
			while(getline(is, line))
			{
				if(line.find(incFileName) != string::npos)
				{
					findInc = true;
					continue;
				}
				if(!findInc && line.find(INCLUDE) != string::npos)
					break;
				cmatch m;
				bool ret = std::regex_match(line.c_str(), m, std::regex(".*\"(.*)\""));
				if(ret && m.size() == 2)
					unifiedFileSet.insert(m.str(1));
			}

			for(auto file : unifiedFileSet)
				system((exec + " " + dirEntry.path().parent_path().string() + "/" + file + " " + incFileName).c_str());

			is.close();
		}
	}
	cout << "========== 还原预编译头文件 ==========" << endl;

	for(auto const& dirEntry : filesystem::directory_iterator{path}) 
	{
		string fileName = dirEntry.path();
		if(fileName.find(".cpp") == string::npos || fileName.find(".swp") != string::npos)
			continue;
		ifstream is{fileName};
		if(is.is_open())
		{
			cout << "modify file:" << fileName << endl;
			string line;
			fstream os{fileName + ".tmp", ios::out | ios::trunc};
			bool findInc = false;
			while(getline(is, line))
			{
				// inc.h
				if(line.find(incFileName) != string::npos)
				{
					// 替换预编译头文件
					findInc = true;
					if(unifiedPrefix.empty() || fileName.find(unifiedPrefix) == string::npos)
					{
						for(auto line : incContentVec)
							os << line << endl;
					}
					continue;
				}
				if(!findInc && line.find(INCLUDE) != string::npos)
					break;
				// 文件其他内容
				os << line << endl;
			}
			if(findInc)
			{
				// 覆盖
				filesystem::copy(fileName + ".tmp", fileName, filesystem::copy_options::overwrite_existing);
			}
			filesystem::remove(fileName + ".tmp");
			is.close();
			os.close();
		}
	}
	
	return 0;
}

