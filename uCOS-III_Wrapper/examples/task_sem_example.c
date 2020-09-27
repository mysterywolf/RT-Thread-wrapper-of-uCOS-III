/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-27     Meco Man     the first verion
 */

/*本例程展示了如何使用uCOS-III的任务内建信号量*/

#include <os.h>

#define THREAD_PRIORITY         6
#define THREAD_STACK_SIZE       80
#define THREAD_TIMESLICE        5

ALIGN(RT_ALIGN_SIZE)
static CPU_STK thread1_stack[THREAD_STACK_SIZE];
static OS_TCB thread1;

ALIGN(RT_ALIGN_SIZE)
static CPU_STK thread2_stack[THREAD_STACK_SIZE];
static OS_TCB thread2;

/* 线程1入口 */
static void thread1_entry(void *param)
{
    OS_ERR err;

    while(1) 
    {
        OSTaskSemPost(&thread2,OS_OPT_POST_NONE,&err);
        //OSTaskSemPendAbort(&thread2,OS_OPT_POST_NONE,&err);
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_NON_STRICT,&err);
    }
}
    

/* 线程2入口 */
static void thread2_entry(void *param)
{
    OS_ERR err;
              
    while(1)
    {
        OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
        if(err==OS_ERR_NONE)
        {
            rt_kprintf("has pended a task sem!\r\n");
        }
        else if(err == OS_ERR_PEND_ABORT)
        {
            rt_kprintf("abort!\r\n");
        }
        else
        {
            rt_kprintf("err:%d\r\n",err);
        }
    }
}
                 

void task_sem_example (void)
{
    OS_ERR err;
        
	OSTaskCreate(&thread2,		            //任务控制块
			   (CPU_CHAR*)"thread2", 		//任务名字
               thread2_entry, 			    //任务函数
               0,					        //传递给任务函数的参数
               THREAD_PRIORITY-1,           //任务优先级
               &thread2_stack[0],           //任务堆栈基地址
               THREAD_STACK_SIZE/10,	    //任务堆栈深度限位
               THREAD_STACK_SIZE,		    //任务堆栈大小
               0,					        //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
               THREAD_TIMESLICE,			//当使能时间片轮转时的时间片长度，为0时为默认长度，
               0,					        //用户补充的存储区
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
               &err);
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("task create err:%d\r\n",err);
    }   
        
        
	OSTaskCreate(&thread1,		            //任务控制块
			   (CPU_CHAR*)"thread1", 		//任务名字
               thread1_entry, 			    //任务函数
               0,					        //传递给任务函数的参数
               THREAD_PRIORITY,             //任务优先级
               &thread1_stack[0],           //任务堆栈基地址
               THREAD_STACK_SIZE/10,	    //任务堆栈深度限位
               THREAD_STACK_SIZE,		    //任务堆栈大小
               0,					        //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
               THREAD_TIMESLICE,			//当使能时间片轮转时的时间片长度，为0时为默认长度，
               0,					        //用户补充的存储区
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
               &err);
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("task create err:%d\r\n",err);
    }
}
