<!--
                 file : c++.md
        creation date : 17-06-2017 21:27
           created by : Hubert Li
        last modified : 2017年06月19日 17:08:35
            copyright : 2015-2019 by Hubert Li
                             Strictly Confidential
                             All rights reserved.
                       No part of this hardware description, either
                       material or conceptual may be copied or distributed,
                       transmitted, transcribed, stored in a retrieval system
                       or translated into any human or computer language in
                       any form by any means, electronic, mechanical, manual
                       or otherwise, or disclosed to third parties without
                       the express written permission of Hubert Li
-->


# Chapter 0 简介
--------------------
<!--{{{ -->

## 特点
* 基于c语言，是c的一个超级，任何合法的C程序都是合法的C++程序
* 静态类型的、编译式的、通用的、大小写敏感的、不规则的编程语言
* 支持过程化编程、面向对象编程和泛编程
* 中级编程语言
* 面向对象的程序设计语言
  * 封装
  * 抽象
  * 继承
  * 多态

## 应用方向

## 开发运行平台
* Windows
* Unix的各种版本
* MAC

## c++编译器
* gcc
  * UNIX/LINUX
  * Mac os
  * Windows
* vs
  * windows
* g++
  * gcc特殊版本
  * 将默认库改为c++标准库

## 标准库
* 核心语言
  * 提供了所有构建快，包括
    * 变量
    * 数据类型
    * 常量等
* c++标准库
  * 提供了大量的函数
  * 用于操作文件、字符串等
* 标准模板库(STL)
  * 提供大量的方法
    * 用于操作数据结构等

## ANSI 标准
* 保证代码在Mac/UNIX/Windows/Alpha系统都能编译

<!--}}} -->


# Chapter 1 基本语法 
--------------------

* c++程序源文件扩展名
  * .cpp,.cp,.c
* 基本概念
  * 对象
    * 类的实例
  * 类
    * 描述对象行为/状态的模板/蓝图
  * 方法
    * 一个方法表示一种行为
    * 一个类可以包含多个方法
    * 在方法中可以
      * 写入逻辑
      * 操作函数
      * 执行所有的动作
  * 即时变量
    * 对象有独特的即时变量
    * 描述对象的状态
* 函数
  * 有很多种叫法
  * 方法
  * 子程序
  * 程序等

## 基本语法
<!--{{{ -->

### 标识符

### 关键字

### 类型限定符
* const 
  * 对象在程序执行期间不能被修改改变
* volatile
  * 告诉编译器，变量的值可能以程序未明确的方式被改变
* restrict
  * 修饰指针
  * 该指针是
  * 只有c99增加

### 存储类
* 定义变量/函数的范围(可见性) 和声明周期
* 放在他们所修饰的类型之前
* auto 
  * 声明变量 根据初始化表达式自动推断该变量的类型
  * 声明函数时，函数返回值的占位符
  * c++11已删除
* register
  * 定义存储于寄存器，而不是RAM中的局部变量
  * 且不能对他应用一元&运算符
* static
* extern
* mutable
* thread_local(c++11)

### 杂项运算符
* sizeof
* Condition?X:Y
* ,
* . ->
* Cast 强制转换运算符
* &    指针运算符
* ```*```    指针运算符

### typedef 声明
* 为已有的类取一个新的名字
* typedef type newname;

### 枚举类型
* enumeration
* c++ 派生数据类型
* 由用户定义的若干枚举常量的集合
* 一般形式
```
enum 枚举名 {
    标识符[=整型常数],
    标识符[=整型常数],
    ......
    标识符[=整型常数]
} 枚举变量;
```

### 函数 
* 函数参数
* 函数调用，两种函数传递参数方式
  * 传值调用
  * 指针调用
  * 引用调用
    * 把参数的引用复制给形式参数
    * 函数内，该引用用于访问调用中要用到的实际参数
    * 意味着，修改形式参数会影响实际参数

### 引用
* 变量的第二别名
* 引用符号:&
* 常用于
  * 函数参数列表，参数传递给函数
    * 访问形式参数，会影响实际参数
  * 函数返回值


<!--}}} -->

## 类 和 对象
<!--{{{ -->

* 类是c++的核心特性
* 类用于指定对象的形式
* 类成员
  * 数据
  * 方法、函数
* 类定义
```
  class 类的名称 {
    public:                   //公共成员，在类的外部可以访问
      数据类型 数据名;        //公共数据成员
      函数定义or 声明；       //公共函数成员 
      ...
    private:                  //私有成员
      数据类型 数据名;
      ...
    protected:                //受保护成员
      数据类型 数据名;
      ...
  }
```
* 访问修饰符
  * public/private/protected
  * 一个类可以有多个相同的访问描述符
  * 每个访问修饰符的有效区域
    * 直到下一个标记区域开
    * 或者到主体结束
  * 默认是private
* 对象定义
  * 基本上对象根据类来创建的
  * 类名 对象名
* 类成员访问
  * 公共成员
    * 在类的外部可访问的
    * 可以不使用任何成员函数,通过运算符(.)直接访问
  * 私有成员
    * 变量和函数在类的外部不可访问，甚至不可查看的
    * 只有类和友元函数可以访问私有成员
  * 保护成员
    * 和私有成员相似
    * 但保护成员在派生类(之类)中是可以访问的
    * 派生类
      * class 父类名:子类名{}
  * 使用
    * 一般我们在私有区域定义数据
    * 在公有区域定义相关函数

### 类的成员函数
* 类定义内部函数
  * 可以是函数的声明
  * 也可以定义
* 成员函数的定义
  * 类内部定义
    * 和普通函数一样，但可以直接操作一个类的数据成员
    * 把函数声明为内联的，即使没有inline标识符
  * 类外部定义
    * 需要使用***范围解析运算符(::)***来定义
    * 格式
    ```
      返回类型 类名::函数名(形式参数) {
      }
    ```
* 成员函数的调用
  * 点运算符
    * 对象.成员函数
<
!--}}} -->

## 类的继承

## 基本结构
* 程序入口
  * main()
* 语句
  * 分号结尾
  * 表明一个逻辑实体的结束
* 块
  * 大括号括起来的
  * 按逻辑连接的语句
* 注释
  * 单行注释
    * //
  * 多行注释
    * ```/**/```
* 返回值
  * return
  * ***c++允许从函数返回数组***

## 常见的头文件
* ```#include <iostream>```
  * cin/cout/cerr/clog对象
  * 对应标准输入流、标准输出流、非缓冲标准错误流、缓冲标准错误流
* ```#include <iomanip>```
  * 通过所谓的参数化的流操纵器(setw,setprecision等）
  * 声明对执行标准化I/O有用的服务
* ```#include <fstream>```
  * 为用户控制的文件处理声明服务
* ```#include <cmath>```
  * 数学运算函数
* ```#include <ctime>```
  * c++ 标准库没有日期类型
  * 继承了c语言用于日期和时间操作的结构和函数
  ```
  clock_t time_t size_t tm
  ```


## 常见函数
* rand()
  * 伪随机数生成函数
  * 必须先调用srand()
* time()
  * 获取系统时间的秒数
* cout



# Appendix A
--------------------
* [c菜鸟教程](http://www.runoob.com/cplusplus/cpp-tutorial.html)




