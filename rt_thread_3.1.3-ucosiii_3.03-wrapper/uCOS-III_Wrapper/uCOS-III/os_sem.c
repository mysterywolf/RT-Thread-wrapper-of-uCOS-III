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
*                                                 SEMAPHORE MANAGEMENT
*
* File    : OS_SEM.C
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
* Note(s)    : 1)关于信号量释放(post/release)策略选项的说明:
*                   RTT支持：
*                       RT_IPC_FLAG_PRIO(相当于OS_OPT_POST_1)
*                       RT_IPC_FLAG_FIFO(uCOS-III没有实现,其均是按照优先级排列的)
*                   uCOS-III支持：
*                       OS_OPT_POST_1 (相当于RT_IPC_FLAG_PRIO)
*                       OS_OPT_POST_ALL (RT-Thread未实现)
*                       OS_OPT_POST_NO_SCHED (RT-Thread未实现)
*                   因此只能实现RT_IPC_FLAG_PRIO与OS_OPT_POST_1作为信号量的兼容
************************************************************************************************************************
*/

#if OS_CFG_SEM_EN > 0u
/*
************************************************************************************************************************
*                                                  CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  : p_sem         is a pointer to the semaphore to initialize.  Your application is responsible for
*                            allocating storage for the semaphore.
*
*              p_name        is a pointer to the name you would like to give the semaphore.
*
*              cnt           is the initial value for the semaphore.
*                            If used to share resources, you should initialize to the number of resources available.
*                            If used to signal the occurrence of event(s) then you should initialize to 0.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                    if the call was successful
*                                OS_ERR_CREATE_ISR              if you called this function from an ISR
*                                OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the semaphore after you
*                                                                 called OSSafetyCriticalStart().
*                                OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                OS_ERR_OBJ_CREATED             if the semaphore has already been created
*                                OS_ERR_OBJ_PTR_NULL            if 'p_sem'  is a NULL pointer
*                              - OS_ERR_OBJ_TYPE                if 'p_sem' has already been initialized to a different
*                                                               object type
*                            -------------说明-------------
*                                OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                              - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                              + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                              应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : none
************************************************************************************************************************
*/

void  OSSemCreate (OS_SEM      *p_sem,
                   CPU_CHAR    *p_name,
                   OS_SEM_CTR   cnt,
                   OS_ERR      *p_err)
{
    rt_err_t rt_err;

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
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_CREATE_ISR;
        return;
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u       
    if(p_sem == RT_NULL)/*检查内核对象指针是否为空*/
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }   
    if(p_name == RT_NULL)/*检查信号量名指针是否为空*/
    {
        *p_err = OS_ERR_NAME;
        return;
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u 
    /*判断内核对象是否已经是信号量，即是否已经创建过*/
    if(rt_object_get_type(&p_sem->Sem.parent.parent) == RT_Object_Class_Semaphore)
    {
        *p_err = OS_ERR_OBJ_CREATED;
        return;       
    }    
#endif

    rt_err = rt_sem_init(&p_sem->Sem,(const char*)p_name,cnt,RT_IPC_FLAG_PRIO);   
    *p_err = rt_err_to_ucosiii(rt_err); 
}

/*
************************************************************************************************************************
*                                                  DELETE A SEMAPHORE
*
* Description: This function deletes a semaphore.
*
* Arguments  : p_sem         is a pointer to the semaphore to delete
*
*              opt           determines delete options as follows:
*
*                                OS_OPT_DEL_NO_PEND          Delete semaphore ONLY if no task pending
*                                OS_OPT_DEL_ALWAYS           Deletes the semaphore even if tasks are waiting.
*                                                            In this case, all the tasks pending will be readied.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                 The call was successful and the semaphore was deleted
*                                OS_ERR_DEL_ISR              If you attempted to delete the semaphore from an ISR
*                                OS_ERR_OBJ_PTR_NULL         If 'p_sem' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE             If 'p_sem' is not pointing at a semaphore
*                                OS_ERR_OPT_INVALID          An invalid option was specified
*                                OS_ERR_TASK_WAITING         One or more tasks were waiting on the semaphore
*
* Returns    : == 0          if no tasks were waiting on the semaphore, or upon error.
*              >  0          if one or more tasks waiting on the semaphore are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the semaphore
*                 MUST check the return code of OSSemPend().
*              2) OSSemAccept() callers will not know that the intended semaphore has been deleted.
*              3) Because ALL tasks pending on the semaphore will be readied, you MUST be careful in applications where
*                 the semaphore is used for mutual exclusion because the resource(s) will no longer be guarded by the
*                 semaphore.
************************************************************************************************************************
*/

#if OS_CFG_SEM_DEL_EN > 0u
OS_OBJ_QTY  OSSemDel (OS_SEM  *p_sem,
                      OS_OPT   opt, 
                      OS_ERR  *p_err)
{
    rt_err_t rt_err;
    rt_uint32_t pend_sem_len;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_OBJ_QTY)0);
    }
#endif
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u    
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_DEL_ISR;
        return 0;
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u   
    if(p_sem == RT_NULL)/*检查指针是否为空*/
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    }
    switch (opt) { 
        case OS_OPT_DEL_NO_PEND:
        case OS_OPT_DEL_ALWAYS:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return ((OS_OBJ_QTY)0);
    }      
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u   
    /*判断内核对象是否为信号量*/
    if(rt_object_get_type(&p_sem->Sem.parent.parent) != RT_Object_Class_Semaphore)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }
#endif
    
    switch (opt)
    {
        case OS_OPT_DEL_NO_PEND:
            CPU_CRITICAL_ENTER();
            if(rt_list_isempty(&(p_sem->Sem.parent.suspend_thread)))/*若没有线程等待信号量*/
            {
                CPU_CRITICAL_EXIT();
                rt_err = rt_sem_detach(&p_sem->Sem);
                *p_err = rt_err_to_ucosiii(rt_err);                 
            }
            else
            {
                CPU_CRITICAL_EXIT();
                *p_err = OS_ERR_TASK_WAITING;
            } 
            break;
            
        case OS_OPT_DEL_ALWAYS:
            rt_err = rt_sem_detach(&p_sem->Sem);
            *p_err = rt_err_to_ucosiii(rt_err);
            break;
    }
    
    CPU_CRITICAL_ENTER();
    pend_sem_len = rt_list_len(&(p_sem->Sem.parent.suspend_thread));
    CPU_CRITICAL_EXIT();
    
    return pend_sem_len;
}
#endif

/*
************************************************************************************************************************
*                                                  PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : p_sem         is a pointer to the semaphore
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for the
*                            resource up to the amount of time (in 'ticks') specified by this argument.  If you specify
*                            0, however, your task will wait forever at the specified semaphore or, until the resource
*                            becomes available (or the event occurs).
*
*              opt           determines whether the user wants to block if the semaphore is not available or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the semaphore was posted
*                            or pend aborted or the semaphore deleted.  If you pass a NULL pointer (i.e. (CPU_TS*)0)
*                            then you will not get the timestamp.  In other words, passing a NULL pointer is valid
*                            and indicates that you don't need the timestamp.
*                            -------------说明-------------
*                            该参数在RTT中没有意义,填NULL即可
*               
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE               The call was successful and your task owns the resource
*                                                          or, the event you are waiting for occurred.
*                              - OS_ERR_OBJ_DEL            If 'p_sem' was deleted
*                                OS_ERR_OBJ_PTR_NULL       If 'p_sem' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE           If 'p_sem' is not pointing at a semaphore
*                                OS_ERR_OPT_INVALID        If you specified an invalid value for 'opt'
*                                OS_ERR_PEND_ABORT         If the pend was aborted by another task
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                                          would lead to a suspension.
*                              - OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the semaphore was not
*                                                          available.
*                                OS_ERR_SCHED_LOCKED       If you called this function when the scheduler is locked
*                              - OS_ERR_STATUS_INVALID     Pend status is invalid
*                                OS_ERR_TIMEOUT            The semaphore was not received within the specified
*                                                          timeout.
*                              + OS_ERR_RT_ERROR           普通错误
*                            -------------说明-------------
*                                OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                              - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                              + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                              应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : The current value of the semaphore counter or 0 if not available.
*
* Note(s)    : 1) RTT在非阻塞模式下不区分OS_ERR_PEND_WOULD_BLOCK还是OS_ERR_TIMEOUT，都按照OS_ERR_TIMEOUT处理
************************************************************************************************************************
*/

OS_SEM_CTR  OSSemPend (OS_SEM   *p_sem,
                       OS_TICK   timeout,
                       OS_OPT    opt,
                       CPU_TS   *p_ts,
                       OS_ERR   *p_err)
{  
    rt_err_t rt_err;
    rt_int32_t time;
    
    CPU_SR_ALLOC();
    
    CPU_VAL_UNUSED(p_ts);
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_SEM_CTR)0);
    }
#endif
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u      
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_PEND_ISR;
        return 0;
    }
#endif  
    
#if OS_CFG_ARG_CHK_EN > 0u    
    if(p_sem == RT_NULL)/*检查信号量指针是否为空*/
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    } 
    switch (opt) {
        case OS_OPT_PEND_BLOCKING:
        case OS_OPT_PEND_NON_BLOCKING:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return ((OS_SEM_CTR)0);
    }    
#endif
       
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为信号量*/
    if(rt_object_get_type(&p_sem->Sem.parent.parent) != RT_Object_Class_Semaphore)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }
#endif
    
    /*在RTT中timeout为0表示不阻塞,为RT_WAITING_FOREVER表示永久阻塞,
    这与uCOS-III有所不同,因此需要转换*/
    if((opt & OS_OPT_PEND_NON_BLOCKING) == (OS_OPT)0)
    {   
        /*检查调度器是否被锁*/
        if(OSSchedLockNestingCtr > (OS_NESTING_CTR)0)
        {
            *p_err = OS_ERR_SCHED_LOCKED;
            return 0;         
        }        
        
        if(timeout == 0)/*在uCOS-III中timeout=0表示永久阻塞*/
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
        time = 0;/*在RTT中timeout为0表示非阻塞*/
    }
    
    CPU_CRITICAL_ENTER();
    OSTCBCurPtr->PendStatus = OS_STATUS_PEND_OK;            /* Clear pend status                                      */
    CPU_CRITICAL_EXIT(); 
    
    rt_err = rt_sem_take(&p_sem->Sem,time);
    *p_err = rt_err_to_ucosiii(rt_err); 
    
    CPU_CRITICAL_ENTER();
    if(OSTCBCurPtr->PendStatus == OS_STATUS_PEND_ABORT)     /* Indicate that we aborted                               */
    {
        CPU_CRITICAL_EXIT(); 
        *p_err = OS_ERR_PEND_ABORT;
        return 0;
    }    
    CPU_CRITICAL_EXIT();    
    
    return p_sem->Sem.value;/*返回信号量还剩多少value*/
}

/*
************************************************************************************************************************
*                                             ABORT WAITING ON A SEMAPHORE
*
* Description: This function aborts & readies any tasks currently waiting on a semaphore.  This function should be used
*              to fault-abort the wait on the semaphore, rather than to normally signal the semaphore via OSSemPost().
*
* Arguments  : p_sem     is a pointer to the semaphore
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the semaphore
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the semaphore
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the semaphore was readied and
*                                                         informed of the aborted wait; check return value for the
*                                                         number of tasks whose wait on the semaphore was aborted.
*                            OS_ERR_OBJ_PTR_NULL          If 'p_sem' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If 'p_sem' is not pointing at a semaphore
*                            OS_ERR_OPT_INVALID           If you specified an invalid option
*                            OS_ERR_PEND_ABORT_ISR        If you called this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the semaphore, or upon error.
*              >  0          if one or more tasks waiting on the semaphore are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_SEM_PEND_ABORT_EN > 0u
OS_OBJ_QTY  OSSemPendAbort (OS_SEM  *p_sem,
                            OS_OPT   opt,
                            OS_ERR  *p_err)
{
    rt_uint32_t pend_sem_len;
    
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

#if OS_CFG_ARG_CHK_EN > 0u
    if (p_sem == (OS_SEM *)0) {                             /* Validate 'p_sem'                                       */
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
    if (rt_object_get_type(&p_sem->Sem.parent.parent) != RT_Object_Class_Semaphore) {/*Make sure semaphore was created*/
       *p_err =  OS_ERR_OBJ_TYPE;
        return ((OS_OBJ_QTY)0u);
    }
#endif  
    
    CPU_CRITICAL_ENTER();
    if(rt_list_isempty(&(p_sem->Sem.parent.suspend_thread)))/*若没有线程等待信号量*/ 
    {
        CPU_CRITICAL_EXIT(); 
       *p_err =  OS_ERR_PEND_ABORT_NONE;
        return ((OS_OBJ_QTY)0u);        
    }
    CPU_CRITICAL_EXIT();
    
    if(opt&OS_OPT_PEND_ABORT_ALL)
    {
        rt_ipc_pend_abort_all(&(p_sem->Sem.parent.suspend_thread));
    }
    else
    {
        rt_ipc_pend_abort_1(&(p_sem->Sem.parent.suspend_thread));
    }
    
    if(!(opt&OS_OPT_POST_NO_SCHED))
    {
        rt_schedule();
    }
    
    *p_err = OS_ERR_NONE;
    
    CPU_CRITICAL_ENTER();
    pend_sem_len = rt_list_len(&(p_sem->Sem.parent.suspend_thread));
    CPU_CRITICAL_EXIT();
    
    return pend_sem_len;
}
#endif

/*
************************************************************************************************************************
*                                                 POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : p_sem    is a pointer to the semaphore
*
*              opt      determines the type of POST performed:
*
*                           OS_OPT_POST_1            POST and ready only the highest priority task waiting on semaphore
*                                                    (if tasks are waiting).
*                         - OS_OPT_POST_ALL          POST to ALL tasks that are waiting on the semaphore
*
*                         - OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*                           Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*                       -------------说明-------------
*                        由于RTT没有实现上述所有功能,因此opt选项只能为OS_OPT_POST_1
*
*              p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE          The call was successful and the semaphore was signaled.
*                           OS_ERR_OBJ_PTR_NULL  If 'p_sem' is a NULL pointer.
*                           OS_ERR_OBJ_TYPE      If 'p_sem' is not pointing at a semaphore
*                         - OS_ERR_SEM_OVF       If the post would cause the semaphore count to overflow.
*                         + OS_ERR_OPT_INVALID   原版中少了一个opt无效的错误码
*                       -------------说明-------------
*                           OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                         - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                         + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                         应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : The current value of the semaphore counter or 0 upon error.
************************************************************************************************************************
*/

OS_SEM_CTR  OSSemPost (OS_SEM  *p_sem,
                       OS_OPT   opt,
                       OS_ERR  *p_err)
{
    rt_err_t rt_err;
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_SEM_CTR)0);
    }
#endif    

#if OS_CFG_ARG_CHK_EN > 0u    
    if(p_sem == RT_NULL)/*检查指针是否为空*/
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    } 
    switch (opt) {
        case OS_OPT_POST_1:
        case OS_OPT_POST_ALL:
        case OS_OPT_POST_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
             break;

        default:
            *p_err =  OS_ERR_OPT_INVALID;
             return ((OS_SEM_CTR)0u);
    } 
    if(opt != OS_OPT_POST_1)/*此opt选项只能为OS_OPT_POST_1*/
    {
        *p_err = OS_ERR_OPT_INVALID;
        RT_DEBUG_LOG(OS_CFG_DBG_EN,("OSSemPost: wrapper can't accept this option\n"));
        return 0;
    }    
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为信号量*/
    if(rt_object_get_type(&p_sem->Sem.parent.parent) != RT_Object_Class_Semaphore)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }
#endif
 
    rt_err = rt_sem_release(&p_sem->Sem);
    *p_err = rt_err_to_ucosiii(rt_err); 
    return p_sem->Sem.value;/*返回信号量还剩多少value*/
}

/*
************************************************************************************************************************
*                                                    SET SEMAPHORE
*
* Description: This function sets the semaphore count to the value specified as an argument.  Typically, this value
*              would be 0 but of course, we can set the semaphore to any value.
*
*              You would typically use this function when a semaphore is used as a signaling mechanism
*              and, you want to reset the count value.
*
* Arguments  : p_sem     is a pointer to the semaphore
*
*              cnt       is the new value for the semaphore count.  You would pass 0 to reset the semaphore count.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE           The call was successful and the semaphore value was set.
*                            OS_ERR_OBJ_PTR_NULL   If 'p_sem' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE       If 'p_sem' is not pointing to a semaphore.
*                            OS_ERR_TASK_WAITING   If tasks are waiting on the semaphore.
*
* Returns    : None
************************************************************************************************************************
*/

#if OS_CFG_SEM_SET_EN > 0u 
void  OSSemSet (OS_SEM      *p_sem,
                OS_SEM_CTR   cnt,
                OS_ERR      *p_err)
{
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif    
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u      
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_SET_ISR;
        return;
    }
#endif  
    
#if OS_CFG_ARG_CHK_EN > 0u
    if (p_sem == (OS_SEM *)0) { 
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u   
    /*判断内核对象是否为信号量*/
    if(rt_object_get_type(&p_sem->Sem.parent.parent) != RT_Object_Class_Semaphore)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return;       
    }
#endif
    
    CPU_CRITICAL_ENTER();
    *p_err = OS_ERR_NONE;
    if (p_sem->Sem.value>0)
    {
        p_sem->Sem.value = cnt;
    }
    else
    {
        if(rt_list_isempty(&(p_sem->Sem.parent.suspend_thread)))/*若没有线程等待信号量*/
        {
            p_sem->Sem.value = cnt;
        }
        else
        {
             *p_err = OS_ERR_TASK_WAITING;
        }
    }
    CPU_CRITICAL_EXIT();
}
#endif

#endif
