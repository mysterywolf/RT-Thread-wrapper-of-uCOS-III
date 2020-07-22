/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-14     Meco Man     the first verion
 */

/*本例程展示了如何使用uCOS-III的定时器、获取任务堆栈使用情况以及获取CPU使用率*/

#include <os.h>

#define TASK_PRIORITY         5		/*任务优先级*/
#define TASK_STACK_SIZE       256	/*任务堆栈大小*/
#define TASK_TIMESLICE        5		/*任务时间片*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask1_Stack[TASK_STACK_SIZE];/*任务堆栈*/
static OS_TCB  AppTask1_TCB;/*任务控制块*/

static OS_TMR  CPUUsageTimer;/*CPU使用率测量定时器*/

/*定时器回调函数*/ 
void tmr1_callback(void *p_tmr, void *p_arg)
{
    rt_kprintf("FROM '%s'-->",((OS_TMR*)p_tmr)->Tmr.parent.name);
    rt_kprintf("CPU usage:%d.%d%%\n",OSStatTaskCPUUsage/100,OSStatTaskCPUUsage%100);
}

/*任务函数*/
static void AppTask1 (void *param)
{
    OS_ERR err;
    CPU_STK_SIZE free,used;
    
	/*创建定时器1*/
	OSTmrCreate((OS_TMR		*)&CPUUsageTimer,/*定时器句柄*/
                (CPU_CHAR	*)"CPUUsageTimer",/*定时器名字*/ 
                (OS_TICK	 )300,/*300*10=3000ms(10由OS_CFG_TMR_TASK_RATE_HZ决定)*/
                (OS_TICK	 )50,/*50*10=500ms(10由OS_CFG_TMR_TASK_RATE_HZ决定)*/
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, /*周期模式*/
                (OS_TMR_CALLBACK_PTR)tmr1_callback,/*定时器1回调函数*/
                (void	    *)0,/*参数为0*/
                (OS_ERR	    *)&err);/*返回的错误码*/
                
    OSTmrStart(&CPUUsageTimer,&err);/*开启定时器*/
              
    while(1)
    {
 		OSTaskStkChk(RT_NULL,&free,&used,&err);/*获取当前任务堆栈信息*/ 
    	rt_kprintf("AppTask1 free:%d,used:%d\n",free,used);   
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_NON_STRICT,&err);/*延时500ms*/
    }
}

void timer_sample (void)
{
    OS_ERR err;

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
            rt_kprintf("task create err:%d\n",err);
        }               
}
