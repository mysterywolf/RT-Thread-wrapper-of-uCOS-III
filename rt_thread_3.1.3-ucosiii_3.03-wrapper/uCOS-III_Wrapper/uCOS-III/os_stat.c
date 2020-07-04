/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-30     Meco Man     the first verion
 */
 
/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                  STATISTICS MODULE
*
* File    : OS_STAT.C
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

#include <os.h>

/*
由于RTT没有统计任务，本文件所有函数不予实现
*/

#if OS_CFG_STAT_TASK_EN > 0u
/*
************************************************************************************************************************
*                                                   RESET STATISTICS
*
* Description: This function is called by your application to reset the statistics.
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE
*
* Returns    : none
************************************************************************************************************************
*/

void  OSStatReset (OS_ERR  *p_err)
{
}

/*
************************************************************************************************************************
*                                                DETERMINE THE CPU CAPACITY
*
* Description: This function is called by your application to establish CPU usage by first determining how high a 32-bit
*              counter would count to in 1/10 second if no other tasks were to execute during that time.  CPU usage is
*              then determined by a low priority task which keeps track of this 32-bit counter every second but this
*              time, with other tasks running.  CPU usage is determined by:
*
*                                             OS_Stat_IdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------------)
*                                            OS_Stat_IdleCtrMax
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE
*
* Returns    : none
************************************************************************************************************************
*/

void  OSStatTaskCPUUsageInit (OS_ERR  *p_err)
{
}

#endif
