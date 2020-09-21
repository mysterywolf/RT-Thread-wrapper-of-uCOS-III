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
*                                                   TIMER MANAGEMENT
*
* File    : OS_TMR.C
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

#if OS_CFG_TMR_EN > 0u
/*
************************************************************************************************************************
*                                                   CREATE A TIMER
*
* Description: This function is called by your application code to create a timer.
*
* Arguments  : p_tmr           Is a pointer to a timer control block
*
*              p_name          Is a pointer to an ASCII string that is used to name the timer.  Names are useful for
*                              debugging.
*
*              dly             Initial delay.
*                              If the timer is configured for ONE-SHOT mode, this is the timeout used
*                              If the timer is configured for PERIODIC mode, this is the first timeout to wait for
*                              before the timer starts entering periodic mode
*
*              period          The 'period' being repeated for the timer.
*                              If you specified 'OS_OPT_TMR_PERIODIC' as an option, when the timer expires, it will
*                              automatically restart with the same period.
*                              -------------说明-------------
*                              RTT和uCOS-III在定时器时钟源的设计不同：
*                              ·RTT的定时器时钟频率与操作系统ostick频率相同
*                              ·uCOS-III的定时器时钟由ostick分频得到，分频系数为OS_CFG_TMR_TASK_RATE_HZ
*                              函数内部已经对上述两个操作系统定义的做出了转换
*
*              opt             Specifies either:
*
*                                  OS_OPT_TMR_ONE_SHOT       The timer counts down only once
*                                  OS_OPT_TMR_PERIODIC       The timer counts down and then reloads itself
*
*              p_callback      Is a pointer to a callback function that will be called when the timer expires.  The
*                              callback function must be declared as follows:
*
*                                  void  MyCallback (OS_TMR *p_tmr, void *p_arg);
*
*              p_callback_arg  Is an argument (a pointer) that is passed to the callback function when it is called.
*
*              p_err           Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                                 OS_ERR_NONE
*                                 OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the timer after you called
*                                                                  OSSafetyCriticalStart().
*                                 OS_ERR_OBJ_CREATED             if the timer has already been created
*                                 OS_ERR_OBJ_PTR_NULL            is 'p_tmr' is a NULL pointer
*                                 OS_ERR_OBJ_TYPE                if the object type is invalid
*                                 OS_ERR_OPT_INVALID             you specified an invalid option
*                                 OS_ERR_TMR_INVALID_CALLBACK    You specified an invalid callback for a periodic timer
*                                 OS_ERR_TMR_INVALID_DLY         you specified an invalid delay
*                                 OS_ERR_TMR_INVALID_PERIOD      you specified an invalid period
*                                 OS_ERR_TMR_ISR                 if the call was made from an ISR
*
* Returns    : none
*
* Note(s)    : 1) This function only creates the timer.  In other words, the timer is not started when created.  To
*                 start the timer, call OSTmrStart().
*              2) 需要开启RT_USING_TIMER_SOFT宏定义
*
************************************************************************************************************************
*/

void  OSTmrCreate (OS_TMR               *p_tmr,
                   CPU_CHAR             *p_name,
                   OS_TICK               dly,
                   OS_TICK               period,
                   OS_OPT                opt,
                   OS_TMR_CALLBACK_PTR   p_callback,
                   void                 *p_callback_arg,
                   OS_ERR               *p_err)
{
    rt_uint8_t rt_flag;
    rt_tick_t  time, time2;
        
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
        *p_err = OS_ERR_TMR_ISR;
        return;
    }
#endif 
    
#if OS_CFG_ARG_CHK_EN > 0u    
    if(p_tmr == RT_NULL)/*检查指针是否为空*/
    {
        *p_err = OS_ERR_OBJ_PTR_NULL;
        return;
    }
    switch (opt) {
        case OS_OPT_TMR_PERIODIC:
             if (period == (OS_TICK)0) {
                *p_err = OS_ERR_TMR_INVALID_PERIOD;
                 return;
             }
             
             if (p_callback == (OS_TMR_CALLBACK_PTR)0) {        /* No point in a periodic timer without a callback      */
                *p_err = OS_ERR_TMR_INVALID_CALLBACK;
                 return;
             }
             break;

        case OS_OPT_TMR_ONE_SHOT:
             if (dly == (OS_TICK)0) {
                *p_err = OS_ERR_TMR_INVALID_DLY;
                 return;
             }
             break;

        default:
            *p_err = OS_ERR_OPT_INVALID;
             return;
    }    
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u 
    /*判断内核对象是否已经是定时器，即是否已经创建过*/
    if(rt_object_get_type(&p_tmr->Tmr.parent) == RT_Object_Class_Timer)
    {
        *p_err = OS_ERR_OBJ_CREATED;
        return;       
    }
#endif
    
    /*uCOS-III原版定时器回调函数就是在定时器线程中调用的,而非在中断中调用,
    因此要使用RTT的RT_TIMER_FLAG_SOFT_TIMER选项,在此之前应将宏定义RT_USING_TIMER_SOFT置1*/
    if(opt == OS_OPT_TMR_ONE_SHOT)
    {
        rt_flag = RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER;
        if(dly == 0)/*检查dly参数是否合法*/
        {
            *p_err = OS_ERR_TMR_INVALID_DLY;
            return;
        }
        /*RTT和uCOS-III在定时器时钟源的设计不同,需要进行转换*/
        time = dly * (1000 / OS_CFG_TMR_TASK_RATE_HZ);
    }
    else if(opt == OS_OPT_TMR_PERIODIC)
    {
        rt_flag = RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER;
        if(period == 0)/*检查period参数是否合法*/
        {
            *p_err = OS_ERR_TMR_INVALID_PERIOD;
            return;
        }   
        /*RTT和uCOS-III在定时器时钟源的设计不同,需要进行转换*/        
        time = period * (1000 / OS_CFG_TMR_TASK_RATE_HZ);
    }
    else
    {
        *p_err = OS_ERR_OPT_INVALID;
        return;
    }
    
    CPU_CRITICAL_ENTER();
    p_tmr->State          = (OS_STATE           )OS_TMR_STATE_STOPPED;     /* Initialize the timer fields             */
    p_tmr->Type           = (OS_OBJ_TYPE        )OS_OBJ_TYPE_TMR; 
#if (OS_CFG_DBG_EN > 0u)
    p_tmr->NamePtr        = (CPU_CHAR          *)p_name;    
#endif
    p_tmr->CallbackPtr    = (OS_TMR_CALLBACK_PTR)p_callback;
    p_tmr->CallbackPtrArg = (void              *)p_callback_arg;
    p_tmr->Opt            = (OS_OPT             )opt;
    p_tmr->Remain         = (OS_TICK            )0;
    p_tmr->Period         = (OS_TICK            )period;
    p_tmr->Dly            = (OS_TICK            )dly;
    p_tmr->_dly           = (OS_TICK            )dly;   /* 该变量为兼容层内部使用,用于带有延迟的周期延时          */
    p_tmr->_set_dly       = (OS_TICK            )0;     /* 该变量为兼容层内部使用,用于配合3.08版本中OSTmrSet函数  */
    p_tmr->_set_period    = (OS_TICK            )0;     /* 该变量为兼容层内部使用,用于配合3.08版本中OSTmrSet函数  */
#if OS_CFG_DBG_EN > 0u
    p_tmr->DbgPrevPtr     = (OS_TMR            *)0;
    p_tmr->DbgPrevPtr     = (OS_TMR            *)0;
#endif
    CPU_CRITICAL_EXIT();
    
    if(p_tmr->Opt==OS_OPT_TMR_PERIODIC && p_tmr->_dly && p_tmr->Period)
    {
        /*带有延迟的周期延时，先延时一次延迟部分，该部分延时完毕后，周期部分由回调函数重新装填*/
        time2 = p_tmr->Dly * (1000 / OS_CFG_TMR_TASK_RATE_HZ);
        rt_timer_init(&p_tmr->Tmr,
                      (const char*)p_name,
                      OS_TmrCallback,
                      p_tmr,/*将p_tmr作为参数传到回调函数中*/
                      time2,
                      RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);          
    }
    else
    {
        rt_timer_init(&p_tmr->Tmr,
                      (const char*)p_name,
                      OS_TmrCallback,
                      p_tmr,/*将p_tmr作为参数传到回调函数中*/
                      time,
                      rt_flag);        
    }
 
    *p_err = OS_ERR_NONE;/*rt_timer_init没有返回错误码*/

    CPU_CRITICAL_ENTER();
#if OS_CFG_DBG_EN > 0u
    OS_TmrDbgListAdd(p_tmr);
#endif
    OSTmrQty++;                                             /* Keep track of the number of timers created             */                  
    CPU_CRITICAL_EXIT();
}

/*
************************************************************************************************************************
*                                                   DELETE A TIMER
*
* Description: This function is called by your application code to delete a timer.
*
* Arguments  : p_tmr          Is a pointer to the timer to stop and delete.
*
*              p_err          Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                                 OS_ERR_NONE
*                                 OS_ERR_OS_NOT_RUNNING       If uC/OS-III is not running yet
*                                 OS_ERR_OBJ_TYPE             'p_tmr' is not pointing to a timer
*                                 OS_ERR_ILLEGAL_DEL_RUN_TIME If you are trying to delete the timer after you called
*                                                                OSStart()
*                                 OS_ERR_TMR_INVALID          'p_tmr' is a NULL pointer
*                                 OS_ERR_TMR_ISR              if the function was called from an ISR
*                                 OS_ERR_TMR_INACTIVE         if the timer was not created
*                                 OS_ERR_TMR_INVALID_STATE    the timer is in an invalid state
*
* Returns    : DEF_TRUE   if the timer was deleted
*              DEF_FALSE  if not or upon an error
************************************************************************************************************************
*/

#if OS_CFG_TMR_DEL_EN > 0u
CPU_BOOLEAN  OSTmrDel (OS_TMR  *p_tmr,
                       OS_ERR  *p_err)
{
    rt_err_t rt_err;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (DEF_FALSE);
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == OS_TRUE) {
       *p_err = OS_ERR_ILLEGAL_DEL_RUN_TIME;
        return (OS_FALSE);
    }
#endif
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u   
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_TMR_ISR;
        return DEF_FALSE;
    }  
#endif 
    
#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                     /* Is the kernel running?                               */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (OS_FALSE);
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u    
    if(p_tmr == RT_NULL)/*检查指针是否为空*/
    {
        *p_err = OS_ERR_TMR_INVALID;
        return DEF_FALSE;
    }
#endif
    
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为定时器*/
    if(rt_object_get_type(&p_tmr->Tmr.parent) != RT_Object_Class_Timer)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return DEF_FALSE;       
    }
#endif

    switch (p_tmr->State) {
        case OS_TMR_STATE_RUNNING:
        case OS_TMR_STATE_STOPPED:                              /* Timer has not started or ...                           */
        case OS_TMR_STATE_COMPLETED:                            /* ... timer has completed the ONE-SHOT time              */
            break;
        
        case OS_TMR_STATE_UNUSED:                               /* Already deleted                                        */
            *p_err = OS_ERR_TMR_INACTIVE;
            return (DEF_FALSE);
        
        default:
            *p_err = OS_ERR_TMR_INVALID_STATE;
            return (DEF_FALSE);            
    }
        
    rt_err = rt_timer_detach(&p_tmr->Tmr);    
    *p_err = rt_err_to_ucosiii(rt_err);
    if(rt_err == RT_EOK)
    {
        CPU_CRITICAL_ENTER();
#if OS_CFG_DBG_EN > 0u
        OS_TmrDbgListRemove(p_tmr);
#endif
        OSTmrQty--;
        OS_TmrClr(p_tmr);    
        CPU_CRITICAL_EXIT(); 
        return DEF_TRUE;
    }
    else
    {
        return DEF_FALSE;
    }
}
#endif

/*
************************************************************************************************************************
*                                    GET HOW MUCH TIME IS LEFT BEFORE A TIMER EXPIRES
*
* Description: This function is called to get the number of ticks before a timer times out.
*
* Arguments  : p_tmr    Is a pointer to the timer to obtain the remaining time from.
*
*              p_err    Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                           OS_ERR_NONE
*                           OS_ERR_OBJ_TYPE           'p_tmr' is not pointing to a timer
*                           OS_ERR_OS_NOT_RUNNING     If uC/OS-III is not running yet
*                           OS_ERR_TMR_INVALID        'p_tmr' is a NULL pointer
*                           OS_ERR_TMR_ISR            if the call was made from an ISR
*                           OS_ERR_TMR_INACTIVE       'p_tmr' points to a timer that is not active
*                           OS_ERR_TMR_INVALID_STATE  the timer is in an invalid state
*
* Returns    : The time remaining for the timer to expire.  The time represents 'timer' increments.  In other words, if
*              OS_TmrTask() is signaled every 1/10 of a second then the returned value represents the number of 1/10 of
*              a second remaining before the timer expires.
************************************************************************************************************************
*/

OS_TICK  OSTmrRemainGet (OS_TMR  *p_tmr,
                         OS_ERR  *p_err)
{
    OS_TICK  remain;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_TICK)0);
    }
#endif   
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u    
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_TMR_ISR;
        return 0;
    }  
#endif   

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                     /* Is the kernel running?                               */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (0u);
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u    
    if(p_tmr == RT_NULL)/*检查指针是否为空*/
    {
        *p_err = OS_ERR_TMR_INVALID;
        return 0;
    }
#endif
    
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为定时器*/
    if(rt_object_get_type(&p_tmr->Tmr.parent) != RT_Object_Class_Timer)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return 0;       
    }
#endif
    switch (p_tmr->State) {
        case OS_TMR_STATE_RUNNING:
            *p_err = OS_ERR_NONE;
            remain = p_tmr->Tmr.timeout_tick - rt_tick_get();
            CPU_CRITICAL_ENTER();
            p_tmr->Remain = remain;
            CPU_CRITICAL_EXIT();
        
        case OS_TMR_STATE_STOPPED:
             if (p_tmr->Opt == OS_OPT_TMR_PERIODIC) {
                 if (p_tmr->Dly == 0u) {
                     remain = p_tmr->Period;
                 } else {
                     remain = p_tmr->Dly;
                 }
             } else {
                 remain = p_tmr->Dly;
             }
             CPU_CRITICAL_ENTER();
             p_tmr->Remain = remain;
             CPU_CRITICAL_EXIT();
             *p_err = OS_ERR_NONE;
             
        case OS_TMR_STATE_COMPLETED: 
            *p_err = OS_ERR_NONE;
            remain = 0;
        
        case OS_TMR_STATE_UNUSED:
            *p_err = OS_ERR_TMR_INACTIVE;
            remain = 0;
        
        default:
            *p_err = OS_ERR_TMR_INVALID_STATE;
            remain = 0;
    }
    
    return (remain);
}

/*
************************************************************************************************************************
*                                                    SET A TIMER
*
* Description: This function is called by your application code to set a timer.
*
* Arguments  : p_tmr           Is a pointer to a timer control block
*
*              dly             Initial delay.
*                              If the timer is configured for ONE-SHOT mode, this is the timeout used
*                              If the timer is configured for PERIODIC mode, this is the first timeout to wait for
*                              before the timer starts entering periodic mode
*
*              period          The 'period' being repeated for the timer.
*                              If you specified 'OS_OPT_TMR_PERIODIC' as an option, when the timer expires, it will
*                              automatically restart with the same period.
*
*              p_callback      Is a pointer to a callback function that will be called when the timer expires.  The
*                              callback function must be declared as follows:
*
*                                  void  MyCallback (OS_TMR *p_tmr, void *p_arg);
*
*              p_callback_arg  Is an argument (a pointer) that is passed to the callback function when it is called.
*
*              p_err           Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                                 OS_ERR_NONE                    The timer was configured as expected
*                                 OS_ERR_OBJ_TYPE                If the object type is invalid
*                                 OS_ERR_OS_NOT_RUNNING          If uC/OS-III is not running yet
*                                 OS_ERR_TMR_INVALID             If 'p_tmr' is a NULL pointer or invalid option
*                                 OS_ERR_TMR_INVALID_CALLBACK    you specified an invalid callback for a periodic timer
*                                 OS_ERR_TMR_INVALID_DLY         You specified an invalid delay
*                                 OS_ERR_TMR_INVALID_PERIOD      You specified an invalid period
*                                 OS_ERR_TMR_ISR                 If the call was made from an ISR
*
* Returns    : none
*
* Note(s)    : 1) This function can be called on a running timer. The change to the delay and period will only
*                 take effect after the current period or delay has passed. Change to the callback will take
*                 effect immediately.
************************************************************************************************************************
*/

void  OSTmrSet (OS_TMR               *p_tmr,
                OS_TICK               dly,
                OS_TICK               period,
                OS_TMR_CALLBACK_PTR   p_callback,
                void                 *p_callback_arg,
                OS_ERR               *p_err)
{
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if (OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u)
    if (OSIntNestingCtr > 0u) {                                 /* See if trying to call from an ISR                    */
       *p_err = OS_ERR_TMR_ISR;
        return;
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                     /* Is the kernel running?                               */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return;
    }
#endif

#if (OS_CFG_ARG_CHK_EN > 0u)
    if (p_tmr == (OS_TMR *)0) {                                 /* Validate 'p_tmr'                                     */
       *p_err = OS_ERR_TMR_INVALID;
        return;
    }
#endif

#if (OS_CFG_OBJ_TYPE_CHK_EN > 0u)
    if(rt_object_get_type(&p_tmr->Tmr.parent) != RT_Object_Class_Timer)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return;       
    }
#endif

#if (OS_CFG_ARG_CHK_EN > 0u)
    switch (p_tmr->Opt) {
        case OS_OPT_TMR_PERIODIC:
             if (period == 0u) {
                *p_err = OS_ERR_TMR_INVALID_PERIOD;
                 return;
             }

             if (p_callback == (OS_TMR_CALLBACK_PTR)0) {        /* No point in a periodic timer without a callback      */
                *p_err = OS_ERR_TMR_INVALID_CALLBACK;
                 return;
             }
             break;

        case OS_OPT_TMR_ONE_SHOT:
             if (dly == 0u) {
                *p_err = OS_ERR_TMR_INVALID_DLY;
                 return;
             }
             break;

        default:
            *p_err = OS_ERR_TMR_INVALID;
             return;
    }
#endif

    CPU_CRITICAL_ENTER();
    p_tmr->_set_dly       = dly;                                   /* Convert Timer Delay  to ticks                     */
    p_tmr->_set_period    = period;                                /* Convert Timer Period to ticks                     */
    p_tmr->CallbackPtr    = p_callback;
    p_tmr->CallbackPtrArg = p_callback_arg;
    CPU_CRITICAL_EXIT();
    
   *p_err                 = OS_ERR_NONE;
}


/*
************************************************************************************************************************
*                                                   START A TIMER
*
* Description: This function is called by your application code to start a timer.
*
* Arguments  : p_tmr    Is a pointer to an OS_TMR
*
*              p_err    Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                           OS_ERR_NONE
*                           OS_ERR_OBJ_TYPE            if 'p_tmr' is not pointing to a timer
*                           OS_ERR_OS_NOT_RUNNING      If uC/OS-III is not running yet
*                           OS_ERR_TMR_INVALID
*                           OS_ERR_TMR_INACTIVE        if the timer was not created
*                           OS_ERR_TMR_INVALID_STATE   the timer is in an invalid state
*                           OS_ERR_TMR_ISR             if the call was made from an ISR
*
* Returns    : DEF_TRUE      is the timer was started
*              DEF_FALSE     if not or upon an error
*
************************************************************************************************************************
*/

CPU_BOOLEAN  OSTmrStart (OS_TMR  *p_tmr,
                         OS_ERR  *p_err)
{
    rt_err_t rt_err;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (DEF_FALSE);
    }
#endif
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u    
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_TMR_ISR;
        return DEF_FALSE;
    }  
#endif  
    
#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                     /* Is the kernel running?                               */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (OS_FALSE);
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u   
    if(p_tmr == RT_NULL)/*检查指针是否为空*/
    {
        *p_err = OS_ERR_TMR_INVALID;
        return DEF_FALSE;
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u    
    /*判断内核对象是否为定时器*/
    if(rt_object_get_type(&p_tmr->Tmr.parent) != RT_Object_Class_Timer)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return DEF_FALSE;       
    }
#endif
    
    switch (p_tmr->State) {
        case OS_TMR_STATE_RUNNING:                              /* Restart the timer                                      */
        case OS_TMR_STATE_STOPPED:                              /* Start the timer                                        */
        case OS_TMR_STATE_COMPLETED:
            break;
        
        case OS_TMR_STATE_UNUSED:                               /* Timer not created                                      */
            *p_err = OS_ERR_TMR_INACTIVE;
            return (DEF_FALSE);
        
        default:
            *p_err = OS_ERR_TMR_INVALID_STATE;
            return (DEF_FALSE);            
    }
    
    rt_err = rt_timer_start(&p_tmr->Tmr);
    *p_err = rt_err_to_ucosiii(rt_err);
    if(rt_err == RT_EOK)
    {
        CPU_CRITICAL_ENTER();
        if (p_tmr->Dly == 0u) {
            p_tmr->Remain = p_tmr->Period;
        } else {
            p_tmr->Remain = p_tmr->Dly;
        }
        p_tmr->Match = p_tmr->Tmr.timeout_tick;
        p_tmr->State = OS_TMR_STATE_RUNNING;
        CPU_CRITICAL_EXIT();
        return DEF_TRUE;
    }
    else
    {
        return DEF_FALSE;
    }
}

/*
************************************************************************************************************************
*                                           FIND OUT WHAT STATE A TIMER IS IN
*
* Description: This function is called to determine what state the timer is in:
*
*                  OS_TMR_STATE_UNUSED     the timer has not been created
*                  OS_TMR_STATE_STOPPED    the timer has been created but has not been started or has been stopped
*                  OS_TMR_STATE_COMPLETED  the timer is in ONE-SHOT mode and has completed it's timeout
*                  OS_TMR_STATE_RUNNING    the timer is currently running
*
* Arguments  : p_tmr    Is a pointer to the desired timer
*
*              p_err    Is a pointer to an error code.  '*p_err' will contain one of the following:
*
*                           OS_ERR_NONE
*                           OS_ERR_OBJ_TYPE           if 'p_tmr' is not pointing to a timer
*                           OS_ERR_OS_NOT_RUNNING     If uC/OS-III is not running yet
*                           OS_ERR_TMR_INVALID        'p_tmr' is a NULL pointer
*                           OS_ERR_TMR_INVALID_STATE  if the timer is not in a valid state
*                           OS_ERR_TMR_ISR            if the call was made from an ISR
*
* Returns    : The current state of the timer (see description).
************************************************************************************************************************
*/

OS_STATE  OSTmrStateGet (OS_TMR  *p_tmr,
                         OS_ERR  *p_err)
{
    OS_STATE  state;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (OS_TMR_STATE_UNUSED);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* See if trying to call from an ISR                      */
       *p_err = OS_ERR_TMR_ISR;
        return (OS_TMR_STATE_UNUSED);
    }
#endif

#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                     /* Is the kernel running?                               */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (OS_TMR_STATE_UNUSED);
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u
    if (p_tmr == (OS_TMR *)0) {
       *p_err = OS_ERR_TMR_INVALID;
        return (OS_TMR_STATE_UNUSED);
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    if(rt_object_get_type(&p_tmr->Tmr.parent) != RT_Object_Class_Timer){/* Make sure timer was created                            */
       *p_err = OS_ERR_OBJ_TYPE;
        return (OS_TMR_STATE_UNUSED);
    }
#endif

    CPU_CRITICAL_ENTER();
    state = p_tmr->State;
    CPU_CRITICAL_EXIT();
    switch (state) {
        case OS_TMR_STATE_UNUSED:
        case OS_TMR_STATE_STOPPED:
        case OS_TMR_STATE_COMPLETED:
        case OS_TMR_STATE_RUNNING:
            *p_err = OS_ERR_NONE;
             break;

        default:
            *p_err = OS_ERR_TMR_INVALID_STATE;
             break;
    }
    
    return (state);
}

/*
************************************************************************************************************************
*                                                    STOP A TIMER
*
* Description: This function is called by your application code to stop a timer.
*
* Arguments  : p_tmr          Is a pointer to the timer to stop.
*
*              opt           Allows you to specify an option to this functions which can be:
*
*                               OS_OPT_TMR_NONE            Do nothing special but stop the timer
*                               OS_OPT_TMR_CALLBACK        Execute the callback function, pass it the callback argument
*                                                          specified when the timer was created.
*                               OS_OPT_TMR_CALLBACK_ARG    Execute the callback function, pass it the callback argument
*                                                          specified in THIS function call
*
*              callback_arg  Is a pointer to a 'new' callback argument that can be passed to the callback function
*                               instead of the timer's callback argument.  In other words, use 'callback_arg' passed in
*                               THIS function INSTEAD of p_tmr->OSTmrCallbackArg
*
*              p_err         Is a pointer to an error code.  '*p_err' will contain one of the following:
*                               OS_ERR_NONE
*                               OS_ERR_OBJ_TYPE            if 'p_tmr' is not pointing to a timer
*                               OS_ERR_OPT_INVALID         if you specified an invalid option for 'opt'
*                               OS_ERR_OS_NOT_RUNNING      If uC/OS-III is not running yet
*                               OS_ERR_TMR_INACTIVE        if the timer was not created
*                               OS_ERR_TMR_INVALID         'p_tmr' is a NULL pointer
*                               OS_ERR_TMR_INVALID_STATE   the timer is in an invalid state
*                               OS_ERR_TMR_ISR             if the function was called from an ISR
*                               OS_ERR_TMR_NO_CALLBACK     if the timer does not have a callback function defined
*                               OS_ERR_TMR_STOPPED         if the timer was already stopped
*
* Returns    : DEF_TRUE       If we stopped the timer (if the timer is already stopped, we also return DEF_TRUE)
*              DEF_FALSE      If not
************************************************************************************************************************
*/

CPU_BOOLEAN  OSTmrStop (OS_TMR  *p_tmr,
                        OS_OPT   opt,
                        void    *p_callback_arg,
                        OS_ERR  *p_err)
{
    rt_err_t rt_err;
    OS_TMR_CALLBACK_PTR  p_fnct;    
    OS_ERR err;
    
    CPU_SR_ALLOC();
    
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return (DEF_FALSE);
    }
#endif
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u   
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)/*检查是否在中断中运行*/
    {
        *p_err = OS_ERR_TMR_ISR;
        return DEF_FALSE;
    }  
#endif   
    
#if (OS_CFG_INVALID_OS_CALLS_CHK_EN > 0u)
    if (OSRunning != OS_STATE_OS_RUNNING) {                     /* Is the kernel running?                               */
       *p_err = OS_ERR_OS_NOT_RUNNING;
        return (OS_FALSE);
    }
#endif
    
#if OS_CFG_ARG_CHK_EN > 0u    
    if(p_tmr == RT_NULL)/*检查指针是否为空*/
    {
        *p_err = OS_ERR_TMR_INVALID;
        return DEF_FALSE;
    }
#endif
    
#if OS_CFG_OBJ_TYPE_CHK_EN > 0u
    /*判断内核对象是否为定时器*/
    if(rt_object_get_type(&p_tmr->Tmr.parent) != RT_Object_Class_Timer)
    {
        *p_err = OS_ERR_OBJ_TYPE;
        return DEF_FALSE;       
    }
#endif
     
    rt_err = rt_timer_stop(&p_tmr->Tmr);
    if(rt_err == -RT_ERROR)
    {
        *p_err = OS_ERR_TMR_STOPPED;/*返回-RT_ERROR 时则说明已经处于停止状态*/
        return DEF_FALSE;
    }
    
    CPU_CRITICAL_ENTER();
    p_tmr->State = OS_TMR_STATE_STOPPED;/*标记目前定时器状态已经停止*/
    p_tmr->Remain  = 0u;
    p_fnct = p_tmr->CallbackPtr;                                      /* Execute callback function ...           */
    CPU_CRITICAL_EXIT();
    
    switch (p_tmr->State) {
        case OS_TMR_STATE_RUNNING:
            break;
        
        case OS_TMR_STATE_COMPLETED:                                  /* Timer has already completed the ONE-SHOT or  */
        case OS_TMR_STATE_STOPPED:                                    /* ... timer has not started yet.               */
            *p_err = OS_ERR_TMR_STOPPED;    
            return (DEF_TRUE);
        
        case OS_TMR_STATE_UNUSED:                                     /* Timer was not created                        */
            *p_err = OS_ERR_TMR_INACTIVE;
             return (DEF_FALSE);
        
        default:
            *p_err = OS_ERR_TMR_INVALID_STATE;
             return (DEF_FALSE);        
    }
    
    
    *p_err = OS_ERR_NONE;
    OSSchedLock(&err);
    switch (opt) 
    {
        case OS_OPT_TMR_CALLBACK:
            if (p_fnct != (OS_TMR_CALLBACK_PTR)0) {      /* ... if available                        */
                (*p_fnct)((void *)p_tmr, p_tmr->CallbackPtrArg);        /* Use callback arg when timer was created */
            } else {
                *p_err = OS_ERR_TMR_NO_CALLBACK;
            }            
            break;
            
        case OS_OPT_TMR_CALLBACK_ARG:
              if (p_fnct != (OS_TMR_CALLBACK_PTR)0) {
                (*p_fnct)((void *)p_tmr, p_callback_arg);               /* .. using the 'callback_arg' provided in call */
              } else {
                 *p_err = OS_ERR_TMR_NO_CALLBACK;
              }            
            break;
              
        case OS_OPT_TMR_NONE:
            break;

         default:
             OSSchedUnlock(&err);
            *p_err = OS_ERR_OPT_INVALID;
             return (DEF_FALSE);        
    }        
    OSSchedUnlock(&err);
    
    if(*p_err != OS_ERR_NONE)
    {
        return DEF_FALSE;
    }
    else
    {
        return DEF_TRUE;
    }
}

/*
************************************************************************************************************************
*                                                 CLEAR TIMER FIELDS
*
* Description: This function is called to clear all timer fields.
*
* Argument(s): p_tmr    is a pointer to the timer to clear
*              -----
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrClr (OS_TMR  *p_tmr)
{
    p_tmr->State          = OS_TMR_STATE_UNUSED;            /* Clear timer fields                                     */
    p_tmr->Type           = OS_OBJ_TYPE_NONE;
#if (OS_CFG_DBG_EN > 0u)
    p_tmr->NamePtr        = (CPU_CHAR          *)((void *)"?TMR");
#endif
    p_tmr->CallbackPtr    = (OS_TMR_CALLBACK_PTR)0;
    p_tmr->CallbackPtrArg = (void              *)0;
    p_tmr->Opt            = (OS_OPT             )0;
    p_tmr->Match          = (OS_TICK            )0;
    p_tmr->Remain         = (OS_TICK            )0;
    p_tmr->Period         = (OS_TICK            )0;
    p_tmr->Dly            = (OS_TICK            )0;
    p_tmr->_set_dly       = (OS_TICK            )0; /* 该变量为兼容层内部使用,用于配合3.08版本中OSTmrSet函数  */
    p_tmr->_set_period    = (OS_TICK            )0; /* 该变量为兼容层内部使用,用于配合3.08版本中OSTmrSet函数  */
    p_tmr->_dly           = (OS_TICK            )0; /* 该变量为兼容层内部使用,用于带有延迟的周期延时          */
#if OS_CFG_DBG_EN > 0u    
    p_tmr->DbgPrevPtr     = (OS_TMR            *)0;
    p_tmr->DbgNextPtr     = (OS_TMR            *)0;
#endif
}

/*
************************************************************************************************************************
*                                         ADD/REMOVE TIMER TO/FROM DEBUG TABLE
*
* Description: These functions are called by uC/OS-III to add or remove a timer to/from a timer debug table.
*
* Arguments  : p_tmr     is a pointer to the timer to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u
void  OS_TmrDbgListAdd (OS_TMR  *p_tmr)
{ 
    p_tmr->DbgPrevPtr               = (OS_TMR *)0;
    if (OSTmrDbgListPtr == (OS_TMR *)0) {
        p_tmr->DbgNextPtr           = (OS_TMR *)0;
    } else {
        p_tmr->DbgNextPtr           =  OSTmrDbgListPtr;
        OSTmrDbgListPtr->DbgPrevPtr =  p_tmr;
    }
    OSTmrDbgListPtr                 =  p_tmr;
}



void  OS_TmrDbgListRemove (OS_TMR  *p_tmr)
{
    OS_TMR  *p_tmr_next;
    OS_TMR  *p_tmr_prev;


    p_tmr_prev = p_tmr->DbgPrevPtr;
    p_tmr_next = p_tmr->DbgNextPtr;

    if (p_tmr_prev == (OS_TMR *)0) {
        OSTmrDbgListPtr = p_tmr_next;
        if (p_tmr_next != (OS_TMR *)0) {
            p_tmr_next->DbgPrevPtr = (OS_TMR *)0;
        }
        p_tmr->DbgNextPtr = (OS_TMR *)0;

    } else if (p_tmr_next == (OS_TMR *)0) {
        p_tmr_prev->DbgNextPtr = (OS_TMR *)0;
        p_tmr->DbgPrevPtr      = (OS_TMR *)0;

    } else {
        p_tmr_prev->DbgNextPtr =  p_tmr_next;
        p_tmr_next->DbgPrevPtr =  p_tmr_prev;
        p_tmr->DbgNextPtr      = (OS_TMR *)0;
        p_tmr->DbgPrevPtr      = (OS_TMR *)0;
    }
}
#endif

/*
************************************************************************************************************************
*                                             INITIALIZE THE TIMER MANAGER
*
* Description: This function is called by OSInit() to initialize the timer manager module.
*
* Argument(s): p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE
*                           OS_ERR_TMR_STK_INVALID       if you didn't specify a stack for the timer task
*                           OS_ERR_TMR_STK_SIZE_INVALID  if you didn't allocate enough space for the timer stack
*                           OS_ERR_PRIO_INVALID          if you specified the same priority as the idle task
*                           OS_ERR_xxx                   any error code returned by OSTaskCreate()
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_TmrInit (OS_ERR  *p_err)
{
    OSTmrQty        = (OS_OBJ_QTY)0;
#if OS_CFG_DBG_EN > 0u
    OSTmrDbgListPtr = (OS_TMR   *)0;
#endif
}


/*
************************************************************************************************************************
*                                                   内部回调函数
*
* Description: 由于RT-Thread的定时器回调函数参数只有一个，而uCOS-III的定时器回调函数参数有两个，因此需要
*              先由RTT调用内部回调函数，再由内部回调函数调用uCOS-III的回调函数，以此完成参数的转换。
************************************************************************************************************************
*/
void OS_TmrCallback(void *p_ara)
{
    OS_TMR *p_tmr;
    OS_ERR err;
    char* nameptr;
    OS_TMR_CALLBACK_PTR callback;
    void * arg;
    OS_OPT opt;
    OS_TICK dly;
    OS_TICK period;
    
    CPU_SR_ALLOC();
    
    p_tmr = (OS_TMR*)p_ara;
    
#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u    
    if(OSIntNestingCtr > (OS_NESTING_CTR)0)                 /* 检查是否在中断中运行                                 */
    {
        RT_DEBUG_LOG(OS_CFG_DBG_EN,("uCOS-III的定时器是在任务中运行的,不可以在RTT的Hard模式下运行\n"));
        return;
    }
#endif
    
    if(p_tmr->Opt==OS_OPT_TMR_PERIODIC && p_tmr->_dly && p_tmr->Period)
    {
        /*带有延迟的周期延时，延迟延时已经完毕，开始进行正常周期延时*/
        CPU_CRITICAL_ENTER();
        p_tmr->_dly = 0;/*延迟部分清零，防止再进入本条件分支语句*/
        p_tmr->Tmr.init_tick = p_tmr->Period * (1000 / OS_CFG_TMR_TASK_RATE_HZ);
        p_tmr->Tmr.timeout_tick = rt_tick_get() + p_tmr->Tmr.init_tick;
        p_tmr->Tmr.parent.flag |= RT_TIMER_FLAG_PERIODIC;/*定时器设置为周期模式*/
        CPU_CRITICAL_EXIT();
        rt_timer_start(&(p_tmr->Tmr));/*开启定时器*/
    } 

    if(p_tmr->Opt == OS_OPT_TMR_ONE_SHOT)
    {
        CPU_CRITICAL_ENTER();
        p_tmr->State = OS_TMR_STATE_COMPLETED;
        p_tmr->Remain = 0;
        CPU_CRITICAL_EXIT();
    }
    else if (p_tmr->Opt == OS_OPT_TMR_PERIODIC)
    {
        CPU_CRITICAL_ENTER();
        /*重新设定下一次定时器的参数*/
        p_tmr->Match = rt_tick_get() + p_tmr->Tmr.init_tick;
        p_tmr->Remain = p_tmr->Period;
        CPU_CRITICAL_EXIT();
    }
    
    /*调用真正uCOS-III的软件定时器回调函数*/
    OSSchedLock(&err);
    p_tmr->CallbackPtr((void *)p_tmr, p_tmr->CallbackPtrArg);
    OSSchedUnlock(&err);
    
    /*开始处理OSTmrSet函数的设置*/
    if(p_tmr->_set_dly || p_tmr->_set_period)               /* 检查是否调用OSTmrSet函数                             */
    {
        OSTmrStop(p_tmr,OS_OPT_TMR_NONE,0,&err);            /* 停止当前定时器                                       */
       
        nameptr = p_tmr->NamePtr;                           /* 将老定时器的参数保存                                 */
        callback = p_tmr->CallbackPtr;
        arg = p_tmr->CallbackPtrArg;
        opt = p_tmr->Opt;
        dly = p_tmr->_set_dly;
        period = p_tmr->_set_period;
        
        CPU_CRITICAL_ENTER();
        OSTmrDel(p_tmr,&err);                               /* 删除老定时器,_set_dly/_set_period会在此函数中清零    */
        OSTmrCreate(p_tmr, nameptr, dly, period,            /* 创建新定时器,并装填新的参数                          */
                    opt, callback, arg, &err);
        OSTmrStart(p_tmr, &err);                            /* 启动装填新参数的定时器                               */
        CPU_CRITICAL_EXIT();
    }
}

#endif
