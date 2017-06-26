-- ------------------------------------------------------------------------------------
--                 file : scons.md
--        creation date : 17-03-2017 11:31
--           created by : Hubert Li
--        last modified : 2017年04月26日 18:42:04
--            copyright : 2015-2019 by Hubert Li
--                             Strictly Confidential
--                             All rights reserved.
--                       No part of this hardware description, either
--                       material or conceptual may be copied or distributed,
--                       transmitted, transcribed, stored in a retrieval system
--                       or translated into any human or computer language in
--                       any form by any means, electronic, mechanical, manual
--                       or otherwise, or disclosed to third parties without
--                       the express written permission of Hubert Li
-- ------------------------------------------------------------------------------------


# Chapter 1 tools 
-------------------

## 1. install (linux)
* wget http://prdownloads.sourceforge.net/scons/scons-2.2.0.tar.gz .
* tar zxvf scons-2.2.0.tar.gz
* cd scons-2.2.0
* python setup.py instal
* scons 					// to exam if correct installed


# Chapter 2 sons
----------------------

## 1.简介
----------------------
* 自动化编译工具
	* 用来编译软件的软件
	* 配置文件就是一个脚本执行文件，python； 其他自动编译工具都是写了自己的一个语法规则来实现的
	* 跨平台      POSIX systems, windows NT, Mac OS X， linux
		* POSIX 可移植操作系统接口，诞生与Unix发展紧密相关，但不局限于UNIX
		* POSIX 定义了操作系统应该为应用程序提供的接口标准，是一系列API标准的总称，标准IEEE1003，国际标准ISO/IEC 9945
	* 支持从中心代码仓库build
	* 最大问题是性能，比起make,jam来说，要慢一些，在要求频繁迭代的场合很难满足要求
* 如何使用(举例-- hello.c)
	* 1 代码部分 (举例 -- hello.c) 
	* 2 scons配置文件 ，实际也是一个python脚本
	  * Program(hello.c)
	* 3 执行脚本，编译完成
		* 脚本当前目录下，scons
* 

## 2.scons配置文件
----------------------

* 配置文件就是一个Python 脚本
	* 支持所有的python语法
	* 相当于make中的makefile
* 不是一个普通python脚本
	* 执行时，不一定能按照你写的顺序执行
* 脚本名称
	* 一般的四种文件格式：SConstruct,Sconstruct,sconstruct,SConscript
* scons编译过程
   * 默认情况下，SCons将在当前目录以以下次序（SConstruct,Sconstruct,sconstruct）来搜索配置文件，从读取的第一个文件中读取相关配置。
   * 我们可以通过-f选项来指定替代的配置文件，具体方法如下： scons –f configfilename 
   * 在配置文件需要指定build哪些目标文件，也可以指定build这些目标的规则，对于大多数项目，我们仅仅需要指定目标和输入文件。 
   * 在配置文件SConstruct中可以使用函数SConscript()函数来定附属的配置文件。按惯例，这些附属配置文件被命名为”SConscript”，当然也可以使用任意其它名字。 
   * Scons中的配置文件SConscript由python脚本编写，所以我们能使用python脚本的灵活性来处理复杂的build。
   * Scons在读取和执行所有SConscript文件之后，才对目标进行构建。
* 编译指令
   * 基本指令：
   ```
      scons      : 执行SConstruct中脚本       //类似于make
      scons -c   : clean
      scons -Q   : 只显示编译信息，去除多余的打印信息
      scons -Q --implicit-cache hello     ：保存依赖关系
               --implicit-deps-changed    ：强制更新依赖关系
               --implicit-deps-unchanged   ：强制使用原先的依赖关系，即使已经改变
    ```
   * 指定目录
      * scons dir/

## 3.基本语法
----------------------
* Program
	* 生成可执行文件 									// .exe on windows , or  
   ```
        Program('hello.c') 									            // 编译hello.c可执行文件   //系统自动查找依赖 
        Program('hello','hello.c')  						            // 指定output文件名字，hello.ext on Windows, hello on POSIX
        Program(['hello.c', 'file1.c', 'file2.c'])               // 编译多个文件用 [] 括起，ouput文件名以第一个文件命名
        Program(source = "hello.c",target = "hello")
        Program(target = "hello" , source = "hello.c")
        Program('hello', Split('hello.c file1.c file2.c'))       //Program('程序名', Split('程序文件序列 空格分隔'))
        Program(Glob("*.c"))       							         //目录下所有的.c, Program('程序名', Glob('*.cpp')) 
        src = ["hello.c","foo.c"];Program(src)  			         //指定变量
   ```
* Object
	* 生成目标文件,与Program类似                                // .obj on windows, .o文件 on POSIX
   ```
	  Object('helloc.c')  	//编译hello.c目标文件，根据系统自动生成
   ```
* Library
	* Library：生成静态/动态库文件
   ```
        Library('foo', ['f1.c', 'f2.c', 'f3.c'])              //编译library //静态，与 static library相似
        SharedLibrary('foo', ['f1.c', 'f2.c', 'f3.c'])        //编译 shared library  生成动态库
        StaticLibrary('bar', ['f4.c', 'f5.c', 'f6.c'])        //编译 static library  生成静态库
   ```
   * 库的使用：
      * 使用关键字LIBS和LIBPATH
         * LIBS后跟程序需要引用的库名
         * LIBPATH后跟程序引用库的时候所需的库文件路径
   ```
      Program('prog.c', LIBS=['foo', 'bar'], LIBPATH='.') 连接库，不需加后缀或是前缀
   ```
* 指定程序编译过程中需要查找的头文件路径
   * 使用关键字CPPPATH
   * 例如：
   ```
      Program('hello','hello.c', CPPPATH = ['include', '/home/project/inc']) 
   ```
* 指定编译选项 
   * CPPFLAGS  指定编译选项
   * LINKFLAGS 指定链接选项, 如 /DEBUG
   * CPPDEFINES指定预编译器
   * LIBS      指定所需要链接的库文件
   * LIBPATH   指定库文件(.lib)的搜索目录
   * CPPPATH   指定[.h, .c, .cpp]等文件搜索路径

## 4.基本函数
------------------------
* SourceSignatures：判断源文件是否修改
   * SourceSignatures('MD5')     根据内容是否改变，默认方式
   * SourceSignatures('timestamp') 根据修改时间
* TargetSignatures：判断目标文件是否改变
   * TargetSignatures('build')   根据编译结果
   * TargetSignatures('content')  根据文件内容，如果只是加了句注释，将不会被重新编译
* Ignore：忽略依赖关系
   * Ignore(hello, 'hello.h')  忽略某个依赖关系
* Depends：明确依赖关系
   * Depends(hello, 'other_file') 明确依赖关系 

## 5.SConscript : scons 的配置文件
------------------------
* 使用
   * 源文件的目录结构如下：
   ```
    src：
    |    SConstruct
    |    test.cpp
    |    mA(目录)：
         |     SConscript
         |     func.cpp
   ```
      * 其中test.cpp为主文件，中调用func.cpp中定义的函数
   * SConstruct内容如下：
   ```     
      [cpp] view plain copy
      subobj = SConscript(['mA/SConscript'])  
      obj = subobj + Object(Glob("*.cpp"))  
      Program("test",list(obj))  
   ```
   * SConscript内容 ：
   ```  
      [cpp] view plain copy
      obj = Object(Glob("*.cpp"))  
      Return("obj") 
   ``` 
   * 在主目录中执行 scons就可以编译整个"工程"。
      * SConstruct编译主目录中的test.cpp，并通过SConscript编译mA目录下的源文件，并最终生成可执行文件；
      * SConscript用于编译mA中的func.cpp并把生成的func.o传递给主目录的SConstruct。
* obj文件路径 
   * 使用VariantDir函数指定

## 6.env:环境变量
------------------------
* 用于设置在编译过程中的各种参数，(实际上env就是一个python字典)
* 一个环境就是能够影响一个程序如何执行的值的集合。SCons里面有三种不同类型的环境：
   * External Environment（外部环境）：
      * 外部环境指的是在用户运行SCons的时候，用户环境中的变量的集合。这些变量在SConscript文件中通过Python的os.environ字典可以获得。
   * Construction Environment（构造环境）：
      * 一个构造环境是在一个SConscript文件中创建的一个唯一的对象，
      * 这个对象包含了一些值可以影响SCons编译一个目标的时候做什么动作，以及决定从那一个源中编译出目标文件。
      * SCons一个强大的功能就是可以创建多个构造环境，包括从一个存在的构造环境中克隆一个新的自定义的构造环境。
   * Execution Environment（执行环境）：
      * 一个执行环境是SCons在执行一个外部命令编译一个或多个目标文件时设置的一些值。这和外部环境是不同的。
      * 与Make不同，SCons不会自动在不同的环境之间拷贝或导入值。这是一个刻意的设计选择，保证了不管用户外部环境的值是怎么样的，编译总是可以重复的。
      * 这会避免编译中的一些问题，比如因为一个自定义的设置使得使用了一个不同的编译器或编译选项，开发者的本地代码编译成功，但是checked-in后编译不成功，因为使用了不同的环境变量设置
### 外部环境
* 当执行SCons的时候，外部环境的值通过os.environ字典获得。这就以为着在任何一个你想使用外部环境的SConscript文件需要增加一个import os语句

### 构造环境
* 创建环境变量
   * 函数
   ```
      env = Environment()   #创建默认的环境变量，默认scons会按编译器的默认选项来进行编译
   ```
   * 使用举例
   ```
      env = Environment(CC = 'gcc',CCFLAGS = '-O2') #创建并设置环境 变量
      env.Program('foo.c')
   ```
* 复制环境变量
   * 函数
   ```
      env = Environment()
      opt =env.Clone()
   ```         
   * 使用举例
   ```
      env = Environment(CC = 'gcc')
      opt = env.Clone(CCFLAGS = '-O2')
      dbg = env.Clone(CCFLAGS = '-g')
   ```
* 替换环境变量
   * 函数
   ```
      env = Environment()
      opt = env.Replace()
   ```         
   * 使用举例
   ```
      env = Environment(CCFLAGS = '-O2')
      opt = env.Replace(CCFLAGS = '-g')
   ```

### 执行环境
* 当SCons编译一个目标文件的时候，它不会使用你用来执行SCons的同样的外部环境来执行一些命令。它会使用$ENV构造变量作为外部环境来执行命令。
* 这个行为最重要的体现就是PATH环境变量，
   * 它决定了操作系统将去哪里查找命令和工具，与你调用SCons使用的外部环境的不一样。
   * 这就意味着SCons将不能找到你在命令行里执行的所有工具。
* PATH环境变量的默认值是/usr/local/bin:/bin:/usr/bin。如果你想执行任何命令不在这些默认地方，你需要在你的构造环境中的$ENV字典中设置PATH。
* 从外部环境获得PATH值
   * 你可能想获得外部的PATH来作为命令的执行环境。你可以使用来自os.environ的PATH值来初始化PATH变量：
   ```    
      import os
      env=Environment(ENV={'PATH':os.environ['PATH']})
   ```    
   * 你设置可以设置整个的外部环境：
   ```    
      import os
      env=Environment(ENV=os.environ)
   ```    
* 在执行环境里增加PATH的值
   * 常见的一个需求就是增加一个或多个自定义的目录到PATH变量中。
   ```
      env=Environment(ENV=os.environ)
      env.PrependENVPath('PATH','/usr/local/bin')
      env.AppendENVPath('LIB','/usr/local/lib')
   ```

## 7.import/from ...import*
* 搜索路径
   * 当你导入一个模块，Python 解析器对模块位置的搜索顺序是：
      * 1、当前目录
      * 2、如果不在当前目录，Python 则搜索在 shell 变量 PYTHONPATH 下的每个目录。
      * 3、如果都找不到，Python会察看默认路径。UNIX下，默认路径一般为/usr/local/lib/python/。
   * 模块搜索路径存储在 system 模块的 sys.path 变量中。变量里包含当前目录，PYTHONPATH和由安装过程决定的默认目录。
   * PYTHONPATH 变量
      * 作为环境变量，PYTHONPATH 由装在一个列表里的许多目录组成。PYTHONPATH 的语法和 shell 变量 PATH 的一样。
      * 在 Windows 系统，典型的 PYTHONPATH 如下：
      ```
         set PYTHONPATH=c:\python27\lib;
      ```
      * 在 UNIX 系统，典型的 PYTHONPATH 如下：
      ```
         set PYTHONPATH=/usr/local/lib/python
      ```
## 8.Controlling Build Output 
* 使用Help 函数 来说明SConstruct脚本
   * 例如：
   ```
      Help('this is a debug version')
   ```
   * 在控制台上使用 scons -h 命令查看此帮助信息
   * 你可以在脚本中多次使用Help，帮助信息会被连接到一起

## 9、scons 命令行参数 
* 用户可以为scons指定三种类型的参数：
   * Options    : 以 一个或两个(-) 开头 , 详细参考 User Guide 10.1
   * Variables  : 形式为：variable=value, 详细参考            10.2
   * Target     : 如果不是一个 Option 或 Variable ，那么就是一个Target , 详细参考 User Guide 10.3
* 读取命令行的Variable参数
   * 命令行：scons debug=1
   * SConstruct脚本如下：
   ```
      debug = ARGUMENTS.get('debug', 0)
      if int(debug) :
         pass # do something
   ```
* Command-Line Targets
   * scons提供 COMMAND_LINE_TARGETS 供用户访问命令行参数中的 Targets列表，
      * 例如：
      ```
         if 'bar' in COMMAND_LINE_TARGETS:
            print "Don't forget to copy 'bar' to the archivel"
         Default(Program('foo.c'))
         Program('bar.c')
      ```
   * 使用 Default函数 定义 默认目标
      * 当你没有在命令行参数中指定目标时，scons会编译每一个目标
      * 例子：
      ```
         env = Environment()
         hello = env.Program('hello.c')
         env.Program('goodbye.c')
         Default(hello)      #如果没有在命令行指定Target，则会编译hello
      ```
   * 使用DEFAULT_TARGETS获取 默认目标， 
      * 例如：
      ```
         prog1 = Program('prog1.c')
         Default(prog1)
         print "DEFAULT_TARGETS is", map(str, DEFAULT_TARGETS)
      ```
   * 使用 BUILD_TARGETS 获取要编译的目标

# Appendix A
--------------
* [scons用户指南：环境](http://blog.csdn.net/andyelvis/article/details/7269830)
