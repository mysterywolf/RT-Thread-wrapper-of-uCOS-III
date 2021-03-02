/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-29     Meco Man     the first verion
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
*                                                  CONFIGURATION FILE
*
* File    : OS_CFG.H
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

#ifndef OS_CFG_H
#define OS_CFG_H

#include <rtconfig.h>

/*
************************************************************************************************************************
*                                                      CONSTANTS
************************************************************************************************************************
*/

                                                            /* ---------------------------- MISCELLANEOUS -------------------------- */
#if defined RT_USING_HOOK && defined RT_USING_IDLE_HOOK
#define  OS_CFG_APP_HOOKS_EN             1u                 /* 读写 Enable (1) or Disable (0) application specific hooks             */
#else
#define  OS_CFG_APP_HOOKS_EN             0u                 /* 只读 Enable (1) or Disable (0) application specific hooks             */
#endif
#define  OS_CFG_ARG_CHK_EN               1u                 /* Enable (1) or Disable (0) argument checking                           */
#define  OS_CFG_CALLED_FROM_ISR_CHK_EN   1u                 /* Enable (1) or Disable (0) check for called from ISR                   */
#define  OS_CFG_DBG_EN                   1u                 /* Enable (1) debug code/variables                                       */
#define  OS_CFG_INVALID_OS_CALLS_CHK_EN  1u                 /* Enable (1) or Disable (0) checks for invalid kernel calls             */
#define  OS_CFG_OBJ_TYPE_CHK_EN          1u                 /* Enable (1) or Disable (0) object type checking                        */
#define  OS_CFG_PEND_MULTI_EN            0u                 /* 只读,Enable (1) or Disable (0) code generation for multi-pend feature */
#define  OS_CFG_PRIO_MAX        RT_THREAD_PRIORITY_MAX      /* 只读 Defines the maximum number of task priorities                    */
#define  OS_CFG_SCHED_ROUND_ROBIN_EN     1u                 /* 只读,RTT时间片轮转为必选项 Include code for Round-Robin scheduling    */
#define  OS_CFG_STK_SIZE_MIN            64u                 /* Minimum allowable task stack size                                     */


                                                            /* ----------------------------- EVENT FLAGS --------------------------- */
#ifdef RT_USING_EVENT                                       /* 是否开启由RT-Thread接管                                               */
#define  OS_CFG_FLAG_EN                  1u                 /* 读写 Enable (1) or Disable (0) code generation for EVENT FLAGS        */
#else
#define  OS_CFG_FLAG_EN                  0u                 /* 只读 Enable (1) or Disable (0) code generation for EVENT FLAGS        */
#endif
#define  OS_CFG_FLAG_DEL_EN              1u                 /* Include code for OSFlagDel()                                          */
#define  OS_CFG_FLAG_MODE_CLR_EN         1u                 /* Include code for Wait on Clear EVENT FLAGS                            */
#define  OS_CFG_FLAG_PEND_ABORT_EN       1u                 /* Include code for OSFlagPendAbort()                                    */


                                                            /* -------------------------- MEMORY MANAGEMENT ------------------------ */
#define  OS_CFG_MEM_EN                   1u                 /* Enable (1) or Disable (0) code generation for MEMORY MANAGER          */


                                                            /* --------------------- MUTUAL EXCLUSION SEMAPHORES ------------------- */
#ifdef RT_USING_MUTEX                                       /* 是否开启由RT-Thread接管                                               */
#define  OS_CFG_MUTEX_EN                 1u                 /* 读写 Enable (1) or Disable (0) code generation for MUTE               */
#else
#define  OS_CFG_MUTEX_EN                 0u                 /* 只读 Enable (1) or Disable (0) code generation for MUTE               */
#endif
#define  OS_CFG_MUTEX_DEL_EN             1u                 /* Include code for OSMutexDel()                                         */
#define  OS_CFG_MUTEX_PEND_ABORT_EN      1u                 /* Include code for OSMutexPendAbort()                                   */


                                                            /* --------------------------- MESSAGE QUEUES -------------------------- */
#ifdef RT_USING_MESSAGEQUEUE                                /* 是否开启由RT-Thread接管                                               */
#define  OS_CFG_Q_EN                     1u                 /* 读写 Enable (1) or Disable (0) code generation for QUEUES             */
#else
#define  OS_CFG_Q_EN                     0u                 /* 只读 Enable (1) or Disable (0) code generation for QUEUES             */
#endif
#define  OS_CFG_Q_DEL_EN                 1u                 /* Include code for OSQDel()                                             */
#define  OS_CFG_Q_FLUSH_EN               1u                 /* Include code for OSQFlush()                                           */
#define  OS_CFG_Q_PEND_ABORT_EN          1u                 /* Include code for OSQPendAbort()                                       */


                                                            /* ----------------------------- SEMAPHORES ---------------------------- */
#ifdef RT_USING_SEMAPHORE                                   /* 是否开启由RT-Thread接管                                               */
#define  OS_CFG_SEM_EN                   1u                 /* 读写 Enable (1) or Disable (0) code generation for SEMAPHORES         */
#else
#define  OS_CFG_SEM_EN                   0u                 /* 只读 Enable (1) or Disable (0) code generation for SEMAPHORES         */
#endif
#define  OS_CFG_SEM_DEL_EN               1u                 /* Include code for OSSemDel()                                           */
#define  OS_CFG_SEM_PEND_ABORT_EN        1u                 /* nclude code for OSSemPendAbort()                                      */
#define  OS_CFG_SEM_SET_EN               1u                 /* Include code for OSSemSet()                                           */


                                                            /* -------------------------- TASK MANAGEMENT -------------------------- */
#define  OS_CFG_STAT_TASK_EN             1u                 /* Enable (1) or Disable(0) the statistics task                          */
#define  OS_CFG_STAT_TASK_STK_CHK_EN     1u                 /* Check task stacks from statistic task                                 */

#define  OS_CFG_TASK_CHANGE_PRIO_EN      1u                 /* Include code for OSTaskChangePrio()                                   */
#define  OS_CFG_TASK_DEL_EN              1u                 /* Include code for OSTaskDel()                                          */
#define  OS_CFG_TASK_SEM_EN              1u                 /* Include code for OSTaskSemXXXX()                                      */
#define  OS_CFG_TASK_Q_EN                1u                 /* Include code for OSTaskQXXXX()                                        */
#define  OS_CFG_TASK_Q_PEND_ABORT_EN     1u                 /* Include code for OSTaskQPendAbort()                                   */
#define  OS_CFG_TASK_PROFILE_EN          0u                 /* 只读 Include variables in OS_TCB for profiling                        */
#define  OS_CFG_TASK_REG_TBL_SIZE        1u                 /* Number of task specific registers                                     */
#define  OS_CFG_TASK_SEM_PEND_ABORT_EN   1u                 /* Include code for OSTaskSemPendAbort()                                 */
#define  OS_CFG_TASK_SUSPEND_EN          1u                 /* Include code for OSTaskSuspend() and OSTaskResume()                   */


                                                            /* -------------------------- TIME MANAGEMENT -------------------------- */
#define  OS_CFG_TIME_DLY_HMSM_EN         1u                 /* Include code for OSTimeDlyHMSM()                                      */
#define  OS_CFG_TIME_DLY_RESUME_EN       1u                 /* Include code for OSTimeDlyResume()                                    */


                                                            /* ------------------- TASK LOCAL STORAGE MANAGEMENT ------------------- */
#define  OS_CFG_TLS_TBL_SIZE             0u                 /* 只读,该功能尚未实现 Include code for Task Local Storage (TLS) registers*/


                                                            /* ------------------------- TIMER MANAGEMENT -------------------------- */
#ifdef RT_USING_TIMER_SOFT                                  /* 是否开启由RT-Thread接管                                               */
#define  OS_CFG_TMR_EN                   1u                 /* 读写 Enable (1) or Disable (0) code generation for TIMERS             */
#else
#define  OS_CFG_TMR_EN                   0u                 /* 只读 Enable (1) or Disable (0) code generation for TIMERS             */
#endif
#define  OS_CFG_TMR_DEL_EN               1u                 /* Enable (1) or Disable (0) code generation for OSTmrDel()              */


#ifdef   OS_SAFETY_CRITICAL
#define  OS_SAFETY_CRITICAL_EXCEPTION()   {}while(0)
#endif

#endif
