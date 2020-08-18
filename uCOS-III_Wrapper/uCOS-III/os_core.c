/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-29     Meco Man     the first verion
 */

/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                    CORE FUNCTIONS
*
* File    : OS_CORE.C
* By      : JJL
* Version : V3.03.00
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#include <os.h>

/*
************************************************************************************************************************
*                                                    INITIALIZATION
*
* Description: This function is used to initialize the internals of uC/OS-III and MUST be called prior to
*              creating any uC/OS-III object and, prior to calling OS_Start().
*
* Arguments  : p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE    Initialization was successful
*                                Other          Other OS_ERR_xxx depending on the sub-functions called by OSInit().
* Returns    : none
************************************************************************************************************************
*/

void  OSInit (OS_ERR  *p_err)
{
    CPU_SR_ALLOC();
    
    *p_err = OS_ERR_NONE;
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif    

    CPU_CRITICAL_ENTER();
    
    OSInitHook();                                           /* Call port specific initialization code                 */
    
    OSRunning = OS_STATE_OS_STOPPED;                        /* Indicate that multitasking not started                 */
    
#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    OSTaskRegNextAvailID = (OS_REG_ID)0;
#endif    
    
#ifdef OS_SAFETY_CRITICAL_IEC61508
    OSSafetyCriticalStartFlag = DEF_FALSE;
#endif  
    
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
    OSSchedRoundRobinEn             = DEF_TRUE;/*RTT的时间片轮转是必选项*/
    OSSchedRoundRobinDfltTimeQuanta = OS_CFG_TICK_RATE_HZ / 10u;
#endif

#if OS_CFG_SEM_EN > 0u                                      /* Initialize the Semaphore Manager module                */
    OS_SemInit(p_err);
    if (*p_err != OS_ERR_NONE) {
        CPU_CRITICAL_EXIT();
        return;
    }
#endif
    
#if OS_CFG_MUTEX_EN > 0u                                    /* Initialize the Mutex Manager module                    */
    OS_MutexInit(p_err);
    if (*p_err != OS_ERR_NONE) {
        CPU_CRITICAL_EXIT();
        return;
    }
#endif
    
#if OS_CFG_FLAG_EN > 0u                                     /* Initialize the Event Flag module                       */
    OS_FlagInit(p_err);
    if (*p_err != OS_ERR_NONE) {
        CPU_CRITICAL_EXIT();
        return;
    }
#endif

#if OS_CFG_Q_EN > 0u
    OS_QInit(p_err);                                        /* Initialize the Message Queue Manager module            */
    if (*p_err != OS_ERR_NONE) {
        CPU_CRITICAL_EXIT();
        return;
    }
#endif
    
    OS_TaskInit(p_err);                                     /* Initialize the task manager                            */
    if (*p_err != OS_ERR_NONE) {
        CPU_CRITICAL_EXIT();
        return;
    }
    
    OS_IdleTaskInit(p_err);                                 /* Initialize the Idle Task                               */
    if (*p_err != OS_ERR_NONE) {
        CPU_CRITICAL_EXIT();
        return;
    }

#if OS_CFG_STAT_TASK_EN > 0u                                /* Initialize the Statistic Task                          */
    OS_StatTaskInit(p_err);
    if (*p_err != OS_ERR_NONE) {
        CPU_CRITICAL_EXIT();
        return;
    }
#endif    
    
#if OS_CFG_TMR_EN > 0u                                      /* Initialize the Timer Manager module                    */
    OS_TmrInit(p_err);
    if (*p_err != OS_ERR_NONE) {
        CPU_CRITICAL_EXIT();
        return;
    }
#endif  
    
#if OS_CFG_DBG_EN > 0u
    OS_Dbg_Init();
    OSCfg_Init();
#endif
    
    
    /*这部分内容是在原版OSStart()函数中运行的,但是在本兼容层中,操作系统已经启动,因此直接在此处进行标记*/
    if (OSRunning == OS_STATE_OS_STOPPED) {
        OSRunning       = OS_STATE_OS_RUNNING;
    }
    
    CPU_CRITICAL_EXIT();
}

/*
************************************************************************************************************************
*                                                      ENTER ISR
*
* Description: This function is used to notify uC/OS-III that you are about to service an interrupt service routine
*              (ISR).  This allows uC/OS-III to keep track of interrupt nesting and thus only perform rescheduling at
*              the last nested ISR.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) This function MUST be called with interrupts already disabled
*
*              2) Your ISR can directly increment 'OSIntNestingCtr' without calling this function because OSIntNestingCtr has
*                 been declared 'global', the port is actually considered part of the OS and thus is allowed to access
*                 uC/OS-III variables.
*
*              3) You MUST still call OSIntExit() even though you increment 'OSIntNestingCtr' directly.
*
*              4) You MUST invoke OSIntEnter() and OSIntExit() in pair.  In other words, for every call to OSIntEnter()
*                 (or direct increment to OSIntNestingCtr) at the beginning of the ISR you MUST have a call to OSIntExit()
*                 at the end of the ISR.
*
*              5) You are allowed to nest interrupts up to 250 levels deep.
************************************************************************************************************************
*/

void  OSIntEnter (void)
{
    rt_interrupt_enter();
}

/*
************************************************************************************************************************
*                                                       EXIT ISR
*
* Description: This function is used to notify uC/OS-III that you have completed servicing an ISR.  When the last nested
*              ISR has completed, uC/OS-III will call the scheduler to determine whether a new, high-priority task, is
*              ready to run.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) You MUST invoke OSIntEnter() and OSIntExit() in pair.  In other words, for every call to OSIntEnter()
*                 (or direct increment to OSIntNestingCtr) at the beginning of the ISR you MUST have a call to OSIntExit()
*                 at the end of the ISR.
*
*              2) Rescheduling is prevented when the scheduler is locked (see OSSchedLock())
************************************************************************************************************************
*/

void  OSIntExit (void)
{
    rt_interrupt_leave();
}

/*
************************************************************************************************************************
*                                    INDICATE THAT IT'S NO LONGER SAFE TO CREATE OBJECTS
*
* Description: This function is called by the application code to indicate that all initialization has been completed
*              and that kernel objects are no longer allowed to be created.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/

#ifdef OS_SAFETY_CRITICAL_IEC61508
void  OSSafetyCriticalStart (void)
{
    OSSafetyCriticalStartFlag = DEF_TRUE;
}

#endif

/*
************************************************************************************************************************
*                                                      SCHEDULER
*
* Description: This function is called by other uC/OS-III services to determine whether a new, high priority task has
*              been made ready to run.  This function is invoked by TASK level code and is not used to reschedule tasks
*              from ISRs (see OSIntExit() for ISR rescheduling).
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) Rescheduling is prevented when the scheduler is locked (see OSSchedLock())
************************************************************************************************************************
*/

void  OSSched (void)
{
    rt_schedule();
}

/*
************************************************************************************************************************
*                                                 PREVENT SCHEDULING
*
* Description: This function is used to prevent rescheduling from taking place.  This allows your application to prevent
*              context switches until you are ready to permit context switching.
*
* Arguments  : p_err     is a pointer to a variable that will receive an error code:
*
*                            OS_ERR_NONE                 The scheduler is locked
*                            OS_ERR_LOCK_NESTING_OVF     If you attempted to nest call to this function > 250 levels
*                            OS_ERR_OS_NOT_RUNNING       If uC/OS-III is not running yet.
*                            OS_ERR_SCHED_LOCK_ISR       If you called this function from an ISR.
*
* Returns    : none
*
* Note(s)    : 1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pair.  In other words, for every
*                 call to OSSchedLock() you MUST have a call to OSSchedUnlock().
************************************************************************************************************************
*/

void  OSSchedLock (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif    
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if(OSIntNestingCtr > (OS_NESTING_CTR)0) /*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_SCHED_LOCK_ISR;
        return; 
    }  
#endif
    
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Make sure multitasking is running                      */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }

    if (OSSchedLockNestingCtr >= (OS_NESTING_CTR)250u) {    /* Prevent OSSchedLockNestingCtr overflowing              */
       *p_err = OS_ERR_LOCK_NESTING_OVF;
        return;
    }
    
    *p_err = OS_ERR_NONE;/*rt_enter_critical没有返回错误码*/    
    rt_enter_critical();
}

/*
************************************************************************************************************************
*                                                  ENABLE SCHEDULING
*
* Description: This function is used to re-allow rescheduling.
*
* Arguments  : p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE
*                            OS_ERR_OS_NOT_RUNNING       The scheduler has been enabled
*                            OS_ERR_SCHED_LOCKED         The scheduler is still locked, still nested
*                            OS_ERR_SCHED_NOT_LOCKED     The scheduler was not locked
*                            OS_ERR_SCHED_UNLOCK_ISR     If you called this function from an ISR.
*
* Returns    : none
*
* Note(s)    : 1) You MUST invoke OSSchedLock() and OSSchedUnlock() in pair.  In other words, for every call to
*                 OSSchedLock() you MUST have a call to OSSchedUnlock().
************************************************************************************************************************
*/

void  OSSchedUnlock (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u   
    if(OSIntNestingCtr > (OS_NESTING_CTR)0) /*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_SCHED_LOCK_ISR;
        return; 
    }
#endif  
    
    if(OSSchedLockNestingCtr == (OS_NESTING_CTR)0)/*检查调度器是否已经完全解锁*/
    {
        *p_err = OS_ERR_SCHED_NOT_LOCKED;
        return;         
    }
    
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Make sure multitasking is running                      */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }  
    
    *p_err = OS_ERR_NONE;/*rt_exit_critical没有返回错误码*/
    
    rt_exit_critical();
    
    if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0)/*检查调度器是否还有锁定嵌套*/
    {
        *p_err = OS_ERR_SCHED_LOCKED;      
    }
}

/*
************************************************************************************************************************
*                                      CONFIGURE ROUND-ROBIN SCHEDULING PARAMETERS
*
* Description: This function is called to change the round-robin scheduling parameters.
*
* Arguments  : en                determines whether round-robin will be enabled (when DEF_EN) or not (when DEF_DIS)
*
*              dflt_time_quanta  default number of ticks between time slices.  0 means assumes OSCfg_TickRate_Hz / 10.
*
*              p_err             is a pointer to a variable that will contain an error code returned by this function.
*
*                                    OS_ERR_NONE    The call was successful
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void  OSSchedRoundRobinCfg (CPU_BOOLEAN   en,
                            OS_TICK       dflt_time_quanta,
                            OS_ERR       *p_err)
{
    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    CPU_CRITICAL_ENTER();
    if (en != DEF_ENABLED) {
        //OSSchedRoundRobinEn = DEF_DISABLED;
        RT_DEBUG_LOG(OS_CFG_DBG_EN,("在RT-Thread中时间片轮转是必选项,不可禁用!\n"));
    } else {
        OSSchedRoundRobinEn = DEF_ENABLED;
    }

    if (dflt_time_quanta > (OS_TICK)0) {
        OSSchedRoundRobinDfltTimeQuanta = dflt_time_quanta;
    } else {
        OSSchedRoundRobinDfltTimeQuanta = (OS_TICK)(OS_CFG_TICK_RATE_HZ / (OS_RATE_HZ)10);
    }
    CPU_CRITICAL_EXIT();
   *p_err = OS_ERR_NONE;
}
#endif

/*
************************************************************************************************************************
*                                    YIELD CPU WHEN TASK NO LONGER NEEDS THE TIME SLICE
*
* Description: This function is called to give up the CPU when it is done executing before its time slice expires.
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE                   The call was successful
*                           - OS_ERR_ROUND_ROBIN_1          Only 1 task at this priority, nothing to yield to
*                             OS_ERR_ROUND_ROBIN_DISABLED   Round Robin is not enabled
*                             OS_ERR_SCHED_LOCKED           The scheduler has been locked
*                             OS_ERR_YIELD_ISR              Can't be called from an ISR
*                         -------------说明-------------
*                             OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                           - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                           + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                           应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : none
*
* Note(s)    : 1) This function MUST be called from a task.
************************************************************************************************************************
*/

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void  OSSchedRoundRobinYield (OS_ERR  *p_err)
{
    rt_err_t rt_err;
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u    
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_YIELD_ISR;
        return; 
    }
#endif
    
    if(OSSchedLockNestingCtr > (OS_NESTING_CTR)0)/*检查调度器是否被锁*/
    {
        *p_err = OS_ERR_SCHED_LOCKED;
        return;         
    }
    
    if (OSSchedRoundRobinEn != DEF_TRUE) {                  /* Make sure round-robin has been enabled                 */
       *p_err = OS_ERR_ROUND_ROBIN_DISABLED;
        return;
    } 
    
    rt_err = rt_thread_yield();
    *p_err = rt_err_to_ucosiii(rt_err); 
}
#endif

/*
************************************************************************************************************************
*                                                 START MULTITASKING
*
* Description: This function is used to start the multitasking process which lets uC/OS-III manages the task that you
*              created.  Before you can call OSStart(), you MUST have called OSInit() and you MUST have created at least
*              one application task.
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_FATAL_RETURN    OS was running and OSStart() returned.
*                             OS_ERR_OS_RUNNING      OS is already running, OSStart() has no effect
*
* Returns    : none
*
* Note(s)    : 1) OSStartHighRdy() MUST:
*                 a) Call OSTaskSwHook() then,
*                 b) Load the context of the task pointed to by OSTCBHighRdyPtr.
*                 c) Execute the task.
*
*              2) OSStart() is not supposed to return.  If it does, that would be considered a fatal error.
************************************************************************************************************************
*/

void  OSStart (OS_ERR  *p_err)
{    
    CPU_SR_ALLOC();    
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif
    
    CPU_CRITICAL_ENTER();
    
    /*由于在兼容层运行之前,RT-Thread操作系统已经运行,因此在本函数对OSRunning的操作转移到OSInit函数中*/
    if (OSRunning == OS_STATE_OS_STOPPED) {
        *p_err           = OS_ERR_FATAL_RETURN;             /* OSStart() is not supposed to return                    */
    } else {
        *p_err           = OS_ERR_OS_RUNNING;               /* OS is already running                                  */
    }      
    
    CPU_CRITICAL_EXIT();
}

/*
************************************************************************************************************************
*                                                    GET VERSION
*
* Description: This function is used to return the version number of uC/OS-III.  The returned value corresponds to
*              uC/OS-III's version number multiplied by 10000.  In other words, version 3.01.02 would be returned as 30102.
*
* Arguments  : p_err   is a pointer to a variable that will receive an error code.  However, OSVersion() set this
*                      variable to
*
*                         OS_ERR_NONE
*
* Returns    : The version number of uC/OS-III multiplied by 10000.
************************************************************************************************************************
*/

CPU_INT16U  OSVersion (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((CPU_INT16U)0u);
    }
#endif
    
    *p_err = OS_ERR_NONE;
    return OS_VERSION;
}

/*
************************************************************************************************************************
*                                                      IDLE TASK
*
* Description: This task is internal to uC/OS-III and executes whenever no other higher priority tasks executes because
*              they are ALL waiting for event(s) to occur.
*
* Arguments  : p_arg    is an argument passed to the task when the task is created.
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
*
*              2) OSIdleTaskHook() is called after the critical section to ensure that interrupts will be enabled for at
*                 least a few instructions.  On some processors (ex. Philips XA), enabling and then disabling interrupts
*                 doesn't allow the processor enough time to have interrupts enabled before they were disabled again.
*                 uC/OS-III would thus never recognize interrupts.
*
*              3) This hook has been added to allow you to do such things as STOP the CPU to conserve power.
*
*              4) 在μCOS-III兼容层中，OS_IdleTask不再是一个函数，而是一个RT-Thread操作系统Idle任务的回调函数
************************************************************************************************************************
*/

void  OS_IdleTask (void)
{
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();
    OSIdleTaskCtr++;
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCtr++;
#endif
    CPU_CRITICAL_EXIT();

    OSIdleTaskHook();                                   /* Call user definable HOOK                               */

}

/*
************************************************************************************************************************
*                                               INITIALIZE THE IDLE TASK
*
* Description: This function initializes the idle task
*
* Arguments  : p_err    is a pointer to a variable that will contain an error code returned by this function.
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
*              2) 在μCOS-III兼容层中，OS_IdleTask不再是一个函数，而是一个RT-Thread操作系统Idle任务的回调函数
************************************************************************************************************************
*/

void  OS_IdleTaskInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    OSIdleTaskCtr = (OS_IDLE_CTR)0;
    
    /*向RTT注册μCOS-III兼容层空闲任务(实则为回调函数)*/      
    rt_thread_idle_sethook(OS_IdleTask);
    
}
