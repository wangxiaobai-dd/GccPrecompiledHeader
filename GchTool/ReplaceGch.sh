#!/bin/bash

# 脚本将analyseInc前N头文件从cpp文件剔除，并且在这些cpp文件首行插入inc.h

if [ $# -ne 2 ];then
	echo 脚本用法,请输入路径及处理数目 e.g. : ./ReplaceGCH.sh dirA/ 10
	exit
fi

printf "**************开始处理*********\n"
# 处理频率前N头文件
# include <xx.h> 替换为 include.<xx.h>
sed 's/\ /./' analyseInc.txt > temp1
content=$(sed -n 1,${2}p temp1)
if [ -e temp2 ]; then
	rm temp2
fi
echo ${content}
for line in ${content}
do
	grep  "${line}" ${1}*.cpp -w  >> temp2
done

awk -F ":" '{print $1}' temp2 > temp3
awk -F "/" '{print $NF}' temp3 > temp2
sort -u temp2 > temp3

#temp3: 受影响的cpp文件集合

# 插入新文件
for line in $(cat temp3)
do
	./InsertInc "${1}${line}"
done

# 删除旧文件include
for line in $(cat temp3)
do
	for del in ${content}
	do
		sed -i '/'${del}'/d' ${1}/${line}
	done
done

# 创建预编译inc.h 头文件
echo 开始创建${1}inc.h 文件...
if [ -e ${1}inc.h ];then
	echo 删除旧inc.h文件
	rm -f ${1}inc.h
fi

echo \#ifndef _INC_H_ > ${1}inc.h
echo \#define _INC_H_ >> ${1}inc.h
echo >> ${1}inc.h

for line in ${content}
do
	echo "${line}" >> ${1}inc.h
done

echo >> ${1}inc.h
echo \#endif >> ${1}inc.h
sed 's/\./\ /' ${1}inc.h > ${1}inc.h.tmp
cp ${1}inc.h.tmp ${1}inc.h
rm ${1}inc.h.tmp

rm -rf temp1 temp2 temp3 cppset
printf "**************结束处理*********\n"
