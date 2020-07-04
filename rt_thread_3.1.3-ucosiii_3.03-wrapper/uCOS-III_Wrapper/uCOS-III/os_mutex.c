/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-02     Meco Man     the first verion
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

#include <os.h>

/*
由于RTT没有相关接口，因此以下函数没有实现
OSMutexPendAbort
*/

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
*                              - OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Mutex after you called
*                                                                 OSSafetyCriticalStart().
*                                OS_ERR_NAME                    if 'p_name'  is a NULL pointer
*                                OS_ERR_OBJ_CREATED             if the mutex has already been created
*                                OS_ERR_OBJ_PTR_NULL            if 'p_mutex' is a NULL pointer
*                            -------------说明-------------
*                                OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                              - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                              + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                              应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : none
************************************************************************************************************************
*/

void  OSMutexCreate (OS_MUTEX  *p_mutex,
                     CPU_CHAR  *p_name,
                     OS_ERR    *p_err)
{
    rt_err_t rt_err;
    
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_CREATE_ISR;
        return; 
    }
    
    /*检查互斥量指针是否为NULL*/
    if(p_mutex == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
    
    /*检查互斥量名称指针是否为NULL*/
    if(p_name == RT_NULL)
    {
        *p_err = OS_ERR_NAME;
        return;
    }
    
    /*判断内核对象是否已经是信号量，即是否已经创建过*/
    if(rt_object_get_type(&p_mutex->parent.parent) == RT_Object_Class_Mutex)
    {
        *p_err = OS_ERR_OBJ_CREATED;
        return;       
    }    

    rt_err = rt_mutex_init(p_mutex,(const char *)p_name,RT_IPC_FLAG_PRIO);/*uCOS-III仅支持以优先级进行排列*/
    *p_err = _err_rtt_to_ucosiii(rt_err);
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
*                              - OS_OPT_DEL_NO_PEND          Delete mutex ONLY if no task pending
*                                OS_OPT_DEL_ALWAYS           Deletes the mutex even if tasks are waiting.
*                                                            In this case, all the tasks pending will be readied.
*                            -------------说明-------------
*                            在RTT中没有实现OS_OPT_DEL_NO_PEND
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                 The call was successful and the mutex was deleted
*                                OS_ERR_DEL_ISR              If you attempted to delete the mutex from an ISR
*                                OS_ERR_OBJ_PTR_NULL         If 'p_mutex' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE             If 'p_mutex' is not pointing to a mutex
*                                OS_ERR_OPT_INVALID          An invalid option was specified
*                              - OS_ERR_STATE_INVALID        Task is in an invalid state
*                              - OS_ERR_TASK_WAITING         One or more tasks were waiting on the mutex
*                            -------------说明-------------
*                                OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                              - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                              + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                              应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : == 0          if no tasks were waiting on the mutex, or upon error.
*              >  0          if one or more tasks waiting on the mutex are now readied and informed.
*              -------------说明-------------
*              返回值不可信,由于RTT没有实现查看该互斥量还有几个任务正在等待的API，因此只能返回0
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

OS_OBJ_QTY  OSMutexDel (OS_MUTEX  *p_mutex,
                        OS_OPT     opt,
                        OS_ERR    *p_err)
{
    rt_err_t rt_err;

    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_DEL_ISR;
        return 0;
    }
    
    /*检查指针是否为空*/
    if(p_mutex == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    }  

    /*判断内核对象是否为互斥量*/
    if(rt_object_get_type(&p_mutex->parent.parent) != RT_Object_Class_Mutex)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }
    
    /*在RTT中没有实现OS_OPT_DEL_NO_PEND*/
    if(opt != OS_OPT_DEL_ALWAYS)
    {
        RT_DEBUG_LOG(RT_DEBUG_UCOSIII,("OSMutexDel: wrapper can't accept this option\r\n"));
        *p_err = OS_ERR_OPT_INVALID;
        return 0;
    }   
    
    rt_err = rt_mutex_detach(p_mutex);
    *p_err = _err_rtt_to_ucosiii(rt_err);
    return 0;/*返回值不可信,RTT没有实现查看该互斥量还有几个任务正在等待的API，因此只能返回0*/
}

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
*                              - OS_ERR_MUTEX_OWNER        If calling task already owns the mutex
*                              - OS_ERR_OBJ_DEL            If 'p_mutex' was deleted
*                                OS_ERR_OBJ_PTR_NULL       If 'p_mutex' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE           If 'p_mutex' is not pointing at a mutex
*                                OS_ERR_OPT_INVALID        If you didn't specify a valid option
*                              - OS_ERR_PEND_ABORT         If the pend was aborted by another task
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                                          would lead to a suspension.
*                              - OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the mutex was not
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
*
* Note(s)    : 1) RTT在非阻塞模式下不区分OS_ERR_PEND_WOULD_BLOCK还是OS_ERR_TIMEOUT，都按照OS_ERR_TIMEOUT处理
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
    
    (void)p_ts;
    
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_PEND_ISR;
        return;
    }
    
    /*检查互斥量指针是否为空*/
    if(p_mutex == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }  
    
    /*检查调度器是否被锁*/
    if(rt_critical_level() > 0)
    {
        *p_err = OS_ERR_SCHED_LOCKED;
        return;         
    }
    
    /*判断内核对象是否为互斥量*/
    if(rt_object_get_type(&p_mutex->parent.parent) != RT_Object_Class_Mutex)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return;       
    }    
    
    /*在RTT中timeout为0表示不阻塞,为RT_WAITING_FOREVER表示永久阻塞,
    这与uCOS-III有所不同,因此需要转换*/
    if(opt == OS_OPT_PEND_BLOCKING)
    {
        if(timeout == 0)/*在uCOS-III中timeout=0表示永久阻塞*/
        {
            time = RT_WAITING_FOREVER;
        }
        else
        {
            time = timeout;
        }
    }
    else if (opt == OS_OPT_PEND_NON_BLOCKING)
    {
        time = 0;/*在RTT中timeout为0表示非阻塞*/
    }
    else
    {
        *p_err = OS_ERR_OPT_INVALID;/*给定的opt参数无效*/
    }    
    
    rt_err = rt_mutex_take(p_mutex,time);
    *p_err = _err_rtt_to_ucosiii(rt_err);
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
*                            OS_ERR_PEND_ABORT_ISR        If you attempted to call this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*                        -------------说明-------------
*                            OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                          - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                          + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                          应用层需要对API返回的错误码判断做出相应的修改
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
*                         - OS_ERR_MUTEX_NESTING    Mutex owner nested its use of the mutex
*                           OS_ERR_MUTEX_NOT_OWNER  If the task posting is not the Mutex owner
*                           OS_ERR_OBJ_PTR_NULL     If 'p_mutex' is a NULL pointer.
*                           OS_ERR_OBJ_TYPE         If 'p_mutex' is not pointing at a mutex
*                           OS_ERR_POST_ISR         If you attempted to post from an ISR
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
    
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_POST_ISR;
        return;
    }      
    
    /*检查指针是否为空*/
    if(p_mutex == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }  
    
    /*判断内核对象是否为信号量*/
    if(rt_object_get_type(&p_mutex->parent.parent) != RT_Object_Class_Mutex)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return;       
    }
    
    /*此opt选项只能为OS_OPT_POST_NONE*/
    if(opt != OS_OPT_POST_NONE)
    {
        *p_err = OS_ERR_OPT_INVALID;
        RT_DEBUG_LOG(RT_DEBUG_UCOSIII,("OSMutexPost: wrapper can't accept this option\r\n"));
        return;
    }    
    
    rt_err = rt_mutex_release(p_mutex);
    
    *p_err = _err_rtt_to_ucosiii(rt_err);
    /*只有已经拥有互斥量控制权的线程才能释放*/
    if(rt_err == -RT_ERROR)/*rt_mutex_release返回-RT_ERROR表示该线程非掌握互斥量的线程*/
    {
        *p_err = OS_ERR_MUTEX_NOT_OWNER;
    }
}
