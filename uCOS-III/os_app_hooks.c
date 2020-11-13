/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-06     Meco Man     the first verion
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
*                                                  APPLICATION HOOKS
*
* File    : OS_APP_HOOKS.C
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
#include <os_app_hooks.h>

/*
************************************************************************************************************************
*                                              SET ALL APPLICATION HOOKS
*
* Description: Set ALL application hooks.
*
* Arguments  : none.
*
* Note(s)    : none
************************************************************************************************************************
*/

void  App_OS_SetAllHooks (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();
    OS_AppTaskCreateHookPtr = App_OS_TaskCreateHook;
    OS_AppTaskDelHookPtr    = App_OS_TaskDelHook;
#if OS_CFG_STAT_TASK_EN > 0u
    OS_AppIdleTaskHookPtr   = App_OS_IdleTaskHook;
    OS_AppStatTaskHookPtr   = App_OS_StatTaskHook;
#endif
    CPU_CRITICAL_EXIT();
#endif
}

/*
************************************************************************************************************************
*                                             CLEAR ALL APPLICATION HOOKS
*
* Description: Clear ALL application hooks.
*
* Arguments  : none.
*
* Note(s)    : none
************************************************************************************************************************
*/

void  App_OS_ClrAllHooks (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    CPU_SR_ALLOC();

    CPU_CRITICAL_ENTER();
    OS_AppTaskCreateHookPtr = (OS_APP_HOOK_TCB)0;
    OS_AppTaskDelHookPtr    = (OS_APP_HOOK_TCB)0;
#if OS_CFG_STAT_TASK_EN > 0u
    OS_AppIdleTaskHookPtr   = (OS_APP_HOOK_VOID)0; 
    OS_AppStatTaskHookPtr   = (OS_APP_HOOK_VOID)0;
#endif
    CPU_CRITICAL_EXIT();
#endif
}

/*
************************************************************************************************************************
*                                            APPLICATION TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : p_tcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : none
************************************************************************************************************************
*/

void  App_OS_TaskCreateHook (OS_TCB  *p_tcb)
{
    CPU_VAL_UNUSED(p_tcb);

}

/*
************************************************************************************************************************
*                                            APPLICATION TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : p_tcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : none
************************************************************************************************************************
*/

void  App_OS_TaskDelHook (OS_TCB  *p_tcb)
{
    CPU_VAL_UNUSED(p_tcb);

}

/*
************************************************************************************************************************
*                                              APPLICATION IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do such things as
*              STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  App_OS_IdleTaskHook (void)
{

}

/*
************************************************************************************************************************
*                                           APPLICATION STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-III's statistics task.  This allows your application to add
*              functionality to the statistics task.
*
* Arguments  : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  App_OS_StatTaskHook (void)
{

}
