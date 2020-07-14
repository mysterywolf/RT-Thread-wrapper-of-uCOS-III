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

#include <os.h>
#include <os_app_hooks.h>

/*finsh使用串口2*/

void timer_sample (void);

int main(void)
{
    OS_ERR err;
    
    OSInit(&err);                                   /*uCOS-III操作系统初始化*/
    
    OSStart(&err);                                  /*开始运行uCOS-III操作系统*/
    
#if OS_CFG_APP_HOOKS_EN > 0u
    App_OS_SetAllHooks();                           /*设置钩子函数*/
#endif  
    
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);  	                /*统计任务*/    
    OSStatReset(&err);                              /*复位统计数据*/    
#endif

    timer_sample();
}
