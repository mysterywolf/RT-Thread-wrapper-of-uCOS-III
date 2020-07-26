/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-14     Meco Man     the first verion
 */

/*本例程展示了如何使用uCOS-III的互斥量进行线程间共享资源保护*/

#include <os.h>

#define TASK_PRIORITY         6		/*任务优先级*/
#define TASK_STACK_SIZE       80	/*任务堆栈大小*/
#define TASK_TIMESLICE        5		/*任务时间片*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask1_Stack[TASK_STACK_SIZE];/*任务堆栈*/
static OS_TCB  AppTask1_TCB;/*任务控制块*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask2_Stack[TASK_STACK_SIZE];/*任务堆栈*/
static OS_TCB  AppTask2_TCB;/*任务控制块*/

static OS_MUTEX	SyncMutex;/*定义互斥量*/

/*任务函数*/
static void AppTask1 (void *param)
{
	OS_ERR err;
	
	while(1)
	{
        /*互斥量嵌套5次*/
        OSMutexPend(&SyncMutex,0,OS_OPT_PEND_BLOCKING,0,&err); /*申请互斥量 请求访问共享资源*/ 
        OSMutexPend(&SyncMutex,0,OS_OPT_PEND_BLOCKING,0,&err); /*申请互斥量 请求访问共享资源*/ 
        OSMutexPend(&SyncMutex,0,OS_OPT_PEND_BLOCKING,0,&err); /*申请互斥量 请求访问共享资源*/ 
        OSMutexPend(&SyncMutex,0,OS_OPT_PEND_BLOCKING,0,&err); /*申请互斥量 请求访问共享资源*/ 
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_NON_STRICT,&err);
        rt_kprintf("This is Task1\n");
        OSMutexPost(&SyncMutex,OS_OPT_POST_NONE,&err);/*释放互斥量*/
        OSMutexPost(&SyncMutex,OS_OPT_POST_NONE,&err);/*释放互斥量*/
        OSMutexPost(&SyncMutex,OS_OPT_POST_NONE,&err);/*释放互斥量*/
        OSMutexPost(&SyncMutex,OS_OPT_POST_NONE,&err);/*释放互斥量*/
	}
}


/*任务函数*/
static void AppTask2 (void *param)
{
	OS_ERR err;
	
	while(1)
	{
		OSMutexPend(&SyncMutex,0,OS_OPT_PEND_BLOCKING,0,&err); /*请求访问共享资源*/
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_NON_STRICT,&err);
        rt_kprintf("This is Task2\n");
        OSMutexPost(&SyncMutex,OS_OPT_POST_NONE,&err);/*释放互斥量*/
	}	
}


void mutex_sample (void)
{
    OS_ERR err;
    
    /*创建互斥量*/
	OSMutexCreate ((OS_MUTEX*)&SyncMutex,
                 (CPU_CHAR*	)"SyncMutex",	
                 (OS_ERR*	)&err);    
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("create mutex err!\n");
    }
    
	OSTaskCreate(&AppTask1_TCB,		            /*任务控制块*/ 
			   (CPU_CHAR*)"AppTask1", 		    /*任务名字*/
               AppTask1, 			            /*任务函数*/
               0,					            /*传递给任务函数的参数*/
               TASK_PRIORITY,                   /*任务优先级*/
               AppTask1_Stack,	                /*任务堆栈基地址*/
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
               AppTask2_Stack,	                /*任务堆栈基地址*/
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
}
