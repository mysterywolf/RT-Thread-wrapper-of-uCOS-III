/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-22     Meco Man     the first verion
 */

/*
本例程展示了如何使用uCOS-III的信号量进行广播同步(RTT内核没有该功能)
task1同时向task2、task3广播信号量
*/

#include <os.h>

#if OS_CFG_SEM_EN > 0u

#define TASK_PRIORITY         6		/*任务优先级*/
#define TASK_STACK_SIZE       80	/*任务堆栈大小*/
#define TASK_TIMESLICE        5		/*任务时间片*/


ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask1_Stack[TASK_STACK_SIZE];/*任务堆栈*/
static OS_TCB  AppTask1_TCB;/*任务控制块*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask2_Stack[TASK_STACK_SIZE];/*任务堆栈*/
static OS_TCB  AppTask2_TCB;/*任务控制块*/


ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask3_Stack[TASK_STACK_SIZE];/*任务堆栈*/
static OS_TCB  AppTask3_TCB;/*任务控制块*/

static OS_SEM sem;

/*任务1 负责广播信号量*/
static void AppTask1 (void *param)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    
    OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_NON_STRICT,&err);/*延时100ms*/
    while(1)
    {
        OSSemPost(&sem,OS_OPT_POST_ALL,&err);
        CPU_CRITICAL_ENTER();
        rt_kprintf("-------------------------\r\n");
        CPU_CRITICAL_EXIT();
        OSTimeDlyHMSM(0,0,0,800,OS_OPT_TIME_HMSM_NON_STRICT,&err);/*延时800ms*/
    }
}

/*任务2 负责等待信号量*/
static void AppTask2 (void *param)
{
    OS_ERR err;
    rt_uint8_t cnt =0;
    CPU_SR_ALLOC();
    
    while(1)
    {
        OSSemPend(&sem,0,OS_OPT_PEND_BLOCKING,0,&err);
        cnt ++;
        if(err == OS_ERR_NONE)
        {
            CPU_CRITICAL_ENTER();
            rt_kprintf("task2 has pended the sem,cnt:%d\r\n",cnt);
            CPU_CRITICAL_EXIT();
        }
        else
        {
            CPU_CRITICAL_ENTER();
            rt_kprintf("task2 got a error when are pending a sem\r\n");
            CPU_CRITICAL_EXIT();
        }        
    }
}

/*任务3 负责等待信号量*/
static void AppTask3 (void *param)
{
    OS_ERR err;
    rt_uint8_t cnt =0;
    CPU_SR_ALLOC();
    
    while(1)
    {
        OSSemPend(&sem,0,OS_OPT_PEND_BLOCKING,0,&err);
        cnt ++;
        if(err == OS_ERR_NONE)
        {
            CPU_CRITICAL_ENTER();
            rt_kprintf("task3 has pended the sem,cmt:%d\r\n",cnt);
            CPU_CRITICAL_EXIT();
        }
        else
        {
            CPU_CRITICAL_ENTER();
            rt_kprintf("task3 got a error when are pending a sem\r\n");
            CPU_CRITICAL_EXIT();
        }
    }
}



void sem_example (void)
{
    OS_ERR err;

    OSSemCreate(&sem,"sem",0,&err);
    
	OSTaskCreate(&AppTask1_TCB,		            /*任务控制块*/ 
			   (CPU_CHAR*)"AppTask1", 		    /*任务名字*/
               AppTask1, 			            /*任务函数*/
               0,					            /*传递给任务函数的参数*/
               TASK_PRIORITY,                   /*任务优先级*/
               &AppTask1_Stack[0],              /*任务堆栈基地址*/
               TASK_STACK_SIZE/10,	            /*任务堆栈深度限位*/
               TASK_STACK_SIZE,		            /*任务堆栈大小*/
               0,					            /*任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息*/
               TASK_TIMESLICE,			        /*当使能时间片轮转时的时间片长度，为0时为默认长度*/
               0,					            /*用户补充的存储区*/
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, /*任务选项*/
               &err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("task1 create err:%d\n",err);
        }    

	OSTaskCreate(&AppTask2_TCB,		            /*任务控制块*/ 
			   (CPU_CHAR*)"AppTask2", 		    /*任务名字*/
               AppTask2, 			            /*任务函数*/
               0,					            /*传递给任务函数的参数*/
               TASK_PRIORITY,                   /*任务优先级*/
               &AppTask2_Stack[0],              /*任务堆栈基地址*/
               TASK_STACK_SIZE/10,	            /*任务堆栈深度限位*/
               TASK_STACK_SIZE,		            /*任务堆栈大小*/
               0,					            /*任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息*/
               TASK_TIMESLICE,			        /*当使能时间片轮转时的时间片长度，为0时为默认长度*/
               0,					            /*用户补充的存储区*/
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, /*任务选项*/
               &err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("task2 create err:%d\n",err);
        }  


	OSTaskCreate(&AppTask3_TCB,		            /*任务控制块*/ 
			   (CPU_CHAR*)"AppTask3", 		    /*任务名字*/
               AppTask3, 			            /*任务函数*/
               0,					            /*传递给任务函数的参数*/
               TASK_PRIORITY,                   /*任务优先级*/
               &AppTask3_Stack[0],              /*任务堆栈基地址*/
               TASK_STACK_SIZE/10,	            /*任务堆栈深度限位*/
               TASK_STACK_SIZE,		            /*任务堆栈大小*/
               0,					            /*任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息*/
               TASK_TIMESLICE,			        /*当使能时间片轮转时的时间片长度，为0时为默认长度*/
               0,					            /*用户补充的存储区*/
               OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, /*任务选项*/
               &err);
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("task3 create err:%d\n",err);
        }          
}

#endif
