# 维护日志

### 2020-6-29 

- 初步搭建框架；完成信号量兼容；完成os_time.c系统延时有关兼容


### 2020-6-30 

- 完成软件定时器兼容


### 2020-7-1  

- 实现互斥量兼容


### 2020-7-2  

- 实现消息队列兼容


### 2020-7-3  

- 实现信号标志组兼容；

- 实现任务内建寄存器兼容；

- 实现任务内建消息队列兼容

### 2020-7-4  

- 实现任务内建信号量兼容；
- 实现uCOS-III安全认证检查相关兼容；
- 对接os_cfg.h宏定义；
- 增加os_cfg_app.h并与之对接；
- 完成os_sem.c os_q.c文件宏定义检查以及流程的确认

### 2020-7-5  

- 完成os_flag.c os_mutex.c os_tmr.c os_task.c os_time.c os_core.c文件宏定义检查以及流程的确认；
- 优化os_cfg_app.h、os_cfg.h

### 2020-7-6  

- 充实readme.md；
- 实现OSSemSet、OSTaskSemSet函数；
- 优化OSTaskSuspend、OSTaskResume函数，使其具备嵌套挂起/解挂能力；
- 实现钩子函数

### 2020-7-7  

- 移植os_mem.c文件；
- 优化了内核对象的结构体；

### 2020-7-8  

- 实现OSTmrStateGet函数；
- 构建统计任务框架；
- 修改OSTaskDel函数没有删除任务内建信号量、任务内建队列的bug；
- 移植OS_TaskDbgListAdd、OS_TaskDbgListRemove函数；
- 实现OS_TaskInitTCB函数；
- 实现Idle任务（实则为RTT Idle任务的回调函数）；
- 实现Idle任务钩子函数OSIdleTaskHook()；

### 2020-7-9

- 实现uCOS-III内部任务——统计任务，可以正确计算CPU使用率、每个任务的使用堆栈；

### 2020-7-10

- 实现OSTimeDlyResume函数
- 修复OSSemDel函数无法返回当前等待信号量数量、无法使用OS_OPT_DEL_NO_PEND选项的问题
- 修复OSMutexDel函数无法返回当前等待信号量数量、无法使用OS_OPT_DEL_NO_PEND选项的问题
- 修复OSQDel函数无法返回当前等待信号量数量、无法使用OS_OPT_DEL_NO_PEND选项的问题
- 修复OSFlagDel函数无法返回当前等待信号量数量、无法使用OS_OPT_DEL_NO_PEND选项的问题

### 2020-7-11

- 实现OS_TCB结构体.StkLimitPtr .StkBasePtr .PendStatus成员变量
- 实现OSSemPendAbort函数






# Release

## v0.1.0

所有内核对象兼容已经全部实现，可以满足正常的兼容需求。本版本尚未实现μCOS-III的统计任务，该功能将在后续版本中发布。



## v0.2.0 

**[bug fix]** 修改OSTaskDel函数没有删除任务内建信号量、任务内建队列的bug；

**[add]** 实现OSTmrStateGet函数（目前仅有13个uCOS-III API无法兼容）；

**[add]** 实现Idle任务（实则为RTT Idle任务的回调函数）；

**[add]** 实现Idle任务钩子函数OSIdleTaskHook()；

**[add]** 实现uCOS-III内部任务——统计任务；



## v0.3.0（尚未发布）

**[bug fix]** 修复OSSemDel函数无法返回当前等待信号量数量、无法使用OS_OPT_DEL_NO_PEND选项的问题（已知）

**[bug fix]** 修复OSMutexDel函数无法返回当前等待信号量数量、无法使用OS_OPT_DEL_NO_PEND选项的问题（已知）

**[bug fix]** 修复OSQDel函数无法返回当前等待信号量数量、无法使用OS_OPT_DEL_NO_PEND选项的问题（已知）

**[bug fix]** 修复OSFlagDel函数无法返回当前等待信号量数量、无法使用OS_OPT_DEL_NO_PEND选项的问题（已知）

**[add]** 实现OS_TCB结构体.StkLimitPtr .StkBasePtr .PendStatus成员变量

**[add]** 实现OSTimeDlyResume函数（目前仅有12个uCOS-III API无法兼容）

**[add]** 实现OSSemPendAbort函数（目前仅有11个uCOS-III API无法兼容）



# TODO

- [ ] 继续完善统计任务的其他统计项
- [ ] 继续努力实现受限项opts
- [ ] 继续努力实现剩余未兼容的API