/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-12     Meco Man     the first verion
 */
 
#include "os.h"
#include <string.h>
#include <rthw.h>

extern void (*rt_object_put_hook)(struct rt_object *object);

/**
 * 将RT-Thread错误码转换为uCOS-III错误码
 *
 * @param RT-Thread错误码
 *
 * @return uCOS-III错误码
 */
OS_ERR rt_err_to_ucosiii(rt_err_t rt_err)
{
    switch(-rt_err)/*RTT返回的错误码都是带负号的*/
    {
        /*以下RTT错误码可以用原版uCOS-III错误码进行兼容*/
        case RT_EOK: /* 无错误 */
            return OS_ERR_NONE;
        case RT_ETIMEOUT:/* 超时错误 */
            return OS_ERR_TIMEOUT;
        case RT_EINVAL:/* 非法参数 */
            return OS_ERR_OPT_INVALID;        
        case RT_EFULL:/* 资源已满,该参数仅在IPC中使用 */
            return OS_ERR_Q_MAX;        
        /*
            由于uCOS-III的错误码分类较细，而RTT的错误码分类较为笼统，
            以下RTT错误码使用uCOS-III的错误码很难准确描述
            因此将针对RTT的错误码重新定义(新增)uCOS-III的错误码
        */
        case RT_ERROR:/* 普通错误    */
            return OS_ERR_RT_ERROR;
        case RT_EEMPTY:/* 无资源     */
            return OS_ERR_RT_EEMPTY;
        case RT_ENOMEM:/* 无内存     */
            return OS_ERR_RT_ENOMEM;
        case RT_ENOSYS:/* 系统不支持 */
            return OS_ERR_RT_ENOSYS;
        case RT_EBUSY:/* 系统忙      */
            return OS_ERR_RT_EBUSY;
        case RT_EIO:/* IO 错误       */
            return OS_ERR_RT_EIO;
        case RT_EINTR:/* 中断系统调用*/
            return OS_ERR_RT_EINTR;

        default:
            return OS_ERR_RT_ERROR;
    }
}

/**
 * 让当挂起表第一个任务放弃等待IPC,进入就绪态(由rt_ipc_list_resume函数改编)
 *
 * @param 挂起表表头指针
 *
 * @return 错误码
 */
rt_err_t rt_ipc_pend_abort_1 (rt_list_t *list)
{
    struct rt_thread *thread;
    register rt_ubase_t temp;
    OS_TCB  *p_tcb;
    
    temp = rt_hw_interrupt_disable();
    /* get thread entry */
    thread = rt_list_entry(list->next, struct rt_thread, tlist);
    p_tcb = (OS_TCB*)thread;
    /* set error code to RT_ERROR */
    thread->error = -RT_ERROR;
    
    /*标记当前任务放弃等待*/
    p_tcb->PendStatus = OS_STATUS_PEND_ABORT; 
    
    rt_hw_interrupt_enable(temp);
    
    /* resume it */
    rt_thread_resume(thread); 

    return RT_EOK;
}

/**
 * 让所有等待该IPC的任务全部放弃等待，进入就绪态(由rt_ipc_list_resume_all函数改编)
 *
 * @param 挂起表表头指针
 *
 * @return 放弃了多少个任务
 */
rt_uint16_t rt_ipc_pend_abort_all (rt_list_t *list)
{
    struct rt_thread *thread;
    register rt_ubase_t temp;
    OS_TCB *p_tcb;
    rt_uint16_t i=0;

    /* wakeup all suspend threads */
    while (!rt_list_isempty(list))
    {
        /* disable interrupt */
        temp = rt_hw_interrupt_disable();

        /* get next suspend thread */
        thread = rt_list_entry(list->next, struct rt_thread, tlist);
        p_tcb = ((OS_TCB*)thread);
        /* set error code to RT_ERROR */
        thread->error = -RT_ERROR;
                
        /*标记当前任务放弃等待*/
        p_tcb->PendStatus = OS_STATUS_PEND_ABORT;
        
        /*
         * resume thread
         * In rt_thread_resume function, it will remove current thread from
         * suspend list
         */
        rt_thread_resume(thread);

        /* enable interrupt */
        rt_hw_interrupt_enable(temp);
        
        i++;
    }

    return i;
}

/**
 * 让所有等待该IPC的任务全部批准进入就绪态(由rt_ipc_list_resume_all函数改编)
 *
 * @param 挂起表表头指针
 *
 * @return 错误码
 */
static rt_err_t rt_ipc_post_all (rt_list_t *list)
{
    struct rt_thread *thread;
    register rt_ubase_t temp;
    
    /* wakeup all suspend threads */
    while (!rt_list_isempty(list))
    {
        /* disable interrupt */
        temp = rt_hw_interrupt_disable();

        /* get next suspend thread */
        thread = rt_list_entry(list->next, struct rt_thread, tlist);       
        
        /*
         * resume thread
         * In rt_thread_resume function, it will remove current thread from
         * suspend list
         */
        rt_thread_resume(thread);

        /* enable interrupt */
        rt_hw_interrupt_enable(temp);
    }

    return RT_EOK;
}

/**
 * This function will wake ALL threads which are WAITTING for semaphores
 * 改编自rt_sem_release函数
 *
 * @param sem the semaphore object
 *
 * @return the error code
 */
rt_err_t rt_sem_release_all(rt_sem_t sem)
{
    register rt_ubase_t temp;
    register rt_bool_t need_schedule;

    /* parameter check */
    RT_ASSERT(sem != RT_NULL);
    RT_ASSERT(rt_object_get_type(&sem->parent.parent) == RT_Object_Class_Semaphore);

    RT_OBJECT_HOOK_CALL(rt_object_put_hook, (&(sem->parent.parent)));
    
    need_schedule = RT_FALSE;

    /* disable interrupt */
    temp = rt_hw_interrupt_disable();

    RT_DEBUG_LOG(RT_DEBUG_IPC, ("thread %s releases sem:%s, which value is: %d\n",
                                rt_thread_self()->name,
                                ((struct rt_object *)sem)->name,
                                sem->value));

    if (!rt_list_isempty(&sem->parent.suspend_thread))
    {
        /* resume the suspended thread */
        rt_ipc_post_all(&(sem->parent.suspend_thread));
        need_schedule = RT_TRUE;
    }

    /* enable interrupt */
    rt_hw_interrupt_enable(temp);

    /* resume a thread, re-schedule */
    if (need_schedule == RT_TRUE)
        rt_schedule();

    return RT_EOK;
}

/**
 * This function will wake ALL threads which are WAITTING for message queue (FIFO)
 * 改编自rt_mq_send函数
 *
 * @param mq the message queue object
 * @param buffer the message
 * @param size the size of buffer
 *
 * @return the error code
 */
rt_err_t rt_mq_send_all(rt_mq_t mq, void *buffer, rt_size_t size)
{
    register rt_ubase_t temp;
    struct _rt_mq_message *msg;
    rt_uint16_t suspend_len;

    /* parameter check */
    RT_ASSERT(mq != RT_NULL);
    RT_ASSERT(rt_object_get_type(&mq->parent.parent) == RT_Object_Class_MessageQueue);
    RT_ASSERT(buffer != RT_NULL);
    RT_ASSERT(size != 0);

    /* greater than one message size */
    if (size > mq->msg_size)
        return -RT_ERROR;

    RT_OBJECT_HOOK_CALL(rt_object_put_hook, (&(mq->parent.parent)));

    /* disable interrupt */
    temp = rt_hw_interrupt_disable();

    /* 获取当前n个线程被当前消息队列挂起 */
    suspend_len = rt_list_len(&mq->parent.suspend_thread);
    
    /* 将相同的消息复制n次,一会一起发出去 */
    while(suspend_len)
    {
        /* get a free list, there must be an empty item */
        msg = (struct _rt_mq_message *)mq->msg_queue_free;
        /* message queue is full */
        if (msg == RT_NULL)
        {
            /* enable interrupt */
            rt_hw_interrupt_enable(temp);

            return -RT_EFULL;
        }
        /* move free list pointer */
        mq->msg_queue_free = msg->next;

        /* enable interrupt */
        rt_hw_interrupt_enable(temp);

        /* the msg is the new tailer of list, the next shall be NULL */
        msg->next = RT_NULL;
        /* copy buffer */
        rt_memcpy(msg + 1, buffer, size);

        /* disable interrupt */
        temp = rt_hw_interrupt_disable();
        /* link msg to message queue */
        if (mq->msg_queue_tail != RT_NULL)
        {
            /* if the tail exists, */
            ((struct _rt_mq_message *)mq->msg_queue_tail)->next = msg;
        }

        /* set new tail */
        mq->msg_queue_tail = msg;
        /* if the head is empty, set head */
        if (mq->msg_queue_head == RT_NULL)
            mq->msg_queue_head = msg;

        /* increase message entry */
        mq->entry ++;

        suspend_len --;
    }

    /* resume suspended thread */
    if (!rt_list_isempty(&mq->parent.suspend_thread))
    {
        /* 将等待本消息队列的所有线程全部解挂,此时他们将同时获得相同的消息 */
        rt_ipc_post_all(&(mq->parent.suspend_thread));

        /* enable interrupt */
        rt_hw_interrupt_enable(temp);

        rt_schedule();

        return RT_EOK;
    }

    /* enable interrupt */
    rt_hw_interrupt_enable(temp);

    return RT_EOK;
}

#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
/**
 * msh命令：uCOS-III兼容层信息获取
 */
#if defined RT_USING_FINSH && OS_CFG_DBG_EN > 0u
static void rt_ucosiii_wrapper_info (int argc, char *argv[])
{
#if OS_CFG_STAT_TASK_EN > 0u
    OS_CPU_USAGE cpu_usage;
#endif
    OS_TCB *p_tcb;
#if OS_CFG_TMR_EN > 0u
    OS_TMR *p_tmr;
#endif
#if OS_CFG_SEM_EN > 0u
    OS_SEM *p_sem;
#endif
#if OS_CFG_MUTEX_EN > 0u
    OS_MUTEX *p_mutex;
#endif
#if OS_CFG_Q_EN > 0u
    OS_Q *p_q;
#endif
#if OS_CFG_FLAG_EN > 0u
    OS_FLAG_GRP *p_flag;
#endif

    CPU_SR_ALLOC();
    
    if(argc == 1)
    {
        rt_kprintf("invalid parameter,use --help to get more information.\n");
        return;
    }
    
    if(!strcmp((const char *)argv[1],(const char *)"--help"))
    {
        rt_kprintf("-v version\n");
#if OS_CFG_STAT_TASK_EN >0u
        rt_kprintf("-u cpu usage\n");
#endif
        rt_kprintf("-t task\n");
#if OS_CFG_SEM_EN > 0u
        rt_kprintf("-s sem\n");
#endif
#if OS_CFG_MUTEX_EN > 0u
        rt_kprintf("-m mutex\n");
#endif
#if OS_CFG_Q_EN > 0u
        rt_kprintf("-q message queue\n");
#endif
#if OS_CFG_FLAG_EN > 0u
        rt_kprintf("-f event flag\n");
#endif
#if OS_CFG_TMR_EN > 0u
        rt_kprintf("-r timer\n");
#endif
    }
    else if(!strcmp((const char *)argv[1],(const char *)"-v"))
    {
        rt_kprintf("template's version: 3.03.00\n");
        rt_kprintf("compatible version: 3.00 - 3.08\n");
    }      
#if OS_CFG_STAT_TASK_EN >0u
    else if(!strcmp((const char *)argv[1],(const char *)"-u"))
    {
        CPU_CRITICAL_ENTER();
        cpu_usage = OSStatTaskCPUUsage;
        CPU_CRITICAL_EXIT();
        rt_kprintf("CPU Usage: %d.%d%%\n",cpu_usage/100,cpu_usage%100);
    }
#endif
#if OS_CFG_TMR_EN > 0u
    else if(!strcmp((const char *)argv[1],(const char *)"-r"))
    {
        CPU_CRITICAL_ENTER();
        p_tmr = OSTmrDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III Timer--------------------\n");
        while(p_tmr)
        {
            rt_kprintf("name:%s\n",p_tmr->Tmr.parent.name);
            p_tmr = p_tmr->DbgNextPtr;
        }
        rt_kprintf("\n");
    }
#endif
    else if(!strcmp((const char *)argv[1],(const char *)"-t"))
    {
        CPU_CRITICAL_ENTER();
        p_tcb = OSTaskDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III Task---------------------\n");
        while(p_tcb)
        {
            rt_kprintf("name:%s\n",p_tcb->Task.name);
            p_tcb = p_tcb->DbgNextPtr;
        }
        rt_kprintf("\n");        
    }
#if OS_CFG_SEM_EN > 0u
    else if(!strcmp((const char *)argv[1],(const char *)"-s"))
    {
        CPU_CRITICAL_ENTER();
        p_sem = OSSemDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III Sem----------------------\n");
        while(p_sem)
        {
            rt_kprintf("name:%s\n",p_sem->Sem.parent.parent.name);
            p_sem = p_sem->DbgNextPtr;
        }
        rt_kprintf("\n");          
    }
#endif
#if OS_CFG_MUTEX_EN > 0u
    else if(!strcmp((const char *)argv[1],(const char *)"-m"))
    {
        CPU_CRITICAL_ENTER();
        p_mutex = OSMutexDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III Mutex--------------------\n");
        while(p_mutex)
        {
            rt_kprintf("name:%s\n",p_mutex->Mutex.parent.parent.name);
            p_mutex = p_mutex->DbgNextPtr;
        }
        rt_kprintf("\n");          
    }
#endif
#if OS_CFG_Q_EN > 0u
    else if(!strcmp((const char *)argv[1],(const char *)"-q"))
    {
        CPU_CRITICAL_ENTER();
        p_q = OSQDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III MsgQ---------------------\n");
        while(p_q)
        {
            rt_kprintf("name:%s\n",p_q->Msg.parent.parent.name);
            p_q = p_q->DbgNextPtr;
        }
        rt_kprintf("\n");          
    } 
#endif
#if OS_CFG_FLAG_EN > 0u
    else if(!strcmp((const char *)argv[1],(const char *)"-f"))
    {
        CPU_CRITICAL_ENTER();
        p_flag = OSFlagDbgListPtr;
        CPU_CRITICAL_EXIT();
        rt_kprintf("-----------------uCOS-III Flag---------------------\n");
        while(p_flag)
        {
            rt_kprintf("name:%s\n",p_flag->FlagGrp.parent.parent.name);
            p_flag = p_flag->DbgNextPtr;
        }
        rt_kprintf("\n");          
    }  
#endif    
    else
    {
        rt_kprintf("invalid parameter,use --help to get more information.\n");
    }
}
MSH_CMD_EXPORT_ALIAS(rt_ucosiii_wrapper_info, ucos, get ucos-iii wrapper info);
#endif
#endif

/**
 * 自动初始化
 * uCOS-III兼容层支持按照uCOS-III原版的初始化步骤进行初始化，但是在有些情况，
 * 用户不想手动初始化uCOS-III兼容层，想要直接运行应用层任务或模块，则可以使用该
 * 宏定义。在rtconfig.h中定义本宏定义后，在RT-Thread初始化完成并进入到main线程之前
 * 会自动将uCOS-III兼容层初始化完毕，用户仅需要专注于uCOS-III的应用级任务即可。
 * The wrapper supports uCOS-III standard startup procedure. Alternatively,
 * if you want to run uCOS-III apps directly and ignore the startup procedure, 
 * you can choose this option.
 */
#ifdef PKG_USING_UCOSIII_WRAPPER_AUTOINIT
#include <os_app_hooks.h>
static int rt_ucosiii_autoinit(void)
{
    OS_ERR err;
    
    OSInit(&err);                                   /*uCOS-III操作系统初始化*/
    OSStart(&err);                                  /*开始运行uCOS-III操作系统*/
    
    CPU_Init();
    
#if OS_CFG_APP_HOOKS_EN > 0u
    App_OS_SetAllHooks();                           /*设置钩子函数*/
#endif  
    
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);  	                /*统计任务*/    
    OSStatReset(&err);                              /*复位统计数据*/    
#endif
    
    return 0;
}
INIT_COMPONENT_EXPORT(rt_ucosiii_autoinit);
#endif
