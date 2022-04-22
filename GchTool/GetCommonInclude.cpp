/* 
 * Copyright (C) 2022 Meepo All Rights Reserved.
 * 
 * File Name     : GetCommonInclude.cpp
 * Author        : Meepo
 * Creation Date : 2022-04-10
 * INFO          : 通过多个目录的analyseInc2.txt-* 得到 大于某频率的 共同include
 * USE			 : ./GetCommonInclude --files analyseInc-1.txt,analyseInc-2.txt --base dirA,dirB 逗号分割参数
 *					files: 频率文件 
 *					base: base或者三方目录
 */


#include <iostream>
#include <string>
#include <getopt.h>
#include <stdio.h>
#include <vector>
#include <filesystem>
#include <string.h>
#include <map>
#include <regex>
#include <fstream>

using namespace std;

static struct option options[] = 
{
	{"files", required_argument, NULL, 'f'},
	{"base", required_argument, NULL, 'b'},
	{"rate", required_argument, NULL, 'r'},
	{0, 0, 0, 0}
};

const char* optstring = "f:b:r:";  

vector<string> fileVec;		// analyseInc2 频率文件
vector<string> baseVec;		// 公共依赖文件目录

vector<string> GetArgs(char* optarg)
{
	vector<string> result;
	auto token = strtok(optarg, ",");		
	while(token != nullptr)
	{
		result.push_back(token);
		token = strtok(NULL, ",");
	}
	return result;
}

bool isBaseFile(string includeStr)
{
	cmatch m;
	bool ret = std::regex_match(includeStr.c_str(), m, std::regex(".*\"(.*)\""));
	if(ret && m.size() == 2)
	{
		for(auto dir : baseVec)
		{
			filesystem::path p{dir + "/" + m.str(1)};
			if(filesystem::exists(p) && filesystem::is_regular_file(p))
				return true;
		}
	}
	return false;
}

int main(int argc, char* argv[])
{
	int optionIndex = 0;
	int option = 0;
	float rate = 0.9f;
	while(EOF != (option = getopt_long(argc, argv, optstring, options, &optionIndex)))
	{
		if(option == 'f')
		{
			for(auto file : GetArgs(optarg))
			{
				filesystem::path p{file};
				if(filesystem::exists(p) && filesystem::is_regular_file(p))
					fileVec.push_back(file);
				else
				{
					cout << file << " file doesn't exit!" << endl;
					//return 1;
				}
			}
		}
		else if(option == 'b')
		{
			for(auto dir : GetArgs(optarg))
			{
				filesystem::path p{dir};
				if(filesystem::exists(p) && filesystem::is_directory(p))
				{
					if(dir.ends_with("/"))
						dir = dir.substr(0, dir.size() - 1);
					baseVec.push_back(dir);
				}
				else 
				{
					cout << dir << " directory doesn't exit!" << endl;
					//return 1;
				}
			}
		}
		else if(option == 'r')
		{
			rate = stof(optarg);
		}
	}
	cout << "rate:" << rate << endl;

	if(fileVec.size() < 3 || baseVec.empty())
	{
		cout << "至少两个analyseInc2.txt 和 baseDir" << endl;
		cout << "使用方法: ./GetCommonInclude --files analyseInc-1.txt,analyseInc-2.txt --base dirA,dirB 逗号分割参数 --rate 0.9" << endl;
		// return 1;
	}

	std::map<string, int> countMap;
	regex reg("(.*)\\t\\tCOUNT.*RATE:(\\d+\\.?\\d*)\\t\\t.*");
	for(auto file : fileVec)
	{
		ifstream in(file);
		if(in.is_open())
		{
			string line;
			while(getline(in, line))
			{
				cmatch m;
				bool ret = std::regex_match(line.c_str(), m, reg);
				if(ret && stof(m.str(2)) >= rate)
				{
					// m.str(1) : #include <iostream> 
					if((line.find("<") != string::npos && line.find(">") != string::npos) || isBaseFile(m.str(1)))
					{
						countMap[m.str(1)] += 1;
						//cout << m.str(1) << " " << m.str(2) << endl;
					}
				}
			}
		}
		else
			cout << "open failed" << endl;
	}

	cout << "result:" << endl;
	for(const auto [include, count] : countMap)
	{
		cout << include << " count:" << count << endl;
	}
	
	ofstream out("AnalyseFinal.txt");
	for(const auto [include, count] : countMap)
	{
		if(count == fileVec.size())
			out << include << endl;
		else if(count > fileVec.size())
			cout << "count error" << endl;
	}
	
	return 0;
}

