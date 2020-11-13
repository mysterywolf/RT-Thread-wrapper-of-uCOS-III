/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-10     Meco Man     the first verion
 */
/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                               PEND ON MULTIPLE OBJECTS
*
* File    : OS_PEND_MULTI.C
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

#include "os.h"

#if (((OS_CFG_Q_EN > 0u) || (OS_CFG_SEM_EN > 0u)) && (OS_CFG_PEND_MULTI_EN > 0u))
/*
************************************************************************************************************************
*                                               PEND ON MULTIPLE OBJECTS
*
* Description: This function pends on multiple objects.  The objects pended on MUST be either semaphores or message
*              queues.  If multiple objects are ready at the start of the pend call, then all available objects that
*              are ready will be indicated to the caller.  If the task must pend on the multiple events then, as soon
*              as one of the object is either posted, aborted or deleted, the task will be readied.
*
*              This function only allows you to pend on semaphores and/or message queues.
*
* Arguments  : p_pend_data_tbl   is a pointer to an array of type OS_PEND_DATA which contains a list of all the
*                                objects we will be waiting on.  The caller must declare an array of OS_PEND_DATA
*                                and initialize the .PendObjPtr (see below) with a pointer to the object (semaphore or
*                                message queue) to pend on.
*
*                                    OS_PEND_DATA  MyPendArray[?];
*
*                                The OS_PEND_DATA field are as follows:
*
*                                    OS_PEND_DATA  *PrevPtr;      Used to link OS_PEND_DATA objects
*                                    OS_PEND_DATA  *NextPtr;      Used to link OS_PEND_DATA objects
*                                    OS_TCB        *TCBPtr;       Pointer to the TCB that is pending on multiple objects
*                                    OS_PEND_OBJ   *PendObjPtr;   USER supplied field which is a pointer to the
*                                                                 semaphore or message queue you want to pend on.  When
*                                                                 you call OSPendMulti() you MUST fill this field for
*                                                                 each of the objects you want to pend on.
*                                    OS_PEND_OBJ   *RdyObjPtr;    OSPendMulti() will return the object that was posted,
*                                                                 aborted or deleted in this field.
*                                    void          *RdyMsgPtr;    OSPendMulti() will fill in this field if the object
*                                                                 posted was a message queue.  This corresponds to the
*                                                                 message posted.
*                                    OS_MSG_SIZE    RdyMsgSize;   OSPendMulti() will fill in this field if the object
*                                                                 posted was a message queue.  This corresponds to the
*                                                                 size of the message posted.
*                                    CPU_TS         RdyTS;        OSPendMulti() will fill in this field if the object
*                                                                 was a message queue.  This corresponds to the time
*                                                                 stamp when the message was posted.  However, if the
*                                                                 object is a semaphore and the object is already ready
*                                                                 the this field will be set to (CPU_TS)0 because it's
*                                                                 not possible to know when the semaphore was posted.
*
*              tbl_size      is the size (in number of elements) of the OS_PEND_DATA array passed to this function.  In
*                            other words, if the called needs to pend on 4 separate objects (semaphores and/or queues)
*                            then you would pass 4 to this call.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait any of
*                            the objects up to the amount of time specified by this argument. If you specify 0, however,
*                            your task will wait forever for the specified objects or, until an object is posted,
*                            aborted or deleted.
*
*              opt           determines whether the user wants to block if none of the objects are available.
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_err         is a pointer to where an error message will be deposited.  Possible error messages are:
*
*                                OS_ERR_NONE              The call was successful and your task owns the resources or,
*                                                         the objects you are waiting for occurred. Check the .RdyObjPtr
*                                                         fields to know which objects have been posted.
*                                OS_ERR_OBJ_TYPE          If any of the .PendPtr is NOT a semaphore or a message queue
*                                OS_ERR_OPT_INVALID       If you specified an invalid option for 'opt'
*                                OS_ERR_PEND_ABORT        The wait on the events was aborted; check the .RdyObjPtr fields
*                                                         for which objects were aborted.
*                                OS_ERR_PEND_DEL          The wait on the events was aborted; check the .RdyObjPtr fields
*                                                         for which objects were aborted.
*                                OS_ERR_PEND_ISR          If you called this function from an ISR
*                                OS_ERR_PEND_LOCKED       If you called this function when the scheduler is locked.
*                                OS_ERR_PEND_WOULD_BLOCK  If the caller didn't want to block and no object ready
*                                OS_ERR_STATUS_INVALID    Invalid pend status
*                                OS_ERR_PTR_INVALID       If you passes a NULL pointer of 'p_pend_data_tbl'
*                                OS_ERR_TIMEOUT           The objects were not posted within the specified 'timeout'.
*
* Returns    : >  0          the number of objects returned as ready, aborted or deleted
*              == 0          if no events are returned as ready because of timeout or upon error.
************************************************************************************************************************
*/

OS_OBJ_QTY  OSPendMulti (OS_PEND_DATA  *p_pend_data_tbl,
                         OS_OBJ_QTY     tbl_size,
                         OS_TICK        timeout,
                         OS_OPT         opt,
                         OS_ERR        *p_err)
{
    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_OBJ_QTY)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* Can't pend from an ISR                                 */
       *p_err = OS_ERR_PEND_ISR;
        return ((OS_OBJ_QTY)0);
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if (p_pend_data_tbl == (OS_PEND_DATA *)0) {             /* Validate 'p_pend_data_tbl'                             */
       *p_err = OS_ERR_PTR_INVALID;
        return ((OS_OBJ_QTY)0);
    }
    if (tbl_size == (OS_OBJ_QTY)0) {                        /* Array size must be > 0                                 */
       *p_err = OS_ERR_PTR_INVALID;
        return ((OS_OBJ_QTY)0);
    }
    switch (opt) {
        case OS_OPT_PEND_BLOCKING:
        case OS_OPT_PEND_NON_BLOCKING:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return ((OS_OBJ_QTY)0);
    }
#endif

    CPU_CRITICAL_ENTER();

    CPU_CRITICAL_EXIT();

    return ((OS_OBJ_QTY)1);
}

#endif
