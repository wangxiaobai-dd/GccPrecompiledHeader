#!/usr/bin/python 
# -*- coding: UTF-8 -*-

# 预编译头可用性检查
# 检查.gch 

import re
import os
import sys

pattern = re.compile(r'include\s".*"')

COMMONDIR = "common/"       # 公共库
TMPFILE = "tmpINCfile"

fileDict = {}

# str: include "a.h" 
# return: a.h
def checkSelfHead(str):
    head = pattern.search(str)
    if head:
        return head.group().replace("\"","").replace("include","").replace(" ","")


# 从inc.h 递归获取包含的头文件
def readFileHead(name, path, headSet):
    with open(name, 'r') as f:
        for line in f:
            head = checkSelfHead(line)
            if head:
                fullPath = ""
                if os.path.isfile(path + "/" + head):
                    fullPath = path + "/" + head
                elif os.path.isfile(COMMONDIR + head):
                    fullPath = COMMONDIR + head
                if (fullPath != "") and (fullPath not in headSet):
                    headSet.add(fullPath)
                    readFileHead(fullPath, path, headSet)


# name: xxxDir/inc.h
def checkGch(name, path):
    headSet = set()
    n = 0
    readFileHead(name, path, headSet)
    fileDict[path] = headSet


if __name__ == '__main__':
    if len(sys.argv) > 1:
        print "GCH_CHECK_WAITTING..."
        for dir in sys.argv[1:]:
            incPath = dir + "/inc.h"
            gchPath = dir + "/inc.h.gch"
            dir = os.path.split(dir)[1]
            if os.path.isdir(dir) and os.path.exists(incPath):
                checkGch(incPath, dir)
                if os.path.exists(TMPFILE):
                    os.remove(TMPFILE)
                os.system("ls -t " + dir + "/*.h " + COMMONDIR + "*.h > " + TMPFILE)
                with open(TMPFILE, 'r') as f:
                    lsList = list(f)
                    for fileName in lsList:
                        # 对proto文件处理
                        fileName = fileName.replace(".proto", ".pb.h")
                        # ls -t 按照文件修改时间排序 比inc.h修改晚的 更改inc.h的修改时间 触发编译
                        if "inc.h" not in fileName:
                            if fileName.strip() in fileDict[dir]:
                                os.utime(incPath, None)
                                if os.path.exists(gchPath):
                                    os.remove(gchPath)
                                print dir + ": " + fileName.strip() + " trigger utime"
                                break
                        else:
                            break
            else:
                print "error:" + dir + "inc not exist!"
        if os.path.exists(TMPFILE):
            os.remove(TMPFILE)
    else:
        pass
                        
