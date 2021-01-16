#!/usr/bin/python
# -*- coding: UTF-8 -*-

# gcc 编译信息友好显示脚本
# python2 

import re 
import os 
import sys 
import time

FILE_SUFFIX = "file"
ERROR_SUFFIX = "error"
EXIT_FILE = "tmp_ccl_file"  # 检查编译进程存在
ERR_PATTERN1 = re.compile(r'(.*)(error:)(.*)')
ERR_PATTERN2 = re.compile(r'(.*)(\[.*\].*)')
EXIT_COUNT = 3       # 编译进程不存在次数 脚本退出 
REFRESH_TIHE = 1     # 刷新时间

targetList = []      # targetAfile
compilingDict = {}   # 正在编译信息
finishDict = {}      # 完成编译信息
errorDict = {}       # 错误信息
targetFileDict = {}
fileList = []        # file open
errorFileDict = {}   # error open
lastOutList = []     # 上次信息 信息不同才刷新
lastRefreshTime = 0
exitCount = 0
beginTime = 0
stateStr = ""          # 状态输出
isError = False
targetStateDict = {} # 编译状态 TargetState
tty = ""             # 当前终端

class TargetState():
    COMPILE = 1 
    LINK = 2 
    FINISH = 3 
    ERROR = 4

class ColorType():
    RED = 1 
    GREEN = 2 
    CYAN = 3 
    YELLOW = 4 
    BLUE_BG = 5


# 输出颜色
def COLOR_STR(str, type):
    if type == ColorType.RED:
        return "\033[1;31;40m" + str + "\033[0m"
    elif type == ColorType.GREEN:
        return "\033[1;32;40m" + str + "\033[0m"
    elif type == ColorType.CYAN:
        return "\033[1;36;40m" + str + "\033[0m"
    elif type == ColorType.YELLOW:
        return "\033[1;33;40m" + str + "\033[0m"
    elif type == ColorType.BLUE_BG:
        return "\033[1;37;40m" + str + "\033[0m"
    else:
        return str

# 删除临时文件
def deleteTmpFile():
    for f in fileList:
        f.close()
    for f in errorFileDict.values():
        f.close()
    if os.path.exists(EXIT_FILE):
        os.remove(EXIT_FILE)
    for target in sys.argv[1:]:
        filePath = target + FILE_SUFFIX
        errorPath = target + ERROR_SUFFIX
        if os.path.exists(filePath):
            os.remove(filePath)
        if os.path.exists(errorPath):
            os.remove(errorPath)

# 编译耗时
def costTime():
    min, sec = divmod(int(time.time() - beginTime), 60)
    return "耗时: " + str(min) + "分" + str(sec) + "秒"

# 检查退出
def checkExit():
    global stateStr
    global isError
    global tty
    finishCount = 0
    errorCount = 0
    for state in targetStateDict.values():
        if state == TargetState.FINISH:
            finishCount += 1
        elif state == TargetState.ERROR:
            errorCount += 1
    if(finishCount == len(targetStateDict)):
        deleteTmpFile()
        stateStr = COLOR_STR("编译完成 " + costTime() + "exit~~~", ColorType.GREEN)
        output()
        exit()

    global exitCount
    os.system("ps x | grep -E \"cclplus|make -C\" | grep " + tty + "| grep -v grep > " + EXIT_FILE)
    if not(os.path.getsize(EXIT_FILE)):
        exitCount += 1
    else:
        exitCount = 0
    if exitCount >= EXIT_COUNT:
        if errorCount > 0:
            isError = True
            deleteTmpFile()
            stateStr = COLOR_STR("编译错误 " + costTime() + "exit!!!", ColorType.RED)
            output()
            exit()
        else:
            pass

# 过滤非编译错误
def filterError(line):
    # 如.d文件不存在
    # if xxxx in line  return True
    return False


# 对gcc编译错误高亮显示
def dealErrorInfo(line):
    retStr = ""
    ret1 = ERR_PATTERN1.search(line)
    if ret1:
        n = 0
        for info in ret1.groups():
            if info == None:
                continue
            if n == 1:
                retStr = retStr + COLOR_STR(info, ColorType.RED)
            else:
                ret2 = ERR_PATTERN2.search(info)
                if ret2:
                    m = 0
                    for info2 in ret2.groups():
                        if m == 1 or m == 3:
                            retStr = retStr + COLOR_STR(info2, ColorType.RED)
                        else:
                            retStr = retStr + info2
                        m += 1
                    break
                else:
                    retStr = retStr + info
            n += 1
        return retStr
    else:
        return line


# 显示编译信息
def output():
    global stateStr
    global lastRefreshTime
    outList = []
    outList.append("\n")
    outList.append(COLOR_STR("-------------------------------- " + "编译信息" + " --------------------------------", ColorType.CYAN))

    for target in targetList:
        outStr = ""
        errStr = ""
        state = targetStateDict[target]
        n = 0
        fileDir = ""
        # target 正在编译
        for file in compilingDict[target]:
            fileSpilt = file.split("/")
            file = fileSpilt[1]
            if fileDir == "":
                fileDir = fileSpilt[0]
            if n != 0 and n % 6 == 0:
                outStr = outStr.rstrip("    ") + "\n"
            if state == TargetState.LINK:
                outStr = outStr + COLOR_STR(file, ColorType.YELLOW) + "    "
            else:
                outStr = outStr + COLOR_STR(file, ColorType.GREEN) + "    "
            n += 1
        # target 错误
        for line in errorDict[target]:
            errStr = errStr + line
        # 打印 target 状态
        outList.append(COLOR_STR("--------------------------------- " + targetFileDict[target] + " --------------------------------", ColorType.CYAN))
        if state == TargetState.LINK:
            outList.append(COLOR_STR("正在链接:   ", ColorType.YELLOW) + COLOR_STR(fileDir, ColorType.BLUE_BG))
        elif state == TargetState.FINISH:
            outList.append(COLOR_STR("已完成", ColorType.GREEN))
        else:
            outList.append(COLOR_STR("正在编译:   ", ColorType.GREEN) + COLOR_STR(fileDir, ColorType.BLUE_BG))
        outList.append(outStr)
        outList.append("\n")
        if len(errStr):
            outList.append(COLOR_STR("编译错误:  ", ColorType.RED))
            outList.append(errStr)


    global lastOutList
    if cmp(outList, lastOutList) != 0 or stateStr != "" or time.time() - lastRefreshTime >= REFRESH_TIHE:
        os.system("clear")
        lastOutList = outList
        lastRefreshTime = time.time()
        for out in outList:
            print out
        if stateStr != "":
            if isError == True:
                print COLOR_STR("状态:  ", ColorType.RED)
            else:
                print COLOR_STR("状态:  ", ColorType.GREEN)
            print stateStr
        else:
            print COLOR_STR("状态:  ", ColorType.YELLOW)
            print COLOR_STR("正在编译 " + costTime(), ColorType.YELLOW)
        print "\n"


# 监控
def monitor():
    global beginTime
    global tty
    beginTime = time.time()
    tty = os.ttyname(1).replace("/dev/", "")
    if os.path.exists(EXIT_FILE):
        os.remove(EXIT_FILE)
    for target in sys.argv[1:]:
        filePath = target + FILE_SUFFIX
        errorPath = target + ERROR_SUFFIX
        if os.path.exists(filePath):
            os.remove(filePath)
        if os.path.exists(errorPath):
            os.remove(errorPath)
        os.mknod(filePath)
        os.mknod(errorPath)
        f = open(filePath, "r")
        fileList.append(f)
        ef = open (errorPath, "r")
        errorFileDict[filePath] = ef

        targetList.append(filePath)     # filePath: targetAfile ...
        compilingDict[filePath] = []
        errorDict[filePath] = []
        finishDict[filePath] = []
        targetFileDict[filePath] = target
        targetStateDict[filePath] = TargetState.COMPILE

    while(1):
        time.sleep(2)
        checkExit()
        for file in fileList:
            fileName = file.name # fileName : targetAfile
            lines = file.readlines()
            for line in lines:
                lineList = line.strip().split(" ")
                op = lineList[0]
                if op == "begin":
                    compilingDict[fileName].append(lineList[1])
                elif op == "end":
                    compilingDict[fileName].remove(lineList[1])
                elif op == "beginlink":
                    compilingDict[fileName].append(lineList[1])
                    targetStateDict[fileName] = TargetState.LINK
                elif op == "endlink":
                    compilingDict[fileName].remove(lineList[1])
                    targetStateDict[fileName] = TargetState.COMPILE
                elif op == "finish":
                    targetStateDict[fileName] = TargetState.FINISH

            # 错误
            errorLines = errorFileDict[fileName].readlines()
            for line in errorLines:
                if filterError(line):
                    continue
                errorDict[fileName].append(dealErrorInfo(line).rstrip() + '\n')
                targetStateDict[fileName] = TargetState.ERROR

        output()
        

if __name__  == '__main__':
    if len(sys.argv) > 1:
        pid = os.fork()
        if pid == 0:
            monitor()
        else:
            pass
