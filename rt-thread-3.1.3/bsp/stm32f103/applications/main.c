/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-14     Meco Man     implement uCOS-III Wrapper
 */

/*本文件展示了兼容层实现的uCOS-III官方给出的标准初始化流程*/
/*启动以及初始化过程严格遵照官方给出的例程*/
/*串口使用USART2*/

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


int main(void)/*RT-Thread main线程*/
{
    OS_ERR err;
    
    OSInit(&err);                                   /*uCOS-III操作系统初始化*/
    if(err != OS_ERR_NONE){
        rt_kprintf("uCOS-III init error!\r\n");
        return 0;
    }
    
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
void timer_example (void);
void mutex_example (void);
void sem_example   (void);
void q_example     (void);
void task_sem_example (void);
void suspend_example (void);
void flag_example(void);

static void AppTaskCreate(void)
{
    /*可以打开注释,运行测试例程*/  
//    timer_example();
//    mutex_example();
//    sem_example();
//    q_example();
//    task_sem_example();
//    suspend_example();
//    flag_example();
}
