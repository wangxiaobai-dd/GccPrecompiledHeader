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

using namespace std;

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		cout << "请输入路径和预编译头文件名: ./RevertInc TestRevertTool/dirA inc.h" << endl;
		return 1;
	}
	string dir = argv[1];
	string incFileName = argv[2];

	ifstream incIs{dir + "/" + incFileName};
	if(!incIs.is_open())
	{
		cout << incFileName << " 不存在" << endl;
		return 1;
	}
	vector<string> incContentVec;
	string includeStr;
	while(getline(incIs, includeStr))
	{
		if(includeStr.find("#include") == string::npos)
			continue;
		cout << includeStr << endl;
		incContentVec.push_back(includeStr);
	}
	incIs.close();

	const filesystem::path path{dir};
	for(auto const& dirEntry : filesystem::directory_iterator{path}) 
	{
		string fileName = dirEntry.path();
		if(fileName.find(".cpp") == string::npos)
			continue;
		ifstream is{fileName};
		if(is.is_open())
		{
			cout << "isopen:" << fileName << endl;
			string data;
			fstream os{fileName + ".bak", ios::out | ios::trunc};
			bool findInc = false;
			while(getline(is, data))
			{
				if(data.find(incFileName) != string::npos)
				{
					// replace
					findInc = true;
					for(auto line : incContentVec)
						os << line << endl;
					continue;
				}
				if(!findInc && data.find("#include") != string::npos)
					break;
				os << data << endl;
			}
			if(findInc)
			{
				// overwrite
				filesystem::copy(fileName + ".bak", fileName, filesystem::copy_options::overwrite_existing);
				filesystem::remove(fileName + ".bak");
			}
			is.close();
			os.close();
		}
	}
	
	return 0;
}

