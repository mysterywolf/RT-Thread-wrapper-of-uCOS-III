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
由于RTT没有相关接口，因此以下函数没有实现
1)任务内建信号量、消息队列
OSTaskQFlush
OSTaskQPend
OSTaskQPendAbort
OSTaskQPost
OSTaskSemPend
OSTaskSemPendAbort
OSTaskSemPost
OSTaskSemSet

2)任务内建寄存器
OSTaskRegGet
OSTaskRegSet

3)运行过程中修改任务参数
OSTaskChangePrio
OSTaskTimeQuantaSet
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

void  OSTaskChangePrio (OS_TCB   *p_tcb,
                        OS_PRIO   prio_new,
                        OS_ERR   *p_err)
{
}

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
*                            -------------说明-------------
*                             该参数在本兼容层中没有意义，填什么都行
*
*              time_quanta    amount of time (in ticks) for time slice when round-robin between tasks.  Specify 0 to use
*                             the default.
*
*              p_ext          is a pointer to a user supplied memory location which is used as a TCB extension.
*                             For example, this user memory can hold the contents of floating-point registers
*                             during a context switch, the time each task takes to execute, the number of times
*                             the task has been switched-in, etc.
*                            -------------说明-------------
*                             该参数在本兼容层中没有意义，填什么都行
*
*              opt            contains additional information (or options) about the behavior of the task.
*                             See OS_OPT_TASK_xxx in OS.H.  Current choices are:
*
*                               - OS_OPT_TASK_NONE            No option selected
*                               - OS_OPT_TASK_STK_CHK         Stack checking to be allowed for the task
*                               - OS_OPT_TASK_STK_CLR         Clear the stack when the task is created
*                               - OS_OPT_TASK_SAVE_FP         If the CPU has floating-point registers, save them
*                                                             during a context switch.
*                               - OS_OPT_TASK_NO_TLS          If the caller doesn't want or need TLS (Thread Local 
*                                                             Storage) support for the task.  If you do not include this
*                                                             option, TLS will be supported by default.
*                            -------------说明-------------
*                             该参数在本兼容层中没有意义，填什么都行
*
*
*              p_err          is a pointer to an error code that will be set during this call.  The value pointer
*                             to by 'p_err' can be:
*
*                                 OS_ERR_NONE                    if the function was successful.
*                               - OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the task after you called
*                                                                   OSSafetyCriticalStart().
*                                 OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                 OS_ERR_PRIO_INVALID            if the priority you specify is higher that the maximum
*                                                                   allowed (i.e. >= OS_CFG_PRIO_MAX-1) or,
*                                                                if OS_CFG_ISR_POST_DEFERRED_EN is set to 1 and you tried
*                                                                   to use priority 0 which is reserved.
*                                 OS_ERR_STK_INVALID             if you specified a NULL pointer for 'p_stk_base'
*                                 OS_ERR_STK_SIZE_INVALID        if you specified zero for the 'stk_size'
*                               - OS_ERR_STK_LIMIT_INVALID       if you specified a 'stk_limit' greater than or equal
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
    
    (void)q_size;
    (void)p_ext;
    (void)opt;
    (void)stk_limit;
    
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_TASK_CREATE_ISR;
        return;
    }

    /*检查TCB指针是否为空*/
    if(p_tcb == RT_NULL)
    {
        *p_err = OS_ERR_TCB_INVALID;
        return;
    }     
    
    /*检查任务函数指针是否为空*/
    if(p_task == RT_NULL)
    {
        *p_err = OS_ERR_TASK_INVALID;
        return;
    }
    
    /*检查任务名是否为NULL*/
    if(p_name == RT_NULL)
    {
        *p_err = OS_ERR_NAME;
        return;
    }    
    
    /*检查任务优先级*/
    if(prio >= RT_THREAD_PRIORITY_MAX-1)
    {
        *p_err = OS_ERR_PRIO_INVALID;
        return;        
    }
    
    /*检查任务堆栈指针是否为NULL*/
    if(p_stk_base == RT_NULL)
    {
        *p_err = OS_ERR_STK_INVALID;
        return;
    }
    
    /*检查堆栈大小是否为0*/
    if(stk_size == 0)
    {
        *p_err = OS_ERR_STK_SIZE_INVALID;
        return;
    }
    
    rt_err = rt_thread_init(p_tcb,
                            (const char*)p_name,
                            p_task,
                            p_arg,
                            p_stk_base,
                            stk_size*sizeof(CPU_STK),/*uCOS-III的任务堆栈时以CPU_STK为单位，而RTT是以字节为单位，因此需要进行转换*/
                            prio,
                            time_quanta);
    
    *p_err = _err_rtt_to_ucosiii(rt_err);
    if(rt_err != RT_EOK)
    {
        return;
    }

    /*在uCOS-III中的任务创建相当于RTT的任务创建+任务启动*/
    rt_err = rt_thread_startup(p_tcb);                 
    *p_err = _err_rtt_to_ucosiii(rt_err);
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

void  OSTaskDel (OS_TCB  *p_tcb,
                 OS_ERR  *p_err)
{
    rt_err_t rt_err;
    
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_TASK_DEL_ISR;
        return;
    }

    /*若为NULL表示删除当前任务*/
    if(p_tcb == RT_NULL)
    {
        rt_err = rt_thread_detach(rt_thread_self());
        *p_err = _err_rtt_to_ucosiii(rt_err);   
        rt_schedule();  
    } 
    else
    {
        rt_err = rt_thread_detach(p_tcb);
        *p_err = _err_rtt_to_ucosiii(rt_err);   
    }
}

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
*
* Note(s)    : 1) uCOS-III是支持嵌套挂起/解挂,即挂起5次,解挂5次,任务即可正常运行
*                 但是RTT并不支持嵌套挂起/解挂,因此该函数无法实现嵌套解挂
************************************************************************************************************************
*/

void  OSTaskResume (OS_TCB  *p_tcb,
                    OS_ERR  *p_err)
{
    rt_err_t rt_err;
    
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_TASK_RESUME_ISR;
        return;
    }

    /*检查TCB指针是否为空*/
    if(p_tcb == RT_NULL)
    {
        /*尊重uCOS-III源码,这种情况属于试图挂起自己的行为*/
        *p_err = OS_ERR_TASK_RESUME_SELF;
        return;
    }     
    
    /*检查任务是否企图自己恢复自己*/
    if(rt_thread_self() == p_tcb)
    {
        *p_err = OS_ERR_TASK_RESUME_SELF;
        return;
    }
    
    /*检查任务是否没有被挂起*/
    if((p_tcb->stat & RT_THREAD_STAT_MASK) != RT_THREAD_SUSPEND)
    {
        *p_err = OS_ERR_TASK_NOT_SUSPENDED;
        return;
    }
    
    rt_err = rt_thread_resume(p_tcb);
    
    *p_err = _err_rtt_to_ucosiii(rt_err);
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
*
*              p_err       is a pointer to a variable that will contain an error code.
*
*                              OS_ERR_NONE               upon success
*                              OS_ERR_PTR_INVALID        if either 'p_free' or 'p_used' are NULL pointers
*                              OS_ERR_TASK_NOT_EXIST     if the stack pointer of the task is a NULL pointer
*                            - OS_ERR_TASK_OPT           if you did NOT specified OS_OPT_TASK_STK_CHK when the task
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

void  OSTaskStkChk (OS_TCB        *p_tcb,
                    CPU_STK_SIZE  *p_free,
                    CPU_STK_SIZE  *p_used,
                    CPU_STK_SIZE  *p_used_max,
                    OS_ERR        *p_err)
{
    rt_uint32_t stack_size;
    rt_uint32_t stack_used_max;
    rt_uint32_t stack_used;
    rt_uint32_t stack_free;
    rt_uint8_t *ptr;
    
    if(p_free == RT_NULL ||
       p_used == RT_NULL
    )
    {
        *p_err = OS_ERR_PTR_INVALID;
        return;
    }
        
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_TASK_STK_CHK_ISR;
        return;
    }   
    
    /*若TCB指针为NULL,表示当前线程*/
    if(p_tcb ==RT_NULL)
    {
        p_tcb = rt_thread_self();
    }
    
    /*检查任务堆栈是否为NULL*/
    if(p_tcb->stack_addr == RT_NULL)
    {
        *p_err = OS_ERR_TASK_NOT_EXIST;
        return;        
    }
    
    *p_err = OS_ERR_NONE;
    
    /*计算堆栈最大使用情况*/
    ptr = (rt_uint8_t *)p_tcb->stack_addr;
    while (*ptr == '#')ptr ++;
    stack_size = p_tcb->stack_size;
    stack_used_max = p_tcb->stack_size - ((rt_ubase_t) ptr - (rt_ubase_t) p_tcb->stack_addr);
    
    /*计算堆栈实时使用情况*/
    stack_used = (rt_ubase_t)p_tcb->stack_addr + p_tcb->stack_size - (rt_ubase_t)p_tcb->sp;
    stack_free = stack_size - stack_used;
    
    *p_used_max = stack_used_max / sizeof(CPU_STK_SIZE);
    *p_used = stack_used / sizeof(CPU_STK_SIZE);
    *p_free = stack_free / sizeof(CPU_STK_SIZE);
}

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
*                           OS_ERR_SCHED_LOCKED              you can't suspend the current task is the scheduler is
*                                                            locked
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
*              2) uCOS-III是支持嵌套挂起/解挂,即挂起5次,解挂5次,任务即可正常运行
*                 但是RTT并不支持嵌套挂起/解挂,因此该函数无法实现嵌套挂起
************************************************************************************************************************
*/

void   OSTaskSuspend (OS_TCB  *p_tcb,
                      OS_ERR  *p_err)
{
    rt_err_t rt_err;
    
    /*检查是否在中断中运行*/
    if(rt_interrupt_get_nest()!=0)
    {
        *p_err = OS_ERR_TASK_SUSPEND_ISR;
        return;
    }

    /*检查调度器是否被锁*/
    if(rt_critical_level() > 0)
    {
        *p_err = OS_ERR_SCHED_LOCKED;
        return;         
    }

    /*TCB指针是否为空,若为空表示删除当前线程*/
    if(p_tcb == RT_NULL)
    {
        rt_err = rt_thread_suspend(rt_thread_self());
        rt_schedule();/* 根据RTT的要求,若挂起自己需要立即调用rt_shedule进行调度*/
    } 
    else
    {
        rt_err = rt_thread_suspend(p_tcb);
        if(rt_thread_self() == p_tcb)/*是否要将自己挂起*/
        {
            rt_schedule();/* 根据RTT的要求,若挂起自己需要立即调用rt_shedule进行调度*/
        }
    }
        
    *p_err = _err_rtt_to_ucosiii(rt_err);
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

void  OSTaskTimeQuantaSet (OS_TCB   *p_tcb,
                           OS_TICK   time_quanta,
                           OS_ERR   *p_err)
{
}
