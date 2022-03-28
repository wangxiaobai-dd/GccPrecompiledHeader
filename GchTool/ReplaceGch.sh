#!/bin/bash

# 脚本将analyseInc前N头文件从cpp文件剔除，并且在这些cpp文件首行插入inc.h

if [ $# -ne 2 ];then
	echo 脚本用法,请输入路径及处理数目 e.g. : ./ReplaceGCH.sh dirA/ 10
	exit
fi

dir=${1}
num=${2}

if [[ "$dir" != */ ]]; then
	dir="$dir"/; 
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
	grep  "${line}" "$dir"*.cpp -w  >> temp2
done

awk -F ":" '{print $1}' temp2 > temp3
awk -F "/" '{print $NF}' temp3 > temp2
sort -u temp2 > temp3

#temp3: 受影响的cpp文件集合

# 插入新文件
for line in $(cat temp3)
do
	./InsertInc ""$dir"${line}" "inc.h"
done

# 删除旧文件include
for line in $(cat temp3)
do
	for del in ${content}
	do
		sed -i '/'${del}'/d' "$dir"/${line}
	done
done

# 创建预编译inc.h 头文件
echo 开始创建"$dir"inc.h 文件...
if [ -e "$dir"inc.h ];then
	echo 删除旧inc.h文件
	rm -f "$dir"inc.h
fi

echo \#ifndef _INC_H_ > "$dir"inc.h
echo \#define _INC_H_ >> "$dir"inc.h
echo >> "$dir"inc.h

for line in ${content}
do
	echo "${line}" >> "$dir"inc.h
done

echo >> "$dir"inc.h
echo \#endif >> "$dir"inc.h
sed 's/\./\ /' "$dir"inc.h > "$dir"inc.h.tmp
cp "$dir"inc.h.tmp "$dir"inc.h
rm "$dir"inc.h.tmp

rm -rf temp1 temp2 temp3 cppset
printf "**************结束处理*********\n"
