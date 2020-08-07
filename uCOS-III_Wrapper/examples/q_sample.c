/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-23     Meco Man     the first verion
 */

/*本例程展示了如何使用uCOS-III的消息队列*/

#include <os.h>

#define TASK_PRIORITY         6		/*任务优先级*/
#define TASK_STACK_SIZE       80 	/*任务堆栈大小*/
#define TASK_TIMESLICE        5		/*任务时间片*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask1_Stack[TASK_STACK_SIZE];/*任务堆栈*/
static OS_TCB  AppTask1_TCB;/*任务控制块*/

ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTask2_Stack[TASK_STACK_SIZE];/*任务堆栈*/
static OS_TCB  AppTask2_TCB;/*任务控制块*/

static OS_Q msg;/*定义消息队列*/

static char buffer[128];

static void AppTask1 (void *param)
{
    OS_ERR err;
    rt_uint8_t i = 0;
    
    while(1)
    {
        i++;
        rt_sprintf(buffer,"task1 msg:%d",i);
        
        /*发送消息到消息队列*/
        OSQPost((OS_Q*		)&msg,		
                (void*		)buffer,
                (OS_MSG_SIZE)rt_strlen(buffer),
                (OS_OPT		)OS_OPT_POST_FIFO,
                (OS_ERR*	)&err);    
        
        if(err!=OS_ERR_NONE)
        {
            rt_kprintf("queue post err:%d!\r\n",err);
        }          
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_NON_STRICT,&err);/*延时500ms*/
    }
}


static void AppTask2 (void *param)
{
    OS_ERR err;
	char  *p;
	OS_MSG_SIZE size;
    
    while(1)
    {
        /*阻塞等待消息*/
		p=OSQPend((OS_Q*		)&msg,   
				  (OS_TICK		)0,
                  (OS_OPT		)OS_OPT_PEND_BLOCKING,
                  (OS_MSG_SIZE*	)&size,	
                  (CPU_TS*		)0,
                  (OS_ERR*		)&err);
        if(err==OS_ERR_NONE)
        {
            rt_kprintf("pended: str:%s,size:%d\r\n",p,size);
        }
        else
        {
            rt_kprintf("queue pend err!:%d\r\n",err);
        }           
    }
}


void q_sample (void)
{
    OS_ERR err;
    
	OSQCreate ((OS_Q*		)&msg,	
                (CPU_CHAR*	)"msg",	
                (OS_MSG_QTY	)10,/*消息队列容纳的最大消息数*/
                (OS_ERR*	)&err);	  
    if(err!=OS_ERR_NONE)
    {
        rt_kprintf("queue create err!:%d\r\n",err);
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
