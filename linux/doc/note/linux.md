<!--
                 file : linux.md
        creation date : 22-06-2017 08:17
           created by : Hubert Li
        last modified : 2017年06月22日 11:32:23
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

# Chapter 1 linux程序开发
-------------------------
## 0.程序开发方式
* 实时/非实时
  * 调度方式配置
  * 是否用中断
* 程序框架/子模块
  * 多线程，线程功能/接口
* 线程间通信/同步的方式
* 书写
  * 开始，signal错误处理

## 1.程序与进程
* 附录1
* 程序是静态的，进程是动态的，程序的一次运行
* 程序：代码段+数据段
* 进程：代码段+数据段+ PCB

## 2.进程
* 附录1
* PCB 
  * process control block
  * task_struct in file :include/linux/sched.h
* PCB组成
  * 标识符,包括pid，别名
  * 资源标识
    * 代码段，数据段
  * 进程状态标识
  * 调度信息
    * nice
    * 调度方式: policy
    * 静态优先级priority
    * 剩余时间counter
    * 实时优先级 rt_priority
  * 进程间通信
    * 低级：信号量
    * 高级
      * 管道
      * system_v
        * 共享内存
          * 共享数据结构
          * 共享一块内存
        * 消息队列
          * message box
    * 信号
      * 错误信息
    
### 进程状态及转换
* 就绪
* 运行
* 阻塞/等待
  * 静态、挂起
  * 动态


### 调度方式

* 调度思想
  * 先入先行：FCFS
  * 优先级优先： 动态优先级/剩余时间 以静态优先级为基础
    * 抢占式
    * 非抢占式
  * 时间片轮询RR：
  * 多级队列调度
    * 按优先级 分为多个队列
    * 高优先级队列执行完，才执行低优先级队列
    * 同一队列FCFS
    * 低优先级时间片大
* linux的三种调度方式
  * 调度程序 schedule()
  * 进程和线程一套调度方式
  * SCHED_OTHER : 普通的分时进程
  * SCHED_FIFO  ：先进先出的实时进程
  * SCHED_RR    : 基于优先级的轮转算法(动态优先级调度算法),实时进程
  * 实时进程获得CPU比普通进程优先
* linux调度设置
  * 系统调用 sched_setschedule() 选择调度方式
  * 系统调用 sys_setpriority(),sys_nice() 改变静态优先级
  * 子进程常从父进程继承，父是实时进程，子也是
* SCHED_OTHER
  * 抢占式动态优先级调度算法
  * 无进程就绪队列
  * 动态优先级counter决定执行顺序，counter表示剩余运行时间，单位是tick ，1/HZ s
  * 中断会将counter清0
  * 重新计算优先级
    * 当大部分程序的counter减到0，或外部触发
    * 所有进程参加计算
    * counter = counter >> 1 + priority
* 实时进程
  * FIFS 和RR 
  * 为了保证优先级优于普通进程，增加第三种优先级,实时优先级rt_priority
  * counter 不作为依据

### 进程间通信


### 信号
* 附录2
* 信号的接收
  * 异常产生的信号，（SIGPIPE.SIGEGV等)只有产生异常的线程会收到并处理
  * 外部kill命令产生的信号，SIGINT,SIGHUP等，会遍历所有线程，直到找到一个不阻塞信号的线程
    * 一般从主线程找起，只有一个线程能收到
  * pthread_kill 产生的，只有对应的线程能收到
* 处理
  * 所有的线程都有自己独立的signal mask
  * 所有的线程共享进程的signal action
    * ？那每个线程对于希望处理的信号是否需要单独设置signal
    * ? 如果上面是，为什么不可以独立设置signal action

## 3.kernel的时间资源
* 附录3/4 
*

### 时钟/时间/时钟事件设备
* struct clocksource
  * 不能被编程，没有产生事件的能力
  * 主要被用于timekeeper来实现对真实时间进行精确的统计
* struct timekeeper
* struct clock_event_device 
  * 可编程
  * 工作模式可配置
    * 周期触发
    * 单次触发
  * 用于
    * 实现普通定时器
    * 高精度定时器
    * 用于产生tick事件
  * kernel/time/clockevents.c

### 时间单位
* struct timeval
  * s/us
  * gettimeofday/settimeofday 用timeval 作为单位
* struct timespec
  * s/ns
  * timekepper ,xtime字段用timespec作为单位
* struct ktime
  * s/ns
  * 高精度定时器一般用ktime作为计时单位

### 定时器
* 时间中断，jiffies
  * 时钟中断定时产生tick信号，每次tick信号，jiffies会产生计数
* clock_event_device
  * 通用时间框架为定时器硬件定义了一个标准接口

#### jiffies
* kernel/time/jiffies.c
* 精度1/HZ s
* 低精度定时器
* HZ:10~1000，固定值
* 早期的所有时钟操作，都是基于jiffies,都是在machine级代码实现

#### hrtimer
* high resolution timer，高精度定时器
* 实现依赖timekeeper 和 clock_event_device


### 忙等待和 sleep

#### sleep
* msleep
  * jiffies
* hrtimer_nanosleep 
  * hrtimer
* schedule_timeout()
  * jiffies
* schedule_hrtimeout()
  * hrtimer

#### 忙等待
* mdelay/udelay/ndelay/
  * jiffies



# Chapter 2 kernel struct
--------------------------
* 附录4



# Chapter 3 linux Driver develop


# Appendix A
------------------------
* 1.[操作系统原理及应用linux](../doc/操作系统原理及应用linux.pdf)
* 2.[多进程的信号处理](http://www.cnblogs.com/coding-my-life/p/4782529.html)
* 3.[Linux时间子系统](http://blog.csdn.net/droidphone/article/details/7975694)
* 4.[Linux Device Driver_3edtion.pdf](../doc/Linux Device Driver_3edtion.pdf)
