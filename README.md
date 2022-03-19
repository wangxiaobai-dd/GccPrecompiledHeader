# GccPrecompiledHeader
GCC 应用预编译头优化工具

---

#### GchTool目录：
```cpp
cd GchTool 
make 
```

- CheckInclude : 分析路径下头文件包含频率

 > 使用方法：  ./CheckInclude TestTool/dirA 

 > 生成 
 1. analyseInc.txt ， 供 ReplaceGCH.sh 脚本使用
 2. analyseInc2.txt ，方便我们自己查看头文件包含频率  

  
- ReplaceGch.sh :  预编译头替换脚本

 > 使用方法：./ReplaceGCH.sh TestTool/dirA/ 3
      
 > 取 analyseInc.txt  前 3 头文件，对 dirA 路径下，将这些头文件从 cpp 文件中删除，新建文件 inc.h 用于包含这些头文件，并且将 #include "inc.h" 插入刚才修改的 cpp 文件的首行
 
 > 要处理的头文件数量，根据我们的实际项目规模来确定，这里举例是 3 

- 自动创建 inc.h 

-  修改 Makefile 依赖（参考 总结）

---
总结：
https://blog.csdn.net/zhuaizi888/article/details/123598645

