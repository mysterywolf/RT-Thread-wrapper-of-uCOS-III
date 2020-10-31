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
*                                               MESSAGE QUEUE MANAGEMENT
*
* File    : OS_Q.C
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
* Note(s)    : 1)关于消息队列发送(post/send)策略选项的说明:
*                   RTT的消息队列与uCOS的消息队列实现机理完全不同：
*                    ·RTT的消息队列是采用数据拷贝的方式，直接完成数据的传递
*                    ·uCOS的消息队列采用传递指针的零拷贝方式
*                   虽然RTT的邮箱也采用传递指针的方式，但是没有提供urgent函数用于LIFO发送消息,因此采用RTT的消息队列实现
*
*                   RTT支持：
*                       RT_IPC_FLAG_PRIO(uCOS-III没有实现)
*                       RT_IPC_FLAG_FIFO(相当于OS_OPT_POST_FIFO)
*                       通过rt_mq_urgent函数将消息插队到队头进行LIFO紧急发布(相当于OS_OPT_POST_LIFO)
*                   uCOS-III支持：
*                       OS_OPT_POST_FIFO(相当于RT_IPC_FLAG_PRIO)
*                       OS_OPT_POST_LIFO(相当于rt_mq_urgent函数) 
*                       OS_OPT_POST_ALL (RT-Thread未实现)
*                       OS_OPT_POST_NO_SCHED (RT-Thread未实现)
************************************************************************************************************************
*/

#if OS_CFG_Q_EN > 0u

/*由于在ipc.c文件中的struct rt_mq_message没有暴露出来,因此需要复制一份,为避免重复改名为struct _rt_mq_message*/
struct _rt_mq_message
{
    struct _rt_mq_message *next;
};

/*
************************************************************************************************************************
*                                               CREATE A MESSAGE QUEUE
*
* Description: This function is called by your application to create a message queue.  Message queues MUST be created
*              before they can be used.
*
* Arguments  : p_q         is a pointer to the message queue
*
*              p_name      is a pointer to an ASCII string that will be used to name the message queue
*
*              max_qty     indicates the maximum size of the message queue (must be non-zero).  Note that it's also not
*                          possible to have a size higher than the maximum number of OS_MSGs available.
*
*              p_err       is a pointer to a variable that will contain an error code returned by this function.
*
*                              OS_ERR_NONE                    the call was successful
*                              OS_ERR_CREATE_ISR              can't create from an ISR
*                              OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Queue after you called
*                                                               OSSafetyCriticalStart().
*                              OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                              OS_ERR_OBJ_CREATED             if the message queue has already been created
*                              OS_ERR_OBJ_PTR_NULL            if you passed a NULL pointer for 'p_q'
*                              OS_ERR_Q_SIZE                  if the size you specified is 0
*                            + OS_ERR_MEM_FULL                本函数内部采用了内存堆分配,该错误表示无法分配到内存
*                          -------------说明-------------
*                              OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                            - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                            + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                            应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : none
************************************************************************************************************************
*/

void  OSQCreate (OS_Q        *p_q,
                 CPU_CHAR    *p_name,
                 OS_MSG_QTY   max_qty,
                 OS_ERR      *p_err)

{
    rt_err_t    rt_err;
    rt_size_t 	msg_size;
    rt_size_t 	pool_size;
    rt_size_t   msg_header_size;
    void       *p_pool;
    
    CPU_SR_ALLOC();
    
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
    if(p_q == RT_NULL)                                      /* 检查消息队列指针是否为NULL                             */
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
    if(p_name == RT_NULL)                                   /* 检查消息队列名称指针是否为NULL                         */
    {
        *p_err = OS_ERR_NAME;
        return;
    }
    if(max_qty == 0)                                        /* 检查消息队列最大长度是否为0                            */
    {
        *p_err = OS_ERR_Q_SIZE;
        return;
    }
#endif
    
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u     
    /*判断内核对象是否已经是消息队列，即是否已经创建过*/
    if(rt_object_get_type(&p_q->Msg.parent.parent) == RT_Object_Class_MessageQueue)
    {
        *p_err = OS_ERR_OBJ_CREATED;
        return;       
    }
#endif
    
    msg_header_size = sizeof(struct _rt_mq_message);        /* sizeof(struct rt_mq_message)                           */
    msg_size = sizeof(ucos_msg_t);                          /* 消息队列中一条消息的最大长度，单位字节                 */
    pool_size = (msg_header_size + msg_size) * max_qty;     /* 存放消息的缓冲区大小                                   */
    p_pool = RT_KERNEL_MALLOC(pool_size);                   /* 分配用于存放消息的缓冲区                               */
    if(p_pool == RT_NULL)
    {
        *p_err = OS_ERR_MEM_FULL;
        return;
    }
    CPU_CRITICAL_ENTER();
    p_q->p_pool = p_pool;
    CPU_CRITICAL_EXIT();
    
    rt_err = rt_mq_init(&p_q->Msg,
                        (const char *)p_name,
                         p_q->p_pool,
                         msg_size,
                         pool_size,
                         RT_IPC_FLAG_FIFO);
    
    *p_err = rt_err_to_ucosiii(rt_err);
    if(rt_err != RT_EOK)
    {
        return;
    }
    
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY    
    CPU_CRITICAL_ENTER();
    p_q->Type    = OS_OBJ_TYPE_Q;                           /* Mark the data structure as a message queue             */
#if (OS_CFG_DBG_EN > 0u)
    p_q->NamePtr = p_name;
    OS_QDbgListAdd(p_q);
#endif

    OSQQty++;                                               /* One more queue created                                 */      
    CPU_CRITICAL_EXIT();    
#endif
}

/*
************************************************************************************************************************
*                                               DELETE A MESSAGE QUEUE
*
* Description: This function deletes a message queue and readies all tasks pending on the queue.
*
* Arguments  : p_q       is a pointer to the message queue you want to delete
*
*              opt       determines delete options as follows:
*
*                            OS_OPT_DEL_NO_PEND          Delete the queue ONLY if no task pending
*                            OS_OPT_DEL_ALWAYS           Deletes the queue even if tasks are waiting.
*                                                        In this case, all the tasks pending will be readied.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                 The call was successful and the queue was deleted
*                            OS_ERR_DEL_ISR              If you tried to delete the queue from an ISR
*                            OS_ERR_ILLEGAL_DEL_RUN_TIME If you are trying to delete the event flag group after you
*                                                           called OSStart()
*                            OS_ERR_OBJ_PTR_NULL         if you pass a NULL pointer for 'p_q'
*                            OS_ERR_OBJ_TYPE             if the message queue was not created
*                            OS_ERR_OPT_INVALID          An invalid option was specified
*                            OS_ERR_OS_NOT_RUNNING       If uC/OS-III is not running yet
*                            OS_ERR_TASK_WAITING         One or more tasks were waiting on the queue
*
* Returns    : == 0          if no tasks were waiting on the queue, or upon error.
*              >  0          if one or more tasks waiting on the queue are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the queue MUST
*                 check the return code of OSQPend().
*
*              2) OSQAccept() callers will not know that the intended queue has been deleted.
*
*              3) Because ALL tasks pending on the queue will be readied, you MUST be careful in applications where the
*                 queue is used for mutual exclusion because the resource(s) will no longer be guarded by the queue.
************************************************************************************************************************
*/

#if OS_CFG_Q_DEL_EN > 0u
OS_OBJ_QTY  OSQDel (OS_Q    *p_q,
                    OS_OPT   opt,
                    OS_ERR  *p_err)
{
    rt_err_t rt_err;
    rt_uint32_t pend_q_len;
    
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
    if(p_q == RT_NULL)                                      /* 检查消息队列指针是否为NULL                             */
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
             return ((OS_OBJ_QTY)0u);
    }   
#endif
    
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为消息队列*/
    if(rt_object_get_type(&p_q->Msg.parent.parent) != RT_Object_Class_MessageQueue)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }
#endif
    
    CPU_CRITICAL_ENTER();
    pend_q_len = rt_list_len(&(p_q->Msg.parent.suspend_thread));
    CPU_CRITICAL_EXIT(); 
    
    switch (opt)
    {
        case OS_OPT_DEL_NO_PEND:
            CPU_CRITICAL_ENTER();
            if(rt_list_isempty(&(p_q->Msg.parent.suspend_thread))) /* 若没有线程等待信号量                            */
            {
                CPU_CRITICAL_EXIT();
                rt_err = rt_mq_detach(&p_q->Msg);
                *p_err = rt_err_to_ucosiii(rt_err);
                RT_KERNEL_FREE(p_q->p_pool);
            }
            else
            {
                CPU_CRITICAL_EXIT();
                *p_err = OS_ERR_TASK_WAITING;
            }
            break;
            
        case OS_OPT_DEL_ALWAYS:
            rt_err = rt_mq_detach(&p_q->Msg);
            *p_err = rt_err_to_ucosiii(rt_err);
            RT_KERNEL_FREE(p_q->p_pool);
            break;
    }
    
    if(*p_err == OS_ERR_NONE)
    {
        CPU_CRITICAL_ENTER();
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
#if OS_CFG_DBG_EN > 0u
        OS_QDbgListRemove(p_q);
#endif
        OSQQty--;
#endif
        OS_QClr(p_q);
        CPU_CRITICAL_EXIT();
    }
    
    return pend_q_len;
}
#endif

/*
************************************************************************************************************************
*                                                     FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments   : p_q        is a pointer to the message queue to flush
*
*               p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                              OS_ERR_NONE           upon success
*                              OS_ERR_FLUSH_ISR      if you called this function from an ISR
*                              OS_ERR_OBJ_PTR_NULL   If you passed a NULL pointer for 'p_q'
*                              OS_ERR_OBJ_TYPE       If you didn't create the message queue
*                              OS_ERR_OS_NOT_RUNNING If uC/OS-III is not running yet
*
* Returns     : The number of entries freed from the queue
*
* Note(s)     : 1) You should use this function with great care because, when to flush the queue, you LOOSE the
*                  references to what the queue entries are pointing to and thus, you could cause 'memory leaks'.  In
*                  other words, the data you are pointing to that's being referenced by the queue entries should, most
*                  likely, need to be de-allocated (i.e. freed).
************************************************************************************************************************
*/

#if OS_CFG_Q_FLUSH_EN > 0u
OS_MSG_QTY  OSQFlush (OS_Q    *p_q,
                      OS_ERR  *p_err)
{
    struct _rt_mq_message *msg;
    OS_MSG_QTY entries = 0;
    
    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_MSG_QTY)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* Can't flush a message queue from an ISR                */
       *p_err = OS_ERR_FLUSH_ISR;
        return ((OS_MSG_QTY)0);
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Is the kernel running?                                 */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (0u);
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u
    if (p_q == (OS_Q *)0) {                                 /* Validate arguments                                     */
       *p_err = OS_ERR_OBJ_PTR_NULL;
        return ((OS_MSG_QTY)0);
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    /*判断内核对象是否为消息队列*/
    if(rt_object_get_type(&p_q->Msg.parent.parent) != RT_Object_Class_MessageQueue) {
       *p_err = OS_ERR_OBJ_TYPE;
        return ((OS_MSG_QTY)0);
    }
#endif
    
    CPU_CRITICAL_ENTER();
    while(p_q->Msg.entry>0)
    {
        msg = (struct _rt_mq_message *)(p_q->Msg.msg_queue_head);/* get message from queue                            */
        p_q->Msg.msg_queue_head = msg->next;                /* move message queue head                                */
        if (p_q->Msg.msg_queue_tail == msg)                 /* reach queue tail, set to NULL                          */
            p_q->Msg.msg_queue_tail = RT_NULL;
        p_q->Msg.entry --;                                  /* decrease message entry                                 */   
        msg->next = (struct _rt_mq_message *)p_q->Msg.msg_queue_free; /* put message to free list                     */
        p_q->Msg.msg_queue_free = msg; 
        entries ++;
    }
    
#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
    p_q->DbgNamePtr =(CPU_CHAR *)((void *)" ");             /* Clear                                                  */
#endif
    CPU_CRITICAL_EXIT();
    
    return entries;
}
#endif

/*
************************************************************************************************************************
*                                            PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : p_q           is a pointer to the message queue
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for a
*                            message to arrive at the queue up to the amount of time specified by this argument.  If you
*                            specify 0, however, your task will wait forever at the specified queue or, until a message
*                            arrives.
*
*              opt           determines whether the user wants to block if the queue is empty or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_msg_size    is a pointer to a variable that will receive the size of the message
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the message was
*                            received, pend aborted or the message queue deleted,  If you pass a NULL pointer (i.e.
*                            (CPU_TS *)0) then you will not get the timestamp.  In other words, passing a NULL pointer
*                            is valid and indicates that you don't need the timestamp.
*                            -------------说明-------------
*                            该参数在RTT中没有意义,填NULL即可
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE               The call was successful and your task received a message.
*                                OS_ERR_OBJ_PTR_NULL       if you pass a NULL pointer for 'p_q'
*                                OS_ERR_OBJ_TYPE           if the message queue was not created
*                                OS_ERR_OS_NOT_RUNNING     If uC/OS-III is not running yet
*                                OS_ERR_PEND_ABORT         the pend was aborted
*                                OS_ERR_PEND_ISR           if you called this function from an ISR
*                              - OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the queue was not empty
*                                OS_ERR_SCHED_LOCKED       the scheduler is locked
*                                OS_ERR_TIMEOUT            A message was not received within the specified timeout
*                                                          would lead to a suspension.
*                            -------------说明-------------
*                                OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                              - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                              + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                              应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : != (void *)0  is a pointer to the message received
*              == (void *)0  if you received a NULL pointer message or,
*                            if no message was received or,
*                            if 'p_q' is a NULL pointer or,
*                            if you didn't pass a pointer to a queue.
*
* Note(s)    : 1) RTT在非阻塞模式下不区分OS_ERR_PEND_WOULD_BLOCK还是OS_ERR_TIMEOUT，都按照OS_ERR_TIMEOUT处理
************************************************************************************************************************
*/

void  *OSQPend (OS_Q         *p_q,
                OS_TICK       timeout,
                OS_OPT        opt,
                OS_MSG_SIZE  *p_msg_size,
                CPU_TS       *p_ts,
                OS_ERR       *p_err)
{
    rt_err_t    rt_err;
    rt_int32_t  time;
    ucos_msg_t  ucos_msg;
    OS_TCB     *p_tcb;
#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
    rt_thread_t thread;
#endif   
    
    CPU_SR_ALLOC();
    
    CPU_VAL_UNUSED(p_ts);

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((void *)0);
    }
#endif
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)                 /* 检查是否在中断中运行                                   */
    {
        *p_err = OS_ERR_PEND_ISR;
        return RT_NULL; 
    }   
#endif
    
#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Is the kernel running?                                 */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return ((void *)0);
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u
    if(p_q == RT_NULL)                                      /* 检查消息队列指针是否为NULL                             */
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return RT_NULL;
    }    
     if (p_msg_size == (OS_MSG_SIZE *)0) {
       *p_err = OS_ERR_PTR_INVALID;
        return ((void *)0);
    }
    switch (opt) {
        case OS_OPT_PEND_BLOCKING:
        case OS_OPT_PEND_NON_BLOCKING:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return ((void *)0);
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为消息队列*/
    if(rt_object_get_type(&p_q->Msg.parent.parent) != RT_Object_Class_MessageQueue)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return RT_NULL;       
    } 
#endif
    
    /*
        在RTT中timeout为0表示不阻塞,为RT_WAITING_FOREVER表示永久阻塞,
        这与uCOS-III有所不同,因此需要转换
    */
    if((opt & OS_OPT_PEND_NON_BLOCKING) == (OS_OPT)0)
    {
        /*检查调度器是否被锁*/
        if(OSSchedLockNestingCtr > (OS_NESTING_CTR)0)
        {
            *p_err = OS_ERR_SCHED_LOCKED;
            return RT_NULL;         
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
        time = 0;                                           /* 在RTT中timeout为0表示非阻塞                            */
    }
    
    CPU_CRITICAL_ENTER();
    p_tcb = OSTCBCurPtr;
    p_tcb->PendStatus = OS_STATUS_PEND_OK;                  /* Clear pend status                                      */
    p_tcb->TaskState |= OS_TASK_STATE_PEND;
    if(p_tcb->PendOn != OS_TASK_PEND_ON_TASK_Q)
    {
        p_tcb->PendOn = OS_TASK_PEND_ON_Q;
    }  
#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
    p_tcb->DbgNamePtr = p_q->NamePtr;
    p_q->DbgNamePtr = p_tcb->Task.name;
#endif
    CPU_CRITICAL_EXIT();     
    
    /*开始消息接收以及处理*/
    rt_err = rt_mq_recv(&p_q->Msg,
                        (void*)&ucos_msg,                   /* uCOS消息段                                             */
                         sizeof(ucos_msg_t),                /* uCOS消息段长度                                         */
                         time);

    *p_err = rt_err_to_ucosiii(rt_err);
                         
    CPU_CRITICAL_ENTER();                
    p_tcb->TaskState &= ~OS_TASK_STATE_PEND;                /* 更新任务状态                                           */
    p_tcb->PendOn = OS_TASK_PEND_ON_NOTHING;                /* 清除当前任务等待状态                                   */
#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
    p_tcb->DbgNamePtr = (CPU_CHAR *)((void *)" "); 
    if(!rt_list_isempty(&(p_q->Msg.parent.suspend_thread)))
    {
        /*若等待表不为空，则将当前等待消息队列的线程赋值给.DbgNamePtr*/
        thread = rt_list_entry((&(p_q->Msg.parent.suspend_thread))->next, struct rt_thread, tlist);
        p_q->DbgNamePtr = thread->name;
    }
    else
    {
        p_q->DbgNamePtr =(CPU_CHAR *)((void *)" ");         /* 若为空,则清空当前.DbgNamePtr                           */
    }
#endif 
    if(p_tcb->PendStatus == OS_STATUS_PEND_ABORT)           /* Indicate that we aborted                               */
    {
        CPU_CRITICAL_EXIT();
        *p_err = OS_ERR_PEND_ABORT;
        *p_msg_size = 0;
        return RT_NULL;
    }
    CPU_CRITICAL_EXIT();                             
                         
    if(*p_err == OS_ERR_NONE)
    {
        *p_msg_size = ucos_msg.data_size;
        return ucos_msg.data_ptr;
    }
    else
    {
        *p_msg_size = 0;
        return RT_NULL;
    }
}

/*
************************************************************************************************************************
*                                             ABORT WAITING ON A MESSAGE QUEUE
*
* Description: This function aborts & readies any tasks currently waiting on a queue.  This function should be used to
*              fault-abort the wait on the queue, rather than to normally signal the queue via OSQPost().
*
* Arguments  : p_q       is a pointer to the message queue
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the queue
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the queue
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the queue was readied and
*                                                         informed of the aborted wait; check return value for the
*                                                         number of tasks whose wait on the queue was aborted.
*                            OS_ERR_OPT_INVALID           if you specified an invalid option
*                            OS_ERR_OBJ_PTR_NULL          if you pass a NULL pointer for 'p_q'
*                            OS_ERR_OBJ_TYPE              if the message queue was not created
*                            OS_ERR_OS_NOT_RUNNING        If uC/OS-III is not running yet
*                            OS_ERR_PEND_ABORT_ISR        If this function was called from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the queue, or upon error.
*              >  0          if one or more tasks waiting on the queue are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_Q_PEND_ABORT_EN > 0u
OS_OBJ_QTY  OSQPendAbort (OS_Q    *p_q,
                          OS_OPT   opt,
                          OS_ERR  *p_err)
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
    if (p_q == (OS_Q *)0) {                                 /* Validate 'p_sem'                                       */
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
    /*判断内核对象是否为消息队列*/
    if (rt_object_get_type(&p_q->Msg.parent.parent) != RT_Object_Class_MessageQueue) {
       *p_err =  OS_ERR_OBJ_TYPE;
        return ((OS_OBJ_QTY)0u);
    }
#endif  
    
    CPU_CRITICAL_ENTER();
    if(rt_list_isempty(&(p_q->Msg.parent.suspend_thread)))  /* 若没有线程等待信号量                                   */ 
    {
        CPU_CRITICAL_EXIT(); 
       *p_err =  OS_ERR_PEND_ABORT_NONE;
        return ((OS_OBJ_QTY)0u);        
    }
    CPU_CRITICAL_EXIT();
    
    if(opt&OS_OPT_PEND_ABORT_ALL)
    {
        abort_tasks = rt_ipc_pend_abort_all(&(p_q->Msg.parent.suspend_thread));
    }
    else
    {
        rt_ipc_pend_abort_1(&(p_q->Msg.parent.suspend_thread));
        abort_tasks = 1;
    }
 
#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
    CPU_CRITICAL_ENTER();
    if(!rt_list_isempty(&(p_q->Msg.parent.suspend_thread)))
    {
        /*若等待表不为空，则将当前等待消息队列的线程赋值给.DbgNamePtr*/
        thread = rt_list_entry((&(p_q->Msg.parent.suspend_thread))->next, struct rt_thread, tlist);
        p_q->DbgNamePtr = thread->name;
    }
    else
    {
        p_q->DbgNamePtr =(CPU_CHAR *)((void *)" ");         /* 若为空,则清空当前.DbgNamePtr                           */
    }
    CPU_CRITICAL_EXIT(); 
#endif
    if(!(opt&OS_OPT_POST_NO_SCHED))
    {
        rt_schedule();
    }
    
    *p_err = OS_ERR_NONE;
    return abort_tasks;
}
#endif

/*
************************************************************************************************************************
*                                               POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue.  With the 'opt' argument, you can specify whether the message
*              is broadcast to all waiting tasks and/or whether you post the message to the front of the queue (LIFO)
*              or normally (FIFO) at the end of the queue.
*
* Arguments  : p_q           is a pointer to a message queue that must have been created by OSQCreate().
*
*              p_void        is a pointer to the message to send.
*
*              msg_size      specifies the size of the message (in bytes)
*
*              opt           determines the type of POST performed:
*
*                              - OS_OPT_POST_ALL          POST to ALL tasks that are waiting on the queue.  This option
*                                                         can be added to either OS_OPT_POST_FIFO or OS_OPT_POST_LIFO
*                                OS_OPT_POST_FIFO         POST message to end of queue (FIFO) and wake up a single
*                                                         waiting task.
*                                OS_OPT_POST_LIFO         POST message to the front of the queue (LIFO) and wake up
*                                                         a single waiting task.
*                              - OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*                            Note(s): 1) OS_OPT_POST_NO_SCHED can be added (or OR'd) with one of the other options.
*                                     2) OS_OPT_POST_ALL      can be added (or OR'd) with one of the other options.
*                                     3) Possible combination of options are:
*
*                                        OS_OPT_POST_FIFO
*                                        OS_OPT_POST_LIFO
*                                      - OS_OPT_POST_FIFO + OS_OPT_POST_ALL
*                                      - OS_OPT_POST_LIFO + OS_OPT_POST_ALL
*                                      - OS_OPT_POST_FIFO + OS_OPT_POST_NO_SCHED
*                                      - OS_OPT_POST_LIFO + OS_OPT_POST_NO_SCHED
*                                      - OS_OPT_POST_FIFO + OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED
*                                      - OS_OPT_POST_LIFO + OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED
*                       -------------说明-------------
*                        由于RTT没有实现上述所有功能,因此opt选项只能为OS_OPT_POST_FIFO或OS_OPT_POST_LIFO
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE            The call was successful and the message was sent
*                              - OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs to use to place the message into
*                                OS_ERR_OBJ_PTR_NULL    If 'p_q' is a NULL pointer
*                                OS_ERR_OBJ_TYPE        If the message queue was not initialized
*                                OS_ERR_OS_NOT_RUNNING  If uC/OS-III is not running yet
*                                OS_ERR_Q_MAX           If the queue is full
*                              + OS_ERR_OPT_INVALID     You specified an invalid option
*                            -------------说明-------------
*                                OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                              - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                              + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                              应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : None
************************************************************************************************************************
*/

void  OSQPost (OS_Q         *p_q,
               void         *p_void,
               OS_MSG_SIZE   msg_size,
               OS_OPT        opt,
               OS_ERR       *p_err)
{
    rt_err_t rt_err;
    ucos_msg_t  ucos_msg;
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
    
#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Is the kernel running?                                 */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u    
    if(p_q == RT_NULL)                                      /* 检查消息队列指针是否为NULL                             */
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
    switch (opt) {
        case OS_OPT_POST_FIFO:
        case OS_OPT_POST_LIFO:
        case OS_OPT_POST_FIFO | OS_OPT_POST_ALL:
        case OS_OPT_POST_LIFO | OS_OPT_POST_ALL:
        case OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_LIFO | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_FIFO | OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_LIFO | OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
             break;

        default:
            *p_err =  OS_ERR_OPT_INVALID;
             return;
    }   
#endif
    
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为消息队列*/
    if(rt_object_get_type(&p_q->Msg.parent.parent) != RT_Object_Class_MessageQueue)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return;       
    }
#endif
    
    /*装填uCOS消息段*/
    ucos_msg.data_size = msg_size;
    ucos_msg.data_ptr = p_void;
    
    if(opt == OS_OPT_POST_FIFO)
    {
        rt_err = rt_mq_send(&p_q->Msg,(void*)&ucos_msg,sizeof(ucos_msg_t));
    }
    else if(opt == OS_OPT_POST_LIFO)
    {
        rt_err = rt_mq_urgent(&p_q->Msg,(void*)&ucos_msg,sizeof(ucos_msg_t));
    }
    else
    {
        *p_err = OS_ERR_OPT_INVALID;
        RT_DEBUG_LOG(OS_CFG_DBG_EN,("OSQPost: wrapper can't accept this option\n"));
        return;
    }
    *p_err = rt_err_to_ucosiii(rt_err); 
    
    CPU_CRITICAL_ENTER();
#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
    if(!rt_list_isempty(&(p_q->Msg.parent.suspend_thread)))
    {
        /*若等待表不为空，则将当前等待消息队列的线程赋值给.DbgNamePtr*/
        thread = rt_list_entry((&(p_q->Msg.parent.suspend_thread))->next, struct rt_thread, tlist);
        p_q->DbgNamePtr = thread->name;
    }
    else
    {
        p_q->DbgNamePtr = (CPU_CHAR *)((void *)" ");        /* 若为空,则清空当前.DbgNamePtr                           */
    }
#endif
    CPU_CRITICAL_EXIT();
}

/*
************************************************************************************************************************
*                                        CLEAR THE CONTENTS OF A MESSAGE QUEUE
*
* Description: This function is called by OSQDel() to clear the contents of a message queue
*

* Argument(s): p_q      is a pointer to the queue to clear
*              ---
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/
void  OS_QClr (OS_Q  *p_q)
{
#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
    p_q->Type    =  OS_OBJ_TYPE_NONE;                       /* Mark the data structure as a NONE                      */
#endif
#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
    p_q->NamePtr = (CPU_CHAR *)((void *)"?Q");
#endif
}

/*
************************************************************************************************************************
*                                      ADD/REMOVE MESSAGE QUEUE TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove a message queue to/from a message queue debug
*              list.
*
* Arguments  : p_q     is a pointer to the message queue to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u && !defined PKG_USING_UCOSIII_WRAPPER_TINY
void  OS_QDbgListAdd (OS_Q  *p_q)
{
    p_q->DbgNamePtr               = (CPU_CHAR *)((void *)" ");
    p_q->DbgPrevPtr               = (OS_Q     *)0;
    if (OSQDbgListPtr == (OS_Q *)0) {
        p_q->DbgNextPtr           = (OS_Q     *)0;
    } else {
        p_q->DbgNextPtr           =  OSQDbgListPtr;
        OSQDbgListPtr->DbgPrevPtr =  p_q;
    }
    OSQDbgListPtr                 =  p_q;
}



void  OS_QDbgListRemove (OS_Q  *p_q)
{
    OS_Q  *p_q_next;
    OS_Q  *p_q_prev;

    p_q->DbgNamePtr               = (CPU_CHAR *)((void *)" ");
    p_q_prev = p_q->DbgPrevPtr;
    p_q_next = p_q->DbgNextPtr;

    if (p_q_prev == (OS_Q *)0) {
        OSQDbgListPtr = p_q_next;
        if (p_q_next != (OS_Q *)0) {
            p_q_next->DbgPrevPtr = (OS_Q *)0;
        }
        p_q->DbgNextPtr = (OS_Q *)0;

    } else if (p_q_next == (OS_Q *)0) {
        p_q_prev->DbgNextPtr = (OS_Q *)0;
        p_q->DbgPrevPtr      = (OS_Q *)0;

    } else {
        p_q_prev->DbgNextPtr =  p_q_next;
        p_q_next->DbgPrevPtr =  p_q_prev;
        p_q->DbgNextPtr      = (OS_Q *)0;
        p_q->DbgPrevPtr      = (OS_Q *)0;
    }
}
#endif

/*
************************************************************************************************************************
*                                              MESSAGE QUEUE INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the message queue management.
*

* Arguments  : p_err         is a pointer to a variable that will receive an error code.
*
*                                OS_ERR_NONE     the call was successful
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_QInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
#if OS_CFG_DBG_EN > 0u
    OSQDbgListPtr = (OS_Q *)0;
#endif
    OSQQty        = (OS_OBJ_QTY)0;
#endif
   *p_err         = OS_ERR_NONE;
}

#endif
