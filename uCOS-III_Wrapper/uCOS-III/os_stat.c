/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-08     Meco Man     the first verion
 */
 
/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                  STATISTICS MODULE
*
* File    : OS_STAT.C
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

#if OS_CFG_STAT_TASK_EN > 0u
/*
************************************************************************************************************************
*                                                   RESET STATISTICS
*
* Description: This function is called by your application to reset the statistics.
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE
*
* Returns    : none
************************************************************************************************************************
*/

void  OSStatReset (OS_ERR  *p_err)
{
#if (OS_CFG_DBG_EN > 0u)
    OS_TCB      *p_tcb;
#endif

    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif
    
    CPU_CRITICAL_ENTER();
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageMax = 0u;
    OSStatTaskCPUUsage    = 0u;
#endif
    CPU_CRITICAL_EXIT();
  
#if OS_CFG_DBG_EN > 0u
    CPU_CRITICAL_ENTER();
    p_tcb = OSTaskDbgListPtr;
    CPU_CRITICAL_EXIT();
    while (p_tcb != (OS_TCB *)0) {                          /* Reset per-Task statistics                              */
        CPU_CRITICAL_ENTER();
//#if OS_CFG_TASK_PROFILE_EN > 0u        
//        p_tcb->CPUUsage         = (OS_CPU_USAGE)0;
//        p_tcb->CPUUsageMax      = (OS_CPU_USAGE)0;    
//#endif
        
        p_tcb                   = p_tcb->DbgNextPtr;
        CPU_CRITICAL_EXIT();    
    }
#endif
    *p_err = OS_ERR_NONE;
}

/*
************************************************************************************************************************
*                                                DETERMINE THE CPU CAPACITY
*
* Description: This function is called by your application to establish CPU usage by first determining how high a 32-bit
*              counter would count to in 1/10 second if no other tasks were to execute during that time.  CPU usage is
*              then determined by a low priority task which keeps track of this 32-bit counter every second but this
*              time, with other tasks running.  CPU usage is determined by:
*
*                                             OS_Stat_IdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------------)
*                                            OS_Stat_IdleCtrMax
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE
*                             OS_ERR_OS_NOT_RUNNING    If uC/OS-III is not running yet
*
* Returns    : none
************************************************************************************************************************
*/

void  OSStatTaskCPUUsageInit (OS_ERR  *p_err)
{
    OS_ERR err;
    OS_TICK  dly;
    
    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif
    
#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Is the kernel running?                               */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }
#endif
    
    OSTimeDly((OS_TICK )2,                                  /* Synchronize with clock tick                            */
              (OS_OPT  )OS_OPT_TIME_PERIODIC,
              (OS_ERR *)&err);
    if (err != OS_ERR_NONE) {
       *p_err = err;
        return;
    }
    CPU_CRITICAL_ENTER();
    OSStatTaskCtr = (OS_TICK)0;                             /* Clear idle counter                                     */
    CPU_CRITICAL_EXIT();

    dly = (OS_TICK)0;
    if (OS_CFG_TICK_RATE_HZ > OS_CFG_STAT_TASK_RATE_HZ) {
        dly = (OS_TICK)(OS_CFG_TICK_RATE_HZ / OS_CFG_STAT_TASK_RATE_HZ);
    }
    if (dly == (OS_TICK)0) {
        dly =  (OS_TICK)(OS_CFG_TICK_RATE_HZ / (OS_RATE_HZ)10);
    } 
    OSTimeDly(dly,                                          /* Determine MAX. idle counter value                      */
              OS_OPT_TIME_PERIODIC,
              &err);

    CPU_CRITICAL_ENTER();
    OSStatTaskCtrMax  = OSStatTaskCtr;                      /* Store maximum idle counter count                       */
    OSStatTaskRdy     = OS_STATE_RDY;
    CPU_CRITICAL_EXIT();
   *p_err             = OS_ERR_NONE;
}

/*
************************************************************************************************************************
*                                                    STATISTICS TASK
*
* Description: This task is internal to uC/OS-III and is used to compute some statistics about the multitasking
*              environment.  Specifically, OS_StatTask() computes the CPU usage.  CPU usage is determined by:
*
*                                                   OSStatTaskCtr
*                 OSStatTaskCPUUsage = 100 * (1 - ------------------)     (units are in %)
*                                                  OSStatTaskCtrMax
*
* Arguments  : p_arg     this pointer is not used at this time.
*
* Returns    : none
*
* Note(s)    : 1) This task runs at a priority level higher than the idle task.
*
*              2) You can disable this task by setting the configuration #define OS_CFG_STAT_TASK_EN to 0.
*
*              3) You MUST have at least a delay of 2/10 seconds to allow for the system to establish the maximum value
*                 for the idle counter.
*
*              4) This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_StatTask (void  *p_arg)
{
#if OS_CFG_DBG_EN > 0u
    OS_TCB      *p_tcb;
#if OS_CFG_TMR_EN > 0u
    OS_TMR      *p_tmr;
#endif
#endif
    OS_TICK      ctr_max;
    OS_TICK      ctr_mult;
    OS_TICK      ctr_div;
    OS_ERR       err;
    OS_TICK      dly;

    CPU_SR_ALLOC();


    p_arg = p_arg;                                          /* Prevent compiler warning for not using 'p_arg'         */
    while (OSStatTaskRdy != OS_STATE_RDY) {
        OSTimeDly(2u * OS_CFG_STAT_TASK_RATE_HZ,            /* Wait until statistic task is ready                     */
                  OS_OPT_TIME_PERIODIC,
                  &err);
    }
    OSStatReset(&err);                                      /* Reset statistics                                       */

    dly = (OS_TICK)0;                                       /* Compute statistic task sleep delay                     */
    if (OS_CFG_TICK_RATE_HZ > OS_CFG_STAT_TASK_RATE_HZ) {
        dly = (OS_TICK)(OS_CFG_TICK_RATE_HZ / OS_CFG_STAT_TASK_RATE_HZ);
    }
    if (dly == (OS_TICK)0) {
        dly =  (OS_TICK)(OS_CFG_TICK_RATE_HZ / (OS_RATE_HZ)10);
    }

    while (DEF_ON) {
        CPU_CRITICAL_ENTER();                               /* ----------------- OVERALL CPU USAGE ------------------ */
        OSStatTaskCtrRun   = OSStatTaskCtr;                 /* Obtain the of the stat counter for the past .1 second  */
        OSStatTaskCtr      = (OS_TICK)0;                    /* Reset the stat counter for the next .1 second          */
        CPU_CRITICAL_EXIT();

        if (OSStatTaskCtrMax > OSStatTaskCtrRun) {          /* Compute CPU Usage with best resolution                 */
            if (OSStatTaskCtrMax < 400000u) {                                        /*            1 to       400,000 */
                ctr_mult = 10000u;
                ctr_div  =     1u;
            } else if (OSStatTaskCtrMax <   4000000u) {                              /*      400,000 to     4,000,000 */
                ctr_mult =  1000u;
                ctr_div  =    10u;
            } else if (OSStatTaskCtrMax <  40000000u) {                              /*    4,000,000 to    40,000,000 */
                ctr_mult =   100u;
                ctr_div  =   100u;
            } else if (OSStatTaskCtrMax < 400000000u) {                              /*   40,000,000 to   400,000,000 */
                ctr_mult =    10u;
                ctr_div  =  1000u;
            } else {                                                                 /*  400,000,000 and up           */
                ctr_mult =     1u;
                ctr_div  = 10000u;
            }
            ctr_max            = OSStatTaskCtrMax / ctr_div;
            OSStatTaskCPUUsage = (OS_CPU_USAGE)((OS_TICK)10000u - ctr_mult * OSStatTaskCtrRun / ctr_max);
            if (OSStatTaskCPUUsageMax < OSStatTaskCPUUsage) {
                OSStatTaskCPUUsageMax = OSStatTaskCPUUsage;
            }
        }
        
        OSStatTaskHook();                                   /* Invoke user definable hook                             */

#if OS_CFG_DBG_EN > 0u
        /*--------------任务TCB------------------*/
        CPU_CRITICAL_ENTER();
        p_tcb = OSTaskDbgListPtr;
        CPU_CRITICAL_EXIT();
        while (p_tcb != (OS_TCB *)0) {                      /* 开始沿着TCB链表对每一个任务进行遍历                    */
#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u           
            OSTaskStkChk( p_tcb,                            /* Compute stack usage of active tasks only               */
                         &p_tcb->StkFree,
                         &p_tcb->StkUsed,
                         &err);
#endif
            CPU_CRITICAL_ENTER();
#if OS_CFG_TASK_PROFILE_EN > 0u
            p_tcb->StkPtr = ((struct rt_thread*)p_tcb)->sp; /* 更新SP指针*/
#endif            
            
            p_tcb = p_tcb->DbgNextPtr;                      /* 指向下一个TCB结构体                                    */
            CPU_CRITICAL_EXIT();
        }
        
#if OS_CFG_TMR_EN > 0u        
        /*--------------定时器--------------------*/
        CPU_CRITICAL_ENTER();
        p_tmr = OSTmrDbgListPtr;
        CPU_CRITICAL_EXIT();     
        while(p_tmr != (OS_TMR *)0){
            CPU_CRITICAL_ENTER();
            p_tmr->Remain = p_tmr->Tmr.timeout_tick - rt_tick_get();
            CPU_CRITICAL_EXIT();
            p_tmr = p_tmr->DbgNextPtr;                      /*指向下一个定时器控制块*/
        }
#endif
        
#endif /*#if OS_CFG_DBG_EN > 0u*/        

        if (OSStatResetFlag == DEF_TRUE) {                  /* Check if need to reset statistics                      */
            OSStatResetFlag  = DEF_FALSE;
            OSStatReset(&err);
        }

        OSTimeDly(dly, OS_OPT_TIME_PERIODIC, &err);
    }
}

/*
************************************************************************************************************************
*                                              INITIALIZE THE STATISTICS
*
* Description: This function is called by OSInit() to initialize the statistic task.
*
* Argument(s): p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_STK_INVALID       If you specified a NULL stack pointer during configuration
*                            OS_ERR_STK_SIZE_INVALID  If you didn't specify a large enough stack.
*                            OS_ERR_PRIO_INVALID      If you specified a priority for the statistic task equal to or
*                                                     lower (i.e. higher number) than the idle task.
*                            OS_ERR_xxx               An error code returned by OSTaskCreate()
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_StatTaskInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    OSStatTaskCtr    = (OS_TICK)0;
    OSStatTaskCtrRun = (OS_TICK)0;
    OSStatTaskCtrMax = (OS_TICK)0;
    OSStatTaskRdy    = OS_STATE_NOT_RDY;                    /* Statistic task is not ready                            */
    OSStatResetFlag  = DEF_FALSE;

                                                            /* ---------------- CREATE THE STAT TASK ---------------- */
    if (OSCfg_StatTaskStk == (CPU_STK *)0) {
       *p_err = OS_ERR_STAT_STK_INVALID;
        return;
    }

    if (OS_CFG_STAT_TASK_STK_SIZE < OS_CFG_STK_SIZE_MIN) {
       *p_err = OS_ERR_STAT_STK_SIZE_INVALID;
        return;
    }

    if (OS_CFG_STAT_TASK_PRIO >= (OS_CFG_PRIO_MAX - 1u)) {
       *p_err = OS_ERR_STAT_PRIO_INVALID;
        return;
    }

    OSTaskCreate((OS_TCB     *)&OSStatTaskTCB,
                 (CPU_CHAR   *)((void *)"Stat Task"),
                 (OS_TASK_PTR )OS_StatTask,
                 (void       *)0,
                 (OS_PRIO     )OS_CFG_STAT_TASK_PRIO,
                 (CPU_STK    *)&OSCfg_StatTaskStk[0],
                 (CPU_STK_SIZE)OS_CFG_STAT_TASK_STK_LIMIT,
                 (CPU_STK_SIZE)OS_CFG_STAT_TASK_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)p_err);
}

#endif
