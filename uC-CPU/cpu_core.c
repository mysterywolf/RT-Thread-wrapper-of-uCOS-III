/*
 * Copyright (c) 2021, Meco Jianting Man <jiantingman@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-15     Meco Man     the first verion
 */
/*
*********************************************************************************************************
*                                               uC/CPU
*                                    CPU CONFIGURATION & PORT LAYER
*
*                    Copyright 2004-2020 Silicon Laboratories Inc. www.silabs.com
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
*********************************************************************************************************
*                                                uC/CPU
*                                    CPU CONFIGURATION & PORT LAYER
*
*                          (c) Copyright 2004-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/CPU is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           CORE CPU MODULE
*
* Filename      : cpu_core.c
* Version       : V1.30.00
* Programmer(s) : SR
*                 ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    CPU_CORE_MODULE
#include  "cpu_core.h"

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if    (CPU_CFG_NAME_EN   == DEF_ENABLED)                           /* ---------------- CPU NAME FNCTS ---------------- */
static  void        CPU_NameInit         (void);
#endif


/*
*********************************************************************************************************
*                                             CPU_Init()
*
* Description : (1) Initialize CPU module :
*
*                   (a) Initialize CPU timestamps
*                   (b) Initialize CPU interrupts disabled time measurements
*                   (c) Initialize CPU host name
*
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Your Product's Application.
*
*               This function is a CPU initialization function & MAY be called by application/
*               initialization function(s).
*
* Note(s)     : (2) CPU_Init() MUST be called ... :
*
*                   (a) ONLY ONCE from a product's application; ...
*                   (b) BEFORE product's application calls any core CPU module function(s)
*
*               (3) The following initialization functions MUST be sequenced as follows :
*
*                   (a) CPU_TS_Init()           SHOULD precede ALL calls to other CPU timestamp functions
*
*                   (b) CPU_IntDisMeasInit()    SHOULD precede ALL calls to CPU_CRITICAL_ENTER()/CPU_CRITICAL_EXIT()
*                                                   & other CPU interrupts disabled time measurement functions
*********************************************************************************************************
*/

void  CPU_Init (void)
{
                                                                /* ------------------ INIT CPU NAME ------------------- */
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
     CPU_NameInit();
#endif
}

/*
*********************************************************************************************************
*                                         CPU_SW_Exception()
*
* Description : Trap unrecoverable software exception.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) CPU_SW_Exception() deadlocks the current code execution -- whether multi-tasked/
*                   -processed/-threaded or single-threaded -- when the current code execution cannot
*                   gracefully recover or report a fault or exception condition.
*
*                   See also 'cpu_core.h  CPU_SW_EXCEPTION()  Note #1'.
*********************************************************************************************************
*/

void  CPU_SW_Exception (void)
{
    while (DEF_ON) {
        ;
    }
}

/*
*********************************************************************************************************
*                                            CPU_NameClr()
*
* Description : Clear CPU Name.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_NameInit(),
*               Application.
*
*               This function is a CPU module application programming interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
void  CPU_NameClr (void)
{
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();
    Mem_Clr((void     *)&CPU_Name[0],
            (CPU_SIZE_T) CPU_CFG_NAME_SIZE);
    CPU_CRITICAL_EXIT();
}
#endif


/*
*********************************************************************************************************
*                                            CPU_NameGet()
*
* Description : Get CPU host name.
*
* Argument(s) : p_name      Pointer to an ASCII character array that will receive the return CPU host
*                               name ASCII string from this function (see Note #1).
*
*               p_err       Pointer to variable that will receive the return error code from this function :
*
*                               CPU_ERR_NONE                    CPU host name successfully returned.
*                               CPU_ERR_NULL_PTR                Argument 'p_name' passed a NULL pointer.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a CPU module application programming interface (API) function & MAY
*               be called by application function(s).
*
* Note(s)     : (1) The size of the ASCII character array that will receive the return CPU host name
*                   ASCII string :
*
*                   (a) MUST   be greater than or equal to the current CPU host name's ASCII string
*                           size including the terminating NULL character;
*                   (b) SHOULD be greater than or equal to CPU_CFG_NAME_SIZE
*********************************************************************************************************
*/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
void  CPU_NameGet (CPU_CHAR  *p_name,
                   CPU_ERR   *p_err)
{
    CPU_SR_ALLOC();


    if (p_err == (CPU_ERR *)0) {
        CPU_SW_EXCEPTION(;);
    }

    if (p_name == (CPU_CHAR *)0) {
       *p_err = CPU_ERR_NULL_PTR;
        return;
    }

    CPU_CRITICAL_ENTER();
   (void)Str_Copy_N(p_name,
                   &CPU_Name[0],
                    CPU_CFG_NAME_SIZE);
    CPU_CRITICAL_EXIT();

   *p_err = CPU_ERR_NONE;
}
#endif


/*
*********************************************************************************************************
*                                            CPU_NameSet()
*
* Description : Set CPU host name.
*
* Argument(s) : p_name      Pointer to CPU host name to set.
*
*               p_err       Pointer to variable that will receive the return error code from this function :
*
*                               CPU_ERR_NONE                    CPU host name successfully set.
*                               CPU_ERR_NULL_PTR                Argument 'p_name' passed a NULL pointer.
*                               CPU_ERR_NAME_SIZE               Invalid CPU host name size (see Note #1).
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
*               This function is a CPU module application programming interface (API) function & MAY be
*               called by application function(s).
*
* Note(s)     : (1) 'p_name' ASCII string size, including the terminating NULL character, MUST be less
*                    than or equal to CPU_CFG_NAME_SIZE.
*********************************************************************************************************
*/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
void  CPU_NameSet (const  CPU_CHAR  *p_name,
                          CPU_ERR   *p_err)
{
    CPU_SIZE_T  len;
    CPU_SR_ALLOC();


    if (p_err == (CPU_ERR *)0) {
        CPU_SW_EXCEPTION(;);
    }

    if (p_name == (const CPU_CHAR *)0) {
       *p_err = CPU_ERR_NULL_PTR;
        return;
    }

    len = Str_Len_N(p_name,
                    CPU_CFG_NAME_SIZE);
    if (len < CPU_CFG_NAME_SIZE) {                              /* If       cfg name len < max name size, ...           */
        CPU_CRITICAL_ENTER();
       (void)Str_Copy_N(&CPU_Name[0],                           /* ... copy cfg name to CPU host name.                  */
                         p_name,
                         CPU_CFG_NAME_SIZE);
        CPU_CRITICAL_EXIT();
       *p_err = CPU_ERR_NONE;

    } else {
       *p_err = CPU_ERR_NAME_SIZE;
    }
}
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           CPU_NameInit()
*
* Description : Initialize CPU Name.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
static  void  CPU_NameInit (void)
{
    CPU_NameClr();
}
#endif
