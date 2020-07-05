# RT-Thread操作系统μC/OS-III(3.03/3.04)兼容层
## μCOS-III Wrapper
------

# 1 概述
这是一个针对国产RT-Thread操作系统的μCOS-III操作系统兼容层，可以让基于美国Micriμm公司的μCOS-III操作系统的项目快速迁移到RT-Thread操作系统上。

本文件内的兼容层为μCOS-III 3.03版本向RT-Thread Nano 3.1.3版本兼容。由于μCOS-III支持8、16、32位CPU，而RT-Thread支持32、64位CPU，**因此本兼容层仅能对基于32位CPU的已有工程进行兼容**。



## 1.1 版本详细信息
μC/OS-III        3.03.00 </br>
μC/CPU          1.30.00 </br>
μC/LIB            1.37.02 </br>
RTT nano       3.1.3  </br>



## 1.2 官网

RT-Thread：https://www.rt-thread.org/  </br>
文档中心：https://www.rt-thread.org/document/site/tutorial/nano/an0038-nano-introduction/

μCOS-III：https://www.micrium.com/  </br>
文档中心：https://doc.micrium.com/display/kernel304/uC-OS-III+Documentation+Home



# 2 使用

## 2.1 Keil-MDK仿真工程
本仿真工程是基于*STM32F103RB*平台。

Keil工程路径：<u>RT-Thread-wrapper-of-uCOS-III\rt_thread_3.1.3-ucosiii_3.03-wrapper\rt-thread-3.1.3\bsp\stm32f103-msh-628\Project.uvprojx</u>

需要提前安装好RT-Thread Nano-3.1.3 Keil支持包：https://www.rt-thread.org/download/mdk/RealThread.RT-Thread.3.1.3.pack



## 2.2 迁移步骤
1. 浏览一下μC-CPU/cpu.h文件，看一下头文件中的定义是否符合你的CPU，一般不需要改这个文件


2. 浏览一下μCOS-III/os.h文件，看一下错误代码，这个错误代码和原版μCOS-III是有一定区别的。</br>
   **注意:** 请勿随意打开注释掉的枚举体成员,如果用户使用到了这些注释掉的成员,则会在迁移时编译报错,用以提醒用户这些错误代码在兼容层已经不可用。


3. 软件定时器：μCOS-III原版的软件定时器回调函数是两个参数，本兼容层由于RT-Thread的回调函数仅为一个参数，因此改为一个参数（详见μCOS-III/os.h）。

   uCOS-III原版软件定时器回调函数定义：</br>

   ```c
   typedef  void  (*OS_TMR_CALLBACK_PTR)(void *p_tmr, void *p_arg);
   ```

   本兼容层软件定时器回调函数定义：</br>

    ```c
   typedef  void (*OS_TMR_CALLBACK_PTR)(void *parameter);
    ```


4. 配置os_cfg.h和os_cfg_app.h
   每个选项的配置说明和原版μCOS-III一致，若有不同，我已经在注释中有所解释。</br>
   **原版uCOS-III配置**说明可参见：</br>
   a)《嵌入式实时操作系统μC/OS-III应用开发:基于STM32微控制器》北京航空航天大学出版社 宫辉等译 邵贝贝审校 </br>
   b) Micriμm公司文档中心: https://doc.micrium.com/display/kernel304/uC-OS-III+Features+os_cfg.h
   
5. 注意：μCOS-III的任务堆栈大小单位是sizeof(CPU_STK),而RT-Thread的线程堆栈大小单位是Byte,虽然在兼容层已经做了转换，但是在填写时一定要注意，所有涉及到μCOS-III的API、宏定义全部是按照μCOS-III的标准，即堆栈大小为sizeof(CPU_STK)，切勿混搭！这种错误极其隐晦，一定要注意！**下面是混搭的错误示例**：</br>
    ```c
    ALIGN(RT_ALIGN_SIZE)
    static rt_uint8_t thread2_stack[1024];//错误：混搭RT-Thread的数据类型定义线程堆栈
    
    OSTaskCreate(&thread2,
                 (CPU_CHAR*)"thread2",
                 thread2_entry,	
                 RT_NULL,
                 THREAD_PRIORITY,
                 thread2_stack,
                 sizeof(thread2_stack)/10,//任务堆栈深度限位(错误：这个参数的单位是sizeof(CPU_STK))
                 sizeof(thread2_stack),//任务堆栈大小(错误：这个参数的单位是sizeof(CPU_STK))
                 0,
                 THREAD_TIMESLICE,
                 0,
                 OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 &err);
    ```
    **下面是正确写法**：</br>
    
    ```c
        #define THREAD_STACK_SIZE       256 //正确，要通过宏定义单独定义堆栈大小，单位为sizeof(CPU_STK)
        ALIGN(RT_ALIGN_SIZE)
        static CPU_STK thread2_stack[THREAD_STACK_SIZE];//正确，使用uCOS-III自己的数据类型定义任务堆栈
    
        OSTaskCreate(&thread2,
                   (CPU_CHAR*)"thread2",
                   thread2_entry,
                   RT_NULL,
                   THREAD_PRIORITY,
                   thread2_stack,
                   THREAD_STACK_SIZE/10,//任务堆栈深度限位(正确)
                   THREAD_STACK_SIZE,//任务堆栈大小(正确)
                   0,
                   THREAD_TIMESLICE,
                   0,
                   OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                   &err);
    ```



## 2.3 os_cfg.h配置文件

```c
#define  OS_CFG_DBG_EN  1     /* Enable (1) debug code/variables  */  
```
​    该宏定义定义是否启用兼容层调试，建议在第一次迁移时打开，因为在兼容层内部，一部分uCOS-III原版功能没有实现，如果用户用到了这部分没有实现的功能，将会通过调试的方式输出，予以提示。用户务必对业务逻辑予以修改。

```c
#define  OS_CFG_TMR_TASK_RATE_HZ 100u /* Rate for timers (100 Hz Typ.) */
```
​    在原版μCOS-III中，该宏定义定义了软件定时器的时基信号，这与RT-Thread的软件定时器有本质的不同，在RT-Thread中，软件定时器的时基信号就等于OS ticks。因此为了能够将μCOS-III软件定时器时间参数转为RT-Thread软件定时器的时间参数，需要用到该宏定义。请使该宏定义与原工程使用μCOS-III时的该宏定义参数一致。



## 2.4 os_cfg_app.h配置文件

RT-Thread无需实现μCOS-III的中断任务，因此相关配置予以取消。



## 2.5 运行

```c
int main(void) /*RT-Thread main线程*/
{
    OS_ERR err;
    
    OSInit(&err); /*uCOS-III操作系统初始化*/
    
    OSStart(&err);/*开始运行uCOS-III操作系统*/
    
    //.....
        
}

```



# 3 API
## 3.1 没有实现的兼容的API

由于RT-Thread没有提供相关接口，以下μCOS-III API无法实现：

### 3.1.1 os_core.c
```c
void  OSSchedRoundRobinCfg (CPU_BOOLEAN en, OS_TICK dflt_time_quanta, OS_ERR *p_err);
```
### 3.1.2 os_flag.c
```c
OS_OBJ_QTY  OSFlagPendAbort (OS_FLAG_GRP *p_grp, OS_OPT opt, OS_ERR *p_err);
OS_FLAGS  OSFlagPendGetFlagsRdy (OS_ERR  *p_err);
```

### 3.1.3 os_mutex.c
```c
OS_OBJ_QTY  OSMutexPendAbort (OS_MUTEX *p_mutex, OS_OPT opt, OS_ERR *p_err);
```

### 3.1.4 os_q.c
```c
OS_MSG_QTY  OSQFlush (OS_Q *p_q, OS_ERR *p_err);
OS_OBJ_QTY  OSQPendAbort (OS_Q *p_q, OS_OPT opt, OS_ERR *p_err);
```

### 3.1.5 os_sem.c
```c
void  OSSemSet (OS_SEM *p_sem, OS_SEM_CTR cnt, OS_ERR *p_err);
OS_OBJ_QTY  OSSemPendAbort (OS_SEM *p_sem, OS_OPT opt, OS_ERR *p_err);
```


### 3.1.6 os_task.c

```c
void  OSTaskChangePrio (OS_TCB *p_tcb, OS_PRIO prio_new, OS_ERR *p_err);
void  OSTaskTimeQuantaSet (OS_TCB *p_tcb, OS_TICK time_quanta, OS_ERR *p_err);
OS_MSG_QTY OSTaskQFlush (OS_TCB *p_tcb, OS_ERR *p_err);
CPU_BOOLEAN OSTaskQPendAbort (OS_TCB *p_tcb, OS_OPT opt, OS_ERR *p_err);
CPU_BOOLEAN OSTaskSemPendAbort (OS_TCB *p_tcb, OS_OPT opt, OS_ERR *p_err);
OS_SEM_CTR OSTaskSemSet (OS_TCB *p_tcb, OS_SEM_CTR cnt, OS_ERR *p_err);
```

### 3.1.7 os_time.c
```c
void  OSTimeDlyResume (OS_TCB  *p_tcb, OS_ERR  *p_err);
```

### 3.1.8 os_tmr.c
```c
OS_STATE  OSTmrStateGet (OS_TMR  *p_tmr, OS_ERR  *p_err);
```

