/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-02     Meco Man     the first verion
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
*                                                   MUTEX MANAGEMENT
*
* File    : OS_MUTEX.C
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

#if OS_CFG_MUTEX_EN > 0u
/*
************************************************************************************************************************
*                                                   CREATE A MUTEX
*
* Description: This function creates a mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex to initialize.  Your application is responsible for allocating
*                            storage for the mutex.
*
*              p_name        is a pointer to the name you would like to give the mutex.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                    if the call was successful
*                                OS_ERR_CREATE_ISR              if you called this function from an ISR
*                                OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Mutex after you called
*                                                                 OSSafetyCriticalStart().
*                                OS_ERR_NAME                    if 'p_name'  is a NULL pointer
*                                OS_ERR_OBJ_CREATED             if the mutex has already been created
*                                OS_ERR_OBJ_PTR_NULL            if 'p_mutex' is a NULL pointer
*
* Returns    : none
************************************************************************************************************************
*/

void  OSMutexCreate (OS_MUTEX  *p_mutex,
                     CPU_CHAR  *p_name,
                     OS_ERR    *p_err)
{
    rt_err_t rt_err;
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
    CPU_SR_ALLOC();
#endif

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;
        return;
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)                 /* 检查是否在中断中运行                                   */
    {
        *p_err = OS_ERR_CREATE_ISR;
        return;
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if(p_mutex == RT_NULL)                                  /* 检查互斥量指针是否为NULL                               */
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
    if(p_name == RT_NULL)                                   /* 检查互斥量名称指针是否为NULL                           */
    {
        *p_err = OS_ERR_NAME;
        return;
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    /*判断内核对象是否已经是信号量，即是否已经创建过*/
    if(rt_object_get_type(&p_mutex->Mutex.parent.parent) == RT_Object_Class_Mutex)
    {
        *p_err = OS_ERR_OBJ_CREATED;
        return;
    }
#endif

    rt_err = rt_mutex_init(&p_mutex->Mutex,(const char *)p_name,RT_IPC_FLAG_PRIO); /* uCOS-III仅支持以优先级进行排列  */
    *p_err = rt_err_to_ucosiii(rt_err);
    if(rt_err != RT_EOK)
    {
        return;
    }

#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
    CPU_CRITICAL_ENTER();
#if (OS_CFG_DBG_EN > 0u)
    p_mutex->NamePtr           =  p_name;
#endif
    p_mutex->Type              =  OS_OBJ_TYPE_MUTEX;
    p_mutex->OwnerNestingCtr   = (OS_NESTING_CTR)0;         /* Mutex is available                                     */
    p_mutex->OwnerTCBPtr       = (OS_TCB       *)0;
    p_mutex->OwnerOriginalPrio =  OS_CFG_PRIO_MAX;
#if OS_CFG_DBG_EN > 0u
    OS_MutexDbgListAdd(p_mutex);
#endif
    OSMutexQty++;
    CPU_CRITICAL_EXIT();
#endif
}

/*
************************************************************************************************************************
*                                                   DELETE A MUTEX
*
* Description: This function deletes a mutex and readies all tasks pending on the mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex to delete
*
*              opt           determines delete options as follows:
*
*                                OS_OPT_DEL_NO_PEND          Delete mutex ONLY if no task pending
*                                OS_OPT_DEL_ALWAYS           Deletes the mutex even if tasks are waiting.
*                                                            In this case, all the tasks pending will be readied.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                 The call was successful and the mutex was deleted
*                                OS_ERR_DEL_ISR              If you attempted to delete the mutex from an ISR
*                                OS_ERR_ILLEGAL_DEL_RUN_TIME If you are trying to delete the event flag group after you
*                                                               called OSStart()
*                                OS_ERR_OBJ_PTR_NULL         If 'p_mutex' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE             If 'p_mutex' is not pointing to a mutex
*                                OS_ERR_OPT_INVALID          An invalid option was specified
*                                OS_ERR_OS_NOT_RUNNING       If uC/OS-III is not running yet
*                                OS_ERR_TASK_WAITING         One or more tasks were waiting on the mutex
*
* Returns    : == 0          if no tasks were waiting on the mutex, or upon error.
*              >  0          if one or more tasks waiting on the mutex are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the mutex MUST
*                 check the return code of OSMutexPend().
*
*              2) OSMutexAccept() callers will not know that the intended mutex has been deleted.
*
*              3) Because ALL tasks pending on the mutex will be readied, you MUST be careful in applications where the
*                 mutex is used for mutual exclusion because the resource(s) will no longer be guarded by the mutex.
************************************************************************************************************************
*/

#if OS_CFG_MUTEX_DEL_EN > 0u
OS_OBJ_QTY  OSMutexDel (OS_MUTEX  *p_mutex,
                        OS_OPT     opt,
                        OS_ERR    *p_err)
{
    rt_err_t rt_err;
    rt_uint32_t pend_mutex_len;

    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_OBJ_QTY)0);
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == OS_TRUE) {
       *p_err = OS_ERR_ILLEGAL_DEL_RUN_TIME;
        return (0u);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)                 /* 检查是否在中断中运行                                   */
    {
        *p_err = OS_ERR_DEL_ISR;
        return 0;
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Is the kernel running?                                 */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (0u);
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if(p_mutex == RT_NULL)                                  /* 检查指针是否为空                                       */
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    }
    switch (opt) {
        case OS_OPT_DEL_NO_PEND:
        case OS_OPT_DEL_ALWAYS:
             break;

        default:
            *p_err =  OS_ERR_OPT_INVALID;
             return ((OS_OBJ_QTY)0);
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    /*判断内核对象是否为互斥量*/
    if(rt_object_get_type(&p_mutex->Mutex.parent.parent) != RT_Object_Class_Mutex)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;
    }
#endif

    CPU_CRITICAL_ENTER();
    pend_mutex_len = rt_list_len(&(p_mutex->Mutex.parent.suspend_thread));
    CPU_CRITICAL_EXIT();

    switch (opt)
    {
        case OS_OPT_DEL_NO_PEND:
            CPU_CRITICAL_ENTER();
            if(rt_list_isempty(&(p_mutex->Mutex.parent.suspend_thread))) /* 若没有线程等待信号量                      */
            {
                CPU_CRITICAL_EXIT();
                rt_err = rt_mutex_detach(&p_mutex->Mutex);
                *p_err = rt_err_to_ucosiii(rt_err);
            }
            else
            {
                CPU_CRITICAL_EXIT();
                *p_err = OS_ERR_TASK_WAITING;
            }
            break;

        case OS_OPT_DEL_ALWAYS:
            rt_err = rt_mutex_detach(&p_mutex->Mutex);
            *p_err = rt_err_to_ucosiii(rt_err);
            break;
    }

    if(*p_err == OS_ERR_NONE)
    {
        CPU_CRITICAL_ENTER();
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
#if OS_CFG_DBG_EN > 0u
        OS_MutexDbgListRemove(p_mutex);
#endif
        OSMutexQty--;
#endif
        OS_MutexClr(p_mutex);
        CPU_CRITICAL_EXIT();
    }

    return pend_mutex_len;
}
#endif

/*
************************************************************************************************************************
*                                                    PEND ON MUTEX
*
* Description: This function waits for a mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for the
*                            resource up to the amount of time (in 'ticks') specified by this argument.  If you specify
*                            0, however, your task will wait forever at the specified mutex or, until the resource
*                            becomes available.
*
*              opt           determines whether the user wants to block if the mutex is not available or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the mutex was posted or
*                            pend aborted or the mutex deleted.  If you pass a NULL pointer (i.e. (CPU_TS *)0) then you
*                            will not get the timestamp.  In other words, passing a NULL pointer is valid and indicates
*                            that you don't need the timestamp.
*                            -------------说明-------------
*                            该参数在RTT中没有意义,填NULL即可
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE               The call was successful and your task owns the resource
*                                OS_ERR_MUTEX_OWNER        If calling task already owns the mutex
*                                OS_ERR_MUTEX_OVF          Mutex nesting counter overflowed
*                              - OS_ERR_OBJ_DEL            If 'p_mutex' was deleted
*                                OS_ERR_OBJ_PTR_NULL       If 'p_mutex' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE           If 'p_mutex' is not pointing at a mutex
*                                OS_ERR_OPT_INVALID        If you didn't specify a valid option
*                                OS_ERR_OS_NOT_RUNNING     If uC/OS-III is not running yet
*                                OS_ERR_PEND_ABORT         If the pend was aborted by another task
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                                          would lead to a suspension.
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the mutex was not
*                                                          available.
*                                OS_ERR_SCHED_LOCKED       If you called this function when the scheduler is locked
*                              - OS_ERR_STATE_INVALID      If the task is in an invalid state
*                              - OS_ERR_STATUS_INVALID     If the pend status has an invalid value
*                                OS_ERR_TIMEOUT            The mutex was not received within the specified timeout.
*                            -------------说明-------------
*                                OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                              - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                              + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                              应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : none
************************************************************************************************************************
*/

void  OSMutexPend (OS_MUTEX  *p_mutex,
                   OS_TICK    timeout,
                   OS_OPT     opt,
                   CPU_TS    *p_ts,
                   OS_ERR    *p_err)
{
    rt_int32_t time;
    rt_err_t rt_err;
    OS_TCB *p_tcb;
#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
    rt_thread_t thread;
#endif

    CPU_SR_ALLOC();

    CPU_VAL_UNUSED(p_ts);

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)                 /* 检查是否在中断中运行                                   */
    {
        *p_err = OS_ERR_PEND_ISR;
        return;
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Is the kernel running?                                 */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if(p_mutex == RT_NULL)                                  /* 检查互斥量指针是否为空                                 */
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
    switch (opt) {
        case OS_OPT_PEND_BLOCKING:
        case OS_OPT_PEND_NON_BLOCKING:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return;
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    /*判断内核对象是否为互斥量*/
    if(rt_object_get_type(&p_mutex->Mutex.parent.parent) != RT_Object_Class_Mutex)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return;
    }
#endif

    /*
        在RTT中timeout为0表示不阻塞,为RT_WAITING_FOREVER表示永久阻塞,
        这与uCOS-III有所不同,因此需要转换
    */
    if((opt & OS_OPT_PEND_NON_BLOCKING) == (OS_OPT)0)
    {
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0)      /* 检查调度器是否被锁                                     */
        {
            *p_err = OS_ERR_SCHED_LOCKED;
            return;
        }
        if(timeout == 0)                                    /* 在uCOS-III中timeout=0表示永久阻塞                      */
        {
            time = RT_WAITING_FOREVER;
        }
        else
        {
            time = timeout;
        }
    }
    else
    {
        time = RT_WAITING_NO;                               /* 在RTT中timeout为0表示非阻塞                            */
    }

    CPU_CRITICAL_ENTER();
    p_tcb = OSTCBCurPtr;
    p_tcb->PendStatus = OS_STATUS_PEND_OK;                  /* Clear pend status                                      */
    p_tcb->TaskState |= OS_TASK_STATE_PEND;
    p_tcb->PendOn = OS_TASK_PEND_ON_MUTEX;
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
    p_mutex->OwnerNestingCtr = p_mutex->Mutex.hold;         /* 更新互斥量的嵌套值                                     */
#endif
    if (p_mutex->Mutex.hold == (OS_NESTING_CTR)-1) {
        CPU_CRITICAL_EXIT();
       *p_err = OS_ERR_MUTEX_OVF;
        return;
    }
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
    p_mutex->OwnerOriginalPrio = p_mutex->Mutex.original_priority;/* 更新互斥量原始优先级                             */
    p_mutex->OwnerTCBPtr = (OS_TCB*)p_mutex->Mutex.owner;   /* 更新互斥量所拥有的任务指针                             */
#if OS_CFG_DBG_EN > 0u
    p_tcb->DbgNamePtr = p_mutex->NamePtr;
    p_mutex->DbgNamePtr = p_tcb->Task.name;
#endif
#endif
    CPU_CRITICAL_EXIT();

    rt_err = rt_mutex_take(&p_mutex->Mutex,time);
    *p_err = rt_err_to_ucosiii(rt_err);
    if(*p_err == OS_ERR_TIMEOUT && time == RT_WAITING_NO)
    {
        *p_err = OS_ERR_PEND_WOULD_BLOCK;
    }

    CPU_CRITICAL_ENTER();
    /*更新任务状态*/
    p_tcb->TaskState &= ~OS_TASK_STATE_PEND;
    /*清除当前任务等待状态*/
    p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING;
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
    p_mutex->OwnerNestingCtr = p_mutex->Mutex.hold;         /* 更新互斥量的嵌套值                                     */
    p_mutex->OwnerOriginalPrio = p_mutex->Mutex.original_priority;/* 更新互斥量原始优先级                             */
    p_mutex->OwnerTCBPtr = (OS_TCB*)p_mutex->Mutex.owner;   /* 更新互斥量所拥有的任务指针                             */
#if OS_CFG_DBG_EN > 0u
    p_tcb->DbgNamePtr = (CPU_CHAR *)((void *)" ");
    if(!rt_list_isempty(&(p_mutex->Mutex.parent.suspend_thread)))
    {
        /*若等待表不为空，则将当前等待互斥量的线程赋值给.DbgNamePtr*/
        thread = rt_list_entry((&(p_mutex->Mutex.parent.suspend_thread))->next, struct rt_thread, tlist);
        p_mutex->DbgNamePtr = thread->name;
    }
    else
    {
        p_mutex->DbgNamePtr =(CPU_CHAR *)((void *)" ");     /* 若为空,则清空当前.DbgNamePtr                           */
    }
#endif
#endif
    if(p_tcb->PendStatus == OS_STATUS_PEND_ABORT)           /* Indicate that we aborted                               */
    {
        CPU_CRITICAL_EXIT();
        *p_err = OS_ERR_PEND_ABORT;
        return;
    }

    if (OSTCBCurPtr == (OS_TCB*)p_mutex->Mutex.owner &&
        p_mutex->Mutex.hold > (OS_NESTING_CTR)1) {          /* See if current task is already the owner of the mutex  */
        CPU_CRITICAL_EXIT();
       *p_err = OS_ERR_MUTEX_OWNER;                         /* Indicate that current task already owns the mutex      */
        return;
    }

    CPU_CRITICAL_EXIT();
}

/*
************************************************************************************************************************
*                                               ABORT WAITING ON A MUTEX
*
* Description: This function aborts & readies any tasks currently waiting on a mutex.  This function should be used
*              to fault-abort the wait on the mutex, rather than to normally signal the mutex via OSMutexPost().
*
* Arguments  : p_mutex   is a pointer to the mutex
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the mutex
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the mutex
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the mutex was readied and
*                                                         informed of the aborted wait; check return value for the
*                                                         number of tasks whose wait on the mutex was aborted.
*                            OS_ERR_OBJ_PTR_NULL          If 'p_mutex' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If 'p_mutex' is not pointing at a mutex
*                            OS_ERR_OPT_INVALID           If you specified an invalid option
*                            OS_ERR_OS_NOT_RUNNING        If uC/OS-III is not running yet
*                            OS_ERR_PEND_ABORT_ISR        If you attempted to call this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the mutex, or upon error.
*              >  0          if one or more tasks waiting on the mutex are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_MUTEX_PEND_ABORT_EN > 0u
OS_OBJ_QTY  OSMutexPendAbort (OS_MUTEX  *p_mutex,
                              OS_OPT     opt,
                              OS_ERR    *p_err)
{
    OS_OBJ_QTY abort_tasks = 0;
#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
    rt_thread_t thread;
#endif

    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_OBJ_QTY)0u);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) {             /* Not allowed to Pend Abort from an ISR                  */
       *p_err =  OS_ERR_PEND_ABORT_ISR;
        return ((OS_OBJ_QTY)0u);
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Is the kernel running?                                 */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (0u);
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if (p_mutex == (OS_MUTEX *)0) {                         /* Validate 'p_sem'                                       */
       *p_err =  OS_ERR_OBJ_PTR_NULL;
        return ((OS_OBJ_QTY)0u);
    }
    switch (opt) {                                          /* Validate 'opt'                                         */
        case OS_OPT_PEND_ABORT_1:
        case OS_OPT_PEND_ABORT_ALL:
        case OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED:
             break;

        default:
            *p_err =  OS_ERR_OPT_INVALID;
             return ((OS_OBJ_QTY)0u);
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    if (rt_object_get_type(&p_mutex->Mutex.parent.parent) != RT_Object_Class_Mutex) {
       *p_err =  OS_ERR_OBJ_TYPE;
        return ((OS_OBJ_QTY)0u);
    }
#endif

    CPU_CRITICAL_ENTER();
    if(rt_list_isempty(&(p_mutex->Mutex.parent.suspend_thread)))/* 若没有线程等待信号量                               */
    {
        CPU_CRITICAL_EXIT();
       *p_err =  OS_ERR_PEND_ABORT_NONE;
        return ((OS_OBJ_QTY)0u);
    }
    CPU_CRITICAL_EXIT();


    if(opt & OS_OPT_PEND_ABORT_ALL)
    {
        abort_tasks = rt_ipc_pend_abort_all(&(p_mutex->Mutex.parent.suspend_thread));
    }
    else
    {
        rt_ipc_pend_abort_1(&(p_mutex->Mutex.parent.suspend_thread));
        abort_tasks = 1;
    }

    CPU_CRITICAL_ENTER();
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
    p_mutex->OwnerNestingCtr = p_mutex->Mutex.hold;         /* 更新互斥量的嵌套值                                     */
    p_mutex->OwnerOriginalPrio = p_mutex->Mutex.original_priority;/* 更新互斥量原始优先级                             */
    p_mutex->OwnerTCBPtr = (OS_TCB*)p_mutex->Mutex.owner;   /* 更新互斥量所拥有的任务指针                             */
#if OS_CFG_DBG_EN > 0u
    if(!rt_list_isempty(&(p_mutex->Mutex.parent.suspend_thread)))
    {
        /*若等待表不为空，则将当前等待互斥量的线程赋值给.DbgNamePtr*/
        thread = rt_list_entry((&(p_mutex->Mutex.parent.suspend_thread))->next, struct rt_thread, tlist);
        p_mutex->DbgNamePtr = thread->name;
    }
    else
    {
        p_mutex->DbgNamePtr =(CPU_CHAR *)((void *)" ");     /* 若为空,则清空当前.DbgNamePtr                           */
    }
#endif
#endif
    CPU_CRITICAL_EXIT();

    if(!(opt & OS_OPT_POST_NO_SCHED))
    {
        rt_schedule();
    }

    *p_err = OS_ERR_NONE;
    return abort_tasks;
}
#endif

/*
************************************************************************************************************************
*                                                   POST TO A MUTEX
*
* Description: This function signals a mutex
*
* Arguments  : p_mutex  is a pointer to the mutex
*
*              opt      is an option you can specify to alter the behavior of the post.  The choices are:
*
*                           OS_OPT_POST_NONE        No special option selected
*                         - OS_OPT_POST_NO_SCHED    If you don't want the scheduler to be called after the post.
*                       -------------说明-------------
*                        RTT没有实现释放互斥量后不调度，OS_OPT_POST_NO_SCHED不可用
*
*              p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE             The call was successful and the mutex was signaled.
*                           OS_ERR_MUTEX_NESTING    Mutex owner nested its use of the mutex
*                           OS_ERR_MUTEX_NOT_OWNER  If the task posting is not the Mutex owner
*                           OS_ERR_OBJ_PTR_NULL     If 'p_mutex' is a NULL pointer.
*                           OS_ERR_OBJ_TYPE         If 'p_mutex' is not pointing at a mutex
*                           OS_ERR_POST_ISR         If you attempted to post from an ISR
*                           OS_ERR_OS_NOT_RUNNING   If uC/OS-III is not running yet
*                         + OS_ERR_OPT_INVALID
*                         + OS_RT_ERROR
*                       -------------说明-------------
*                           OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                         - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                         + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                         应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : none
************************************************************************************************************************
*/

void  OSMutexPost (OS_MUTEX  *p_mutex,
                   OS_OPT     opt,
                   OS_ERR    *p_err)
{
    rt_err_t rt_err;
#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
    rt_thread_t thread;
#endif

    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)                 /* 检查是否在中断中运行                                   */
    {
        *p_err = OS_ERR_POST_ISR;
        return;
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Is the kernel running?                                 */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if(p_mutex == RT_NULL)                                  /* 检查指针是否为空                                       */
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
    switch (opt) {
        case OS_OPT_POST_NONE:
        case OS_OPT_POST_NO_SCHED:
             break;

        default:
            *p_err =  OS_ERR_OPT_INVALID;
             return;
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    /*判断内核对象是否为信号量*/
    if(rt_object_get_type(&p_mutex->Mutex.parent.parent) != RT_Object_Class_Mutex)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return;
    }
#endif

    rt_err = rt_mutex_release(&p_mutex->Mutex);
    *p_err = rt_err_to_ucosiii(rt_err);
    /*只有已经拥有互斥量控制权的线程才能释放*/
    if(rt_err == -RT_ERROR)                                 /* rt_mutex_release返回-RT_ERROR表示该线程非掌握互斥量的线程*/
    {
        *p_err = OS_ERR_MUTEX_NOT_OWNER;
    }

    CPU_CRITICAL_ENTER();
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
    p_mutex->OwnerNestingCtr = p_mutex->Mutex.hold;         /* 更新互斥量的嵌套值                                     */
    p_mutex->OwnerOriginalPrio = p_mutex->Mutex.original_priority; /* 更新互斥量原始优先级                            */
    p_mutex->OwnerTCBPtr = (OS_TCB*)p_mutex->Mutex.owner;   /* 更新互斥量所拥有的任务指针                             */
#if OS_CFG_DBG_EN > 0u
    if(!rt_list_isempty(&(p_mutex->Mutex.parent.suspend_thread)))
    {
        /*若等待表不为空，则将当前等待互斥量的线程赋值给.DbgNamePtr*/
        thread = rt_list_entry((&(p_mutex->Mutex.parent.suspend_thread))->next, struct rt_thread, tlist);
        p_mutex->DbgNamePtr = thread->name;
    }
    else
    {
        p_mutex->DbgNamePtr =(CPU_CHAR *)((void *)" ");     /* 若为空,则清空当前.DbgNamePtr                           */
    }
#endif
#endif
    if (p_mutex->Mutex.hold > (OS_NESTING_CTR)0) {          /* Are we done with all nestings?                         */
        CPU_CRITICAL_EXIT();                                /* No                                                     */
       *p_err = OS_ERR_MUTEX_NESTING;
        return;
    }

    CPU_CRITICAL_EXIT();
}

/*
************************************************************************************************************************
*                                            CLEAR THE CONTENTS OF A MUTEX
*
* Description: This function is called by OSMutexDel() to clear the contents of a mutex
*

* Argument(s): p_mutex      is a pointer to the mutex to clear
*              -------
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_MutexClr (OS_MUTEX  *p_mutex)
{
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
#if (OS_CFG_DBG_EN > 0u)
    p_mutex->NamePtr           = (CPU_CHAR     *)((void *)"?MUTEX");
#endif
    p_mutex->Type              =  OS_OBJ_TYPE_NONE;         /* Mark the data structure as a NONE                      */
    p_mutex->OwnerNestingCtr   = (OS_NESTING_CTR)0;
    p_mutex->OwnerTCBPtr       = (OS_TCB       *)0;
    p_mutex->OwnerOriginalPrio =  OS_CFG_PRIO_MAX;
#endif
}

/*
************************************************************************************************************************
*                                          ADD/REMOVE MUTEX TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove a mutex to/from the debug list.
*
* Arguments  : p_mutex     is a pointer to the mutex to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
void  OS_MutexDbgListAdd (OS_MUTEX  *p_mutex)
{
    p_mutex->DbgNamePtr               = (CPU_CHAR *)((void *)" ");
    p_mutex->DbgPrevPtr               = (OS_MUTEX *)0;
    if (OSMutexDbgListPtr == (OS_MUTEX *)0) {
        p_mutex->DbgNextPtr           = (OS_MUTEX *)0;
    } else {
        p_mutex->DbgNextPtr           =  OSMutexDbgListPtr;
        OSMutexDbgListPtr->DbgPrevPtr =  p_mutex;
    }
    OSMutexDbgListPtr                 =  p_mutex;
}



void  OS_MutexDbgListRemove (OS_MUTEX  *p_mutex)
{
    OS_MUTEX  *p_mutex_next;
    OS_MUTEX  *p_mutex_prev;

    p_mutex->DbgNamePtr               = (CPU_CHAR *)((void *)" ");
    p_mutex_prev = p_mutex->DbgPrevPtr;
    p_mutex_next = p_mutex->DbgNextPtr;

    if (p_mutex_prev == (OS_MUTEX *)0) {
        OSMutexDbgListPtr = p_mutex_next;
        if (p_mutex_next != (OS_MUTEX *)0) {
            p_mutex_next->DbgPrevPtr = (OS_MUTEX *)0;
        }
        p_mutex->DbgNextPtr = (OS_MUTEX *)0;

    } else if (p_mutex_next == (OS_MUTEX *)0) {
        p_mutex_prev->DbgNextPtr = (OS_MUTEX *)0;
        p_mutex->DbgPrevPtr      = (OS_MUTEX *)0;

    } else {
        p_mutex_prev->DbgNextPtr =  p_mutex_next;
        p_mutex_next->DbgPrevPtr =  p_mutex_prev;
        p_mutex->DbgNextPtr      = (OS_MUTEX *)0;
        p_mutex->DbgPrevPtr      = (OS_MUTEX *)0;
    }
}
#endif

/*
************************************************************************************************************************
*                                                MUTEX INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the mutex management.
*

* Argument(s): p_err        is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE     the call was successful
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_MutexInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
#if OS_CFG_DBG_EN > 0u
    OSMutexDbgListPtr = (OS_MUTEX *)0;
#endif
    OSMutexQty        = (OS_OBJ_QTY)0;
#endif
   *p_err             =  OS_ERR_NONE;
}

#endif
