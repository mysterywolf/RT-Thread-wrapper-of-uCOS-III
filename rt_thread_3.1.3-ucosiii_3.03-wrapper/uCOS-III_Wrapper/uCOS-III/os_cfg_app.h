/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-04     Meco Man     the first verion
 */
 
/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                       OS CONFIGURATION (APPLICATION SPECIFICS)
*
* File    : OS_CFG_APP.H
* By      : JJL
* Version : V3.03.00
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your
*           application/product.   We provide ALL the source code for your convenience and to help you
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#ifndef OS_CFG_APP_H
#define OS_CFG_APP_H

#include <rtconfig.h>

/*
************************************************************************************************************************
*                                                      CONSTANTS
************************************************************************************************************************
*/

                                                            /* ------------------- STATISTIC TASK ------------------- */
#define  OS_CFG_STAT_TASK_PRIO  	(OS_CFG_PRIO_MAX-2u)   	/* Priority 统计任务优先级                                */
#define  OS_CFG_STAT_TASK_RATE_HZ         10u               /* Rate of execution (1 to 10 Hz)                         */
#define  OS_CFG_STAT_TASK_STK_SIZE       128u               /* Stack size (number of CPU_STK elements)                */

                                                            /* ----------------------- TIMERS ----------------------- */
#define  OS_CFG_TMR_TASK_PRIO        RT_TIMER_THREAD_PRIO   /* 只读 Priority of 'Timer Task' 定时任务优先级           */
#define  OS_CFG_TMR_TASK_RATE_HZ         100u               /* 参数要和原版工程一致,用于与RTT定时器兼容转换Rate for timers (100 Hz Typ.)*/
#define  OS_CFG_TMR_TASK_STK_SIZE \
    RT_TIMER_THREAD_STACK_SIZE/sizeof(CPU_STK)              /* 只读 Stack size (number of CPU_STK elements)           */


                                                            /* ------------------------ TICKS ----------------------- */
#define  OS_CFG_TICK_RATE_HZ         RT_TICK_PER_SECOND     /* 只读 Tick rate in Hertz (10 to 1000 Hz)                */


#endif
