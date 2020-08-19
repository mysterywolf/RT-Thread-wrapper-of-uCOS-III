/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-01     Meco Man     the first verion
 */

/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   TASK MANAGEMENT
*
* File    : OS_TASK.C
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
* Note(s)    : 1)由于RTT没有相关接口，因此以下函数没有实现
*                   OSTaskChangePrio
*                   OSTaskTimeQuantaSet
************************************************************************************************************************
*/

/*
************************************************************************************************************************
*                                                CHANGE PRIORITY OF A TASK
*
* Description: This function allows you to change the priority of a task dynamically.  Note that the new
*              priority MUST be available.
*
* Arguments  : p_tcb      is the TCB of the tack to change the priority for
*
*              prio_new   is the new priority
*
*              p_err      is a pointer to an error code returned by this function:
*
*                             OS_ERR_NONE                 is the call was successful
*                             OS_ERR_PRIO_INVALID         if the priority you specify is higher that the maximum allowed
*                                                         (i.e. >= (OS_CFG_PRIO_MAX-1))
*                             OS_ERR_STATE_INVALID        if the task is in an invalid state
*                             OS_ERR_TASK_CHANGE_PRIO_ISR if you tried to change the task's priority from an ISR
************************************************************************************************************************
*/

#if OS_CFG_TASK_CHANGE_PRIO_EN > 0u
void  OSTaskChangePrio (OS_TCB   *p_tcb,
                        OS_PRIO   prio_new,
                        OS_ERR   *p_err)
{
}
#endif

/*
************************************************************************************************************************
*                                                    CREATE A TASK
*
* Description: This function is used to have uC/OS-III manage the execution of a task.  Tasks can either be created
*              prior to the start of multitasking or by a running task.  A task cannot be created by an ISR.
*
* Arguments  : p_tcb          is a pointer to the task's TCB
*
*              p_name         is a pointer to an ASCII string to provide a name to the task.
*
*              p_task         is a pointer to the task's code
*
*              p_arg          is a pointer to an optional data area which can be used to pass parameters to
*                             the task when the task first executes.  Where the task is concerned it thinks
*                             it was invoked and passed the argument 'p_arg' as follows:
*
*                                 void Task (void *p_arg)
*                                 {
*                                     for (;;) {
*                                         Task code;
*                                     }
*                                 }
*
*              prio           is the task's priority.  A unique priority MUST be assigned to each task and the
*                             lower the number, the higher the priority.
*
*              p_stk_base     is a pointer to the base address of the stack (i.e. low address).
*
*              stk_limit      is the number of stack elements to set as 'watermark' limit for the stack.  This value
*                             represents the number of CPU_STK entries left before the stack is full.  For example,
*                             specifying 10% of the 'stk_size' value indicates that the stack limit will be reached
*                             when the stack reaches 90% full.
*                            -------------说明-------------
*                             该参数在本兼容层中没有意义，填什么都行
*
*              stk_size       is the size of the stack in number of elements.  If CPU_STK is set to CPU_INT08U,
*                             'stk_size' corresponds to the number of bytes available.  If CPU_STK is set to
*                             CPU_INT16U, 'stk_size' contains the number of 16-bit entries available.  Finally, if
*                             CPU_STK is set to CPU_INT32U, 'stk_size' contains the number of 32-bit entries
*                             available on the stack.
*
*              q_size         is the maximum number of messages that can be sent to the task
*
*              time_quanta    amount of time (in ticks) for time slice when round-robin between tasks.  Specify 0 to use
*                             the default.
*
*              p_ext          is a pointer to a user supplied memory location which is used as a TCB extension.
*                             For example, this user memory can hold the contents of floating-point registers
*                             during a context switch, the time each task takes to execute, the number of times
*                             the task has been switched-in, etc.
*
*              opt            contains additional information (or options) about the behavior of the task.
*                             See OS_OPT_TASK_xxx in OS.H.  Current choices are:
*
*                                 OS_OPT_TASK_NONE            No option selected
*                                 OS_OPT_TASK_STK_CHK         Stack checking to be allowed for the task
*                                 OS_OPT_TASK_STK_CLR         Clear the stack when the task is created
*                                 OS_OPT_TASK_SAVE_FP         If the CPU has floating-point registers, save them
*                                                             during a context switch.
*                                 OS_OPT_TASK_NO_TLS          If the caller doesn't want or need TLS (Thread Local 
*                                                             Storage) support for the task.  If you do not include this
*                                                             option, TLS will be supported by default.
*
*              p_err          is a pointer to an error code that will be set during this call.  The value pointer
*                             to by 'p_err' can be:
*
*                                 OS_ERR_NONE                    if the function was successful.
*                                 OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the task after you called
*                                                                   OSSafetyCriticalStart().
*                                 OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                 OS_ERR_PRIO_INVALID            if the priority you specify is higher that the maximum
*                                                                   allowed (i.e. >= OS_CFG_PRIO_MAX-1) or,
*                                                                if OS_CFG_ISR_POST_DEFERRED_EN is set to 1 and you tried
*                                                                   to use priority 0 which is reserved.
*                                 OS_ERR_STK_OVF                 If the stack was overflowed during stack init
*                                 OS_ERR_STK_INVALID             if you specified a NULL pointer for 'p_stk_base'
*                                 OS_ERR_STK_SIZE_INVALID        if you specified zero for the 'stk_size'
*                                 OS_ERR_STK_LIMIT_INVALID       if you specified a 'stk_limit' greater than or equal
*                                                                   to 'stk_size'
*                                 OS_ERR_TASK_CREATE_ISR         if you tried to create a task from an ISR.
*                                 OS_ERR_TASK_INVALID            if you specified a NULL pointer for 'p_task'
*                                 OS_ERR_TCB_INVALID             if you specified a NULL pointer for 'p_tcb'
*                               + OS_ERR_RT_ERROR
*                             -------------说明-------------
*                                 OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                               - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                               + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                               应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : A pointer to the TCB of the task created.  This pointer must be used as an ID (i.e handle) to the task.
************************************************************************************************************************
*/

void  OSTaskCreate (OS_TCB        *p_tcb,
                    CPU_CHAR      *p_name,
                    OS_TASK_PTR    p_task,
                    void          *p_arg,
                    OS_PRIO        prio,
                    CPU_STK       *p_stk_base,
                    CPU_STK_SIZE   stk_limit,
                    CPU_STK_SIZE   stk_size,
                    OS_MSG_QTY     q_size,
                    OS_TICK        time_quanta,
                    void          *p_ext,
                    OS_OPT         opt,
                    OS_ERR        *p_err)
{ 
    rt_err_t rt_err;
    OS_ERR err;
#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    OS_TLS_ID      id;
#endif
#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    OS_REG_ID      reg_nbr;
#endif
#if OS_CFG_TASK_PROFILE_EN > 0u
    CPU_STK       *p_stk_limit;
#endif
    CPU_STK       *p_sp;
    CPU_STK_SIZE   i;
    
    CPU_SR_ALLOC();
    
    CPU_VAL_UNUSED(stk_limit);

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
        *p_err = OS_ERR_TASK_CREATE_ISR;
        return;
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u                                  /* ---------------- VALIDATE ARGUMENTS ------------------ */   
    if(p_tcb == RT_NULL)                                    /* User must supply a valid OS_TCB                        */
    {
        *p_err = OS_ERR_TCB_INVALID;
        return;
    }     
    if(p_task == RT_NULL)                                   /* User must supply a valid task                          */
    {
        *p_err = OS_ERR_TASK_INVALID;
        return;
    }
    if(p_name == RT_NULL)                                   /*检查任务名是否为NULL*/
    {
        *p_err = OS_ERR_NAME;
        return;
    }
    if(prio >= RT_THREAD_PRIORITY_MAX-1)                    /* 检查任务优先级,不可占用空闲任务优先级*/
    {
        *p_err = OS_ERR_PRIO_INVALID;
        return;        
    }
    if(p_stk_base == RT_NULL)                               /* User must supply a valid stack base address            */
    {
        *p_err = OS_ERR_STK_INVALID;
        return;
    }  
    if(stk_size < OS_CFG_STK_SIZE_MIN)                      /* User must supply a valid minimum stack size            */
    {
        *p_err = OS_ERR_STK_SIZE_INVALID;
        return;
    }
    if (stk_limit >= stk_size) {                            /* User must supply a valid stack limit                   */
       *p_err = OS_ERR_STK_LIMIT_INVALID;
        return;
    }    
    if (prio == (OS_CFG_PRIO_MAX - 1u)) {
       *p_err = OS_ERR_PRIO_INVALID;                        /* Not allowed to use same priority as idle task          */
        return;
    } 
#endif
    
    OS_TaskInitTCB(p_tcb);                                  /* Initialize the TCB to default values                   */
    *p_err = OS_ERR_NONE;
                                                            /* --------------- CLEAR THE TASK'S STACK --------------- */
    if ((opt & OS_OPT_TASK_STK_CHK) != (OS_OPT)0) {         /* See if stack checking has been enabled                 */
        if ((opt & OS_OPT_TASK_STK_CLR) != (OS_OPT)0) {     /* See if stack needs to be cleared                       */
            p_sp = p_stk_base;
            for (i = 0u; i < stk_size; i++) {               /* Stack grows from HIGH to LOW memory                    */
               *p_sp = (CPU_STK)0;                          /* Clear from bottom of stack and up!                     */
                p_sp++;
            }
        }
    }

    p_sp = p_stk_base;
#if (CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO)         /* Check if we overflown the stack during init          */
    if (p_sp < p_stk_base) {
       *p_err = OS_ERR_STK_OVF;
        return;
    }
#else
    if (p_sp > (p_stk_base + stk_size)) {
       *p_err = OS_ERR_STK_OVF;
        return;
    }
#endif
    
    CPU_CRITICAL_ENTER();        
    p_tcb->MsgCreateSuc = RT_FALSE;
    p_tcb->SemCreateSuc = RT_FALSE;
    p_tcb->ExtPtr = p_ext;/*用户附加区指针*/
    p_tcb->SuspendCtr = 0;/*嵌套挂起为0层*/
    
#if OS_CFG_TASK_PROFILE_EN > 0u
    p_tcb->TimeQuanta    = time_quanta;                     /* Save the #ticks for time slice (0 means not sliced)    */
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
    if (time_quanta == (OS_TICK)0) {
        p_tcb->TimeQuantaCtr = OSSchedRoundRobinDfltTimeQuanta;
    } else {
        p_tcb->TimeQuantaCtr = time_quanta;
    }
#endif    
#endif
    
#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    for (reg_nbr = 0u; reg_nbr < OS_CFG_TASK_REG_TBL_SIZE; reg_nbr++) {
        p_tcb->RegTbl[reg_nbr] = (OS_REG)0;
    }
#endif 
    CPU_CRITICAL_EXIT();
    
#if OS_CFG_TASK_Q_EN > 0u   
    if(q_size>0)/*开启任务内建消息队列*/
    {
        OSQCreate(&p_tcb->MsgQ, (CPU_CHAR*)p_name, q_size, &err);
        if(err != OS_ERR_NONE)/*任务内建消息队列创建失败*/
        {
            CPU_CRITICAL_ENTER();
            p_tcb->MsgCreateSuc = RT_FALSE;
            CPU_CRITICAL_EXIT();
            RT_DEBUG_LOG(OS_CFG_DBG_EN,("task qmsg %s create err!\n",p_name));
        }
        else
        {
            CPU_CRITICAL_ENTER(); 
            p_tcb->MsgCreateSuc = RT_TRUE;
            CPU_CRITICAL_EXIT();
        }
    }
#else
    CPU_VAL_UNUSED(q_size);
#endif
    
    /*创建任务内建信号量*/
    OSSemCreate(&p_tcb->Sem,(CPU_CHAR*)p_name,0,&err);/*任务内建信号量value初始化为0*/
    if(err != OS_ERR_NONE)/*任务内建信号量创建失败*/
    {
        CPU_CRITICAL_ENTER(); 
        p_tcb->SemCreateSuc = RT_FALSE;
        CPU_CRITICAL_EXIT();
        RT_DEBUG_LOG(OS_CFG_DBG_EN,("task sem %s create err!\n",p_name));
    }
    else
    {
        CPU_CRITICAL_ENTER(); 
        p_tcb->SemCreateSuc = RT_TRUE;
        CPU_CRITICAL_EXIT();
    }
    
    CPU_CRITICAL_ENTER();
#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)/*线程私有变量暂时没有实现*/
    for (id = 0u; id < OS_CFG_TLS_TBL_SIZE; id++) {
        p_tcb->TLS_Tbl[id] = (OS_TLS)0;
    }
    OS_TLS_TaskCreate(p_tcb);                               /* Call TLS hook                                          */
#endif
    CPU_CRITICAL_EXIT();
    
    /*创建线程*/
    rt_err = rt_thread_init(&p_tcb->Task,
                            (const char*)p_name,
                            p_task,
                            p_arg,
                            p_stk_base,
                            stk_size*sizeof(CPU_STK),/*uCOS-III的任务堆栈时以CPU_STK为单位，而RTT是以字节为单位，因此需要进行转换*/
                            prio,
#if OS_CFG_TASK_PROFILE_EN > 0u
                            p_tcb->TimeQuantaCtr
#else
                            time_quanta
#endif
                            );
    *p_err = rt_err_to_ucosiii(rt_err);
    if(rt_err != RT_EOK)
    {
        return;
    }
     
    /*调用钩子函数*/
    OSTaskCreateHook(p_tcb);
    
    CPU_CRITICAL_ENTER();
    
#if OS_CFG_DBG_EN > 0u
    OS_TaskDbgListAdd(p_tcb);/*将任务加入到Debug链表中*/
#endif    
    OSTaskQty++; /* Increment the #tasks counter */

#if OS_CFG_TASK_PROFILE_EN > 0u  
#if (CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO)
    p_stk_limit = p_stk_base + stk_limit;
#else
    p_stk_limit = p_stk_base + (stk_size - 1u) - stk_limit;
#endif 
#if OS_CFG_DBG_EN > 0u    
    p_tcb->StkPtr = ((struct rt_thread*)p_tcb)->sp;/* (非实时)该数据在本兼容层中不能反映实时SP指针位置,数据在统计任务中更新*/
#endif
    p_tcb->Opt = opt;
    p_tcb->StkSize = stk_size;
    p_tcb->StkBasePtr = p_stk_base;
    p_tcb->StkLimitPtr = p_stk_limit;
#if (OS_CFG_DBG_EN > 0u)
    p_tcb->NamePtr = p_tcb->Task.name;
#endif
    p_tcb->TaskEntryAddr = (OS_TASK_PTR)p_tcb->Task.entry;
    p_tcb->TaskEntryArg = p_tcb->Task.parameter;
    p_tcb->Prio = p_tcb->Task.init_priority;
    p_tcb->SemCtr = p_tcb->Sem.Sem.value;
#endif    
    CPU_CRITICAL_EXIT();   
    
    /*在uCOS-III中的任务创建相当于RTT的任务创建+任务启动*/
    rt_err = rt_thread_startup(&p_tcb->Task);                 
    *p_err = rt_err_to_ucosiii(rt_err);
}

/*
************************************************************************************************************************
*                                                     DELETE A TASK
*
* Description: This function allows you to delete a task.  The calling task can delete itself by specifying a NULL
*              pointer for 'p_tcb'.  The deleted task is returned to the dormant state and can be re-activated by
*              creating the deleted task again.
*
* Arguments  : p_tcb      is the TCB of the tack to delete,若为NULL表示删除当前任务
*
*              p_err      is a pointer to an error code returned by this function:
*
*                             OS_ERR_NONE                  if the call is successful
*                             OS_ERR_ILLEGAL_DEL_RUN_TIME  If you are trying to delete the task after you called
*                                                             OSStart()
*                           - OS_ERR_STATE_INVALID         if the state of the task is invalid
*                           - OS_ERR_TASK_DEL_IDLE         if you attempted to delete uC/OS-III's idle task
*                           - OS_ERR_TASK_DEL_INVALID      if you attempted to delete uC/OS-III's ISR handler task
*                             OS_ERR_TASK_DEL_ISR          if you tried to delete a task from an ISR
*                         -------------说明-------------
*                             OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                           - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                           + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                          应用层需要对API返回的错误码判断做出相应的修改
*
* Note(s)    : 1) 'p_err' gets set to OS_ERR_NONE before OSSched() to allow the returned error code to be monitored even
*                 for a task that is deleting itself. In this case, 'p_err' MUST point to a global variable that can be
*                 accessed by another task.
*              2) 大多数线程是循环执行的，无需删除；而能运行完毕的线程，RT-Thread 在线程运行完毕后，自动删除线程，在 
*                 rt_thread_exit() 里完成删除动作。用户只需要了解该接口的作用，不推荐使用该接口（可以由其他线程调用此接
*                 口或在定时器超时函数中调用此接口删除一个线程，但是这种使用非常少）。
************************************************************************************************************************
*/

#if OS_CFG_TASK_DEL_EN > 0u
void  OSTaskDel (OS_TCB  *p_tcb,
                 OS_ERR  *p_err)
{
    rt_err_t rt_err;
    OS_ERR err;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif
    
#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == OS_TRUE) {
       *p_err = OS_ERR_ILLEGAL_DEL_RUN_TIME;
        return;
    }
#endif
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u    
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_TASK_DEL_ISR;
        return;
    }
#endif
    
    CPU_CRITICAL_ENTER();
#if OS_CFG_DBG_EN > 0u
    OS_TaskDbgListRemove(p_tcb);
#endif
    OSTaskQty--;                                            /* One less task being managed                            */
    CPU_CRITICAL_EXIT();
     
    if(p_tcb == RT_NULL)/*若为NULL表示删除当前任务*/
    {
        rt_err = rt_thread_detach(rt_thread_self());
        *p_err = rt_err_to_ucosiii(rt_err);   
        rt_schedule();  
    } 
    else
    {
        rt_err = rt_thread_detach(&p_tcb->Task);
        *p_err = rt_err_to_ucosiii(rt_err);   
    }
    
    OSSemDel(&p_tcb->Sem,OS_OPT_DEL_ALWAYS,&err);/*删除任务内建信号量*/
    OSQDel(&p_tcb->MsgQ,OS_OPT_DEL_ALWAYS,&err);/*删除任务内建消息队列*/
    OSTaskDelHook(p_tcb);/*调用钩子函数*/ 
    OS_TaskInitTCB(p_tcb);                                  /* Initialize the TCB to default values                   */
    p_tcb->TaskState = (OS_STATE)OS_TASK_STATE_DEL;         /* Indicate that the task was deleted                     */
}
#endif

/*
************************************************************************************************************************
*                                                    FLUSH TASK's QUEUE
*
* Description: This function is used to flush the task's internal message queue.
*
* Arguments  : p_tcb       is a pointer to the task's OS_TCB.  Specifying a NULL pointer indicates that you wish to
*                          flush the message queue of the calling task.
*
*              p_err       is a pointer to a variable that will contain an error code returned by this function.
*
*                              OS_ERR_NONE           upon success
*                              OS_ERR_FLUSH_ISR      if you called this function from an ISR
*
* Returns     : The number of entries freed from the queue
*
* Note(s)     : 1) You should use this function with great care because, when to flush the queue, you LOOSE the
*                  references to what the queue entries are pointing to and thus, you could cause 'memory leaks'.  In
*                  other words, the data you are pointing to that's being referenced by the queue entries should, most
*                  likely, need to be de-allocated (i.e. freed).
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u
OS_MSG_QTY  OSTaskQFlush (OS_TCB  *p_tcb,
                          OS_ERR  *p_err)
{    
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

    if (p_tcb == (OS_TCB *)0) {                             /* Flush message queue of calling task?                   */
        CPU_CRITICAL_ENTER();
        p_tcb = OSTCBCurPtr;
        CPU_CRITICAL_EXIT();
    }

    return OSQFlush(&p_tcb->MsgQ, p_err);
}
#endif

/*
************************************************************************************************************************
*                                                  WAIT FOR A MESSAGE
*
* Description: This function causes the current task to wait for a message to be posted to it.
*
* Arguments  : timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for a
*                            message to arrive up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever or, until a message arrives.
*
*              opt           determines whether the user wants to block if the task's queue is empty or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_msg_size    is a pointer to a variable that will receive the size of the message
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the message was
*                            received.  If you pass a NULL pointer (i.e. (CPU_TS *)0) then you will not get the
*                            timestamp.  In other words, passing a NULL pointer is valid and indicates that you don't
*                            need the timestamp.
*
*              p_err         is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                                OS_ERR_NONE               The call was successful and your task received a message.
*                                OS_ERR_PEND_ABORT
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                              - OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the queue was not empty
*                              - OS_ERR_Q_EMPTY
*                                OS_ERR_SCHED_LOCKED       If the scheduler is locked
*                                OS_ERR_TIMEOUT            A message was not received within the specified timeout
*                                                          would lead to a suspension.
*                              + OS_ERR_TASK_Q_CREATE_FALSE 任务内建消息队列创建失败
*                            -------------说明-------------
*                                OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                              - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                              + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                              应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : A pointer to the message received or a NULL pointer upon error.
*
* Note(s)    : 1) It is possible to receive NULL pointers when there are no errors.
*              2) RTT在非阻塞模式下不区分OS_ERR_PEND_WOULD_BLOCK还是OS_ERR_TIMEOUT，都按照OS_ERR_TIMEOUT处理
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u
void  *OSTaskQPend (OS_TICK       timeout,
                    OS_OPT        opt,
                    OS_MSG_SIZE  *p_msg_size,
                    CPU_TS       *p_ts,
                    OS_ERR       *p_err)
{
    OS_TCB  *p_tcb;
      
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((void *)0);;
    }
#endif
      
    p_tcb = OSTCBCurPtr;
    
    if(p_tcb->MsgCreateSuc == RT_TRUE)/*检查任务内建消息队列是否创建成功*/
    {
        p_tcb->PendOn = OS_TASK_PEND_ON_TASK_Q;
        return OSQPend(&p_tcb->MsgQ,timeout,opt,p_msg_size,p_ts,p_err);
    }
    else
    {
        *p_err = OS_ERR_TASK_Q_CREATE_FALSE;
        return RT_NULL;
    }
}
#endif

/*
************************************************************************************************************************
*                                              ABORT WAITING FOR A MESSAGE
*
* Description: This function aborts & readies the task specified.  This function should be used to fault-abort the wait
*              for a message, rather than to normally post the message to the task via OSTaskQPost().
*
* Arguments  : p_tcb     is a pointer to the task to pend abort
*
*              opt       provides options for this function:
*
*                            OS_OPT_POST_NONE         No option specified
*                            OS_OPT_POST_NO_SCHED     Indicates that the scheduler will not be called.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE              If the task was readied and informed of the aborted wait
*                            OS_ERR_PEND_ABORT_ISR    If you called this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE   If task was not pending on a message and thus there is nothing to
*                                                     abort.
*                            OS_ERR_PEND_ABORT_SELF   If you passed a NULL pointer for 'p_tcb'
*
* Returns    : == DEF_FALSE   if task was not waiting for a message, or upon error.
*              == DEF_TRUE    if task was waiting for a message and was readied and informed.
************************************************************************************************************************
*/

#if (OS_CFG_TASK_Q_EN > 0u) && (OS_CFG_TASK_Q_PEND_ABORT_EN > 0u)
CPU_BOOLEAN  OSTaskQPendAbort (OS_TCB  *p_tcb,
                               OS_OPT   opt,
                               OS_ERR  *p_err)
{
    OS_OPT _opt;
    
    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (DEF_FALSE);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* See if called from ISR ...                             */
       *p_err = OS_ERR_PEND_ABORT_ISR;                      /* ... can't Pend Abort from an ISR                       */
        return (DEF_FALSE);
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                                  /* ---------------- VALIDATE ARGUMENTS ------------------ */
    switch (opt) {                                          /* User must supply a valid option                        */
        case OS_OPT_POST_NONE:
        case OS_OPT_POST_NO_SCHED:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return (DEF_FALSE);
    }
#endif
    
    CPU_CRITICAL_ENTER();
#if OS_CFG_ARG_CHK_EN > 0u
    if ((p_tcb == (OS_TCB *)0) ||                           /* Pend abort self?                                       */
        (p_tcb == OSTCBCurPtr)) {
        CPU_CRITICAL_EXIT();  
       *p_err = OS_ERR_PEND_ABORT_SELF;                     /* ... doesn't make sense                                 */
        return (DEF_FALSE);
    }
#endif
//    if (p_tcb->PendOn != OS_TASK_PEND_ON_TASK_Q) {          /* Is task waiting for a message?                         */
//        CPU_CRITICAL_EXIT();                                /* No                                                     */
//       *p_err = OS_ERR_PEND_ABORT_NONE;
//        return (DEF_FALSE);
//    }    
    CPU_CRITICAL_EXIT();  
    
    _opt = OS_OPT_PEND_ABORT_1 | opt;
    OSQPendAbort(&p_tcb->MsgQ,_opt,p_err);
    if(*p_err != OS_ERR_NONE)
    {
        return DEF_FALSE;
    }   
    else
    {
        return DEF_TRUE;
    }    
}
#endif

/*
************************************************************************************************************************
*                                               POST MESSAGE TO A TASK
*
* Description: This function sends a message to a task
*
* Arguments  : p_tcb      is a pointer to the TCB of the task receiving a message.  If you specify a NULL pointer then
*                         the message will be posted to the task's queue of the calling task.  In other words, you'd be
*                         posting a message to yourself.
*
*              p_void     is a pointer to the message to send.
*
*              msg_size   is the size of the message sent (in #bytes)
*
*              opt        specifies whether the post will be FIFO or LIFO:
*
*                             OS_OPT_POST_FIFO       Post at the end   of the queue
*                             OS_OPT_POST_LIFO       Post at the front of the queue
*
*                             OS_OPT_POST_NO_SCHED   Do not run the scheduler after the post
*
*                          Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*
*
*              p_err      is a pointer to a variable that will hold the error code associated
*                         with the outcome of this call.  Errors can be:
*
*                             OS_ERR_NONE            The call was successful and the message was sent
*                             OS_ERR_Q_MAX           If the queue is full
*                           - OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs available from the pool
*                           + OS_ERR_TASK_Q_CREATE_FALSE 任务内建消息队列创建失败
*                         -------------说明-------------
*                             OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                           - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                           + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                           应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u
void  OSTaskQPost (OS_TCB       *p_tcb,
                   void         *p_void,
                   OS_MSG_SIZE   msg_size,
                   OS_OPT        opt,
                   OS_ERR       *p_err)
{   
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif
    
    if(p_tcb == RT_NULL)
    {
        p_tcb = OSTCBCurPtr;
    }
    
    if(p_tcb->MsgCreateSuc == RT_TRUE)/*检查任务内建消息队列是否创建成功*/
    {
        OSQPost(&p_tcb->MsgQ,p_void,msg_size,opt,p_err);
    }
    else
    {
        *p_err = OS_ERR_TASK_Q_CREATE_FALSE;
    }  
}
#endif

/*
************************************************************************************************************************
*                                       GET THE CURRENT VALUE OF A TASK REGISTER
*
* Description: This function is called to obtain the current value of a task register.  Task registers are application
*              specific and can be used to store task specific values such as 'error numbers' (i.e. errno), statistics,
*              etc.
*
* Arguments  : p_tcb     is a pointer to the OS_TCB of the task you want to read the register from.  If 'p_tcb' is a
*                        NULL pointer then you will get the register of the current task.
*
*              id        is the 'id' of the desired task variable.  Note that the 'id' must be less than
*                        OS_CFG_TASK_REG_TBL_SIZE
*
*              p_err     is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE            if the call was successful
*                            OS_ERR_REG_ID_INVALID  if the 'id' is not between 0 and OS_CFG_TASK_REG_TBL_SIZE-1
*
* Returns    : The current value of the task's register or 0 if an error is detected.
************************************************************************************************************************
*/

#if OS_CFG_TASK_REG_TBL_SIZE > 0u
OS_REG  OSTaskRegGet (OS_TCB     *p_tcb,
                      OS_REG_ID   id,
                      OS_ERR     *p_err)
{
    OS_REG     value;
    rt_thread_t p_thread;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_REG)0);
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if (id >= OS_CFG_TASK_REG_TBL_SIZE) {
       *p_err = OS_ERR_REG_ID_INVALID;
        return ((OS_REG)0);
    }
#endif

    CPU_CRITICAL_ENTER();
    if (p_tcb == (OS_TCB *)0) {
        p_thread = rt_thread_self();
    }
    else{
        p_thread = &p_tcb->Task;
    }
    value = ((OS_TCB*)p_thread)->RegTbl[id];
    CPU_CRITICAL_EXIT();
    
   *p_err = OS_ERR_NONE;
    return ((OS_REG)value);
}
#endif

/*
************************************************************************************************************************
*                                    ALLOCATE THE NEXT AVAILABLE TASK REGISTER ID
*
* Description: This function is called to obtain a task register ID.  This function thus allows task registers IDs to be
*              allocated dynamically instead of statically.
*
* Arguments  : p_err       is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE               if the call was successful
*                            OS_ERR_NO_MORE_ID_AVAIL   if you are attempting to assign more task register IDs than you 
*                                                           have available through OS_CFG_TASK_REG_TBL_SIZE.
*
* Returns    : The next available task register 'id' or OS_CFG_TASK_REG_TBL_SIZE if an error is detected.
************************************************************************************************************************
*/

#if OS_CFG_TASK_REG_TBL_SIZE > 0u
OS_REG_ID  OSTaskRegGetID (OS_ERR  *p_err)
{
    OS_REG_ID  id;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_REG_ID)OS_CFG_TASK_REG_TBL_SIZE);
    }
#endif
    
    CPU_CRITICAL_ENTER();
    if (OSTaskRegNextAvailID >= OS_CFG_TASK_REG_TBL_SIZE) {       /* See if we exceeded the number of IDs available   */
       *p_err = OS_ERR_NO_MORE_ID_AVAIL;                          /* Yes, cannot allocate more task register IDs      */
        CPU_CRITICAL_EXIT();
        return ((OS_REG_ID)OS_CFG_TASK_REG_TBL_SIZE);
    }
     
    id    = OSTaskRegNextAvailID;								  /* Assign the next available ID                     */
    OSTaskRegNextAvailID++;										  /* Increment available ID for next request          */
    CPU_CRITICAL_EXIT();
   *p_err = OS_ERR_NONE;
    return (id);
}
#endif

/*
************************************************************************************************************************
*                                       SET THE CURRENT VALUE OF A TASK REGISTER
*
* Description: This function is called to change the current value of a task register.  Task registers are application
*              specific and can be used to store task specific values such as 'error numbers' (i.e. errno), statistics,
*              etc.
*
* Arguments  : p_tcb     is a pointer to the OS_TCB of the task you want to set the register for.  If 'p_tcb' is a NULL
*                        pointer then you will change the register of the current task.
*
*              id        is the 'id' of the desired task register.  Note that the 'id' must be less than
*                        OS_CFG_TASK_REG_TBL_SIZE
*
*              value     is the desired value for the task register.
*
*              p_err     is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE            if the call was successful
*                            OS_ERR_REG_ID_INVALID  if the 'id' is not between 0 and OS_CFG_TASK_REG_TBL_SIZE-1
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_TASK_REG_TBL_SIZE > 0u
void  OSTaskRegSet (OS_TCB     *p_tcb,
                    OS_REG_ID   id,
                    OS_REG      value,
                    OS_ERR     *p_err)
{
    rt_thread_t p_thread;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u
    if (id >= OS_CFG_TASK_REG_TBL_SIZE) {
       *p_err = OS_ERR_REG_ID_INVALID;
        return;
    }
#endif

    CPU_CRITICAL_ENTER();
    if (p_tcb == (OS_TCB *)0) {
        p_thread = rt_thread_self();
    }
    else{
        p_thread = &p_tcb->Task;
    }
    ((OS_TCB*)p_thread)->RegTbl[id] = value;
    CPU_CRITICAL_EXIT();
   *p_err             = OS_ERR_NONE;
}
#endif

/*
************************************************************************************************************************
*                                               RESUME A SUSPENDED TASK
*
* Description: This function is called to resume a previously suspended task.  This is the only call that will remove an
*              explicit task suspension.
*
* Arguments  : p_tcb      Is a pointer to the task's OS_TCB to resume
*
*              p_err      Is a pointer to a variable that will contain an error code returned by this function
*
*                             OS_ERR_NONE                  if the requested task is resumed
*                           - OS_ERR_STATE_INVALID         if the task is in an invalid state
*                             OS_ERR_TASK_RESUME_ISR       if you called this function from an ISR
*                             OS_ERR_TASK_RESUME_SELF      You cannot resume 'self'
*                             OS_ERR_TASK_NOT_SUSPENDED    if the task to resume has not been suspended
*                         -------------说明-------------
*                             OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                           - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                           + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                          应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_TASK_SUSPEND_EN > 0u
void  OSTaskResume (OS_TCB  *p_tcb,
                    OS_ERR  *p_err)
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
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_TASK_RESUME_ISR;
        return;
    }
#endif
    
    if(p_tcb == RT_NULL)/*检查TCB指针是否为空*/
    {
        p_tcb = OSTCBCurPtr;
    }
    
#if OS_CFG_ARG_CHK_EN > 0u
    if(rt_thread_self() == &p_tcb->Task)/*检查任务是否企图自己恢复自己*/
    {
        *p_err = OS_ERR_TASK_RESUME_SELF;
        return;
    }
    /*检查任务是否没有被挂起*/
    if((p_tcb->Task.stat & RT_THREAD_STAT_MASK) != RT_THREAD_SUSPEND)
    {
        *p_err = OS_ERR_TASK_NOT_SUSPENDED;
        return;
    }
#endif
    
    if(p_tcb->SuspendCtr>0)
    {
        CPU_CRITICAL_ENTER();
        p_tcb->SuspendCtr--;
        CPU_CRITICAL_EXIT();
        *p_err = OS_ERR_NONE;
    }
    else
    {
        p_tcb->TaskState = OS_TASK_STATE_RDY;
        rt_err = rt_thread_resume(&p_tcb->Task);
        *p_err = rt_err_to_ucosiii(rt_err);       
    }

}
#endif

/*
************************************************************************************************************************
*                                              WAIT FOR A TASK SEMAPHORE
*
* Description: This function is called to block the current task until a signal is sent by another task or ISR.
*
* Arguments  : timeout       is the amount of time you are will to wait for the signal
*
*              opt           determines whether the user wants to block if a semaphore post was not received:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the semaphore was posted
*                            or pend aborted.  If you pass a NULL pointer (i.e. (CPU_TS *)0) then you will not get the
*                            timestamp.  In other words, passing a NULL pointer is valid and indicates that you don't
*                            need the timestamp.
*
*              p_err         is a pointer to an error code that will be set by this function
*
*                                OS_ERR_NONE               The call was successful and your task received a message.
*                                OS_ERR_PEND_ABORT
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                              - OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but no signal was received
*                                OS_ERR_SCHED_LOCKED       If the scheduler is locked
*                              - OS_ERR_STATUS_INVALID     If the pend status is invalid
*                                OS_ERR_TIMEOUT            A message was not received within the specified timeout
*                                                          would lead to a suspension.
*                              + OS_ERR_TASK_SEM_CREATE_FALSE 任务内建信号量创建失败
*                            -------------说明-------------
*                                OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                              - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                              + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                              应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : The current count of signals the task received, 0 if none.
************************************************************************************************************************
*/

OS_SEM_CTR  OSTaskSemPend (OS_TICK   timeout,
                           OS_OPT    opt,
                           CPU_TS   *p_ts,
                           OS_ERR   *p_err)
{    
    OS_TCB *p_tcb;
    OS_SEM_CTR ctr;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_SEM_CTR)0);
    }
#endif
    
    p_tcb = OSTCBCurPtr;   
    if(p_tcb->SemCreateSuc == RT_TRUE)/*检查任务内建信号量是否创建成功*/
    {
        CPU_CRITICAL_ENTER();
        p_tcb->PendOn = OS_TASK_PEND_ON_TASK_SEM;/*设置任务等待状态*/
#if OS_CFG_TASK_PROFILE_EN > 0u
        p_tcb->SemCtr = p_tcb->Sem.Sem.value;/*更新value*/
#endif
        CPU_CRITICAL_EXIT();
        
        ctr = OSSemPend(&p_tcb->Sem,timeout,opt,p_ts,p_err); 
        
        CPU_CRITICAL_ENTER();
#if OS_CFG_TASK_PROFILE_EN > 0u
        p_tcb->SemCtr = p_tcb->Sem.Sem.value;/*更新value*/
#endif
        CPU_CRITICAL_EXIT();
        return ctr;
    }
    else
    {
        *p_err = OS_ERR_TASK_SEM_CREATE_FALSE;
        return 0;
    }
}

/*
************************************************************************************************************************
*                                               ABORT WAITING FOR A SIGNAL
*
* Description: This function aborts & readies the task specified.  This function should be used to fault-abort the wait
*              for a signal, rather than to normally post the signal to the task via OSTaskSemPost().
*
* Arguments  : p_tcb     is a pointer to the task to pend abort
*
*              opt       provides options for this function:
*
*                            OS_OPT_POST_NONE         No option selected
*                            OS_OPT_POST_NO_SCHED     Indicates that the scheduler will not be called.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE              If the task was readied and informed of the aborted wait
*                            OS_ERR_PEND_ABORT_ISR    If you tried calling this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE   If the task was not waiting for a signal
*                            OS_ERR_PEND_ABORT_SELF   If you attempted to pend abort the calling task.  This is not
*                                                     possible since the calling task cannot be pending because it's
*                                                     running.
*
* Returns    : == DEF_FALSE   if task was not waiting for a message, or upon error.
*              == DEF_TRUE    if task was waiting for a message and was readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_TASK_SEM_PEND_ABORT_EN > 0u
CPU_BOOLEAN  OSTaskSemPendAbort (OS_TCB  *p_tcb,
                                 OS_OPT   opt,
                                 OS_ERR  *p_err)
{
    OS_OPT _opt;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (DEF_FALSE);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* See if called from ISR ...                             */
       *p_err = OS_ERR_PEND_ABORT_ISR;                      /* ... can't Pend Abort from an ISR                       */
        return (DEF_FALSE);
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    switch (opt) {                                          /* Validate 'opt'                                         */
        case OS_OPT_POST_NONE:
        case OS_OPT_POST_NO_SCHED:
             break;

        default:
            *p_err =  OS_ERR_OPT_INVALID;
             return (DEF_FALSE);
    }
#endif  
    
    CPU_CRITICAL_ENTER();
#if OS_CFG_ARG_CHK_EN > 0u    
    if ((p_tcb == (OS_TCB *)0) ||                           /* Pend abort self?                                       */
        (p_tcb == OSTCBCurPtr)) {
        CPU_CRITICAL_EXIT(); 
       *p_err = OS_ERR_PEND_ABORT_SELF;
        return (DEF_FALSE);
    }
#endif
//    if (p_tcb->PendOn != OS_TASK_PEND_ON_TASK_SEM) {        /* Is task waiting for a signal?                          */
//        CPU_CRITICAL_EXIT();
//       *p_err = OS_ERR_PEND_ABORT_NONE;
//        return (DEF_FALSE);
//    }
    CPU_CRITICAL_EXIT(); 
    
    _opt = OS_OPT_PEND_ABORT_1 | opt;
    OSSemPendAbort(&p_tcb->Sem,_opt,p_err);
    
    CPU_CRITICAL_ENTER();
#if OS_CFG_TASK_PROFILE_EN > 0u    
    p_tcb->SemCtr = p_tcb->Sem.Sem.value;
#endif
    CPU_CRITICAL_EXIT();
    
    if(*p_err != OS_ERR_NONE)
    {
        return DEF_FALSE;
    }   
    else
    {
        return DEF_TRUE;
    }
}
#endif

/*
************************************************************************************************************************
*                                                    SIGNAL A TASK
*
* Description: This function is called to signal a task waiting for a signal.
*
* Arguments  : p_tcb     is the pointer to the TCB of the task to signal.  A NULL pointer indicates that you are sending
*                        a signal to yourself.
*
*              opt       determines the type of POST performed:
*
*                             OS_OPT_POST_NONE         No option
*                           - OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*              p_err     is a pointer to an error code returned by this function:
*
*                            OS_ERR_NONE              If the requested task is signaled
*                            OS_ERR_SEM_OVF           If the post would cause the semaphore count to overflow.
*                          + OS_ERR_TASK_SEM_CREATE_FALSE 任务内建信号量创建失败
*                        -------------说明-------------
*                            OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                          - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                          + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                          应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : The current value of the task's signal counter or 0 if called from an ISR
************************************************************************************************************************
*/

OS_SEM_CTR  OSTaskSemPost (OS_TCB  *p_tcb,
                           OS_OPT   opt,
                           OS_ERR  *p_err)
{
    OS_SEM_CTR ctr;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_SEM_CTR)0);
    }
#endif
    
    if(p_tcb == RT_NULL)
    {
        p_tcb = OSTCBCurPtr;
    }
    if(p_tcb->SemCreateSuc == RT_TRUE)/*检查任务内建信号量是否创建成功*/
    {
        ctr = OSSemPost(&p_tcb->Sem,opt,p_err);
        CPU_CRITICAL_ENTER();
#if OS_CFG_TASK_PROFILE_EN > 0u
        p_tcb->SemCtr = p_tcb->Sem.Sem.value;
#endif
        CPU_CRITICAL_EXIT();
        return ctr;
    }
    else
    {
        *p_err = OS_ERR_TASK_SEM_CREATE_FALSE;
        return 0;
    }
}

/*
************************************************************************************************************************
*                                            SET THE SIGNAL COUNTER OF A TASK
*
* Description: This function is called to clear the signal counter
*
* Arguments  : p_tcb      is the pointer to the TCB of the task to clear the counter.  If you specify a NULL pointer
*                         then the signal counter of the current task will be cleared.
*
*              cnt        is the desired value of the semaphore counter
*
*              p_err      is a pointer to an error code returned by this function
*
*                             OS_ERR_NONE        if the signal counter of the requested task is cleared
*                             OS_ERR_SET_ISR     if the function was called from an ISR
*
* Returns    : none
************************************************************************************************************************
*/

OS_SEM_CTR  OSTaskSemSet (OS_TCB      *p_tcb,
                          OS_SEM_CTR   cnt,
                          OS_ERR      *p_err)
{
    OS_SEM_CTR  ctr;
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_SEM_CTR)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u      
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_SET_ISR;
        return ((OS_SEM_CTR)0);
    }
#endif
    
    if(p_tcb == RT_NULL)
    {
        p_tcb = OSTCBCurPtr;
    }
    
    CPU_CRITICAL_ENTER();
    ctr = p_tcb->Sem.Sem.value;
    p_tcb->Sem.Sem.value = (OS_SEM_CTR)cnt;/*设置RTT信号量value*/
#if OS_CFG_TASK_PROFILE_EN > 0u
    p_tcb->SemCtr = p_tcb->Sem.Sem.value;/*更新.SemCtr*/
#endif
    CPU_CRITICAL_EXIT();
    *p_err = OS_ERR_NONE;
    return ctr;
}

/*
************************************************************************************************************************
*                                                    STACK CHECKING
*
* Description: This function is called to calculate the amount of free memory left on the specified task's stack.
*
* Arguments  : p_tcb       is a pointer to the TCB of the task to check.  If you specify a NULL pointer then
*                          you are specifying that you want to check the stack of the current task.
*
*              p_free      is a pointer to a variable that will receive the number of free 'entries' on the task's stack.
*
*              p_used      is a pointer to a variable that will receive the number of used 'entries' on the task's stack.
*
*              p_used_max  is a pointer to a variable that will receive the maximum number of used 'entries' on the task's stack.
*                          -------------说明-------------
*                          该参数是相较于原版新增的参数
*
*              p_err       is a pointer to a variable that will contain an error code.
*
*                              OS_ERR_NONE               upon success
*                              OS_ERR_PTR_INVALID        if either 'p_free' or 'p_used' are NULL pointers
*                              OS_ERR_TASK_NOT_EXIST     if the stack pointer of the task is a NULL pointer
*                              OS_ERR_TASK_OPT           if you did NOT specified OS_OPT_TASK_STK_CHK when the task
*                                                        was created
*                              OS_ERR_TASK_STK_CHK_ISR   you called this function from an ISR
*                         -------------说明-------------
*                             OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                           - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                           + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                          应用层需要对API返回的错误码判断做出相应的修改
* Returns    : none
*
* Note(s)    : 1) 原版函数获取的是实时堆栈使用情况,在本兼容层中该函数获取到的截止到当前时刻的堆栈最大使用和实时使用/剩余,
*                 比原版函数多了一个p_used_max参数,这要比原版更能有效的评估当前任务(线程)的堆栈使用情况
************************************************************************************************************************
*/

#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
void  OSTaskStkChk (OS_TCB        *p_tcb,
                    CPU_STK_SIZE  *p_free,
                    CPU_STK_SIZE  *p_used,
                    OS_ERR        *p_err)
{
    rt_uint32_t stack_size;
    rt_uint32_t stack_used;
    rt_uint32_t stack_free;
    rt_uint8_t *ptr;
    rt_thread_t thread;
    
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
        *p_err = OS_ERR_TASK_STK_CHK_ISR;
        return;
    }
#endif  
    
#if OS_CFG_ARG_CHK_EN > 0u
    if (p_free == (CPU_STK_SIZE*)0) {                       /* User must specify valid destinations for the sizes     */
       *p_err  = OS_ERR_PTR_INVALID;
        return;
    }

    if (p_used == (CPU_STK_SIZE*)0) {
       *p_err  = OS_ERR_PTR_INVALID;
        return;
    }
#endif
    
    /*若TCB指针为NULL,表示当前线程*/
    if(p_tcb ==RT_NULL)
    {
        p_tcb = OSTCBCurPtr;
    }
    
    thread = (rt_thread_t)p_tcb;
    
    CPU_CRITICAL_ENTER();
#if OS_CFG_TASK_PROFILE_EN > 0u 
#if OS_CFG_DBG_EN > 0u    
    if (p_tcb->StkPtr == (CPU_STK*)0) {                     /* Make sure task exist                                   */
        CPU_CRITICAL_EXIT();
       *p_free = (CPU_STK_SIZE)0;
       *p_used = (CPU_STK_SIZE)0;
       *p_err  =  OS_ERR_TASK_NOT_EXIST;
        return;
    }
#endif
    if ((p_tcb->Opt & OS_OPT_TASK_STK_CHK) == (OS_OPT)0) {  /* Make sure stack checking option is set                 */
        CPU_CRITICAL_EXIT();
       *p_free = (CPU_STK_SIZE)0;
       *p_used = (CPU_STK_SIZE)0;
       *p_err  =  OS_ERR_TASK_OPT;
        return;
    }
#endif
    CPU_CRITICAL_EXIT();
    
    *p_err = OS_ERR_NONE;
    
    /*计算RT-Thread堆栈最大使用情况*/    
#if CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO       
    ptr = (rt_uint8_t *)thread->stack_addr;
    while (*ptr == '#')ptr ++;
    stack_size = thread->stack_size;
    stack_used = thread->stack_size - ((rt_ubase_t) ptr - (rt_ubase_t) thread->stack_addr);
    stack_free = stack_size - stack_used;
    *p_used = stack_used / sizeof(CPU_STK_SIZE);
    *p_free = stack_free / sizeof(CPU_STK_SIZE);
#else
    ptr = (rt_uint8_t *)thread->stack_addr + thread->stack_size - 1;
    while (*ptr == '#')ptr --;
    stack_size = thread->stack_size;
    stack_used = (rt_ubase_t)ptr - (rt_ubase_t)thread->stack_addr
    stack_free = stack_size - stack_used;
    *p_used = stack_used / sizeof(CPU_STK_SIZE);
    *p_free = stack_free / sizeof(CPU_STK_SIZE);    
#endif
}
#endif

/*
************************************************************************************************************************
*                                                   SUSPEND A TASK
*
* Description: This function is called to suspend a task.  The task can be the calling task if 'p_tcb' is a NULL pointer
*              or the pointer to the TCB of the calling task.
*
* Arguments  : p_tcb    is a pointer to the TCB to suspend.
*                       If p_tcb is a NULL pointer then, suspend the current task.
*
*              p_err    is a pointer to a variable that will receive an error code from this function.
*
*                           OS_ERR_NONE                      if the requested task is suspended
*                           OS_ERR_OS_NOT_RUNNING            If uC/OS-III is not running yet
*                           OS_ERR_SCHED_LOCKED              you can't suspend the current task is the scheduler is
*                                                            locked
*                           OS_ERR_STATE_INVALID             If the task is in an invalid state
*                           OS_ERR_TASK_SUSPEND_CTR_OVF      If the nesting counter overflowed.
*                           OS_ERR_TASK_SUSPEND_ISR          if you called this function from an ISR
*                         - OS_ERR_TASK_SUSPEND_IDLE         if you attempted to suspend the idle task which is not
*                                                            allowed.
*                         - OS_ERR_TASK_SUSPEND_INT_HANDLER  if you attempted to suspend the idle task which is not
*                                                            allowed.
*                       -------------说明-------------
*                           OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                         - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                         + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                        应用层需要对API返回的错误码判断做出相应的修改
*
* Note(s)    : 1) You should use this function with great care.  If you suspend a task that is waiting for an event
*                 (i.e. a message, a semaphore, a queue ...) you will prevent this task from running when the event
*                 arrives.
************************************************************************************************************************
*/

void   OSTaskSuspend (OS_TCB  *p_tcb,
                      OS_ERR  *p_err)
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
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_TASK_SUSPEND_ISR;
        return;
    }
#endif

    if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0)/*检查调度器是否被锁*/
    {
        *p_err = OS_ERR_SCHED_LOCKED;
        return;         
    }

    if (OSRunning != OS_STATE_OS_RUNNING) {                 /* Can't suspend self when the kernel isn't running     */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }
    
    /*TCB指针是否为空,若为空则为当前线程*/
    if(p_tcb == RT_NULL)
    {
        p_tcb = OSTCBCurPtr;
    } 

    /*检查.SuspendCtr是否将要溢出*/
    if (p_tcb->SuspendCtr == (OS_NESTING_CTR)-1) {
       *p_err = OS_ERR_TASK_SUSPEND_CTR_OVF;
        return;
    }
    
    if((p_tcb->Task.stat & RT_THREAD_STAT_MASK) == RT_THREAD_SUSPEND)
    {
        /*如果任务是挂起状态*/
        CPU_CRITICAL_ENTER();
        p_tcb->SuspendCtr++;
        CPU_CRITICAL_EXIT();
        *p_err = OS_ERR_NONE;
    }
    else if((p_tcb->Task.stat & RT_THREAD_STAT_MASK) == RT_THREAD_READY)
    {
        /*任务处于运行态才能被挂起*/
        p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;
        rt_err = rt_thread_suspend(&p_tcb->Task);
        if(rt_thread_self() == &p_tcb->Task)/*是否要将自己挂起*/
        {
            rt_schedule();/* 根据RTT的要求,若挂起自己需要立即调用rt_shedule进行调度*/
        }    
        *p_err = rt_err_to_ucosiii(rt_err);        
    }
    else
    {
        *p_err = OS_ERR_STATE_INVALID;
    }
}

/*
************************************************************************************************************************
*                                                CHANGE A TASK'S TIME SLICE
*
* Description: This function is called to change the value of the task's specific time slice.
*
* Arguments  : p_tcb        is the pointer to the TCB of the task to change. If you specify an NULL pointer, the current
*                           task is assumed.
*
*              time_quanta  is the number of ticks before the CPU is taken away when round-robin scheduling is enabled.
*
*              p_err        is a pointer to an error code returned by this function:
*
*                               OS_ERR_NONE       upon success
*                               OS_ERR_SET_ISR    if you called this function from an ISR
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void  OSTaskTimeQuantaSet (OS_TCB   *p_tcb,
                           OS_TICK   time_quanta,
                           OS_ERR   *p_err)
{
}
#endif

/*
************************************************************************************************************************
*                                            ADD/REMOVE TASK TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove an OS_TCB from the debug list.
*
* Arguments  : p_tcb     is a pointer to the OS_TCB to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u
void  OS_TaskDbgListAdd (OS_TCB  *p_tcb)
{
    p_tcb->DbgPrevPtr                = (OS_TCB *)0;
    if (OSTaskDbgListPtr == (OS_TCB *)0) {
        p_tcb->DbgNextPtr            = (OS_TCB *)0;
    } else {
        p_tcb->DbgNextPtr            =  OSTaskDbgListPtr;
        OSTaskDbgListPtr->DbgPrevPtr =  p_tcb;
    }
    OSTaskDbgListPtr                 =  p_tcb;
}



void  OS_TaskDbgListRemove (OS_TCB  *p_tcb)
{
    OS_TCB  *p_tcb_next;
    OS_TCB  *p_tcb_prev;


    p_tcb_prev = p_tcb->DbgPrevPtr;
    p_tcb_next = p_tcb->DbgNextPtr;

    if (p_tcb_prev == (OS_TCB *)0) {
        OSTaskDbgListPtr = p_tcb_next;
        if (p_tcb_next != (OS_TCB *)0) {
            p_tcb_next->DbgPrevPtr = (OS_TCB *)0;
        }
        p_tcb->DbgNextPtr = (OS_TCB *)0;

    } else if (p_tcb_next == (OS_TCB *)0) {
        p_tcb_prev->DbgNextPtr = (OS_TCB *)0;
        p_tcb->DbgPrevPtr      = (OS_TCB *)0;

    } else {
        p_tcb_prev->DbgNextPtr =  p_tcb_next;
        p_tcb_next->DbgPrevPtr =  p_tcb_prev;
        p_tcb->DbgNextPtr      = (OS_TCB *)0;
        p_tcb->DbgPrevPtr      = (OS_TCB *)0;
    }
}
#endif

/*
************************************************************************************************************************
*                                             TASK MANAGER INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the task management.
*

* Argument(s): p_err        is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE     the call was successful
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TaskInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSTaskDbgListPtr = (OS_TCB      *)0;
#endif

    OSTaskQty        = (OS_OBJ_QTY   )0;                    /* Clear the number of tasks                              */
    
    *p_err            = OS_ERR_NONE;
}

/*
************************************************************************************************************************
*                                               INITIALIZE TCB FIELDS
*
* Description: This function is called to initialize a TCB to default values
*
* Arguments  : p_tcb    is a pointer to the TCB to initialize
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TaskInitTCB (OS_TCB  *p_tcb)
{
#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    OS_REG_ID   reg_id;
#endif
    
    CPU_SR_ALLOC();    
    
    CPU_CRITICAL_ENTER();        
    p_tcb->SemCreateSuc   = (CPU_BOOLEAN    )RT_FALSE;
#if OS_CFG_TASK_Q_EN > 0u      
    p_tcb->MsgPtr         = (void          *)0u;
    p_tcb->MsgSize        = (OS_MSG_SIZE    )0u;
    p_tcb->MsgCreateSuc   = (CPU_BOOLEAN    )RT_FALSE;
#endif
    p_tcb->ExtPtr             = (void          *)0u;  
#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    for (reg_id = 0u; reg_id < OS_CFG_TASK_REG_TBL_SIZE; reg_id++) {
        p_tcb->RegTbl[reg_id] = (OS_REG)0u;
    }
#endif
    p_tcb->PendStatus         = (OS_STATUS      )OS_STATUS_PEND_OK;
#if OS_CFG_TASK_SUSPEND_EN > 0u
    p_tcb->SuspendCtr         = (OS_NESTING_CTR )0u;
#endif
#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
    p_tcb->StkFree            = (CPU_STK_SIZE   )0u;
    p_tcb->StkUsed            = (CPU_STK_SIZE   )0u;
#endif
#if OS_CFG_DBG_EN > 0u
    p_tcb->DbgPrevPtr         = (OS_TCB        *)0;
    p_tcb->DbgNextPtr         = (OS_TCB        *)0;
    p_tcb->DbgNamePtr         = (CPU_CHAR      *)((void *)" ");
#endif
    p_tcb->TaskState          = (OS_STATE       )OS_TASK_STATE_RDY;    
    p_tcb->PendOn             = (OS_STATE       )OS_TASK_PEND_ON_NOTHING;
    
#if OS_CFG_TASK_PROFILE_EN > 0u 
#if OS_CFG_DBG_EN > 0u 
    p_tcb->StkPtr             = (CPU_STK       *)0;
#endif
    p_tcb->TimeQuanta         = (OS_TICK        )0u;
    p_tcb->TimeQuantaCtr      = (OS_TICK        )0u;
    p_tcb->SemCtr             = (OS_SEM_CTR     )0u;
    p_tcb->Opt                = (OS_OPT         )0u;
    p_tcb->StkSize            = (CPU_STK        )0u;
    p_tcb->StkLimitPtr        = (CPU_STK       *)0;    
    p_tcb->StkBasePtr         = (CPU_STK       *)0;     
#if (OS_CFG_DBG_EN > 0u)
    p_tcb->NamePtr            = (CPU_CHAR      *)((void *)"?Task");
#endif
    p_tcb->TaskEntryAddr      = (OS_TASK_PTR    )0;
    p_tcb->TaskEntryArg       = (void          *)0;
    p_tcb->Prio               = (OS_PRIO        )OS_PRIO_INIT;
#if OS_CFG_FLAG_EN > 0u
    p_tcb->FlagsPend          = (OS_FLAGS       )0u;
    p_tcb->FlagsOpt           = (OS_OPT         )0u;
    p_tcb->FlagsRdy           = (OS_FLAGS       )0u;
#endif    
#endif    
    CPU_CRITICAL_EXIT();
}

