
/*
 * 分析目录下cpp包含.h 包含频率
 * analyseInc.txt 供 ReplaceGCH.sh 使用 
 * analyseInc2.txt 便于我们查看头文件包含频率
 *
 * 使用方法: ./CheckInclude YourDir
 */

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <cstdlib>
#include <string.h>
#include <map>
#include <algorithm>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "请输入路径" << std::endl;
		return 0;
	}
	std::string execStr = "ls ";
	execStr += argv[1];
	execStr += "/*.cpp > cppset";
	system(execStr.c_str());
	
	std::ifstream in("cppset");
	std::string fileNameStr;
	std::vector<std::string> fileVec;

	while(getline(in, fileNameStr))
		fileVec.push_back(fileNameStr);
	in.close();

	std::string findStr = "#include";
	std::ofstream out("analyseInc.txt");
	std::ofstream out2("analyseInc2.txt");
	std::map<std::string, int> countMap;
	for(const auto& fileName : fileVec)
	{
		std::ifstream in(fileName);
		if(in.is_open())
		{
			std::string lineStr;
			while(getline(in, lineStr))
			{
				if(lineStr.find(".cpp") != std::string::npos)
					continue;
				if(lineStr.find(findStr) != std::string::npos)
				{
					// 只包含#include <> ""
					auto pos = lineStr.find("\"");
					if(pos != std::string::npos)
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
					lineStr = findStr + " " + lineStr;
					countMap[lineStr] += 1;
				}
			}
			in.close ();
		}
	}
	std::vector<std::pair<std::string,int>> countVec(countMap.begin(), countMap.end());
	std::sort(countVec.begin(), countVec.end(), [](const auto& k1, const auto& k2) { return k1.second > k2.second; });
	for(const auto& item : countVec)
		out << item.first << std::endl;
	for(const auto& item : countVec)
		out2 << item.first <<"频率: " << item.second << std::endl;
	out.close();
	out2.close();
	return 0;
}
