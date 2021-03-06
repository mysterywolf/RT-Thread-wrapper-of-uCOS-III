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
*********************************************************************************************************
*                                              uC/OS-III
*                                        The Real-Time Kernel
*
*                    Copyright 2009-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/
/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   TIME MANAGEMENT
*
* File    : OS_TIME.C
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

#include "os.h"

/*
************************************************************************************************************************
*                                                  DELAY TASK 'n' TICKS
*
* Description: This function is called to delay execution of the currently running task until the specified number of
*              system ticks expires.  This, of course, directly equates to delaying the current task for some time to
*              expire.  No delay will result if the specified delay is 0.  If the specified delay is greater than 0
*              then, a context switch will result.
*
* Arguments  : dly       is a value in 'clock ticks' that the task will either delay for or, the target match value
*                        of the tick counter (OSTickCtr).  Note that specifying 0 means the task is not to delay.
*
*                        depending on the option argument, the task will wake up when OSTickCtr reaches:
*
*                            OS_OPT_TIME_DLY      : OSTickCtr + dly
*                            OS_OPT_TIME_TIMEOUT  : OSTickCtr + dly
*                            OS_OPT_TIME_MATCH    : dly
*                            OS_OPT_TIME_PERIODIC : OSTCBCurPtr->TickCtrPrev + dly
*
*              opt       specifies whether 'dly' represents absolute or relative time; default option marked with *** :
*
*                        *** OS_OPT_TIME_DLY        specifies a relative time from the current value of OSTickCtr.
*                            OS_OPT_TIME_TIMEOUT    same as OS_OPT_TIME_DLY.
*                            OS_OPT_TIME_MATCH      indicates that 'dly' specifies the absolute value that OSTickCtr
*                                                   must reach before the task will be resumed.
*                            OS_OPT_TIME_PERIODIC   indicates that 'dly' specifies the periodic value that OSTickCtr
*                                                   must reach before the task will be resumed.
*
*              p_err     is a pointer to a variable that will contain an error code from this call.
*
*                            OS_ERR_NONE            the call was successful and the delay occurred.
*                            OS_ERR_OPT_INVALID     if you specified an invalid option for this function.
*                            OS_ERR_OS_NOT_RUNNING  If uC/OS-III is not running yet
*                            OS_ERR_SCHED_LOCKED    can't delay when the scheduler is locked.
*                            OS_ERR_TIME_DLY_ISR    if you called this function from an ISR.
*                            OS_ERR_TIME_ZERO_DLY   if you specified a delay of zero.
*
* Returns    : none
************************************************************************************************************************
*/

void  OSTimeDly (OS_TICK   dly,
                 OS_OPT    opt,
                 OS_ERR   *p_err)
{
    rt_err_t rt_err;

    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)                     /* 检查是否在中断中运行                                 */
    {
        *p_err = OS_ERR_TIME_DLY_ISR;
        return;
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                     /* Is the kernel running?                               */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }
#endif

    if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0)              /* 检查调度器是否被锁                                   */
    {
        *p_err = OS_ERR_SCHED_LOCKED;
        return;
    }

#if OS_CFG_ARG_CHK_EN > 0u
    if(dly == 0)                                                /* 检查是否为0延时                                      */
    {
        *p_err = OS_ERR_TIME_ZERO_DLY;
        return;
    }

    opt &= OS_OPT_TIME_MASK;
    switch (opt) {
        case OS_OPT_TIME_DLY:
        case OS_OPT_TIME_TIMEOUT:
        case OS_OPT_TIME_PERIODIC:
             if (dly == (OS_TICK)0u) {                          /* 0 means no delay!                                    */
                *p_err = OS_ERR_TIME_ZERO_DLY;
                 return;
             }
             break;

        case OS_OPT_TIME_MATCH:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return;
    }
#endif

    CPU_CRITICAL_ENTER();
    OSTCBCurPtr->TaskState |= OS_TASK_STATE_DLY;
    CPU_CRITICAL_EXIT();

    if(opt == OS_OPT_TIME_MATCH)
    {
        rt_err = rt_thread_delay(dly - rt_tick_get());
    }
    else/*不区分OS_OPT_TIME_DLY、OS_OPT_TIME_TIMEOUT、OS_OPT_TIME_MATCH*/
    {
        rt_err = rt_thread_delay(dly);
    }

    *p_err = rt_err_to_ucosiii(rt_err);

    CPU_CRITICAL_ENTER();
    OSTCBCurPtr->TaskState &= ~OS_TASK_STATE_DLY;
    CPU_CRITICAL_EXIT();
}

/*
************************************************************************************************************************
*                                             DELAY TASK FOR SPECIFIED TIME
*
* Description: This function is called to delay execution of the currently running task until some time expires.  This
*              call allows you to specify the delay time in HOURS, MINUTES, SECONDS and MILLISECONDS instead of ticks.
*
* Arguments  : hours     specifies the number of hours that the task will be delayed (max. is 999 if the tick rate is
*                        1000 Hz or less otherwise, a higher value would overflow a 32-bit unsigned counter).
*
*              minutes   specifies the number of minutes (max. 59 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
*
*              seconds   specifies the number of seconds (max. 59 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
*
*              milli     specifies the number of milliseconds (max. 999 if 'opt' is OS_OPT_TIME_HMSM_STRICT)
*
*              opt       specifies time delay bit-field options logically OR'd; default options marked with *** :
*
*                            OS_OPT_TIME_DLY        specifies a relative time from the current value of OSTickCtr.
*                            OS_OPT_TIME_TIMEOUT    same as OS_OPT_TIME_DLY.
*                            OS_OPT_TIME_MATCH      indicates that the delay specifies the absolute value that OSTickCtr
*                                                   must reach before the task will be resumed.
*                            OS_OPT_TIME_PERIODIC   indicates that the delay specifies the periodic value that OSTickCtr
*                                                   must reach before the task will be resumed.
*
*                        *** OS_OPT_TIME_HMSM_STRICT            strictly allow only hours        (0...99)
*                                                                                   minutes      (0...59)
*                                                                                   seconds      (0...59)
*                                                                                   milliseconds (0...999)
*                            OS_OPT_TIME_HMSM_NON_STRICT        allow any value of  hours        (0...999)
*                                                                                   minutes      (0...9999)
*                                                                                   seconds      (0...65535)
*                                                                                   milliseconds (0...4294967295)
*
*              p_err     is a pointer to a variable that will receive an error code from this call.
*
*                            OS_ERR_NONE                        If the function returns from the desired delay
*                            OS_ERR_OPT_INVALID                 If you specified an invalid option for 'opt'
*                            OS_ERR_OS_NOT_RUNNING              If uC/OS-III is not running yet
*                            OS_ERR_SCHED_LOCKED                Can't delay when the scheduler is locked
*                            OS_ERR_TIME_DLY_ISR                If called from an ISR
*                            OS_ERR_TIME_INVALID_HOURS          If you didn't specify a valid value for 'hours'
*                            OS_ERR_TIME_INVALID_MINUTES        If you didn't specify a valid value for 'minutes'
*                            OS_ERR_TIME_INVALID_SECONDS        If you didn't specify a valid value for 'seconds'
*                            OS_ERR_TIME_INVALID_MILLISECONDS   If you didn't specify a valid value for 'milli'
*                            OS_ERR_TIME_ZERO_DLY               If hours, minutes, seconds and milli are all 0
*
* Returns    : none
*
* Note(s)    : 1) The resolution on the milliseconds depends on the tick rate.  For example, you can't do a 10 mS delay
*                 if the ticker interrupts every 100 mS.  In this case, the delay would be set to 0.  The actual delay
*                 is rounded to the nearest tick.
*
*              2) Although this function allows you to delay a task for many, many hours, it's not recommended to put
*                 a task to sleep for that long.
*
*              3) opt默认是含有OS_OPT_TIME_HMSM_STRICT，如果定义为1000ms直接返回错误,为防止这种错误建议使用
*                 OS_OPT_TIME_PERIODIC|OS_OPT_TIME_HMSM_NON_STRICT
************************************************************************************************************************
*/

#if OS_CFG_TIME_DLY_HMSM_EN > 0u
void  OSTimeDlyHMSM (CPU_INT16U   hours,
                     CPU_INT16U   minutes,
                     CPU_INT16U   seconds,
                     CPU_INT32U   milli,
                     OS_OPT       opt,
                     OS_ERR      *p_err)
{
    rt_int32_t dly_ms;
#if OS_CFG_ARG_CHK_EN > 0u
    CPU_BOOLEAN  opt_invalid;
    CPU_BOOLEAN  opt_non_strict;
#endif

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)                     /* 检查是否在中断中运行                                 */
    {
        *p_err = OS_ERR_TIME_DLY_ISR;
        return;
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                     /* Is the kernel running?                               */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }
#endif

    if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0)              /* 检查调度器是否被锁                                   */
    {
        *p_err = OS_ERR_SCHED_LOCKED;
        return;
    }

#if OS_CFG_ARG_CHK_EN > 0u
    opt_invalid = DEF_BIT_IS_SET_ANY(opt, ~OS_OPT_TIME_OPTS_MASK);
    if (opt_invalid == DEF_YES) {
       *p_err = OS_ERR_OPT_INVALID;
        return;
    }
    opt_non_strict = DEF_BIT_IS_SET(opt, OS_OPT_TIME_HMSM_NON_STRICT);
    if (opt_non_strict != DEF_YES) {
         if (milli   > (CPU_INT32U)999u) {
            *p_err = OS_ERR_TIME_INVALID_MILLISECONDS;
             return;
         }
         if (seconds > (CPU_INT16U)59u) {
            *p_err = OS_ERR_TIME_INVALID_SECONDS;
             return;
         }
         if (minutes > (CPU_INT16U)59u) {
            *p_err = OS_ERR_TIME_INVALID_MINUTES;
             return;
         }
         if (hours   > (CPU_INT16U)99u) {
            *p_err = OS_ERR_TIME_INVALID_HOURS;
             return;
         }
    } else {
         if (minutes > (CPU_INT16U)9999u) {
            *p_err = OS_ERR_TIME_INVALID_MINUTES;
             return;
         }
         if (hours   > (CPU_INT16U)999u) {
            *p_err = OS_ERR_TIME_INVALID_HOURS;
             return;
         }
    }
#endif

    dly_ms = hours*3600*1000+minutes*60*1000+seconds*1000+milli;

#if OS_CFG_ARG_CHK_EN > 0u
    if(dly_ms == 0)                                             /* 检查是否为0延时                                      */
    {
        *p_err = OS_ERR_TIME_ZERO_DLY;
        return;
    }
#endif

    OSTimeDly(rt_tick_from_millisecond(dly_ms), opt, p_err);
}
#endif

/*
************************************************************************************************************************
*                                                RESUME A DELAYED TASK
*
* Description: This function is used resume a task that has been delayed through a call to either OSTimeDly() or
*              OSTimeDlyHMSM().  Note that cannot call this function to resume a task that is waiting for an event
*              with timeout.
*
* Arguments  : p_tcb    is a pointer to the TCB of the task to resume.
*
*              p_err    is a pointer to a variable that will receive an error code
*
*                           OS_ERR_NONE                  Task has been resumed
*                           OS_ERR_OS_NOT_RUNNING        If uC/OS-III is not running yet
*                           OS_ERR_STATE_INVALID         Task is in an invalid state
*                           OS_ERR_TIME_DLY_RESUME_ISR   If called from an ISR
*                           OS_ERR_TIME_NOT_DLY          Task is not waiting for time to expire
*                           OS_ERR_TASK_SUSPENDED        Task cannot be resumed, it was suspended by OSTaskSuspend()
*
* Note(s)    : none
************************************************************************************************************************
*/

#if OS_CFG_TIME_DLY_RESUME_EN > 0u
void  OSTimeDlyResume (OS_TCB  *p_tcb,
                       OS_ERR  *p_err)
{
    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) {             /* Not allowed to call from an ISR                        */
       *p_err = OS_ERR_TIME_DLY_RESUME_ISR;
        return;
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if (p_tcb == (OS_TCB *)0) {                             /* Not possible for the running task to be delayed!       */
       *p_err = OS_ERR_TASK_NOT_DLY;
        return;
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Is the kernel running?                                 */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }
#endif

    if (p_tcb == (OS_TCB*)rt_thread_self()) {               /* Not possible for the running task to be delayed!       */
       *p_err = OS_ERR_TASK_NOT_DLY;
        return;
    }

    if ((!(p_tcb->Task.thread_timer.parent.flag & RT_TIMER_FLAG_ACTIVATED)) /* Cannot Abort delay if task is ready    */
         || (p_tcb->Task.stat != RT_THREAD_SUSPEND))
    {
        *p_err = OS_ERR_TASK_NOT_DLY;
        return;
    }

    CPU_CRITICAL_ENTER();
    switch (p_tcb->TaskState) {
        case OS_TASK_STATE_RDY:                             /* Cannot Abort delay if task is ready                    */
        case OS_TASK_STATE_PEND:
        case OS_TASK_STATE_PEND_TIMEOUT:
        case OS_TASK_STATE_SUSPENDED:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             CPU_CRITICAL_EXIT();
            *p_err = OS_ERR_TASK_NOT_DLY;
             break;

        case OS_TASK_STATE_DLY:
             p_tcb->TaskState = OS_TASK_STATE_RDY;
             CPU_CRITICAL_EXIT();
            *p_err = OS_ERR_NONE;
             break;

        case OS_TASK_STATE_DLY_SUSPENDED:
             /*在uCOS-III中可以将延时取消,依然保持挂起；但是在RTT中挂起和延时是绑定的*/
//             p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;
             CPU_CRITICAL_EXIT();
            *p_err            = OS_ERR_TASK_SUSPENDED;
             break;

        default:
             CPU_CRITICAL_EXIT();
            *p_err = OS_ERR_STATE_INVALID;
             break;
    }

    if(*p_err == OS_ERR_NONE)
    {
        p_tcb->Task.error = -RT_ETIMEOUT;
        rt_thread_resume(&p_tcb->Task);
        rt_schedule();
    }
}
#endif

/*
************************************************************************************************************************
*                                               GET CURRENT SYSTEM TIME
*
* Description: This function is used by your application to obtain the current value of the counter which keeps track of
*              the number of clock ticks.
*
* Arguments  : p_err    is a pointer to a variable that will receive an error code
*
*                           OS_ERR_NONE           If the call was successful
*
* Returns    : The current value of OSTickCtr
************************************************************************************************************************
*/

OS_TICK  OSTimeGet (OS_ERR  *p_err)
{
    *p_err = OS_ERR_NONE;
    return rt_tick_get();
}

/*
************************************************************************************************************************
*                                                   SET SYSTEM CLOCK
*
* Description: This function sets the counter which keeps track of the number of clock ticks.
*
* Arguments  : ticks    is the desired tick value
*
*              p_err    is a pointer to a variable that will receive an error code
*
*                           OS_ERR_NONE           If the call was successful
*
* Returns    : none
************************************************************************************************************************
*/

void  OSTimeSet (OS_TICK   ticks,
                 OS_ERR   *p_err)
{
    *p_err = OS_ERR_NONE;
    rt_tick_set(ticks);
}

/*
************************************************************************************************************************
*                                                 PROCESS SYSTEM TICK
*
* Description: This function is used to signal to uC/OS-III the occurrence of a 'system tick' (also known as a
*              'clock tick').  This function should be called by the tick ISR.
*
* Arguments  : none
*
* Returns    : none
************************************************************************************************************************
*/

void  OSTimeTick (void)
{
    rt_tick_increase();
}
