# 维护日志

### 2020-6-29 

- 初步搭建框架
- 完成信号量兼容
- 完成os_time.c系统延时有关兼容


### 2020-6-30 

- 完成软件定时器兼容


### 2020-7-1  

- 实现互斥量兼容


### 2020-7-2  

- 实现消息队列兼容


### 2020-7-3  

- 实现信号标志组兼容

- 实现任务内建寄存器兼容

- 实现任务内建消息队列兼容

### 2020-7-4  

- 实现任务内建信号量兼容
- 实现uCOS-III安全认证检查相关兼容
- 对接os_cfg.h宏定义
- 增加os_cfg_app.h并与之对接
- 完成os_sem.c os_q.c文件宏定义检查以及流程的确认

### 2020-7-5  

- 完成os_flag.c os_mutex.c os_tmr.c os_task.c os_time.c os_core.c文件宏定义检查以及流程的确认
- 优化os_cfg_app.h、os_cfg.h

### 2020-7-6  

- 充实readme.md
- 实现`OSSemSet`、`OSTaskSemSet`函数
- 优化`OSTaskSuspend`、`OSTaskResume`函数，使其具备嵌套挂起/解挂能力
- 实现钩子函数

### 2020-7-7  

- 移植os_mem.c文件
- 优化了内核对象的结构体

### 2020-7-8  

- 实现`OSTmrStateGet`函数
- 构建统计任务框架
- 修改OSTaskDel函数没有删除任务内建信号量、任务内建队列的bug
- 移植`OS_TaskDbgListAdd`、`OS_TaskDbgListRemove`函数
- 实现`OS_TaskInitTCB`函数
- 实现Idle任务（实则为RTT Idle任务的回调函数）
- 实现Idle任务钩子函数`OSIdleTaskHook()`

### 2020-7-9

- 实现uCOS-III内部任务——统计任务，可以正确计算CPU使用率、每个任务的使用堆栈

### 2020-7-10

- 实现`OSTimeDlyResume`函数
- 修复`OSSemDel`函数无法返回当前等待信号量数量、无法使用`OS_OPT_DEL_NO_PEND`选项的问题
- 修复`OSMutexDel`函数无法返回当前等待信号量数量、无法使用`OS_OPT_DEL_NO_PEND`选项的问题
- 修复`OSQDel`函数无法返回当前等待信号量数量、无法使用`OS_OPT_DEL_NO_PEND`选项的问题
- 修复`OSFlagDel`函数无法返回当前等待信号量数量、无法使用`OS_OPT_DEL_NO_PEND`选项的问题

- ### 2020-7-11


- 增加兼容原版OS_TCB结构体成员变量
- 实现`OSSemPendAbort`函数，`OSSemPend`函数增加了处理`OS_ERR_PEND_ABORT`的功能

### 2020-7-12

- 新增os_rtwrap.c文件，负责对RT-Thread和μCOS-III的转换提供支持。并在os.h文件末尾增加os_rtwrap.c内函数的声明
- 实现`OSTaskSemPendAbort`函数
- 实现`OSTaskQPendAbort`函数
- 实现`OSSemPendAbort`函数，`OSSemPend`函数增加了处理`OS_ERR_PEND_ABORT`的功能
- 实现`OSSemMutexAbort`函数，`OSMutexPend`函数增加了处理`OS_ERR_PEND_ABORT`的功能
- 实现`OSSFlagPendAbort`函数，`OSFlagPend`函数增加了处理`OS_ERR_PEND_ABORT`的功能
- 实现`OSQPendAbort`函数，`OSQPend`函数增加了处理`OS_ERR_PEND_ABORT`的功能

### 2020-7-14

- 实现`OSSchedRoundRobinCfg`函数
- 实现`OSFlagPendGetFlagsRdy`函数
- 实现`OSQFlush`函数
- 实现`OSTaskQFlush`函数
- 补充`timer_sample.c`例程

### 2020-7-15

- 移植`cpu_core.c`文件增加`CPU_Init`函数，老项目如果用到该函数在迁移时不会报错，函数内部没有实际动作
- 优化了`timer_sample.c`例程




# Release

## v0.1.0

所有内核对象兼容已经全部实现，可以满足正常的兼容需求。本版本尚未实现μCOS-III的统计任务，该功能将在后续版本中发布。



## v0.2.0 

**[bug fix]** 修改`OSTaskDel`函数没有删除任务内建信号量、任务内建队列的bug

**[add]** 实现`OSTmrStateGet`函数（目前仅有13个uCOS-III API无法兼容）

**[add]** 实现Idle任务（实则为RTT Idle任务的回调函数）

**[add]** 实现Idle任务钩子函数`OSIdleTaskHook()`

**[add]** 实现uCOS-III内部任务——统计任务



## v0.3.0

**[bug fix]** 修复`OSSemDel`函数无法返回当前等待信号量数量、无法使用`OS_OPT_DEL_NO_PEND`选项的问题（已知）

**[bug fix]** 修复`OSMutexDel`函数无法返回当前等待信号量数量、无法使用`OS_OPT_DEL_NO_PEND`选项的问题（已知）

**[bug fix]** 修复`OSQDel`函数无法返回当前等待信号量数量、无法使用`OS_OPT_DEL_NO_PEND`选项的问题（已知）

**[bug fix]** 修复`OSFlagDel`函数无法返回当前等待信号量数量、无法使用`OS_OPT_DEL_NO_PEND`选项的问题（已知）

**[bug fix]** `OSSemPend`函数增加了处理`OS_ERR_PEND_ABORT`的功能（已知）

**[bug fix]** `OSMutexPend`函数增加了处理`OS_ERR_PEND_ABORT`的功能（已知）

**[bug fix]** `OSQPend`函数增加了处理`OS_ERR_PEND_ABORT`的功能（已知）

**[bug fix]** `OSFlagPend`函数增加了处理`OS_ERR_PEND_ABORT`的功能（已知）

**[add]** 增加兼容原版OS_TCB结构体成员变量

**[add]** 实现`OSTimeDlyResume`函数

**[add]** 实现`OSSemPendAbort`函数

**[add]** 实现`OSTaskSemPendAbort`函数

**[add]** 实现`OSMutexPendAbort`函数

**[add]** 实现`OSQPendAbort`函数

**[add]** 实现`OSTaskQPendAbort`函数

**[add]** 实现`OSFlagPendAbort`函数

**[add]** 实现`OSSchedRoundRobinCfg`函数

**[add]** 实现`OSFlagPendGetFlagsRdy`函数

**[add]** 实现`OSFlagPendGetFlagsRdy`函数

**[add]** 实现`OSTaskQFlush`函数

**[add]** 实现`OSQFlush`函数（目前仅有2个uCOS-III API无法兼容）



## v0.4.0（尚未发布）

**[add]**移植`cpu_core.c`文件增加`CPU_Init`函数，防止老项目移植时因为没有该函数而报错，在函数内没有任何动作(DO NOTHING)





# TODO

- [ ] 继续完善统计任务的其他统计项
- [ ] 继续努力实现受限项opts
- [ ] 继续努力实现剩余未兼容的API