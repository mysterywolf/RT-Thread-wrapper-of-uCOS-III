/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-05     yangjie      First edition
 * 2020-07-14     Meco Man     implement uCOS-III Wrapper
 */

/*串口使用USART2*/
/*启动以及初始化过程严格遵照官方给出的例程*/

#include <os.h>
#include <os_app_hooks.h>

/*宏定义*/
#define APP_TASK_START_STK_SIZE     150   /*开始任务 任务堆栈大小*/
#define APP_TASK_START_PRIO         5     /*开始任务 任务优先级*/

/*任务堆栈以及TCB*/
ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];/*任务堆栈*/
static OS_TCB AppTaskStartTCB;

/*函数声明*/
static void AppTaskStart(void *p_arg);
static void AppTaskCreate(void);

/*空函数*/
#define BSP_Init()          do{ rt_kprintf("BSP init!\r\n"); }while(0)
#define BSP_LED_Toggle(x)   do{ rt_kprintf("LED Toggle!\r\n"); }while(0)


int main(void)
{
    OS_ERR err;
    
    OSInit(&err);                                   /*uCOS-III操作系统初始化*/
        
    /*创建开始任务*/
	OSTaskCreate((OS_TCB 	* )&AppTaskStartTCB,		
				 (CPU_CHAR	* )"App Task Start", 		
                 (OS_TASK_PTR )AppTaskStart, 			
                 (void		* )0,			
                 (OS_PRIO	  )APP_TASK_START_PRIO,     
                 (CPU_STK   * )&AppTaskStartStk[0],	
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE/10,
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,	
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);    
                 
    OSStart(&err);                                  /*开始运行uCOS-III操作系统*/
}


/*开始任务*/
static void AppTaskStart(void *p_arg)
{
    OS_ERR err;
    
    (void)&p_arg;
    
    BSP_Init();
    CPU_Init();
    
#if OS_CFG_APP_HOOKS_EN > 0u
    App_OS_SetAllHooks();                           /*设置钩子函数*/
#endif  
    
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);  	                /*统计任务*/    
    OSStatReset(&err);                              /*复位统计数据*/    
#endif
    
    AppTaskCreate();                                /*创建任务*/
    
    while(DEF_TRUE)
    {
        BSP_LED_Toggle(1);
        OSTimeDlyHMSM(0,0,1,0,
                      OS_OPT_TIME_HMSM_NON_STRICT,
                      &err);
    }
}


/*示例任务*/
void timer_sample (void);
void mutex_sample (void);
void sem_sample   (void);
void q_sample     (void);
void task_sem_sample (void);


static void AppTaskCreate(void)
{
//    timer_sample();
//    mutex_sample();
//    sem_sample();
//    q_sample();
//    task_sem_sample();
}
