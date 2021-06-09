/*
 * Copyright (c) 2021, Meco Jianting Man <jiantingman@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-25     Meco Man     the first verion
 */
/*
*********************************************************************************************************
*                                              uC/OS-III
*                                        The Real-Time Kernel
*
*                    Copyright 2009-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
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
* File    : OS_CFG_APP.C
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
* Note(s) : DO NOT CHANGE THIS FILE!
************************************************************************************************************************
*/

#include "os_cfg_app.h"
#include "os.h"
#include <rtconfig.h>


/*
************************************************************************************************************************
*                                                    DATA STORAGE
************************************************************************************************************************
*/
#if (OS_CFG_STAT_TASK_EN > 0u)
CPU_STK        OSCfg_StatTaskStk   [OS_CFG_STAT_TASK_STK_SIZE];
#endif

#ifndef PKG_USING_UCOSIII_WRAPPER_TINY
#if OS_CFG_DBG_EN > 0u
/*
************************************************************************************************************************
*                                                      CONSTANTS
************************************************************************************************************************
*/

#if (OS_CFG_STAT_TASK_EN > 0u)
OS_PRIO        const  OSCfg_StatTaskPrio         = (OS_PRIO     )OS_CFG_STAT_TASK_PRIO;
OS_RATE_HZ     const  OSCfg_StatTaskRate_Hz      = (OS_RATE_HZ  )OS_CFG_STAT_TASK_RATE_HZ;
CPU_STK      * const  OSCfg_StatTaskStkBasePtr   = (CPU_STK    *)&OSCfg_StatTaskStk[0];
CPU_STK_SIZE   const  OSCfg_StatTaskStkLimit     = (CPU_STK_SIZE)OS_CFG_STAT_TASK_STK_LIMIT;
CPU_STK_SIZE   const  OSCfg_StatTaskStkSize      = (CPU_STK_SIZE)OS_CFG_STAT_TASK_STK_SIZE;
CPU_INT32U     const  OSCfg_StatTaskStkSizeRAM   = (CPU_INT32U  )sizeof(OSCfg_StatTaskStk);
#else
OS_PRIO        const  OSCfg_StatTaskPrio         = (OS_PRIO     )0;
OS_RATE_HZ     const  OSCfg_StatTaskRate_Hz      = (OS_RATE_HZ  )0;
CPU_STK      * const  OSCfg_StatTaskStkBasePtr   = (CPU_STK    *)0;
CPU_STK_SIZE   const  OSCfg_StatTaskStkLimit     = (CPU_STK_SIZE)0;
CPU_STK_SIZE   const  OSCfg_StatTaskStkSize      = (CPU_STK_SIZE)0;
CPU_INT32U     const  OSCfg_StatTaskStkSizeRAM   = (CPU_INT32U  )0;
#endif


CPU_STK_SIZE   const  OSCfg_StkSizeMin           = (CPU_STK_SIZE)OS_CFG_STK_SIZE_MIN;


OS_RATE_HZ     const  OSCfg_TickRate_Hz          = (OS_RATE_HZ  )OS_CFG_TICK_RATE_HZ;


#if (OS_CFG_TMR_EN > 0u)
OS_RATE_HZ     const  OSCfg_TmrTaskRate_Hz       = (OS_RATE_HZ  )OS_CFG_TMR_TASK_RATE_HZ;
#else
OS_RATE_HZ     const  OSCfg_TmrTaskRate_Hz       = (OS_RATE_HZ  )0;
#endif


/*$PAGE*/
/*
************************************************************************************************************************
*                                         TOTAL SIZE OF APPLICATION CONFIGURATION
************************************************************************************************************************
*/

CPU_INT32U     const  OSCfg_DataSizeRAM          =
#if (OS_CFG_STAT_TASK_EN > 0u)
                                                 + sizeof(OSCfg_StatTaskStk)
#else
                                                 + 0
#endif
                                                 ;

/*
************************************************************************************************************************
*                                             OS CONFIGURATION INITIALIZATION
*
* Description: This function is used to make sure that debug variables that are unused in the application are not
*              optimized away.  This function might not be necessary for all compilers.  In this case, you should simply
*              DELETE the code in this function while still leaving the declaration of the function itself.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : (1) This code doesn't do anything, it simply prevents the compiler from optimizing out the 'const'
*                  variables which are declared in this file.
*              (2) You may decide to 'compile out' the code (by using #if 0/#endif) INSIDE the function if your compiler
*                  DOES NOT optimize out the 'const' variables above.
************************************************************************************************************************
*/

void  OSCfg_Init (void)
{
    void const *p_temp;


    p_temp = (void const *)&OSCfg_DataSizeRAM;

#if (OS_CFG_STAT_TASK_EN > 0u)
    p_temp = (void const *)&OSCfg_StatTaskPrio;
    p_temp = (void const *)&OSCfg_StatTaskRate_Hz;
    p_temp = (void const *)&OSCfg_StatTaskStkBasePtr;
    p_temp = (void const *)&OSCfg_StatTaskStkLimit;
    p_temp = (void const *)&OSCfg_StatTaskStkSize;
    p_temp = (void const *)&OSCfg_StatTaskStkSizeRAM;
#endif

    p_temp = (void const *)&OSCfg_StkSizeMin;

#if (OS_CFG_TMR_EN > 0u)
    p_temp = (void const *)&OSCfg_TmrTaskRate_Hz;
#endif
    p_temp = p_temp;
}

#endif

#endif /*PKG_USING_UCOSIII_WRAPPER_TINY*/
