/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-12     Meco Man     the first verion
 */
 
#include <os.h>
#include <stdlib.h>
#include <string.h>

/**
 * 将RT-Thread错误码转换为uCOS-III错误码
 *
 * @param RT-Thread错误码
 *
 * @return uCOS-III错误码
 */
OS_ERR rt_err_to_ucosiii(rt_err_t rt_err)
{
    int rt_err2 = abs((int)rt_err);/*RTT返回的错误码都是带负号的*/
    switch(rt_err2)
    {
        /*以下RTT错误码可以用原版uCOS-III错误码进行兼容*/
        case RT_EOK: /* 无错误       */
            return OS_ERR_NONE;
        case RT_ETIMEOUT:/* 超时错误 */
            return OS_ERR_TIMEOUT;
        case RT_EINVAL:/* 非法参数   */
            return OS_ERR_OPT_INVALID;        
        case RT_EFULL:/* 资源已满,该参数仅在IPC中使用*/
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
    
    CPU_SR_ALLOC();
    
    CPU_CRITICAL_ENTER();
    thread = rt_list_entry(list->next, struct rt_thread, tlist);/* get thread entry */
    thread->error = -RT_ERROR;/* set error code to RT_ERROR */
    ((OS_TCB*)thread)->PendStatus = OS_STATUS_PEND_ABORT; /*标记当前任务放弃等待*/
    CPU_CRITICAL_EXIT();
   
    rt_thread_resume(thread); /* resume it */

    return RT_EOK;
}

/**
 * 让所有等待该IPC的任务全部放弃等待，进入就绪态(由rt_ipc_list_resume_all函数改编)
 *
 * @param 挂起表表头指针
 *
 * @return 错误码
 */
rt_err_t rt_ipc_pend_abort_all (rt_list_t *list)
{
    struct rt_thread *thread;

    CPU_SR_ALLOC();

    /* wakeup all suspend threads */
    while (!rt_list_isempty(list))
    {
        /* disable interrupt */
        CPU_CRITICAL_ENTER();

        /* get next suspend thread */
        thread = rt_list_entry(list->next, struct rt_thread, tlist);
        /* set error code to RT_ERROR */
        thread->error = -RT_ERROR;
        /*标记当前任务放弃等待*/
        ((OS_TCB*)thread)->PendStatus = OS_STATUS_PEND_ABORT; 
        
        /*
         * resume thread
         * In rt_thread_resume function, it will remove current thread from
         * suspend list
         */
        rt_thread_resume(thread);

        /* enable interrupt */
        CPU_CRITICAL_EXIT();
    }

    return RT_EOK;
}

static void ucos_wrap_info (int argc, char *argv[])
{
    OS_ERR err;
    CPU_INT16U version;
    OS_CPU_USAGE cpu_usage;
    
    CPU_SR_ALLOC();
    
    if(argc == 1)
    {
        rt_kprintf("invalid parameter,use --help to get more information.\r\n");
        return;
    }
    
    if(!strcmp((const char *)argv[1],(const char *)"--help"))
    {
        rt_kprintf("-v version\r\n");
        rt_kprintf("-u cpu usage\r\n");
        rt_kprintf("-s sem\r\n");
        rt_kprintf("-m mutex\r\n");
        rt_kprintf("-q message queue\r\n");
        rt_kprintf("-f event flag\r\n");
        rt_kprintf("-t timer\r\n");
        rt_kprintf("-m memory pool\r\n");
    }
    else if(!strcmp((const char *)argv[1],(const char *)"-v"))
    {
        version = OSVersion(&err);
        rt_kprintf("template's version: %d\r\n",version);
        rt_kprintf("compatible version: 3.03.00 - 3.08.00\r\n");
    }    
    else if(!strcmp((const char *)argv[1],(const char *)"-u"))
    {
        CPU_CRITICAL_ENTER();
        cpu_usage = OSStatTaskCPUUsage;
        CPU_CRITICAL_EXIT();
        rt_kprintf("CPU Usage: %d.%d%%\r\n",cpu_usage/100,cpu_usage%100);
    }
    else
    {
        rt_kprintf("invalid parameter,use --help to get more information.\r\n");
    }
}
MSH_CMD_EXPORT_ALIAS(ucos_wrap_info, ucos, get ucos wrapper info);
