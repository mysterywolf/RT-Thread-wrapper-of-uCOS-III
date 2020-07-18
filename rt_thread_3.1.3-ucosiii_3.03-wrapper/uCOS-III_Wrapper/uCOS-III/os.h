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
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
* File    : OS.H
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
* Note(s) : (1) Assumes the following versions (or more recent) of software modules are included in the project build:
*
*               (a) uC/LIB V1.36.01
*               (b) uC/CPU V1.29.00
************************************************************************************************************************
*/
#ifndef   OS_H
#define   OS_H

/*
************************************************************************************************************************
*                                               uC/OS-III VERSION NUMBER
************************************************************************************************************************
*/

#define  OS_VERSION  30300u                       /* Version of uC/OS-III (Vx.yy.zz mult. by 10000)                   */

/*
************************************************************************************************************************
*                                                 INCLUDE HEADER FILES
************************************************************************************************************************
*/
#include <rtthread.h>
#include <cpu.h>
#include <cpu_core.h>
#include <os_type.h>
#include <os_cfg.h>
#include <os_cfg_app.h>
#include <lib_def.h>

/*
************************************************************************************************************************
*                                               CRITICAL SECTION HANDLING
************************************************************************************************************************
*/
#define  OS_CRITICAL_ENTER()                    CPU_CRITICAL_ENTER()

#define  OS_CRITICAL_ENTER_CPU_CRITICAL_EXIT()

#define  OS_CRITICAL_EXIT()                     CPU_CRITICAL_EXIT()

#define  OS_CRITICAL_EXIT_NO_SCHED()            CPU_CRITICAL_EXIT()

/*
************************************************************************************************************************
*                                                     MISCELLANEOUS
************************************************************************************************************************
*/

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

#define  OS_MSG_EN                 (((OS_CFG_TASK_Q_EN > 0u) || (OS_CFG_Q_EN > 0u)) ? 1u : 0u)


/*
************************************************************************************************************************
************************************************************************************************************************
*                                                   # D E F I N E S
************************************************************************************************************************
************************************************************************************************************************
*/

/*
========================================================================================================================
*                                                      TASK STATUS
========================================================================================================================
*/

#define  OS_STATE_OS_STOPPED                    (OS_STATE)(0u)
#define  OS_STATE_OS_RUNNING                    (OS_STATE)(1u)

#define  OS_STATE_NOT_RDY                    (CPU_BOOLEAN)(0u)
#define  OS_STATE_RDY                        (CPU_BOOLEAN)(1u)


                                                                /* ------------------- TASK STATES ------------------ */
#define  OS_TASK_STATE_BIT_DLY               (OS_STATE)(0x01u)  /*   /-------- SUSPENDED bit                          */
                                                                /*   |                                                */
#define  OS_TASK_STATE_BIT_PEND              (OS_STATE)(0x02u)  /*   | /-----  PEND      bit                          */
                                                                /*   | |                                              */
#define  OS_TASK_STATE_BIT_SUSPENDED         (OS_STATE)(0x04u)  /*   | | /---  Delayed/Timeout bit                    */
                                                                /*   | | |                                            */
                                                                /*   V V V                                            */

#define  OS_TASK_STATE_RDY                    (OS_STATE)(  0u)  /*   0 0 0     Ready                                  */
#define  OS_TASK_STATE_DLY                    (OS_STATE)(  1u)  /*   0 0 1     Delayed or Timeout                     */
#define  OS_TASK_STATE_PEND                   (OS_STATE)(  2u)  /*   0 1 0     Pend                                   */
#define  OS_TASK_STATE_PEND_TIMEOUT           (OS_STATE)(  3u)  /*   0 1 1     Pend + Timeout                         */
#define  OS_TASK_STATE_SUSPENDED              (OS_STATE)(  4u)  /*   1 0 0     Suspended                              */
#define  OS_TASK_STATE_DLY_SUSPENDED          (OS_STATE)(  5u)  /*   1 0 1     Suspended + Delayed or Timeout         */
#define  OS_TASK_STATE_PEND_SUSPENDED         (OS_STATE)(  6u)  /*   1 1 0     Suspended + Pend                       */
#define  OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED (OS_STATE)(  7u)  /*   1 1 1     Suspended + Pend + Timeout             */
#define  OS_TASK_STATE_DEL                    (OS_STATE)(255u)

                                                                /* ----------------- PENDING ON ... ----------------- */
#define  OS_TASK_PEND_ON_NOTHING              (OS_STATE)(  0u)  /* Pending on nothing                                 */
#define  OS_TASK_PEND_ON_FLAG                 (OS_STATE)(  1u)  /* Pending on event flag group                        */
#define  OS_TASK_PEND_ON_TASK_Q               (OS_STATE)(  2u)  /* Pending on message to be sent to task              */
#define  OS_TASK_PEND_ON_MULTI                (OS_STATE)(  3u)  /* Pending on multiple semaphores and/or queues       */
#define  OS_TASK_PEND_ON_MUTEX                (OS_STATE)(  4u)  /* Pending on mutual exclusion semaphore              */
#define  OS_TASK_PEND_ON_Q                    (OS_STATE)(  5u)  /* Pending on queue                                   */
#define  OS_TASK_PEND_ON_SEM                  (OS_STATE)(  6u)  /* Pending on semaphore                               */
#define  OS_TASK_PEND_ON_TASK_SEM             (OS_STATE)(  7u)  /* Pending on signal  to be sent to task              */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                    TASK PEND STATUS
*                                      (Status codes for OS_TCBs field .PendStatus)
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_STATUS_PEND_OK                   (OS_STATUS)(  0u)  /* Pending status OK, !pending, or pending complete   */
#define  OS_STATUS_PEND_ABORT                (OS_STATUS)(  1u)  /* Pending aborted                                    */
#define  OS_STATUS_PEND_DEL                  (OS_STATUS)(  2u)  /* Pending object deleted                             */
#define  OS_STATUS_PEND_TIMEOUT              (OS_STATUS)(  3u)  /* Pending timed out                                  */

/*
========================================================================================================================
*                                                   OS OBJECT TYPES
*
* Note(s) : (1) OS_OBJ_TYPE_&&& #define values specifically chosen as ASCII representations of the kernel
*               object types.  Memory displays of kernel objects will display the kernel object TYPEs with
*               their chosen ASCII names.
========================================================================================================================
*/

#define  OS_OBJ_TYPE_NONE                    (OS_OBJ_TYPE)CPU_TYPE_CREATE('N', 'O', 'N', 'E')
#define  OS_OBJ_TYPE_FLAG                    (OS_OBJ_TYPE)CPU_TYPE_CREATE('F', 'L', 'A', 'G')
#define  OS_OBJ_TYPE_MEM                     (OS_OBJ_TYPE)CPU_TYPE_CREATE('M', 'E', 'M', ' ')
#define  OS_OBJ_TYPE_MUTEX                   (OS_OBJ_TYPE)CPU_TYPE_CREATE('M', 'U', 'T', 'X')
#define  OS_OBJ_TYPE_Q                       (OS_OBJ_TYPE)CPU_TYPE_CREATE('Q', 'U', 'E', 'U')
#define  OS_OBJ_TYPE_SEM                     (OS_OBJ_TYPE)CPU_TYPE_CREATE('S', 'E', 'M', 'A')
#define  OS_OBJ_TYPE_TMR                     (OS_OBJ_TYPE)CPU_TYPE_CREATE('T', 'M', 'R', ' ')


/*
========================================================================================================================
*                                           Possible values for 'opt' argument
========================================================================================================================
*/

#define  OS_OPT_NONE                         (OS_OPT)(0x0000u)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                    DELETE OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_DEL_NO_PEND                  (OS_OPT)(0x0000u)
#define  OS_OPT_DEL_ALWAYS                   (OS_OPT)(0x0001u)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     PEND OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_PEND_FLAG_MASK               (OS_OPT)(0x000Fu)
#define  OS_OPT_PEND_FLAG_CLR_ALL            (OS_OPT)(0x0001u)  /* Wait for ALL    the bits specified to be CLR       */
#define  OS_OPT_PEND_FLAG_CLR_AND            (OS_OPT)(0x0001u)

#define  OS_OPT_PEND_FLAG_CLR_ANY            (OS_OPT)(0x0002u)  /* Wait for ANY of the bits specified to be CLR       */
#define  OS_OPT_PEND_FLAG_CLR_OR             (OS_OPT)(0x0002u)

#define  OS_OPT_PEND_FLAG_SET_ALL            (OS_OPT)(0x0004u)  /* Wait for ALL    the bits specified to be SET       */
#define  OS_OPT_PEND_FLAG_SET_AND            (OS_OPT)(0x0004u)

#define  OS_OPT_PEND_FLAG_SET_ANY            (OS_OPT)(0x0008u)  /* Wait for ANY of the bits specified to be SET       */
#define  OS_OPT_PEND_FLAG_SET_OR             (OS_OPT)(0x0008u)

#define  OS_OPT_PEND_FLAG_CONSUME            (OS_OPT)(0x0100u)  /* Consume the flags if condition(s) satisfied        */


#define  OS_OPT_PEND_BLOCKING                (OS_OPT)(0x0000u)
#define  OS_OPT_PEND_NON_BLOCKING            (OS_OPT)(0x8000u)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                  PEND ABORT OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_PEND_ABORT_1                 (OS_OPT)(0x0000u)  /* Pend abort a single waiting task                   */
#define  OS_OPT_PEND_ABORT_ALL               (OS_OPT)(0x0100u)  /* Pend abort ALL tasks waiting                       */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     POST OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_POST_NONE                    (OS_OPT)(0x0000u)

#define  OS_OPT_POST_FLAG_SET                (OS_OPT)(0x0000u)
#define  OS_OPT_POST_FLAG_CLR                (OS_OPT)(0x0001u)

#define  OS_OPT_POST_FIFO                    (OS_OPT)(0x0000u)  /* Default is to post FIFO                            */
#define  OS_OPT_POST_LIFO                    (OS_OPT)(0x0010u)  /* Post to highest priority task waiting              */
#define  OS_OPT_POST_1                       (OS_OPT)(0x0000u)  /* Post message to highest priority task waiting      */
#define  OS_OPT_POST_ALL                     (OS_OPT)(0x0200u)  /* Broadcast message to ALL tasks waiting             */

#define  OS_OPT_POST_NO_SCHED                (OS_OPT)(0x8000u)  /* Do not call the scheduler if this is selected      */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     TASK OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_TASK_NONE                    (OS_OPT)(0x0000u)  /* No option selected                                 */
#define  OS_OPT_TASK_STK_CHK                 (OS_OPT)(0x0001u)  /* Enable stack checking for the task                 */
#define  OS_OPT_TASK_STK_CLR                 (OS_OPT)(0x0002u)  /* Clear the stack when the task is create            */
#define  OS_OPT_TASK_SAVE_FP                 (OS_OPT)(0x0004u)  /* Save the contents of any floating-point registers  */
#define  OS_OPT_TASK_NO_TLS                  (OS_OPT)(0x0008u)  /* Specifies the task DOES NOT require TLS support    */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     TIME OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_TIME_DLY                             DEF_BIT_NONE
#define  OS_OPT_TIME_TIMEOUT                ((OS_OPT)DEF_BIT_01)
#define  OS_OPT_TIME_MATCH                  ((OS_OPT)DEF_BIT_02)
#define  OS_OPT_TIME_PERIODIC               ((OS_OPT)DEF_BIT_03)

#define  OS_OPT_TIME_HMSM_STRICT            ((OS_OPT)DEF_BIT_NONE)
#define  OS_OPT_TIME_HMSM_NON_STRICT        ((OS_OPT)DEF_BIT_04)

#define  OS_OPT_TIME_MASK                   ((OS_OPT)(OS_OPT_TIME_DLY      | \
                                                      OS_OPT_TIME_TIMEOUT  | \
                                                      OS_OPT_TIME_PERIODIC | \
                                                      OS_OPT_TIME_MATCH))

#define  OS_OPT_TIME_OPTS_MASK                       (OS_OPT_TIME_DLY            | \
                                                      OS_OPT_TIME_TIMEOUT        | \
                                                      OS_OPT_TIME_PERIODIC       | \
                                                      OS_OPT_TIME_MATCH          | \
                                                      OS_OPT_TIME_HMSM_NON_STRICT)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                    TIMER OPTIONS
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_OPT_TMR_NONE                          (OS_OPT)(0u)  /* No option selected                                 */

#define  OS_OPT_TMR_ONE_SHOT                      (OS_OPT)(1u)  /* Timer will not auto restart when it expires        */
#define  OS_OPT_TMR_PERIODIC                      (OS_OPT)(2u)  /* Timer will     auto restart when it expires        */

#define  OS_OPT_TMR_CALLBACK                      (OS_OPT)(3u)  /* OSTmrStop() option to call 'callback' w/ timer arg */
#define  OS_OPT_TMR_CALLBACK_ARG                  (OS_OPT)(4u)  /* OSTmrStop() option to call 'callback' w/ new   arg */

/*
------------------------------------------------------------------------------------------------------------------------
*                                                     TIMER STATES
------------------------------------------------------------------------------------------------------------------------
*/

#define  OS_TMR_STATE_UNUSED                    (OS_STATE)(0u)
#define  OS_TMR_STATE_STOPPED                   (OS_STATE)(1u)
#define  OS_TMR_STATE_RUNNING                   (OS_STATE)(2u)
#define  OS_TMR_STATE_COMPLETED                 (OS_STATE)(3u)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                       PRIORITY
------------------------------------------------------------------------------------------------------------------------
*/
                                                                    /* Dflt prio to init task TCB                     */
#define  OS_PRIO_INIT                       (OS_PRIO)(OS_CFG_PRIO_MAX)

/*
------------------------------------------------------------------------------------------------------------------------
*                                                 TIMER TICK THRESHOLDS
------------------------------------------------------------------------------------------------------------------------
*/
                                                                    /* Threshold to init previous tick time           */
#define  OS_TICK_TH_INIT                    (OS_TICK)(DEF_BIT       ((sizeof(OS_TICK) * DEF_OCTET_NBR_BITS) - 1u))

                                                                    /* Threshold to check if tick time already ready  */
#define  OS_TICK_TH_RDY                     (OS_TICK)(DEF_BIT_FIELD(((sizeof(OS_TICK) * DEF_OCTET_NBR_BITS) / 2u), \
                                                                    ((sizeof(OS_TICK) * DEF_OCTET_NBR_BITS) / 2u)))

                                                                    
/*
************************************************************************************************************************
************************************************************************************************************************
*                                                E N U M E R A T I O N S
************************************************************************************************************************
************************************************************************************************************************
*/
/*
------------------------------------------------------------------------------------------------------------------------
*                                                      ERROR CODES
*
* Note(s) : 请勿随意打开注释掉的枚举体成员,如果用户使用到了这些注释掉的成员,则会在迁移时编译报错,用以提醒用户这些错误代
*           码在兼容层已经不可用
------------------------------------------------------------------------------------------------------------------------
*/
typedef  enum  os_err {
    OS_ERR_NONE                      =     0u,

    OS_ERR_A                         = 10000u,
//    OS_ERR_ACCEPT_ISR                = 10001u,

    OS_ERR_B                         = 11000u,

    OS_ERR_C                         = 12000u,
    OS_ERR_CREATE_ISR                = 12001u,

    OS_ERR_D                         = 13000u,
    OS_ERR_DEL_ISR                   = 13001u,

    OS_ERR_E                         = 14000u,

    OS_ERR_F                         = 15000u,
    OS_ERR_FATAL_RETURN              = 15001u,

//    OS_ERR_FLAG_GRP_DEPLETED         = 15101u,
//    OS_ERR_FLAG_NOT_RDY              = 15102u,
//    OS_ERR_FLAG_PEND_OPT             = 15103u,
    OS_ERR_FLUSH_ISR                 = 15104u,

    OS_ERR_G                         = 16000u,

    OS_ERR_H                         = 17000u,

    OS_ERR_I                         = 18000u,
    OS_ERR_ILLEGAL_CREATE_RUN_TIME   = 18001u,
//    OS_ERR_INT_Q                     = 18002u,
//    OS_ERR_INT_Q_FULL                = 18003u,
//    OS_ERR_INT_Q_SIZE                = 18004u,
//    OS_ERR_INT_Q_STK_INVALID         = 18005u,
//    OS_ERR_INT_Q_STK_SIZE_INVALID    = 18006u,

    OS_ERR_J                         = 19000u,

    OS_ERR_K                         = 20000u,

    OS_ERR_L                         = 21000u,
//    OS_ERR_LOCK_NESTING_OVF          = 21001u,

    OS_ERR_M                         = 22000u,

    OS_ERR_MEM_CREATE_ISR            = 22201u,
    OS_ERR_MEM_FULL                  = 22202u,
    OS_ERR_MEM_INVALID_P_ADDR        = 22203u,
    OS_ERR_MEM_INVALID_BLKS          = 22204u,
    OS_ERR_MEM_INVALID_PART          = 22205u,
    OS_ERR_MEM_INVALID_P_BLK         = 22206u,
    OS_ERR_MEM_INVALID_P_MEM         = 22207u,
    OS_ERR_MEM_INVALID_P_DATA        = 22208u,
    OS_ERR_MEM_INVALID_SIZE          = 22209u,
    OS_ERR_MEM_NO_FREE_BLKS          = 22210u,

//    OS_ERR_MSG_POOL_EMPTY            = 22301u,
//    OS_ERR_MSG_POOL_NULL_PTR         = 22302u,

    OS_ERR_MUTEX_NOT_OWNER           = 22401u,
//    OS_ERR_MUTEX_OWNER               = 22402u,
//    OS_ERR_MUTEX_NESTING             = 22403u,

    OS_ERR_N                         = 23000u,
    OS_ERR_NAME                      = 23001u,
    OS_ERR_NO_MORE_ID_AVAIL          = 23002u,

    OS_ERR_O                         = 24000u,
    OS_ERR_OBJ_CREATED               = 24001u,
//    OS_ERR_OBJ_DEL                   = 24002u,
    OS_ERR_OBJ_PTR_NULL              = 24003u,
    OS_ERR_OBJ_TYPE                  = 24004u,

    OS_ERR_OPT_INVALID               = 24101u,

    OS_ERR_OS_NOT_RUNNING            = 24201u,
    OS_ERR_OS_RUNNING                = 24202u,

    OS_ERR_P                         = 25000u,
    OS_ERR_PEND_ABORT                = 25001u,
    OS_ERR_PEND_ABORT_ISR            = 25002u,
    OS_ERR_PEND_ABORT_NONE           = 25003u,
    OS_ERR_PEND_ABORT_SELF           = 25004u,
//    OS_ERR_PEND_DEL                  = 25005u,
    OS_ERR_PEND_ISR                  = 25006u,
//    OS_ERR_PEND_LOCKED               = 25007u,
//    OS_ERR_PEND_WOULD_BLOCK          = 25008u,

//    OS_ERR_POST_NULL_PTR             = 25101u,
    OS_ERR_POST_ISR                  = 25102u,

//    OS_ERR_PRIO_EXIST                = 25201u,
//    OS_ERR_PRIO                      = 25202u,
    OS_ERR_PRIO_INVALID              = 25203u,

    OS_ERR_PTR_INVALID               = 25301u,

    OS_ERR_Q                         = 26000u,
//    OS_ERR_Q_FULL                    = 26001u,
//    OS_ERR_Q_EMPTY                   = 26002u,
    OS_ERR_Q_MAX                     = 26003u,
    OS_ERR_Q_SIZE                    = 26004u,

    OS_ERR_R                         = 27000u,
    OS_ERR_REG_ID_INVALID            = 27001u,
//    OS_ERR_ROUND_ROBIN_1             = 27002u,
//    OS_ERR_ROUND_ROBIN_DISABLED      = 27003u,

    OS_ERR_S                         = 28000u,
//    OS_ERR_SCHED_INVALID_TIME_SLICE  = 28001u,
    OS_ERR_SCHED_LOCK_ISR            = 28002u,
    OS_ERR_SCHED_LOCKED              = 28003u,
    OS_ERR_SCHED_NOT_LOCKED          = 28004u,
    OS_ERR_SCHED_UNLOCK_ISR          = 28005u,

//    OS_ERR_SEM_OVF                   = 28101u,
    OS_ERR_SET_ISR                   = 28102u,

    OS_ERR_STAT_RESET_ISR            = 28201u,
    OS_ERR_STAT_PRIO_INVALID         = 28202u,
    OS_ERR_STAT_STK_INVALID          = 28203u,
    OS_ERR_STAT_STK_SIZE_INVALID     = 28204u,
    OS_ERR_STATE_INVALID             = 28205u,
//    OS_ERR_STATUS_INVALID            = 28206u,
    OS_ERR_STK_INVALID               = 28207u,
    OS_ERR_STK_SIZE_INVALID          = 28208u,
//    OS_ERR_STK_LIMIT_INVALID         = 28209u,

    OS_ERR_T                         = 29000u,
//    OS_ERR_TASK_CHANGE_PRIO_ISR      = 29001u,
    OS_ERR_TASK_CREATE_ISR           = 29002u,
//    OS_ERR_TASK_DEL                  = 29003u,
//    OS_ERR_TASK_DEL_IDLE             = 29004u,
//    OS_ERR_TASK_DEL_INVALID          = 29005u,
    OS_ERR_TASK_DEL_ISR              = 29006u,
    OS_ERR_TASK_INVALID              = 29007u,
//    OS_ERR_TASK_NO_MORE_TCB          = 29008u,
    OS_ERR_TASK_NOT_DLY              = 29009u,
    OS_ERR_TASK_NOT_EXIST            = 29010u,
    OS_ERR_TASK_NOT_SUSPENDED        = 29011u,
//    OS_ERR_TASK_OPT                  = 29012u,
    OS_ERR_TASK_RESUME_ISR           = 29013u,
//    OS_ERR_TASK_RESUME_PRIO          = 29014u,
    OS_ERR_TASK_RESUME_SELF          = 29015u,
//    OS_ERR_TASK_RUNNING              = 29016u,
    OS_ERR_TASK_STK_CHK_ISR          = 29017u,
//    OS_ERR_TASK_SUSPENDED            = 29018u,
//    OS_ERR_TASK_SUSPEND_IDLE         = 29019u,
//    OS_ERR_TASK_SUSPEND_INT_HANDLER  = 29020u,
    OS_ERR_TASK_SUSPEND_ISR          = 29021u,
//    OS_ERR_TASK_SUSPEND_PRIO         = 29022u,
    OS_ERR_TASK_WAITING              = 29023u,

    OS_ERR_TCB_INVALID               = 29101u,

    OS_ERR_TLS_ID_INVALID            = 29120u,
    OS_ERR_TLS_ISR                   = 29121u,
    OS_ERR_TLS_NO_MORE_AVAIL         = 29122u,
    OS_ERR_TLS_NOT_EN                = 29123u,
    OS_ERR_TLS_DESTRUCT_ASSIGNED     = 29124u,

//    OS_ERR_TICK_PRIO_INVALID         = 29201u,
//    OS_ERR_TICK_STK_INVALID          = 29202u,
//    OS_ERR_TICK_STK_SIZE_INVALID     = 29203u,
//    OS_ERR_TICK_WHEEL_SIZE           = 29204u,

    OS_ERR_TIME_DLY_ISR              = 29301u,
    OS_ERR_TIME_DLY_RESUME_ISR       = 29302u,
//    OS_ERR_TIME_GET_ISR              = 29303u,
    OS_ERR_TIME_INVALID_HOURS        = 29304u,
    OS_ERR_TIME_INVALID_MINUTES      = 29305u,
    OS_ERR_TIME_INVALID_SECONDS      = 29306u,
    OS_ERR_TIME_INVALID_MILLISECONDS = 29307u,
//    OS_ERR_TIME_NOT_DLY              = 29308u,
//    OS_ERR_TIME_SET_ISR              = 29309u,
    OS_ERR_TIME_ZERO_DLY             = 29310u,

    OS_ERR_TIMEOUT                   = 29401u,

//    OS_ERR_TMR_INACTIVE              = 29501u,
//    OS_ERR_TMR_INVALID_DEST          = 29502u,
    OS_ERR_TMR_INVALID_DLY           = 29503u,
    OS_ERR_TMR_INVALID_PERIOD        = 29504u,
    OS_ERR_TMR_INVALID_STATE         = 29505u,
    OS_ERR_TMR_INVALID               = 29506u,
    OS_ERR_TMR_ISR                   = 29507u,
    OS_ERR_TMR_NO_CALLBACK           = 29508u,
//    OS_ERR_TMR_NON_AVAIL             = 29509u,
//    OS_ERR_TMR_PRIO_INVALID          = 29510u,
//    OS_ERR_TMR_STK_INVALID           = 29511u,
//    OS_ERR_TMR_STK_SIZE_INVALID      = 29512u,
    OS_ERR_TMR_STOPPED               = 29513u,

    OS_ERR_U                         = 30000u,

    OS_ERR_V                         = 31000u,

    OS_ERR_W                         = 32000u,

    OS_ERR_X                         = 33000u,

    OS_ERR_Y                         = 34000u,
    OS_ERR_YIELD_ISR                 = 34001u,

    OS_ERR_Z                         = 35000u,
    
    /*
    由于uCOS-III的错误码分类较细，而RTT的错误码分类较为笼统，
    以下RTT错误码使用uCOS-III的错误码很难准确描述
    因此将针对RTT的错误码重新定义(新增)uCOS-III的错误码
    */    
    OS_ERR_RT                        = 36000u,/* RTT专用错误码集 */
    OS_ERR_RT_ERROR                  = 36001u,/* 普通错误     */
    OS_ERR_RT_EEMPTY                 = 36002u,/* 无资源       */
    OS_ERR_RT_ENOMEM                 = 36003u,/* 无内存       */
    OS_ERR_RT_ENOSYS                 = 36004u,/* 系统不支持   */
    OS_ERR_RT_EBUSY                  = 36005u,/* 系统忙       */
    OS_ERR_RT_EIO                    = 36006u,/* IO 错误      */
    OS_ERR_RT_EINTR                  = 36007u,/* 中断系统调用 */

    /*兼容层新增错误码*/
    OS_ERR_TASK_SEM_CREATE_FALSE     = 37001u,/*任务内建信号量创建失败*/
    OS_ERR_TASK_Q_CREATE_FALSE       = 37002u,/*任务内建消息队列创建失败*/
} OS_ERR;


/*
************************************************************************************************************************
************************************************************************************************************************
*                                                  D A T A   T Y P E S
************************************************************************************************************************
************************************************************************************************************************
*/
typedef  struct  os_mem              OS_MEM;

typedef  struct  os_q                OS_Q;

typedef  struct  os_mutex            OS_MUTEX;

typedef  struct  os_sem              OS_SEM;

typedef  struct  os_flag_grp         OS_FLAG_GRP;

typedef  void                      (*OS_TMR_CALLBACK_PTR)  (void *p_tmr, void *p_arg);
typedef  struct  os_tmr              OS_TMR;

typedef  void                      (*OS_TASK_PTR)          (void *p_arg);
typedef  struct  os_tcb              OS_TCB;

#if OS_CFG_APP_HOOKS_EN > 0u
typedef  void                      (*OS_APP_HOOK_VOID)     (void);
typedef  void                      (*OS_APP_HOOK_TCB)      (OS_TCB *p_tcb);
#endif

/*
************************************************************************************************************************
************************************************************************************************************************
*                                          D A T A   S T R U C T U R E S
************************************************************************************************************************
************************************************************************************************************************
*/

/*
------------------------------------------------------------------------------------------------------------------------
*                                                    MESSAGE QUEUES
*
* Note(s) : RTT的消息队列与uCOS的消息队列实现机理完全不同：
*           ·RTT的消息队列是采用数据拷贝的方式，直接完成数据的传递
*           ·uCOS的消息队列采用传递指针的零拷贝方式
*           虽然RTT的邮箱也采用传递指针的方式，但是没有提供urgent函数用于LIFO发送消息,因此采用RTT的消息队列实现
*           将uCOS传递的数据指针和数据大小作为RTT消息队列的数据段封装到RTT的消息队列中,因此需要构建ucos_msg_t结构体
*           并重新构建os_q结构体
------------------------------------------------------------------------------------------------------------------------
*/

#if OS_CFG_Q_EN > 0u
typedef  struct 
{
    rt_uint8_t *data_ptr;/*uCOS-III消息数据指针*/
    rt_uint32_t data_size;/*uCOS-III消息数据长度*/
}ucos_msg_t;/*uCOS消息段*/

struct os_q
{
    struct  rt_messagequeue Msg;
    OS_OBJ_TYPE  Type;
    void    *p_pool;
    ucos_msg_t ucos_msg;
};
#endif

/*
------------------------------------------------------------------------------------------------------------------------
*                                                      SEMAPHORES
------------------------------------------------------------------------------------------------------------------------
*/

struct  os_sem { 
    struct  rt_semaphore  Sem;
    OS_OBJ_TYPE           Type;
#if OS_CFG_DBG_EN > 0u
    OS_SEM              *DbgPrevPtr;
    OS_SEM              *DbgNextPtr;
#endif    
};
/*
------------------------------------------------------------------------------------------------------------------------
*                                                     EVENT FLAGS
------------------------------------------------------------------------------------------------------------------------
*/

struct  os_flag_grp {
    struct  rt_event     FlagGrp;
    OS_OBJ_TYPE          Type;
};

/*
------------------------------------------------------------------------------------------------------------------------
*                                                  TASK CONTROL BLOCK
------------------------------------------------------------------------------------------------------------------------
*/
struct os_tcb
{
    struct rt_thread Task;          /*任务,要确保该成员位于结构体第一个*/
    OS_SEM           Sem;           /*任务内建信号量*/
    CPU_BOOLEAN      SemCreateSuc;  /*标记任务内建信号量是否创建成功*/
#if OS_CFG_TASK_Q_EN > 0u      
    OS_Q             MsgQ;          /*任务内建消息队列*/
    void            *MsgPtr;        /*任务内建消息队列消息指针*/
    OS_MSG_SIZE      MsgSize;       /*任务内建消息队列消息大小*/
    CPU_BOOLEAN      MsgCreateSuc;  /*标记任务内建消息队列是否创建成功*/
#endif    
    void            *ExtPtr;        /*指向用户附加区指针*/

#if OS_CFG_TASK_REG_TBL_SIZE > 0u       
    OS_REG           RegTbl[OS_CFG_TASK_REG_TBL_SIZE];/*任务寄存器*/
#endif    

    OS_STATUS        PendStatus;    /* Pend status：OS_STATUS_PEND_ABORT OS_STATUS_PEND_OK可用*/ 
#if OS_CFG_TASK_SUSPEND_EN > 0u
    OS_NESTING_CTR   SuspendCtr;    /* Nesting counter for OSTaskSuspend() */
#endif
    OS_TICK          TimeQuanta;
    OS_TICK          TimeQuantaCtr;
#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
    CPU_STK_SIZE     StkUsed;       /* Number of stack elements used from the stack */
    CPU_STK_SIZE     StkFree;       /* Number of stack elements free on   the stack */
#endif
#if OS_CFG_DBG_EN > 0u
    OS_TCB          *DbgPrevPtr;
    OS_TCB          *DbgNextPtr;  
    CPU_CHAR        *DbgNamePtr;
#endif
    /*---------兼容层非必须成员变量---------*/
    CPU_STK        **StkPtr;        /* 比原版多了一级指针,堆栈指针的指针,引用需要(*xxx.StkPtr) */
    OS_SEM_CTR      *SemCtr;        /* 比原版多了一级指针,Task specific semaphore counter,引用需要(*xxx.SemCtr)*/
    OS_TICK         *TickCtrMatch;  /* 比原版多了一级指针,Absolute time when task is going to be ready */  
    OS_TICK          TickCtrPrev;   /* Previous time when task was */    
    OS_OPT           Opt;           /* Task options as passed by OSTaskCreate() */    
    OS_STATE         TaskState;     /* (未完成)See OS_TASK_STATE_xxx */
    OS_STATE         PendOn;        /* (未完成)Indicates what task is pending on */
    CPU_STK          StkSize;       /* 任务堆栈大小*/    
    CPU_STK         *StkLimitPtr;   /* Pointer used to set stack 'watermark' limit */
    CPU_STK         *StkBasePtr;    /* Pointer to base address of stack */
    CPU_CHAR        *NamePtr;       /* Pointer to task name */    
    OS_TASK_PTR      TaskEntryAddr; /* Pointer to task entry point address */
    void            *TaskEntryArg;  /* Argument passed to task when it was created */
    OS_PRIO          Prio;          /* Task priority (0 == highest) */          
};

/*
------------------------------------------------------------------------------------------------------------------------
*                                                   MEMORY PARTITIONS
------------------------------------------------------------------------------------------------------------------------
*/

struct os_mem {                                             /* MEMORY CONTROL BLOCK                                   */
    OS_OBJ_TYPE          Type;                              /* Should be set to OS_OBJ_TYPE_MEM                       */
    void                *AddrPtr;                           /* Pointer to beginning of memory partition               */
    CPU_CHAR            *NamePtr;
    void                *FreeListPtr;                       /* Pointer to list of free memory blocks                  */
    OS_MEM_SIZE          BlkSize;                           /* Size (in bytes) of each block of memory                */
    OS_MEM_QTY           NbrMax;                            /* Total number of blocks in this partition               */
    OS_MEM_QTY           NbrFree;                           /* Number of memory blocks remaining in this partition    */
#if OS_CFG_DBG_EN > 0u
    OS_MEM              *DbgPrevPtr;
    OS_MEM              *DbgNextPtr;
#endif
};

/*
------------------------------------------------------------------------------------------------------------------------
*                                              MUTUAL EXCLUSION SEMAPHORES
------------------------------------------------------------------------------------------------------------------------
*/

struct  os_mutex {
    struct rt_mutex     Mutex;
    OS_OBJ_TYPE         Type;
};

/*
------------------------------------------------------------------------------------------------------------------------
*                                                   TIMER DATA TYPES
------------------------------------------------------------------------------------------------------------------------
*/

struct  os_tmr {
    struct  rt_timer     Tmr;
    OS_OBJ_TYPE          Type;
    OS_TMR_CALLBACK_PTR  CallbackPtr;                       /* Function to call when timer expires                    */
    void                *CallbackPtrArg;                    /* Argument to pass to function when timer expires        */
    OS_STATE             State;
    OS_OPT               Opt;                               /* Options (see OS_OPT_TMR_xxx)                           */
    OS_TICK              Dly;                               /* Delay before start of repeat                           */
    OS_TICK              Period;                            /* Period to repeat timer                                 */
#if OS_CFG_DBG_EN > 0u
    OS_TMR              *DbgPrevPtr;
    OS_TMR              *DbgNextPtr;
#endif
};


/*
************************************************************************************************************************
************************************************************************************************************************
*                                           G L O B A L   V A R I A B L E S
************************************************************************************************************************
************************************************************************************************************************
*/
extern           rt_uint8_t                 rt_current_priority;
extern           rt_list_t                  rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

#define          OSSchedLockNestingCtr      rt_critical_level()         /* Lock nesting level                         */
#define          OSIntNestingCtr            rt_interrupt_get_nest()     /* Interrupt nesting level                    */
#define          OSTCBCurPtr                ((OS_TCB*)rt_thread_self()) /* Pointer to currently running TCB           */
                                                                        /* PRIORITIES ------------------------------- */
#define          OSPrioCur                  rt_current_priority         /* Priority of current task                   */
#define          OSPrioTbl                  rt_thread_priority_table

#if OS_CFG_APP_HOOKS_EN > 0u
OS_EXT           OS_APP_HOOK_TCB            OS_AppTaskCreateHookPtr;    /* Application hooks                          */
OS_EXT           OS_APP_HOOK_TCB            OS_AppTaskDelHookPtr;
OS_EXT           OS_APP_HOOK_VOID           OS_AppIdleTaskHookPtr;
OS_EXT           OS_APP_HOOK_VOID           OS_AppStatTaskHookPtr;
#endif

OS_EXT            OS_STATE                  OSRunning;                  /* Flag indicating that kernel is running     */

#ifdef OS_SAFETY_CRITICAL_IEC61508
OS_EXT            CPU_BOOLEAN               OSSafetyCriticalStartFlag;  /* Flag indicating that all init. done        */
#endif

                                                                        /* SEMAPHORES ------------------------------- */
#if OS_CFG_SEM_EN > 0u
#if OS_CFG_DBG_EN > 0u
OS_EXT            OS_SEM                   *OSSemDbgListPtr;
#endif
OS_EXT            OS_OBJ_QTY                OSSemQty;                   /* Number of semaphores created               */
#endif

                                                                        /* QUEUES ----------------------------------- */
#if OS_CFG_Q_EN   > 0u
#if OS_CFG_DBG_EN > 0u
OS_EXT            OS_Q                     *OSQDbgListPtr;
#endif
OS_EXT            OS_OBJ_QTY                OSQQty;                     /* Number of message queues created           */
#endif

                                                                        /* MUTEX MANAGEMENT ------------------------- */
#if OS_CFG_MUTEX_EN > 0u
#if OS_CFG_DBG_EN   > 0u
OS_EXT            OS_MUTEX                 *OSMutexDbgListPtr;
#endif
OS_EXT            OS_OBJ_QTY                OSMutexQty;                 /* Number of mutexes created                  */
#endif

                                                                        /* FLAGS ------------------------------------ */
#if OS_CFG_FLAG_EN > 0u
#if OS_CFG_DBG_EN  > 0u
OS_EXT            OS_FLAG_GRP              *OSFlagDbgListPtr;
#endif
OS_EXT            OS_OBJ_QTY                OSFlagQty;
#endif

                                                                        /* MEMORY MANAGEMENT ------------------------ */
#if OS_CFG_MEM_EN > 0u
#if OS_CFG_DBG_EN > 0u
OS_EXT            OS_MEM                   *OSMemDbgListPtr;
#endif
OS_EXT            OS_OBJ_QTY                OSMemQty;                   /* Number of memory partitions created        */
#endif

                                                                        /* TASKS ------------------------------------ */
#if OS_CFG_DBG_EN > 0u
OS_EXT            OS_TCB                   *OSTaskDbgListPtr;
#endif
OS_EXT            OS_OBJ_QTY                OSTaskQty;                  /* Number of tasks created                    */
#if OS_CFG_TASK_REG_TBL_SIZE > 0u
OS_EXT            OS_REG_ID                 OSTaskRegNextAvailID;       /* Next available Task Register ID            */
#endif
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
OS_EXT            OS_TICK                   OSSchedRoundRobinDfltTimeQuanta;
OS_EXT            CPU_BOOLEAN               OSSchedRoundRobinEn;        /* Enable/Disable round-robin scheduling      */
#endif
                                                                        /* IDLE TASK -------------------------------- */
OS_EXT            OS_IDLE_CTR               OSIdleTaskCtr;              

#if OS_CFG_STAT_TASK_EN > 0u                                            /* STATISTICS ------------------------------- */
OS_EXT            CPU_BOOLEAN               OSStatResetFlag;            /* Force the reset of the computed statistics */
OS_EXT            OS_CPU_USAGE              OSStatTaskCPUUsage;         /* CPU Usage in %                             */
OS_EXT            OS_CPU_USAGE              OSStatTaskCPUUsageMax;      /* CPU Usage in % (Peak)                      */
OS_EXT            OS_TICK                   OSStatTaskCtr;
OS_EXT            OS_TICK                   OSStatTaskCtrMax;
OS_EXT            OS_TICK                   OSStatTaskCtrRun;
OS_EXT            CPU_BOOLEAN               OSStatTaskRdy;
OS_EXT            OS_TCB                    OSStatTaskTCB;
#endif

#if OS_CFG_TMR_EN > 0u                                                  /* TIMERS ----------------------------------- */
#if OS_CFG_DBG_EN > 0u
OS_EXT            OS_TMR                   *OSTmrDbgListPtr;
#endif
OS_EXT            OS_OBJ_QTY                OSTmrQty;                   /* Number of timers created                   */
#endif


/*
************************************************************************************************************************
************************************************************************************************************************
*                                        F U N C T I O N   P R O T O T Y P E S
************************************************************************************************************************
************************************************************************************************************************
*/

/* ================================================================================================================== */
/*                                                    EVENT FLAGS                                                     */
/* ================================================================================================================== */

#if OS_CFG_FLAG_EN > 0u

void          OSFlagCreate              (OS_FLAG_GRP           *p_grp,
                                         CPU_CHAR              *p_name,
                                         OS_FLAGS               flags,
                                         OS_ERR                *p_err);

#if OS_CFG_FLAG_DEL_EN > 0u
OS_OBJ_QTY    OSFlagDel                 (OS_FLAG_GRP           *p_grp,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

OS_FLAGS      OSFlagPend                (OS_FLAG_GRP           *p_grp,
                                         OS_FLAGS               flags,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

#if OS_CFG_FLAG_PEND_ABORT_EN > 0u
OS_OBJ_QTY    OSFlagPendAbort           (OS_FLAG_GRP           *p_grp,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

OS_FLAGS      OSFlagPendGetFlagsRdy     (OS_ERR                *p_err);

OS_FLAGS      OSFlagPost                (OS_FLAG_GRP           *p_grp,
                                         OS_FLAGS               flags,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

/* ================================================================================================================== */
/*                                                 TASK MANAGEMENT                                                    */
/* ================================================================================================================== */

#if OS_CFG_TASK_CHANGE_PRIO_EN > 0u
void          OSTaskChangePrio          (OS_TCB                *p_tcb,
                                         OS_PRIO                prio_new,
                                         OS_ERR                *p_err);
#endif

void          OSTaskCreate              (OS_TCB                *p_tcb,
                                         CPU_CHAR              *p_name,
                                         OS_TASK_PTR            p_task,
                                         void                  *p_arg,
                                         OS_PRIO                prio,
                                         CPU_STK               *p_stk_base,
                                         CPU_STK_SIZE           stk_limit,
                                         CPU_STK_SIZE           stk_size,
                                         OS_MSG_QTY             q_size,
                                         OS_TICK                time_quanta,
                                         void                  *p_ext,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

#if OS_CFG_TASK_DEL_EN > 0u
void          OSTaskDel                 (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);
#endif

#if OS_CFG_TASK_Q_EN > 0u
OS_MSG_QTY    OSTaskQFlush              (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);

void         *OSTaskQPend               (OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         OS_MSG_SIZE           *p_msg_size,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

CPU_BOOLEAN   OSTaskQPendAbort          (OS_TCB                *p_tcb,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

void          OSTaskQPost               (OS_TCB                *p_tcb,
                                         void                  *p_void,
                                         OS_MSG_SIZE            msg_size,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

#endif

#if OS_CFG_TASK_REG_TBL_SIZE > 0u
OS_REG        OSTaskRegGet              (OS_TCB                *p_tcb,
                                         OS_REG_ID              id,
                                         OS_ERR                *p_err);

OS_REG_ID     OSTaskRegGetID            (OS_ERR                *p_err);

void          OSTaskRegSet              (OS_TCB                *p_tcb,
                                         OS_REG_ID              id,
                                         OS_REG                 value,
                                         OS_ERR                *p_err);
#endif

#if OS_CFG_TASK_SUSPEND_EN > 0u
void          OSTaskResume              (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);

void          OSTaskSuspend             (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);
#endif

OS_SEM_CTR    OSTaskSemPend             (OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

#if (OS_CFG_TASK_SEM_PEND_ABORT_EN > 0u)
CPU_BOOLEAN   OSTaskSemPendAbort        (OS_TCB                *p_tcb,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

OS_SEM_CTR    OSTaskSemPost             (OS_TCB                *p_tcb,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

OS_SEM_CTR    OSTaskSemSet              (OS_TCB                *p_tcb,
                                         OS_SEM_CTR             cnt,
                                         OS_ERR                *p_err);

#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
void          OSTaskStkChk              (OS_TCB                *p_tcb,
                                         CPU_STK_SIZE          *p_free,
                                         CPU_STK_SIZE          *p_used,
                                         OS_ERR                *p_err);
#endif

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void          OSTaskTimeQuantaSet       (OS_TCB                *p_tcb,
                                         OS_TICK                time_quanta,
                                         OS_ERR                *p_err);
#endif

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

#if OS_CFG_DBG_EN > 0u
void          OS_TaskDbgListAdd         (OS_TCB                *p_tcb);

void          OS_TaskDbgListRemove      (OS_TCB                *p_tcb);
#endif

void          OS_TaskInit               (OS_ERR                *p_err);

void          OS_TaskInitTCB            (OS_TCB                *p_tcb);


/* ================================================================================================================== */
/*                                             MUTUAL EXCLUSION SEMAPHORES                                            */
/* ================================================================================================================== */

#if OS_CFG_MUTEX_EN > 0u

void          OSMutexCreate             (OS_MUTEX              *p_mutex,
                                         CPU_CHAR              *p_name,
                                         OS_ERR                *p_err);

#if OS_CFG_MUTEX_DEL_EN > 0u
OS_OBJ_QTY    OSMutexDel                (OS_MUTEX              *p_mutex,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

void          OSMutexPend               (OS_MUTEX              *p_mutex,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

#if OS_CFG_MUTEX_PEND_ABORT_EN > 0u
OS_OBJ_QTY    OSMutexPendAbort          (OS_MUTEX              *p_mutex,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

void          OSMutexPost               (OS_MUTEX              *p_mutex,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

/* ================================================================================================================== */
/*                                                   MESSAGE QUEUES                                                   */
/* ================================================================================================================== */

#if OS_CFG_Q_EN > 0u

void          OSQCreate                 (OS_Q                  *p_q,
                                         CPU_CHAR              *p_name,
                                         OS_MSG_QTY             max_qty,
                                         OS_ERR                *p_err);

#if OS_CFG_Q_DEL_EN > 0u
OS_OBJ_QTY    OSQDel                    (OS_Q                  *p_q,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

#if OS_CFG_Q_FLUSH_EN > 0u
OS_MSG_QTY    OSQFlush                  (OS_Q                  *p_q,
                                         OS_ERR                *p_err);
#endif

void         *OSQPend                   (OS_Q                  *p_q,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         OS_MSG_SIZE           *p_msg_size,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

#if OS_CFG_Q_PEND_ABORT_EN > 0u
OS_OBJ_QTY    OSQPendAbort              (OS_Q                  *p_q,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

void          OSQPost                   (OS_Q                  *p_q,
                                         void                  *p_void,
                                         OS_MSG_SIZE            msg_size,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif
                                         
/* ================================================================================================================== */
/*                                                     SEMAPHORES                                                     */
/* ================================================================================================================== */

#if OS_CFG_SEM_EN > 0u

void          OSSemCreate               (OS_SEM                *p_sem,
                                         CPU_CHAR              *p_name,
                                         OS_SEM_CTR             cnt,
                                         OS_ERR                *p_err);

OS_OBJ_QTY    OSSemDel                  (OS_SEM                *p_sem,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

OS_SEM_CTR    OSSemPend                 (OS_SEM                *p_sem,
                                         OS_TICK                timeout,
                                         OS_OPT                 opt,
                                         CPU_TS                *p_ts,
                                         OS_ERR                *p_err);

#if OS_CFG_SEM_PEND_ABORT_EN > 0u
OS_OBJ_QTY    OSSemPendAbort            (OS_SEM                *p_sem,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

OS_SEM_CTR    OSSemPost                 (OS_SEM                *p_sem,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

#if OS_CFG_SEM_SET_EN > 0u
void          OSSemSet                  (OS_SEM                *p_sem,
                                         OS_SEM_CTR             cnt,
                                         OS_ERR                *p_err);
#endif

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_SemClr                 (OS_SEM                *p_sem);

#if OS_CFG_DBG_EN > 0u
void          OS_SemDbgListAdd          (OS_SEM                *p_sem);

void          OS_SemDbgListRemove       (OS_SEM                *p_sem);
#endif

void          OS_SemInit                (OS_ERR                *p_err);

#endif

/* ================================================================================================================== */
/*                                                    MISCELLANEOUS                                                   */
/* ================================================================================================================== */

void          OSInit                    (OS_ERR                *p_err);

void          OSIntEnter                (void);
void          OSIntExit                 (void);

#ifdef OS_SAFETY_CRITICAL_IEC61508
void          OSSafetyCriticalStart     (void);
#endif

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
void          OSSchedRoundRobinCfg      (CPU_BOOLEAN            en,
                                         OS_TICK                dflt_time_quanta,
                                         OS_ERR                *p_err);

void          OSSchedRoundRobinYield    (OS_ERR                *p_err);

#endif

void          OSSched                   (void);

void          OSSchedLock               (OS_ERR                *p_err);
void          OSSchedUnlock             (OS_ERR                *p_err);

void          OSStart                   (OS_ERR                *p_err);

#if OS_CFG_STAT_TASK_EN > 0u
void          OSStatReset               (OS_ERR                *p_err);

void          OSStatTaskCPUUsageInit    (OS_ERR                *p_err);
#endif

CPU_INT16U    OSVersion                 (OS_ERR                *p_err);

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_IdleTask               (void);
void          OS_IdleTaskInit           (OS_ERR                *p_err);

#if OS_CFG_STAT_TASK_EN > 0u
void          OS_StatTask               (void                  *p_arg);
void          OS_StatTaskInit           (OS_ERR                *p_err);
#endif


/* ================================================================================================================== */
/*                                          TASK LOCAL STORAGE (TLS) SUPPORT                                          */
/* ================================================================================================================== */

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
OS_TLS_ID  OS_TLS_GetID       (OS_ERR              *p_err);

OS_TLS     OS_TLS_GetValue    (OS_TCB              *p_tcb,
                               OS_TLS_ID            id,
                               OS_ERR              *p_err);

void       OS_TLS_Init        (OS_ERR              *p_err);

void       OS_TLS_SetValue    (OS_TCB              *p_tcb,
                               OS_TLS_ID            id,
                               OS_TLS               value,
                               OS_ERR              *p_err);

void       OS_TLS_SetDestruct (OS_TLS_ID            id,
                               OS_TLS_DESTRUCT_PTR  p_destruct,
                               OS_ERR              *p_err);

void       OS_TLS_TaskCreate  (OS_TCB              *p_tcb);

void       OS_TLS_TaskDel     (OS_TCB              *p_tcb);

void       OS_TLS_TaskSw      (void);
#endif

/* ================================================================================================================== */
/*                                                 TIME MANAGEMENT                                                    */
/* ================================================================================================================== */

void          OSTimeDly                 (OS_TICK                dly,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);

#if OS_CFG_TIME_DLY_HMSM_EN > 0u
void          OSTimeDlyHMSM             (CPU_INT16U             hours,
                                         CPU_INT16U             minutes,
                                         CPU_INT16U             seconds,
                                         CPU_INT32U             milli,
                                         OS_OPT                 opt,
                                         OS_ERR                *p_err);
#endif

#if OS_CFG_TIME_DLY_RESUME_EN > 0u
void          OSTimeDlyResume           (OS_TCB                *p_tcb,
                                         OS_ERR                *p_err);
#endif

OS_TICK       OSTimeGet                 (OS_ERR                *p_err);

void          OSTimeSet                 (OS_TICK                ticks,
                                         OS_ERR                *p_err);

void          OSTimeTick                (void);

/* ================================================================================================================== */
/*                                                 TIMER MANAGEMENT                                                   */
/* ================================================================================================================== */

#if OS_CFG_TMR_EN > 0u
void          OSTmrCreate               (OS_TMR                *p_tmr,
                                         CPU_CHAR              *p_name,
                                         OS_TICK                dly,
                                         OS_TICK                period,
                                         OS_OPT                 opt,
                                         OS_TMR_CALLBACK_PTR    p_callback,
                                         void                  *p_callback_arg,
                                         OS_ERR                *p_err);

CPU_BOOLEAN   OSTmrDel                  (OS_TMR                *p_tmr,
                                         OS_ERR                *p_err);

OS_TICK       OSTmrRemainGet            (OS_TMR                *p_tmr,
                                         OS_ERR                *p_err);

CPU_BOOLEAN   OSTmrStart                (OS_TMR                *p_tmr,
                                         OS_ERR                *p_err);

OS_STATE      OSTmrStateGet             (OS_TMR                *p_tmr,
                                         OS_ERR                *p_err);

CPU_BOOLEAN   OSTmrStop                 (OS_TMR                *p_tmr,
                                         OS_OPT                 opt,
                                         void                  *p_callback_arg,
                                         OS_ERR                *p_err);

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

void          OS_TmrClr                 (OS_TMR                *p_tmr);

#if OS_CFG_DBG_EN > 0u
void          OS_TmrDbgListAdd          (OS_TMR                *p_tmr);

void          OS_TmrDbgListRemove       (OS_TMR                *p_tmr);
#endif

void          OS_TmrInit                (OS_ERR                *p_err);

void          OS_TmrCallback            (void *p_ara);
#endif
  
/* ================================================================================================================== */
/*                                          FIXED SIZE MEMORY BLOCK MANAGEMENT                                        */
/* ================================================================================================================== */

#if OS_CFG_MEM_EN > 0u

void          OSMemCreate               (OS_MEM                *p_mem,
                                         CPU_CHAR              *p_name,
                                         void                  *p_addr,
                                         OS_MEM_QTY             n_blks,
                                         OS_MEM_SIZE            blk_size,
                                         OS_ERR                *p_err);

void         *OSMemGet                  (OS_MEM                *p_mem,
                                         OS_ERR                *p_err);

void          OSMemPut                  (OS_MEM                *p_mem,
                                         void                  *p_blk,
                                         OS_ERR                *p_err);

/* ------------------------------------------------ INTERNAL FUNCTIONS ---------------------------------------------- */

#if OS_CFG_DBG_EN > 0u
void          OS_MemDbgListAdd          (OS_MEM                *p_mem);
#endif

void          OS_MemInit                (OS_ERR                *p_err);

#endif
                                         
                                         
/*
************************************************************************************************************************
************************************************************************************************************************
*                                    T A R G E T   S P E C I F I C   F U N C T I O N S
************************************************************************************************************************
************************************************************************************************************************
*/

void          OSInitHook                (void);

void          OSTaskCreateHook          (OS_TCB                *p_tcb);
void          OSTaskDelHook             (OS_TCB                *p_tcb);
void          OSIdleTaskHook            (void);
void          OSStatTaskHook            (void);
                                         

/*
************************************************************************************************************************
************************************************************************************************************************
*                                                    RT-Thread Wrap
************************************************************************************************************************
************************************************************************************************************************
*/
OS_ERR        rt_err_to_ucosiii         (rt_err_t rt_err);
rt_err_t      rt_ipc_pend_abort_1       (rt_list_t *list);
rt_err_t      rt_ipc_pend_abort_all     (rt_list_t *list);

/*
************************************************************************************************************************
*                                          LOOK FOR MISSING #define CONSTANTS
*
* This section is used to generate ERROR messages at compile time if certain #define constants are
* MISSING in OS_CFG.H.  This allows you to quickly determine the source of the error.
*
* You SHOULD NOT change this section UNLESS you would like to add more comments as to the source of the
* compile time error.
************************************************************************************************************************
*/

/*
************************************************************************************************************************
*                                                     MISCELLANEOUS
************************************************************************************************************************
*/

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


#endif
