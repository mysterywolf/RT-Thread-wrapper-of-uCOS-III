# 一、 概述
这是一个针对RT-Thread操作系统的uCOS-III兼容层，可以让基于uCOS-III的应用层程序做最小的修改，使项目快速从uCOS-III迁移到RT-Thread。

本文件内的兼容层为uCOS-III3.03版本向RT-Thread Nano-3.1.3兼容。由于uCOS-III支持8、16、32位CPU，而RT-Thread支持32、64位CPU，**因此本兼容层仅能对基于32位CPU的已有工程进行兼容**。


## 版本详细信息
uC/OS-III        3.03.00 </br>
uC/CPU          1.30.00 </br>
uC/LIB            1.37.02 </br>
RTT nano       3.1.3  </br>



# 二、使用

## 1. Keil-MDK仿真工程
本仿真工程是基于*STM32F103RB*平台。

Keil工程路径：<u>RT-Thread-wrapper-of-uCOS-III\rt_thread_3.1.3-ucosiii_3.03-wrapper\rt-thread-3.1.3\bsp\stm32f103-msh-628\Project.uvprojx</u>

需要提前安装好RT-Thread Nano-3.1.3 Keil支持包：https://www.rt-thread.org/download/mdk/RealThread.RT-Thread.3.1.3.pack

## 2.迁移步骤
1) 浏览一下uC-CPU/cpu.h文件，看一下头文件中的定义是否符合你的CPU，一般不需要改这个文件

2) 浏览一下uCOS-III/os.h文件，看一下错误代码，这个错误代码和原版uCOS是有一定区别的。</br>
**注意:** 请勿随意打开注释掉的枚举体成员,如果用户使用到了这些注释掉的成员,则会在迁移时编译报错,用以提醒用户这些错误代码在兼容层已经不可用。

3) 软件定时器：uCOS-III原版的软件定时器回调函数是两个参数，本兼容层由于RT-Thread的回调函数仅为一个参数，因此改为一个参数（详见uCOS-III/os.h）。

uCOS-III原版软件定时器回调函数定义：</br>

```c
typedef  void  (*OS_TMR_CALLBACK_PTR)(void *p_tmr, void *p_arg);
```
本兼容层软件定时器回调函数定义：</br>
```c
typedef  void (*OS_TMR_CALLBACK_PTR)(void *parameter);
```



## 3.os_cfg.h配置文件
**该文件合并了原版os_cfg.h文件和os_cfg_app.h文件**

```c
#define  RT_DEBUG_UCOSIII  1 /* RT-Thread Debug for uCOS-III wrapper */  
```
​    该宏定义定义是否启用兼容层调试，建议在第一次迁移时打开，因为在兼容层内部，一部分uCOS-III原版功能没有实现，如果用户用到了这部分没有实现的功能，将会通过调试的方式输出，予以提示。用户务必对业务逻辑予以修改。

```c
#define  OS_CFG_TMR_TASK_RATE_HZ 100u /* Rate for timers (100 Hz Typ.) */
```
​    在原版uCOS-III中，该宏定义定义了软件定时器的时基信号，这与RT-Thread的软件定时器有本质的不同，在RT-Thread中，软件定时器的时基信号就等于OS ticks。因此为了能够将uCOS-III软件定时器时间参数转为RT-Thread软件定时器的时间参数，需要用到该宏定义。请使该宏定义与原工程使用uCOS-III时的该宏定义参数一致。

```c
#define  OS_CFG_ARG_CHK_EN  1  /* Enable (1) or Disable (0) argument checking */
```
​    本兼容层为了满足uCOS-III的容错要求，封装时，在真正调用RT-Thread接口函数之前，替RT-Thread做了大量的函数参数合法性检查，这些检查如果觉得没有必要，可以使用该宏定义予以关闭。</br>



## 4.没有实现的兼容的API

由于RT-Thread没有提供相关接口，以下uCOS-III APi无法实现：

### 4.1 os_core.c
```c
void  OSSchedRoundRobinCfg (CPU_BOOLEAN en, OS_TICK dflt_time_quanta, OS_ERR *p_err);
```
### 4.2 os_flag.c
```c
OS_OBJ_QTY  OSFlagPendAbort (OS_FLAG_GRP *p_grp, OS_OPT opt, OS_ERR *p_err);
OS_FLAGS  OSFlagPendGetFlagsRdy (OS_ERR  *p_err);
```

### 4.3 os_mutex.c
```c
OS_OBJ_QTY  OSMutexPendAbort (OS_MUTEX *p_mutex, OS_OPT opt, OS_ERR *p_err);
```

### 4.4 os_q.c
```c
OS_MSG_QTY  OSQFlush (OS_Q *p_q, OS_ERR *p_err);
OS_OBJ_QTY  OSQPendAbort (OS_Q *p_q, OS_OPT opt, OS_ERR *p_err);
```

### 4.5 os_sem.c
```c
void  OSSemSet (OS_SEM *p_sem, OS_SEM_CTR cnt, OS_ERR *p_err);
OS_OBJ_QTY  OSSemPendAbort (OS_SEM *p_sem, OS_OPT opt, OS_ERR *p_err);
```

### 4.6 os_stat.c
由于RTT没有统计任务，本文件**所有函数**不予实现

```c
void  OSStatReset (OS_ERR  *p_err);
void  OSStatTaskCPUUsageInit (OS_ERR  *p_err);
```

### 4.7 os_task.c

```c
void  OSTaskChangePrio (OS_TCB *p_tcb, OS_PRIO prio_new, OS_ERR *p_err);
void  OSTaskTimeQuantaSet (OS_TCB *p_tcb, OS_TICK time_quanta, OS_ERR *p_err);
```

### 4.8 os_time.c
```c
void  OSTimeDlyResume (OS_TCB  *p_tcb, OS_ERR  *p_err);
```

### 4.9 os_tmr.c
```c
OS_STATE  OSTmrStateGet (OS_TMR  *p_tmr, OS_ERR  *p_err);
```



## 5.被RT-Thread接管的API

下列uCOS-III API 被RT-Thread接管，无需实现：

### 5.1 os_core.c
```c
void  OSInit (OS_ERR  *p_err);
void  OSStart (OS_ERR  *p_err);
```



## 5.TODO

实现任务内建信号量、任务内建消息队列、任务内建寄存器以及任务用户补充的存储区的相关API兼容