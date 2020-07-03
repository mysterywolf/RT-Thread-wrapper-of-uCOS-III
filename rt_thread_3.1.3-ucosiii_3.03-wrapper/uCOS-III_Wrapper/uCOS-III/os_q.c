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
关于消息队列发送(post/send)策略选项的说明:
    RTT的消息队列与uCOS的消息队列实现机理完全不同：
     ·RTT的消息队列是采用数据拷贝的方式，直接完成数据的传递
     ·uCOS的消息队列采用传递指针的零拷贝方式
    虽然RTT的邮箱也采用传递指针的方式，但是没有提供urgent函数用于LIFO发送消息,因此采用RTT的消息队列实现

    RTT支持：
        RT_IPC_FLAG_PRIO(uCOS-III没有实现)
        RT_IPC_FLAG_FIFO(相当于OS_OPT_POST_FIFO)
        通过rt_mq_urgent函数将消息插队到队头进行LIFO紧急发布(相当于OS_OPT_POST_LIFO)
    uCOS-III支持：
        OS_OPT_POST_FIFO(相当于RT_IPC_FLAG_PRIO)
        OS_OPT_POST_LIFO(相当于rt_mq_urgent函数) 
        OS_OPT_POST_ALL (RT-Thread未实现)
        OS_OPT_POST_NO_SCHED (RT-Thread未实现)
*/

/*
由于RTT没有相关接口，因此以下函数没有实现
OSQFlush
OSQPendAbort
*/

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
*                            - OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Queue after you called
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
    
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_CREATE_ISR;
        return; 
    } 

    /*检查消息队列指针是否为NULL*/
    if(p_q == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
    
    /*检查消息队列名称指针是否为NULL*/
    if(p_name == RT_NULL)
    {
        *p_err = OS_ERR_NAME;
        return;
    }

    /*检查消息队列最大长度是否为0*/
    if(max_qty == 0)
    {
        *p_err = OS_ERR_Q_SIZE;
        return;
    }
    
    /*判断内核对象是否已经是消息队列，即是否已经创建过*/
    if(rt_object_get_type(&p_q->rt_msg.parent.parent) == RT_Object_Class_MessageQueue)
    {
        *p_err = OS_ERR_OBJ_CREATED;
        return;       
    }
    
    /*RTT消息队列内部消息头大小,由于该结构体在ipc.c文件内部没有暴露出来,因此直接写成sizeof(rt_base_t)指针字节数*/
    msg_header_size = sizeof(rt_base_t);/*sizeof(struct rt_mq_message)*/
    
    msg_size = sizeof(ucos_msg_t);/*消息队列中一条消息的最大长度，单位字节*/
    pool_size = (msg_header_size+msg_size) * max_qty;/*存放消息的缓冲区大小*/
    p_q->p_pool = RT_KERNEL_MALLOC(pool_size);/*分配用于存放消息的缓冲区*/
    if(p_q->p_pool == RT_NULL)
    {
        *p_err = OS_ERR_MEM_FULL;
        return;
    }
    
    rt_err = rt_mq_init(&p_q->rt_msg,
                        (const char *)p_name,
                         p_q->p_pool,
                         msg_size,
                         pool_size,
                         RT_IPC_FLAG_FIFO);
    
    *p_err = _err_rtt_to_ucosiii(rt_err);
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
*                            -------------说明-------------
*                            在RTT中没有实现OS_OPT_DEL_NO_PEND
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                 The call was successful and the queue was deleted
*                            OS_ERR_DEL_ISR              If you tried to delete the queue from an ISR
*                            OS_ERR_OBJ_PTR_NULL         if you pass a NULL pointer for 'p_q'
*                            OS_ERR_OBJ_TYPE             if the message queue was not created
*                            OS_ERR_OPT_INVALID          An invalid option was specified
*                          - OS_ERR_TASK_WAITING         One or more tasks were waiting on the queue
*                        -------------说明-------------
*                            OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                          - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                          + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                          应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : == 0          if no tasks were waiting on the queue, or upon error.
*              >  0          if one or more tasks waiting on the queue are now readied and informed.
*              -------------说明-------------
*              返回值不可信,由于RTT没有实现查看该消息队列还有几个任务正在等待的API，因此只能返回0
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

OS_OBJ_QTY  OSQDel (OS_Q    *p_q,
                    OS_OPT   opt,
                    OS_ERR  *p_err)
{
    rt_err_t rt_err;
    
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_DEL_ISR;
        return 0; 
    } 

    /*检查消息队列指针是否为NULL*/
    if(p_q == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    }

    /*判断内核对象是否为消息队列*/
    if(rt_object_get_type(&p_q->rt_msg.parent.parent) != RT_Object_Class_MessageQueue)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }
    
    /*在RTT中没有实现OS_OPT_DEL_NO_PEND*/
    if(opt != OS_OPT_DEL_ALWAYS)
    {
        *p_err = OS_ERR_OPT_INVALID;
        RT_DEBUG_LOG(RT_DEBUG_UCOSIII,("OSQDel: wrapper can't accept this option\r\n"));
        return 0;
    }
    
    rt_err = rt_mq_detach(&p_q->rt_msg);
    RT_KERNEL_FREE(p_q->p_pool);/*释放消息池空间*/
    
    *p_err = _err_rtt_to_ucosiii(rt_err);
    return 0;/*返回值不可信,由于RTT没有实现查看该消息队列还有几个任务正在等待的API，因此只能返回0*/
}

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
*                              - OS_ERR_PEND_ABORT         the pend was aborted
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
    
    (void)p_ts;
    
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_PEND_ISR;
        return 0; 
    }   
    
    /*检查调度器是否被锁*/
    if(rt_critical_level() > 0)
    {
        *p_err = OS_ERR_SCHED_LOCKED;
        return 0;         
    }  
    
    /*检查消息队列指针是否为NULL*/
    if(p_q == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    }    
    
    /*判断内核对象是否为消息队列*/
    if(rt_object_get_type(&p_q->rt_msg.parent.parent) != RT_Object_Class_MessageQueue)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
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
    
    /*开始消息接收以及处理*/
    rt_err = rt_mq_recv(&p_q->rt_msg,
                        (void*)&ucos_msg,/*uCOS消息段*/
                         sizeof(ucos_msg_t),/*uCOS消息段长度*/
                         time);

    *p_err = _err_rtt_to_ucosiii(rt_err);
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
*                                        OS_OPT_POST_FIFO + OS_OPT_POST_ALL
*                                        OS_OPT_POST_LIFO + OS_OPT_POST_ALL
*                                        OS_OPT_POST_FIFO + OS_OPT_POST_NO_SCHED
*                                        OS_OPT_POST_LIFO + OS_OPT_POST_NO_SCHED
*                                        OS_OPT_POST_FIFO + OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED
*                                        OS_OPT_POST_LIFO + OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED
*                       -------------说明-------------
*                        由于RTT没有实现上述所有功能,因此opt选项只能为OS_OPT_POST_FIFO或OS_OPT_POST_LIFO
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE            The call was successful and the message was sent
*                              - OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs to use to place the message into
*                                OS_ERR_OBJ_PTR_NULL    If 'p_q' is a NULL pointer
*                                OS_ERR_OBJ_TYPE        If the message queue was not initialized
*                                OS_ERR_Q_MAX           If the queue is full
8                              + OS_ERR_OPT_INVALID     You specified an invalid option
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
    
    /*检查消息队列指针是否为NULL*/
    if(p_q == RT_NULL)
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
    
    /*判断内核对象是否为消息队列*/
    if(rt_object_get_type(&p_q->rt_msg.parent.parent) != RT_Object_Class_MessageQueue)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return;       
    }
    
    /*装填uCOS消息段*/
    ucos_msg.data_size = msg_size;
    ucos_msg.data_ptr = p_void;
    
    if(opt == OS_OPT_POST_FIFO)
    {
        rt_err = rt_mq_send(&p_q->rt_msg,(void*)&ucos_msg,sizeof(ucos_msg_t));
    }
    else if(opt == OS_OPT_POST_LIFO)
    {
        rt_err = rt_mq_urgent(&p_q->rt_msg,(void*)&ucos_msg,sizeof(ucos_msg_t));
    }
    else
    {
        *p_err = OS_ERR_OPT_INVALID;
        RT_DEBUG_LOG(RT_DEBUG_UCOSIII,("OSQPost: wrapper can't accept this option\r\n"));
        return;
    }
    *p_err = _err_rtt_to_ucosiii(rt_err); 
}
