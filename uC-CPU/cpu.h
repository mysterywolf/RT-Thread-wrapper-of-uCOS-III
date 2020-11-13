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
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            CPU PORT FILE
*
*                                            ARM-Cortex-M4
*                                            IAR C Compiler
*
* Filename      : cpu.h
* Version       : V1.30.00
* Programmer(s) : JJL
*                 BAN
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) This CPU header file is protected from multiple pre-processor inclusion through use of 
*               the  CPU module present pre-processor macro definition.
*           (2) RT-Thread支持32位、64位CPU,uCOS-III支持8位、16位、32位但不支持64位CPU,因此当且仅当CPU为
*               32位时,才能将原有项目从uCOS-III兼容到RT-Thread上.
*               当然,如果想在64位CPU上使用uCOS-III接口编程也是可以的(因为底层CPU调用的是RTT的接口)
*********************************************************************************************************
*/

#ifndef  CPU_MODULE_PRESENT                                     /* See Note #1.                                         */
#define  CPU_MODULE_PRESENT

#ifdef __cplusplus
extern  "C" {
#endif

#include <rtdef.h>
#include "cpu_def.h"
#include <rtconfig.h>

/*
*********************************************************************************************************
*                                    CONFIGURE STANDARD DATA TYPES
*
* Note(s) : (1) Configure standard data types according to CPU-/compiler-specifications.
*
*           (2) (a) (1) 'CPU_FNCT_VOID' data type defined to replace the commonly-used function pointer
*                       data type of a pointer to a function which returns void & has no arguments.
*
*                   (2) Example function pointer usage :
*
*                           CPU_FNCT_VOID  FnctName;
*
*                           FnctName();
*
*********************************************************************************************************
*/

typedef            void        CPU_VOID;
typedef            char        CPU_CHAR;                        /*  8-bit character                                     */
typedef       rt_bool_t        CPU_BOOLEAN;                     /*  8-bit boolean or logical                            */
typedef      rt_uint8_t        CPU_INT08U;                      /*  8-bit unsigned integer                              */
typedef       rt_int8_t        CPU_INT08S;                      /*  8-bit   signed integer                              */
typedef     rt_uint16_t        CPU_INT16U;                      /* 16-bit unsigned integer                              */
typedef      rt_int16_t        CPU_INT16S;                      /* 16-bit   signed integer                              */
typedef     rt_uint32_t        CPU_INT32U;                      /* 32-bit unsigned integer                              */
typedef      rt_int32_t        CPU_INT32S;                      /* 32-bit   signed integer                              */
typedef     rt_uint64_t        CPU_INT64U;                      /* 64-bit unsigned integer                              */
typedef      rt_int64_t        CPU_INT64S;                      /* 64-bit   signed integer                              */

typedef            float       CPU_FP32;                        /* 32-bit floating point                                */
typedef            double      CPU_FP64;                        /* 64-bit floating point                                */


typedef  volatile  CPU_INT08U  CPU_REG08;                       /*  8-bit register                                      */
typedef  volatile  CPU_INT16U  CPU_REG16;                       /* 16-bit register                                      */
typedef  volatile  CPU_INT32U  CPU_REG32;                       /* 32-bit register                                      */
typedef  volatile  CPU_INT64U  CPU_REG64;                       /* 64-bit register                                      */


/*
*********************************************************************************************************
*                                       CPU WORD CONFIGURATION
*
* Note(s) : (1) Configure CPU_CFG_ADDR_SIZE, CPU_CFG_DATA_SIZE, & CPU_CFG_DATA_SIZE_MAX with CPU's &/or 
*               compiler's word sizes :
*
*                   CPU_WORD_SIZE_08             8-bit word size
*                   CPU_WORD_SIZE_16            16-bit word size
*                   CPU_WORD_SIZE_32            32-bit word size
*                   CPU_WORD_SIZE_64            64-bit word size
*
*           (2) Configure CPU_CFG_ENDIAN_TYPE with CPU's data-word-memory order :
*
*               (a) CPU_ENDIAN_TYPE_BIG         Big-   endian word order (CPU words' most  significant
*                                                                         octet @ lowest memory address)
*               (b) CPU_ENDIAN_TYPE_LITTLE      Little-endian word order (CPU words' least significant
*                                                                         octet @ lowest memory address)
*********************************************************************************************************
*/

                                                                /* Define  CPU         word sizes (see Note #1) :       */
#define  CPU_CFG_ADDR_SIZE              CPU_WORD_SIZE_32        /* Defines CPU address word size  (in octets).          */
#define  CPU_CFG_DATA_SIZE              CPU_WORD_SIZE_32        /* Defines CPU data    word size  (in octets).          */
#define  CPU_CFG_DATA_SIZE_MAX          CPU_WORD_SIZE_64        /* Defines CPU maximum word size  (in octets).          */

#define  CPU_CFG_ENDIAN_TYPE            CPU_ENDIAN_TYPE_LITTLE  /* Defines CPU data    word-memory order (see Note #2). */


/*
*********************************************************************************************************
*                                 CONFIGURE CPU ADDRESS & DATA TYPES
*********************************************************************************************************
*/

typedef  rt_ubase_t  CPU_ADDR;                                  /* CPU address type based on address bus size.          */
typedef  rt_ubase_t  CPU_DATA;                                  /* CPU data    type based on data    bus size.          */

typedef  CPU_DATA    CPU_ALIGN;                                 /* Defines CPU data-word-alignment size.                */
typedef  CPU_ADDR    CPU_SIZE_T;                                /* Defines CPU standard 'size_t'   size.                */

/*
*********************************************************************************************************
*                                       CPU STACK CONFIGURATION
*
* Note(s) : (1) Configure CPU_CFG_STK_GROWTH in 'cpu.h' with CPU's stack growth order :
*
*               (a) CPU_STK_GROWTH_LO_TO_HI     CPU stack pointer increments to the next higher  stack
*                                                   memory address after data is pushed onto the stack
*               (b) CPU_STK_GROWTH_HI_TO_LO     CPU stack pointer decrements to the next lower   stack
*                                                   memory address after data is pushed onto the stack
*********************************************************************************************************
*/

#if defined(ARCH_CPU_STACK_GROWS_UPWARD)                        /* Defines CPU stack growth order (see Note #1).        */
#define  CPU_CFG_STK_GROWTH     CPU_STK_GROWTH_LO_TO_HI    
#else
#define  CPU_CFG_STK_GROWTH     CPU_STK_GROWTH_HI_TO_LO         
#endif

typedef  rt_ubase_t             CPU_STK;                        /* Defines CPU stack word size (in octets).             */
typedef  CPU_ADDR               CPU_STK_SIZE;                   /* Defines CPU stack      size (in number of CPU_STKs). */


/*
*********************************************************************************************************
*                                   CRITICAL SECTION CONFIGURATION
*
* Note(s) : (1) (a) To save/restore interrupt status, a local variable 'cpu_sr' of type 'CPU_SR' MAY need
*                   to be declared (e.g. if 'CPU_CRITICAL_METHOD_STATUS_LOCAL' method is configured).
*
*                   (1) 'cpu_sr' local variable SHOULD be declared via the CPU_SR_ALLOC() macro which, if 
*                        used, MUST be declared following ALL other local variables.
*
*                        Example :
*
*                           void  Fnct (void)
*                           {
*                               CPU_INT08U  val_08;
*                               CPU_INT16U  val_16;
*                               CPU_INT32U  val_32;
*                               CPU_SR_ALLOC();         MUST be declared after ALL other local variables
*                                   :
*                                   :
*                           }
*
*               (b) Configure 'CPU_SR' data type with the appropriate-sized CPU data type large enough to
*                   completely store the CPU's/compiler's status word.
*********************************************************************************************************
*/

typedef  rt_ubase_t                 CPU_SR;                     /* Defines   CPU status register size (see Note #3b).   */

#define  CPU_SR_ALLOC()             CPU_SR  cpu_sr = (CPU_SR)0
                                                                        /* Save    CPU status word & disable interrupts.*/
#define  CPU_INT_DIS()         do { cpu_sr = CPU_SR_Save();} while (0)
#define  CPU_INT_EN()          do { CPU_SR_Restore(cpu_sr); } while (0) /* Restore CPU status word.                     */


#define  CPU_CRITICAL_ENTER()  do { CPU_INT_DIS(); } while (0)          /* Disable   interrupts.                        */
#define  CPU_CRITICAL_EXIT()   do { CPU_INT_EN();  } while (0)          /* Re-enable interrupts.                        */


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

CPU_SR      CPU_SR_Save      (void);
void        CPU_SR_Restore   (CPU_SR      cpu_sr);

#ifdef __cplusplus
}
#endif

#endif                                                          /* End of CPU module include.                           */
