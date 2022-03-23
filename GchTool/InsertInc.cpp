
/*
 * ReplaceGch.sh 调用
 * 在cpp文件中首行插入inc.h文件
 */

#include <iostream> 
#include <fstream> 
#include <set> 
#include <vector> 
#include <cstdlib> 
#include <algorithm>

int main (int argc, char* argv[])
{
	if(argc < 3)
	{
		std::cout << "参数错误 ./InsertInc fileName inc.h" << std::endl;
		return 1;
	}
	std::string fileName = argv[1]; 
	std::string incName = argv[2];
	std::vector<std::string> fileVec;
	{
		std::ifstream in(fileName, std::ios::in);
		std::string lineStr;
		while(getline(in, lineStr))
			fileVec.push_back(lineStr);
		in.close();
	}
	std::string inStr {"#include"};
	auto iter = std::find_if(fileVec.begin(), fileVec.end(), [&inStr](const auto& str) { return str.find(inStr) != std::string::npos; });

	std::string fileBak = fileName + ".bak"; 
	std::string incStr = "#include \"" + incName + "\""; 
	std::fstream out(fileBak, std::ios::out | std::ios::trunc);
	// 最先#include inc.h
	if(iter != fileVec.end())
	{
		fileVec.insert(iter, incStr); 
	}
	else
	{
		std::cout << argv[1] << "没有include" << std::endl;
		auto iter = std::find_if(fileVec.begin(), fileVec.end(), [&inStr](const auto& str) { return str.find("*") == std::string::npos && str.find("//") == std::string::npos; });
		if(iter != fileVec.end() && ++iter != fileVec.end())
			fileVec.insert(iter, incStr);
	}
	for(const auto& str : fileVec) 
		out << str << std::endl; 
	std::string exec = "cp ";
	exec += fileBak; 
	exec += " ";
	exec += fileName; 
	exec += ";rm "; 
	exec += fileBak; 
	out.close();
	system(exec.c_str());
	return 0;
}
