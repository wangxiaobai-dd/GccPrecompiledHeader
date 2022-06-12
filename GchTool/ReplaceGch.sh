#!/bin/bash

# 脚本将analyseInc前N头文件从cpp文件剔除，并且在这些cpp文件首行插入${GchHeader}

if [[ $# < 1 ]];then
	echo 脚本用法,请输入路径及处理数目 e.g. : ./ReplaceGch.sh dirA/ 10
	exit
fi


dir=${1}
num=${2}

if [[ "$dir" != */ ]]; then
	dir="$dir"/; 
fi

GchHeader=inc.h
GchFile=${dir}${GchHeader}
AnalyseFile=AnalyseFinal.txt
rm -rf temp2

printf "**************开始处理*********\n"

# 处理频率前N头文件
if [[ $num == "" ]];then 
	content=`cat ${AnalyseFile}`
else
	content=$(sed -n 1,"$num"p ${AnalyseFile})
fi
echo ${content}
IFS=$'\n' # 对所有cpp包含inc.h 可以注释这行
for line in ${content}
do
	echo "${line}"
	grep  "${line}" "$dir"*.cpp -w  >> temp2
done

awk -F ":" '{print $1}' temp2 > temp3
awk -F "/" '{print $NF}' temp3 > temp2
sort -u temp2 > temp3

# temp3: 受影响的cpp文件集合
# 插入新文件
for line in $(cat temp3)
do
#	echo 
	./InsertInc ""$dir"${line}" ${GchHeader}
done

# 删除旧文件include
for line in $(cat temp3)
do
	for del in ${content}
	do
		#echo ${del} "$dir"${line}
		sed -i "\~${del}~d" "$dir"${line}
	done
done

# 创建预编译${GchFile} 头文件
echo 开始创建${GchFile} 文件...
if [ -e ${GchFile} ];then
	echo 删除旧${GchFile}文件
	rm -f ${GchFile}
fi

echo \#ifndef _INC_H_ > ${GchFile}
echo \#define _INC_H_ >> ${GchFile}
echo >> ${GchFile}

for line in ${content}
do
	IFS=$'\n'
	echo ${line}
	echo ${line} >> ${GchFile}
done

echo >> ${GchFile}
echo \#endif >> ${GchFile}

#rm -rf temp1 temp2 temp3 cppset

printf "**************结束处理*********\n"
