/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-03     Meco Man     the first verion
 */

/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                EVENT FLAG MANAGEMENT
*
* File    : OS_FLAG.C
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
************************************************************************************************************************
* Note(s)    : 1)uCOS-III称之为事件标志组(flag group),RTT称之为事件集(event),以下统一使用"事件标志组"称呼
*
************************************************************************************************************************
*/

#if OS_CFG_FLAG_EN > 0u

/*
************************************************************************************************************************
*                                                 CREATE AN EVENT FLAG
*
* Description: This function is called to create an event flag group.
*
* Arguments  : p_grp          is a pointer to the event flag group to create
*
*              p_name         is the name of the event flag group
*
*              flags          contains the initial value to store in the event flag group (typically 0).
*                             -------------说明-------------
*                             在uCOS中可以让用户选择是置1为事件发生还是清0为事件发生，但是在RTT中直接定死
*                             置1为事件发生,因此该位必须为0。
*
*              p_err          is a pointer to an error code which will be returned to your application:
*
*                                 OS_ERR_NONE                    if the call was successful.
*                                 OS_ERR_CREATE_ISR              if you attempted to create an Event Flag from an ISR.
*                                 OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Event Flag after you
*                                                                   called OSSafetyCriticalStart().
*                                 OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                 OS_ERR_OBJ_CREATED             if the event flag group has already been created
*                                 OS_ERR_OBJ_PTR_NULL            if 'p_grp' is a NULL pointer
*                               + OS_ERR_OPT_INVALID
*                             -------------说明-------------
*                                 OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                               - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                               + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                               应用层需要对API返回的错误码判断做出相应的修改
* Returns    : none
************************************************************************************************************************
*/

void  OSFlagCreate (OS_FLAG_GRP  *p_grp,
                    CPU_CHAR     *p_name,
                    OS_FLAGS      flags,
                    OS_ERR       *p_err)
{
    rt_err_t rt_err;
        
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;
        return;
    }
#endif    
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u    
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_CREATE_ISR;
        return;
    }    
#endif    
    
#if OS_CFG_ARG_CHK_EN > 0u    
    if(p_grp == RT_NULL)/*检查事件标志组指针是否为空*/
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
    if(p_name == RT_NULL)/*检查事件标志组名指针是否为空*/
    {
        *p_err = OS_ERR_NAME;
        return;
    } 
    if(flags)
    {
        *p_err = OS_ERR_OPT_INVALID;
        RT_DEBUG_LOG(OS_CFG_DBG_EN,("OSFlagCreate: wrapper can't accept this option\n"));
        return;        
    }  
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u     
    /*判断内核对象是否已经是事件标志组，即是否已经创建过*/
    if(rt_object_get_type(&p_grp->FlagGrp.parent.parent) == RT_Object_Class_Event)
    {
        *p_err = OS_ERR_OBJ_CREATED;
        return;       
    }   
#endif
 
    /*在uCOS中事件是直接被插入到链表,不按照优先级排列*/
    rt_err = rt_event_init(&p_grp->FlagGrp,(const char*)p_name,RT_IPC_FLAG_FIFO);
    *p_err = rt_err_to_ucosiii(rt_err);
    if(rt_err != RT_EOK)
    {
        return;
    }
    
    CPU_CRITICAL_ENTER();
    p_grp->Type    = OS_OBJ_TYPE_FLAG;                      /* Set to event flag group type                           */
    p_grp->NamePtr = p_name;
    p_grp->Flags   = flags;                                 /* Set to desired initial value                           */
#if OS_CFG_DBG_EN > 0u
    OS_FlagDbgListAdd(p_grp);
#endif
    OSFlagQty++;
    CPU_CRITICAL_EXIT();
}

/*
************************************************************************************************************************
*                                             DELETE AN EVENT FLAG GROUP
*
* Description: This function deletes an event flag group and readies all tasks pending on the event flag group.
*
* Arguments  : p_grp     is a pointer to the desired event flag group.
*
*              opt       determines delete options as follows:
*
*                            OS_OPT_DEL_NO_PEND           Deletes the event flag group ONLY if no task pending
*                            OS_OPT_DEL_ALWAYS            Deletes the event flag group even if tasks are waiting.
*                                                         In this case, all the tasks pending will be readied.
*
*              p_err     is a pointer to an error code that can contain one of the following values:
*
*                            OS_ERR_NONE                  The call was successful and the event flag group was deleted
*                            OS_ERR_DEL_ISR               If you attempted to delete the event flag group from an ISR
*                            OS_ERR_OBJ_PTR_NULL          If 'p_grp' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If you didn't pass a pointer to an event flag group
*                            OS_ERR_OPT_INVALID           An invalid option was specified
*                            OS_ERR_TASK_WAITING          One or more tasks were waiting on the event flag group.
*
* Returns    : == 0          if no tasks were waiting on the event flag group, or upon error.
*              >  0          if one or more tasks waiting on the event flag group are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the event flag
*                 group MUST check the return code of OSFlagPost and OSFlagPend().
************************************************************************************************************************
*/

#if OS_CFG_FLAG_DEL_EN > 0u
OS_OBJ_QTY  OSFlagDel (OS_FLAG_GRP  *p_grp,
                       OS_OPT        opt,
                       OS_ERR       *p_err)
{
    rt_err_t rt_err;
    rt_uint32_t pend_flag_len;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_OBJ_QTY)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u    
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_DEL_ISR;
        return 0;
    }        
#endif    

#if OS_CFG_ARG_CHK_EN > 0u    
    if(p_grp == RT_NULL)/*检查指针是否为空*/
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    }
    switch (opt) {                                       
        case OS_OPT_DEL_NO_PEND:
        case OS_OPT_DEL_ALWAYS:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return ((OS_OBJ_QTY)0);
    }     
#endif
    
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为事件标志组*/
    if(rt_object_get_type(&p_grp->FlagGrp.parent.parent) != RT_Object_Class_Event)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }   
#endif
    
    CPU_CRITICAL_ENTER();
    pend_flag_len = rt_list_len(&(p_grp->FlagGrp.parent.suspend_thread));
    CPU_CRITICAL_EXIT();  
    
    switch (opt)
    {
        case OS_OPT_DEL_NO_PEND:
            CPU_CRITICAL_ENTER();
            if(rt_list_isempty(&(p_grp->FlagGrp.parent.suspend_thread)))/*若没有线程等待信号量*/
            {
                CPU_CRITICAL_EXIT();
                rt_err = rt_event_detach(&p_grp->FlagGrp);
                *p_err = rt_err_to_ucosiii(rt_err);                 
            }
            else
            {
                CPU_CRITICAL_EXIT();
                *p_err = OS_ERR_TASK_WAITING;
            }
            break;
            
        case OS_OPT_DEL_ALWAYS:
            rt_err = rt_event_detach(&p_grp->FlagGrp);
            *p_err = rt_err_to_ucosiii(rt_err);
            break;
    }
    
    if(*p_err == OS_ERR_NONE)
    {
        CPU_CRITICAL_ENTER();
#if OS_CFG_DBG_EN > 0u
        OS_FlagDbgListRemove(p_grp);
#endif
        OSFlagQty--;
        OS_FlagClr(p_grp);
        CPU_CRITICAL_EXIT();
    }
    
    return pend_flag_len;
}
#endif

/*
************************************************************************************************************************
*                                             WAIT ON AN EVENT FLAG GROUP
*
* Description: This function is called to wait for a combination of bits to be set in an event flag group.  Your
*              application can wait for ANY bit to be set or ALL bits to be set.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to wait for.
*                            The bits you want are specified by setting the corresponding bits in 'flags'.
*                            e.g. if your application wants to wait for bits 0 and 1 then 'flags' would contain 0x03.
*
*              timeout       is an optional timeout (in clock ticks) that your task will wait for the
*                            desired bit combination.  If you specify 0, however, your task will wait
*                            forever at the specified event flag group or, until a message arrives.
*
*              opt           specifies whether you want ALL bits to be set or ANY of the bits to be set.
*                            You can specify the 'ONE' of the following arguments:
*
*                                OS_OPT_PEND_FLAG_CLR_ALL   You will wait for ALL bits in 'flags' to be clear (0)
*                                OS_OPT_PEND_FLAG_CLR_ANY   You will wait for ANY bit  in 'flags' to be clear (0)
*                                OS_OPT_PEND_FLAG_SET_ALL   You will wait for ALL bits in 'flags' to be set   (1)
*                                OS_OPT_PEND_FLAG_SET_ANY   You will wait for ANY bit  in 'flags' to be set   (1)
*                            -------------说明-------------
*                               在本兼容层中：
*                               OS_OPT_PEND_FLAG_CLR_ALL和OS_OPT_PEND_FLAG_SET_ALL意义一样
*                               OS_OPT_PEND_FLAG_CLR_ANY和OS_OPT_PEND_FLAG_SET_ANY意义一样
*
*                            You can 'ADD' OS_OPT_PEND_FLAG_CONSUME if you want the event flag to be 'consumed' by
*                                      the call.  Example, to wait for any flag in a group AND then clear
*                                      the flags that are present, set 'wait_opt' to:
*
*                                      OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME
*
*                            You can also 'ADD' the type of pend with 'ONE' of the two option:
*
*                                OS_OPT_PEND_NON_BLOCKING   Task will NOT block if flags are not available
*                                OS_OPT_PEND_BLOCKING       Task will     block if flags are not available
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the event flag group was
*                            posted, aborted or the event flag group deleted.  If you pass a NULL pointer (i.e. (CPU_TS *)0)
*                            then you will not get the timestamp.  In other words, passing a NULL pointer is valid and
*                            indicates that you don't need the timestamp.
*                            -------------说明-------------
*                            该参数在RTT中没有意义,填NULL即可
*
*              p_err         is a pointer to an error code and can be:
*
*                                OS_ERR_NONE                The desired bits have been set within the specified 'timeout'
*                                OS_ERR_OBJ_PTR_NULL        If 'p_grp' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                                OS_ERR_OPT_INVALID         You didn't specify a proper 'opt' argument.
*                                OS_ERR_PEND_ABORT          The wait on the flag was aborted.
*                                OS_ERR_PEND_ISR            If you tried to PEND from an ISR
*                              - OS_ERR_PEND_WOULD_BLOCK    If you specified non-blocking but the flags were not
*                                                           available.
*                                OS_ERR_SCHED_LOCKED        If you called this function when the scheduler is locked
*                                OS_ERR_TIMEOUT             The bit(s) have not been set in the specified 'timeout'.
*                            -------------说明-------------
*                                OS_ERR_XXXX        表示可以继续沿用uCOS-III原版的错误码
*                              - OS_ERR_XXXX        表示该错误码在本兼容层已经无法使用
*                              + OS_ERR_RT_XXXX     表示该错误码为新增的RTT专用错误码集
*                              应用层需要对API返回的错误码判断做出相应的修改
*
* Returns    : The flags in the event flag group that made the task ready or, 0 if a timeout or an error
*              occurred.
*
* Note(s)    : 1) RTT在非阻塞模式下不区分OS_ERR_PEND_WOULD_BLOCK还是OS_ERR_TIMEOUT，都按照OS_ERR_TIMEOUT处理
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPend (OS_FLAG_GRP  *p_grp,
                      OS_FLAGS      flags,
                      OS_TICK       timeout,
                      OS_OPT        opt,
                      CPU_TS       *p_ts,
                      OS_ERR       *p_err)
{
    rt_err_t        rt_err;
    rt_int32_t      time;
    CPU_BOOLEAN     consume;
    OS_OPT          mode;
    rt_uint8_t      rt_option;
    rt_uint32_t     recved;
    
    CPU_SR_ALLOC();
    
    CPU_VAL_UNUSED(p_ts);
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_FLAGS)0);
    }
#endif
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u    
    if(OSIntNestingCtr> (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_PEND_ISR;
        return ((OS_OBJ_QTY)0);
    }       
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u    
    if(p_grp == RT_NULL)/*检查指针是否为空*/
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return ((OS_OBJ_QTY)0);
    }
    switch (opt) {
        case OS_OPT_PEND_FLAG_CLR_ALL:
        case OS_OPT_PEND_FLAG_CLR_ANY:
        case OS_OPT_PEND_FLAG_SET_ALL:
        case OS_OPT_PEND_FLAG_SET_ANY:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return ((OS_OBJ_QTY)0);
    }    
#endif 
    
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为事件标志组*/
    if(rt_object_get_type(&p_grp->FlagGrp.parent.parent) != RT_Object_Class_Event)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return ((OS_OBJ_QTY)0);       
    }  
#endif    
   
    /*提取opt*/
    if ((opt & OS_OPT_PEND_FLAG_CONSUME) != (OS_OPT)0) {    /* See if we need to consume the flags                    */
        consume = DEF_TRUE;
    } else {
        consume = DEF_FALSE;
    }

    mode = opt & OS_OPT_PEND_FLAG_MASK;
    switch (mode) {
        /*OS_OPT_PEND_FLAG_CLR_ALL和OS_OPT_PEND_FLAG_SET_ALL意义一样,相当于RTT的RT_EVENT_FLAG_AND*/
        case OS_OPT_PEND_FLAG_SET_ALL:
        case OS_OPT_PEND_FLAG_CLR_ALL:    
            rt_option = RT_EVENT_FLAG_AND;
            break;
        
        /*OS_OPT_PEND_FLAG_CLR_ANY和OS_OPT_PEND_FLAG_SET_ANY意义一样,相当于RTT的RT_EVENT_FLAG_OR*/
        case OS_OPT_PEND_FLAG_SET_ANY:
        case OS_OPT_PEND_FLAG_CLR_ANY:
            rt_option = RT_EVENT_FLAG_OR;
            break;
             
        default:
            *p_err = OS_ERR_OPT_INVALID;
    }
    
    if (consume == DEF_TRUE)
    {
        /*OS_OPT_PEND_FLAG_CONSUME相当于RTT中的RT_EVENT_FLAG_CLEAR*/
        rt_option |= RT_EVENT_FLAG_CLEAR;
    }
    
    /*在RTT中timeout为0表示不阻塞,为RT_WAITING_FOREVER表示永久阻塞,
    这与uCOS-III有所不同,因此需要转换*/
    if((opt & OS_OPT_PEND_NON_BLOCKING) == (OS_OPT)0)
    {
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0)/*检查调度器是否被锁*/
        {
            *p_err = OS_ERR_SCHED_LOCKED;
            return ((OS_OBJ_QTY)0);         
        }
        if(timeout == 0)/*在uCOS-III中timeout=0表示永久阻塞*/
        {
            time = RT_WAITING_FOREVER;
        }
        else
        {
            time = timeout;
        }
    }
    else
    {
        time = 0;/*在RTT中timeout为0表示非阻塞*/
    }

    CPU_CRITICAL_ENTER();
    OSTCBCurPtr->PendStatus = OS_STATUS_PEND_OK;            /* Clear pend status                                      */
    CPU_CRITICAL_EXIT(); 
    
    rt_err = rt_event_recv(&p_grp->FlagGrp,
                           flags,
                           rt_option,
                           time,
                           &recved);
    *p_err = rt_err_to_ucosiii(rt_err);  
    
    CPU_CRITICAL_ENTER();
    if(OSTCBCurPtr->PendStatus == OS_STATUS_PEND_ABORT)     /* Indicate that we aborted                               */
    {
        CPU_CRITICAL_EXIT(); 
        *p_err = OS_ERR_PEND_ABORT;
        return 0;
    }    
    CPU_CRITICAL_EXIT();        
    
    return recved;
}

/*
************************************************************************************************************************
*                                          ABORT WAITING ON AN EVENT FLAG GROUP
*
* Description: This function aborts & readies any tasks currently waiting on an event flag group.  This function should
*              be used to fault-abort the wait on the event flag group, rather than to normally post to the event flag
*              group OSFlagPost().
*
* Arguments  : p_grp     is a pointer to the event flag group
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the event flag
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the event flag
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the event flag group and was
*                                                         readied and informed of the aborted wait; check return value
*                                                         for the number of tasks whose wait on the event flag group
*                                                         was aborted.
*                            OS_ERR_OBJ_PTR_NULL          If 'p_grp' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If 'p_grp' is not pointing at an event flag group
*                            OS_ERR_OPT_INVALID           If you specified an invalid option
*                            OS_ERR_PEND_ABORT_ISR        If you called this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the event flag group, or upon error.
*              >  0          if one or more tasks waiting on the event flag group are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_FLAG_PEND_ABORT_EN > 0u
OS_OBJ_QTY  OSFlagPendAbort (OS_FLAG_GRP  *p_grp,
                             OS_OPT        opt,
                             OS_ERR       *p_err)
{
    rt_uint32_t pe_flag_len;
    
    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_OBJ_QTY)0u);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) {             /* Not allowed to Pend Abort from an ISR                  */
       *p_err =  OS_ERR_PEND_ABORT_ISR;
        return ((OS_OBJ_QTY)0u);
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if (p_grp == (OS_FLAG_GRP *)0) {                             /* Validate 'p_sem'                                       */
       *p_err =  OS_ERR_OBJ_PTR_NULL;
        return ((OS_OBJ_QTY)0u);
    }
    switch (opt) {                                          /* Validate 'opt'                                         */
        case OS_OPT_PEND_ABORT_1:
        case OS_OPT_PEND_ABORT_ALL:
        case OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED:
             break;

        default:
            *p_err =  OS_ERR_OPT_INVALID;
             return ((OS_OBJ_QTY)0u);
    }
#endif
    
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    if (rt_object_get_type(&p_grp->FlagGrp.parent.parent) != RT_Object_Class_Event) {/*Make sure semaphore was created*/
       *p_err =  OS_ERR_OBJ_TYPE;
        return ((OS_OBJ_QTY)0u);
    }
#endif  
    
    CPU_CRITICAL_ENTER();
    if(rt_list_isempty(&(p_grp->FlagGrp.parent.suspend_thread)))/*若没有线程等待信号量*/ 
    {
        CPU_CRITICAL_EXIT(); 
       *p_err =  OS_ERR_PEND_ABORT_NONE;
        return ((OS_OBJ_QTY)0u);        
    }
    CPU_CRITICAL_EXIT();
    
    if(opt&OS_OPT_PEND_ABORT_ALL)
    {
        rt_ipc_pend_abort_all(&(p_grp->FlagGrp.parent.suspend_thread));
    }
    else
    {
        rt_ipc_pend_abort_1(&(p_grp->FlagGrp.parent.suspend_thread));
    }
    
    if(!(opt&OS_OPT_POST_NO_SCHED))
    {
        rt_schedule();
    }
    
    *p_err = OS_ERR_NONE;
    
    CPU_CRITICAL_ENTER();
    pe_flag_len = rt_list_len(&(p_grp->FlagGrp.parent.suspend_thread));
    CPU_CRITICAL_EXIT();
    
    return pe_flag_len;
}
#endif

/*
************************************************************************************************************************
*                                       GET FLAGS WHO CAUSED TASK TO BECOME READY
*
* Description: This function is called to obtain the flags that caused the task to become ready to run.
*              In other words, this function allows you to tell "Who done it!".
*
* Arguments  : p_err     is a pointer to an error code
*
*                            OS_ERR_NONE       if the call was successful
*                            OS_ERR_PEND_ISR   if called from an ISR
*
* Returns    : The flags that caused the task to be ready.
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPendGetFlagsRdy (OS_ERR  *p_err)
{
    OS_FLAGS   flags;
    
    CPU_SR_ALLOC();

#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_FLAGS)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* See if called from ISR ...                             */
       *p_err = OS_ERR_PEND_ISR;                            /* ... can't get from an ISR                              */
        return ((OS_FLAGS)0);
    }
#endif

    CPU_CRITICAL_ENTER();
    flags = OSTCBCurPtr->Task.event_set;
    CPU_CRITICAL_EXIT();
   *p_err = OS_ERR_NONE;
    return (flags);
}

/*
************************************************************************************************************************
*                                                POST EVENT FLAG BIT(S)
*
* Description: This function is called to set or clear some bits in an event flag group.  The bits to set or clear are
*              specified by a 'bit mask'.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         If 'opt' (see below) is OS_OPT_POST_FLAG_SET, each bit that is set in 'flags' will
*                            set the corresponding bit in the event flag group.  e.g. to set bits 0, 4
*                            and 5 you would set 'flags' to:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*                            If 'opt' (see below) is OS_OPT_POST_FLAG_CLR, each bit that is set in 'flags' will
*                            CLEAR the corresponding bit in the event flag group.  e.g. to clear bits 0,
*                            4 and 5 you would specify 'flags' as:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*              opt           indicates whether the flags will be:
*
*                                OS_OPT_POST_FLAG_SET       set
*                              - OS_OPT_POST_FLAG_CLR       cleared
*
*                            you can also 'add' OS_OPT_POST_NO_SCHED to prevent the scheduler from being called.
*                             -------------说明-------------
*                             在uCOS中可以让用户选择是置1为事件发生还是清0为事件发生，但是在RTT中直接定死
*                             置1为事件发生,因此该位必须填OS_OPT_POST_FLAG_SET
*
*              p_err         is a pointer to an error code and can be:
*
*                                OS_ERR_NONE                The call was successful
*                                OS_ERR_OBJ_PTR_NULL        You passed a NULL pointer
*                                OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                                OS_ERR_OPT_INVALID         You specified an invalid option
*
* Returns    : the new value of the event flags bits that are still set.
*
* Note(s)    : 1) The execution time of this function depends on the number of tasks waiting on the event flag group.
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPost (OS_FLAG_GRP  *p_grp,
                      OS_FLAGS      flags,
                      OS_OPT        opt,
                      OS_ERR       *p_err)
{
    rt_err_t rt_err;
        
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_FLAGS)0);
    }
#endif    
    
#if OS_CFG_ARG_CHK_EN > 0u   
    if(p_grp == RT_NULL) /*检查指针是否为空*/
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return 0;
    }
    switch (opt) {     
        case OS_OPT_POST_FLAG_SET:
        case OS_OPT_POST_FLAG_CLR:
        case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED:
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return ((OS_FLAGS)0);
    }
    if(opt != OS_OPT_POST_FLAG_SET)
    {
        *p_err = OS_ERR_OPT_INVALID;
        RT_DEBUG_LOG(OS_CFG_DBG_EN,("OSFlagPost: wrapper can't accept this option\n"));
        return ((OS_FLAGS)0);  
    }      
#endif
    
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为事件标志组*/
    if(rt_object_get_type(&p_grp->FlagGrp.parent.parent) != RT_Object_Class_Event)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }  
#endif
    
    rt_err = rt_event_send(&p_grp->FlagGrp,flags);
    *p_err = rt_err_to_ucosiii(rt_err);
    return p_grp->FlagGrp.set;/*返回执行后事件标志组的值*/
}


/*$PAGE*/
/*
************************************************************************************************************************
*                                      CLEAR THE CONTENTS OF AN EVENT FLAG GROUP
*
* Description: This function is called by OSFlagDel() to clear the contents of an event flag group
*

* Argument(s): p_grp     is a pointer to the event flag group to clear
*              -----
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_FlagClr (OS_FLAG_GRP  *p_grp)
{
    p_grp->Type             = OS_OBJ_TYPE_NONE;
    p_grp->NamePtr          = (CPU_CHAR *)((void *)"?FLAG");    /* Unknown name                                       */
    p_grp->Flags            = (OS_FLAGS )0;
}

/*
************************************************************************************************************************
*                                          INITIALIZE THE EVENT FLAG MODULE
*
* Description: This function is called by uC/OS-III to initialize the event flag module.  Your application MUST NOT call
*              this function.  In other words, this function is internal to uC/OS-III.
*
* Arguments  : p_err     is a pointer to an error code that can contain one of the following values:
*
*                            OS_ERR_NONE   The call was successful.
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_FlagInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSFlagDbgListPtr = (OS_FLAG_GRP *)0;
#endif

    OSFlagQty        = (OS_OBJ_QTY   )0;
   *p_err            = OS_ERR_NONE;
}

/*
************************************************************************************************************************
*                                    ADD/REMOVE EVENT FLAG GROUP TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove an event flag group from the event flag debug
*              list.
*
* Arguments  : p_grp     is a pointer to the event flag group to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u
void  OS_FlagDbgListAdd (OS_FLAG_GRP  *p_grp)
{
    p_grp->DbgPrevPtr                = (OS_FLAG_GRP *)0;
    if (OSFlagDbgListPtr == (OS_FLAG_GRP *)0) {
        p_grp->DbgNextPtr            = (OS_FLAG_GRP *)0;
    } else {
        p_grp->DbgNextPtr            =  OSFlagDbgListPtr;
        OSFlagDbgListPtr->DbgPrevPtr =  p_grp;
    }
    OSFlagDbgListPtr                 =  p_grp;
}



void  OS_FlagDbgListRemove (OS_FLAG_GRP  *p_grp)
{
    OS_FLAG_GRP  *p_grp_next;
    OS_FLAG_GRP  *p_grp_prev;


    p_grp_prev = p_grp->DbgPrevPtr;
    p_grp_next = p_grp->DbgNextPtr;

    if (p_grp_prev == (OS_FLAG_GRP *)0) {
        OSFlagDbgListPtr = p_grp_next;
        if (p_grp_next != (OS_FLAG_GRP *)0) {
            p_grp_next->DbgPrevPtr = (OS_FLAG_GRP *)0;
        }
        p_grp->DbgNextPtr = (OS_FLAG_GRP *)0;

    } else if (p_grp_next == (OS_FLAG_GRP *)0) {
        p_grp_prev->DbgNextPtr = (OS_FLAG_GRP *)0;
        p_grp->DbgPrevPtr      = (OS_FLAG_GRP *)0;

    } else {
        p_grp_prev->DbgNextPtr =  p_grp_next;
        p_grp_next->DbgPrevPtr =  p_grp_prev;
        p_grp->DbgNextPtr      = (OS_FLAG_GRP *)0;
        p_grp->DbgPrevPtr      = (OS_FLAG_GRP *)0;
    }
}
#endif

#endif
