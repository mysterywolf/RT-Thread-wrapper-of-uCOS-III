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
#include <stdlib.h>

#ifndef RT_USING_IDLE_HOOK
#error "μCOS-III兼容层必须开启要求RT_USING_IDLE_HOOK宏定义"
#endif

#ifndef RT_USING_TIMER_SOFT
#warning "RT_USING_TIMER_SOFT宏没有开启,μCOS-III兼容层将无法启用软件定时器,如果您确认这么做的话请将本警告注释掉"
#endif

OS_ERR _err_rtt_to_ucosiii(rt_err_t rt_err)
{
    int rt_err2 = abs((int)rt_err);/*RTT返回的错误码都是带负号的*/
    switch(rt_err2)
    {
        /*以下RTT错误码可以用原版uCOS-III错误码进行兼容*/
        case RT_EOK: /* 无错误       */
            return OS_ERR_NONE;
        case RT_ETIMEOUT:/* 超时错误 */
            return OS_ERR_TIMEOUT;
        case RT_EINVAL:/* 非法参数   */
            return OS_ERR_OPT_INVALID;        
        case RT_EFULL:/* 资源已满,该参数仅在IPC中使用*/
            return OS_ERR_Q_MAX;        
        /*
        由于uCOS-III的错误码分类较细，而RTT的错误码分类较为笼统，
        以下RTT错误码使用uCOS-III的错误码很难准确描述
        因此将针对RTT的错误码重新定义(新增)uCOS-III的错误码
        */
        case RT_ERROR:/* 普通错误    */
            return OS_ERR_RT_ERROR;
        case RT_EEMPTY:/* 无资源     */
            return OS_ERR_RT_EEMPTY;
        case RT_ENOMEM:/* 无内存     */
            return OS_ERR_RT_ENOMEM;
        case RT_ENOSYS:/* 系统不支持 */
            return OS_ERR_RT_ENOSYS;
        case RT_EBUSY:/* 系统忙      */
            return OS_ERR_RT_EBUSY;
        case RT_EIO:/* IO 错误       */
            return OS_ERR_RT_EIO;
        case RT_EINTR:/* 中断系统调用*/
            return OS_ERR_RT_EINTR;

        default:
            return OS_ERR_RT_ERROR;
    }
}
