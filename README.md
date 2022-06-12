# GccPrecompiledHeader
GCC 应用预编译头优化工具

---

#### GchTool目录：
```cpp
cd GchTool 
make 
```

- CheckInclude : 分析路径下头文件包含频率

```cpp
 使用方法： ./CheckInclude CheckDir [FindDirA] [FindDirB] ...
```
 
 进行递归检查头文件包含， 若 a.cpp 包含 a.h ，而 a.h 中还包含其他头文件，并且在 FindDirN 中，递归 Check.
 
 [FindDirA]，[FindDirB]，... :  变长参数，指进行递归搜索头文件的目录
 
 生成 
 1. analyseInc.txt-dirA ， 供 ReplaceGCH.sh 脚本使用
 2. analyseInc2.txt-dirA ，方便我们自己查看头文件包含频率  

---
- GchTool/GetCommonInclude.cpp

```cpp
./GetCommonInclude --files analyseInc-1.txt,analyseInc-2.txt --base dirA,dirB 逗号分割参数 --rate 0.9
```
从得到的多个频率文件中，获取共同的头文件包含，生成 AnalyseFinal.txt 

  base ：指产生频率文件的目录，如 analyseInc-1.txt 产生于 dirA，analyseInc-2.txt 产生于 dirB 
  rate :  输出每个目录下频率都超过 0.9 的头文件 
  
--- 
 
- ReplaceGch.sh :  预编译头替换脚本

```cpp
 使用方法：./ReplaceGch.sh dirA/ 3
```
      
  取 AnalyseFinal.txt  前 3 头文件，对 dirA 路径下，将这些头文件从 cpp 文件中删除，新建文件 inc.h 用于包含这些头文件， 
  
  并且将 #include "inc.h" 插入刚才修改的 cpp 文件的首行 要处理的头文件数量，根据我们的实际项目规模来确定，这里举例是 3 

- 自动创建 inc.h 

- 修改 Makefile 依赖（参考 总结）

---

- 增加 GchTool/RevertInc.cpp ：

```cpp
/RevertInc TestRevertTool/dirA inc.h [unified]
```

  可以将包含预编译头文件（如inc.h）的 cpp 文件替换还原 
  
  若 inc.h 包含 a.h、b.h，使用工具后 目录下 *.cpp 中 inc.h 都被替换为 a.h  b.h [unified] 可选参数，指包含多个 cpp 文件的 unified cpp 


--- 

总结：
https://blog.csdn.net/zhuaizi888/article/details/123598645

https://blog.csdn.net/zhuaizi888/article/details/125248520


