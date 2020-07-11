/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-02     Meco Man     the first verion
 */
 
/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                  DEBUGGER CONSTANTS
*
* File    : OS_DBG.C
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

#ifndef RT_USING_IDLE_HOOK
#error "μCOS-III兼容层必须开启要求RT_USING_IDLE_HOOK宏定义"
#endif

#ifndef RT_USING_TIMER_SOFT
#warning "RT_USING_TIMER_SOFT宏没有开启,μCOS-III兼容层将无法启用软件定时器,如果您确认这么做的话请将本警告注释掉"
#endif

#if OS_CFG_TASK_Q_EN && !OS_CFG_Q_EN
#error "任务内建消息队列需要消息队列的支持，需要将OS_CFG_Q_EN置1方可使用"
#endif

#if OS_CFG_TASK_Q_PEND_ABORT_EN && !OS_CFG_Q_PEND_ABORT_EN
#error "请将OS_CFG_Q_PEND_ABORT_EN置1"
#endif

#if OS_CFG_TASK_SEM_PEND_ABORT_EN && !OS_CFG_SEM_PEND_ABORT_EN
#error "请将OS_CFG_SEM_PEND_ABORT_EN置1"
#endif
