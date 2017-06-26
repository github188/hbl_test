<!--
                 file : xcs.md
        creation date : 10-06-2017 22:32
           created by : Hubert Li
        last modified : 2017年06月22日 11:32:32
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



# Chapter 0 xcs结构

## xcs 结构
* 信号异常处理
  * extSignaland()
  * signal
  * sigaction
* 硬件版本号获取
  * hardWareVerGet()
* 初始化日志模块
  * mcLogInit()
  * 无
* 创建pid文件，方便monitor监控
  * writeThreadInfor()
  * 将xcs所有创建的pid都存进文件
  * 每次使用都单独开启关闭
* 创建消息队列
  * msgQCreate()
* 已关闭的功能
  * CYC200XZD/CYC500XZD
  * CCD控制模块
  * CMOS初始化
  * ICX274_CCD
* dspCameraInfoShow()
  * icx274CCDCtl.h
  * 图像的所有信息，宽高、快门、增益，目标亮度、锐化
  * 代码里写死，最后shell_print输出
  * ***可删除***
* 初始化系统配置文件
  * SysConfInit()
  * debugShellLogSet()
  *

## 日志、打印
* printf
  * 默认串口
* SYSHANDLE_ERROR
  * shell_print
* syslog
  * syslog协议属于一种主从式协议：syslog发送端会传送出一个小的文字讯息（小于1024字节）到syslog接收端。
  * 接收端通常名为“syslogd”、“syslog daemon”或syslog服务
* mclog
  * 写日志

# Chapter 1 具体模块

## 1. 信号异常处理
<!--{{{ -->

* exitSignalHand()
* sigaction 是POSIX的信号接口，而signal()是标准C的信号接口(如果程序必须在非POSIX系统上运行，那么就应该使用这个接口)

#### sigaction

```
相关函数 signal，sigprocmask() ，sigpending，sigsuspend, sigemptyset
表头文件 #include<signal.h>
定义函数 int sigaction(int signum,const struct sigaction *act ,struct sigaction *oldact);
函数说明 sigaction()会依参数signum指定的信号编号来设置该信号的处理函数。参数signum可以指定SIGKILL和SIGSTOP以外的所有信号
```


```
struct sigaction {
  void (*sa_handler)(int);
  void (*sa_sigaction)(int, siginfo_t *, void *);
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_restorer)(void);
}
```

#### signal


<!--}}} -->

## 2.创建消息队列 MSGQ 

### 消息队列结构体
<!--{{{ -->

* 设置的单个消息最大长度
* 设置的消息最大个数
* 设置的消息选项
* 信号量
  * 用于查看队列资源是否有剩余可用(可读)
  * 延时操作一般只能到这
* 互斥锁
  * 用于队列资源使用互斥，保证同时只有一个线程使用队列资源
  * 延时操作不包括时间
* 单个消息的长度 = 设置的 + int 
  * 起始1int，存放写入的消息长度
* 消息的个数     = 设置的 + 1
* 消息队列的总长度 = 消息个数x单个消息长度
* 消息存放的地址
* 写入消息循环计数
* 读出消息循环计数

#### 信号量 
* 信号量
  * 数据类型为sem_t
  * 本质是一个长整型的数
* 声明
```
  sem_t sem;
```
* 初始化函数
```
  #include <semaphore.h>
  int sem_init(sem_t *sem, int pshared, unsigned in value);
```
  * sem: 信号量
  * pshared: 0 进程内多线程共享，1 进程间共享，linux一般不支持1
  * value: 初始值
  * 返回值：0成功，非0失败
* 常见操作
  * sem_trywait 
    * 非阻塞
  * sem_wait 
    * 原子操作
    * 信号量减1，操作对象最小为0
    * 阻塞
  * sem_pos
    * 原子操作
    * 信号量加1
  * sem_getvalue
    * 获取信号量的值
* 信号量释放
  * sem_destroy

#### 互斥锁 
* 互斥锁
  * 用于保持多个线程间的同步
  * pthread_mutex_t的结构体
* 锁的创建
  * 动态创建
    * pthread_mutex_init
  * 静态创建
    * 用宏PTHREAD_MUTEX_INITIALIZER来静态初始化
      * 该宏是一个结构常量
    ```
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    ```
* 锁的属性设置
  * pthread_mutexattr_init 初始化
  * 互斥锁的范围设置
    * 进程间同步
    * 同一进程不同线程同步
    * 设置与获取锁的范围
      * setshared,getshared
  * 锁的类型 
    * 普通锁 缺省值
      * 一个线程加锁后，其他请求的线程形成一个等待队列，解锁后按优先级获得锁
    * 嵌套锁
      * 允许一个线程对同一个锁成功获得多次，并通过多次unlock解锁
    * 检错锁
      * 如果同一线程请求同一个锁，会返回 EDEADLK
      * 其他和普通锁一致
      * 保证当不允许多次加锁时，不会出现最简单情况下的死锁
    * 适应锁
      * 动作最简单的锁类型，仅等待解锁后重新竞争
    * 设置或获取锁的类型
      * settype,gettype
* 锁的操作
  * 加锁
    * pthread_mutex_lock
  * 解锁
    * pthread_mutex_unlock
  * 测试加锁
    * pthread_mutex_trylock
    * 类似lock，但当锁被占据时，返回EBUSY 而不是挂起等待
* 锁的释放
  * pthread_mutex_destory
  * 前提是锁当前状态没有被锁
<!--}}} -->

### 消息队列的处理
<!--{{{ -->
* create 创建函数
  * 设置的单个消息最大长度
  * 设置的消息最大个数
  * 设置的消息选项
  * 返回值： 实例化的消息队列指针
* send 写入队列函数
  * 实例化消息队列指针
  * 消息指针
  * 消息长度
  * 超时等待时间
  * 返回值：写入成功与否
* receive 读出队列函数
  * 实例化消息队列指针
  * 存放消息内存指针
  * 读取消息长度
  * 超时等待时间
  * 返回值：读出成功与否
  * 超时时间实现
    * -1， 永久等待
      * 使用set_wait阻塞
    * 0， 立马返回
      * 使用set_trywait阻塞
    * 等待时间 ms
      * 使用set_timedwait
* Delete 队列消除函数
  * pthread_mutex_lock
  * sem_destroy
  * pthread_mutex_unlock
  * pthread_mutex_destroy
  * delete

<!--}}} -->

## 3. 初始化系统配置文件

### SysConfInit()












## 

### flashLampStaMonitor(char cMod)
* 功能：
  * 动态检测外部摄像机同步中断信号。
  * 若不来，则由闪光灯模式切换到卤光灯模式
* cMode
  * 0: 获取初始同步中断数
  * 1: 动态检测同步中断数有没有变化

# Chapter 2 拓展知识

* unlink
  * c语言函数， 删除指令目录的文件(在所有的使用都关闭后)
* pthread_self
  * 函数，获得线程自身ID
* new/delete
  * c++ 关键字、操作符
  * 在堆上动态创建一个对象
    * 获得一块内存空间
    * 调用构造函数
    * 返回正确的指针


# Appendix A
--------------
* [sigaction](http://baike.baidu.com/link?url=2ycN0Nss6UqebptEB_C7yDzEjJQTQmDTazGwc3VH0oh04H4ipNeGiZYT7tAB-VXj_nh2fZnUsbkaqH7fOxh_0WTB840QeGqho0VUQa-psqa)
* [sigaction和signal](http://blog.csdn.net/beginning1126/article/details/8680757)
* [signal各种异常](http://blog.csdn.net/tiany524/article/details/17048069)
