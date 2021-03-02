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
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      CPU CONFIGURATION DEFINES
*
* Filename      : cpu_def.h
* Version       : V1.30.00
* Programmer(s) : ITJ
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) This CPU definition header file is protected from multiple pre-processor inclusion
*               through use of the CPU definition module present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  CPU_DEF_MODULE_PRESENT
#define  CPU_DEF_MODULE_PRESENT


/*
*********************************************************************************************************
*                                   CORE CPU MODULE VERSION NUMBER
*
* Note(s) : (1) (a) The core CPU module software version is denoted as follows :
*
*                       Vx.yy.zz
*
*                           where
*                                   V               denotes 'Version' label
*                                   x               denotes     major software version revision number
*                                   yy              denotes     minor software version revision number
*                                   zz              denotes sub-minor software version revision number
*
*               (b) The software version label #define is formatted as follows :
*
*                       ver = x.yyzz * 100 * 100
*
*                           where
*                                   ver             denotes software version number scaled as an integer value
*                                   x.yyzz          denotes software version number, where the unscaled integer
*                                                       portion denotes the major version number & the unscaled
*                                                       fractional portion denotes the (concatenated) minor
*                                                       version numbers
*********************************************************************************************************
*/

#define  CPU_CORE_VERSION                              13000u   /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                       CPU WORD CONFIGURATION
*
* Note(s) : (1) Configure CPU_CFG_ADDR_SIZE & CPU_CFG_DATA_SIZE in 'cpu.h' with CPU's word sizes :
*
*                   CPU_WORD_SIZE_08             8-bit word size
*                   CPU_WORD_SIZE_16            16-bit word size
*                   CPU_WORD_SIZE_32            32-bit word size
*                   CPU_WORD_SIZE_64            64-bit word size
*
*           (2) Configure CPU_CFG_ENDIAN_TYPE in 'cpu.h' with CPU's data-word-memory order :
*
*               (a) CPU_ENDIAN_TYPE_BIG         Big-   endian word order (CPU words' most  significant
*                                                                         octet @ lowest memory address)
*               (b) CPU_ENDIAN_TYPE_LITTLE      Little-endian word order (CPU words' least significant
*                                                                         octet @ lowest memory address)
*********************************************************************************************************
*/

                                                        /* ---------------------- CPU WORD SIZE ----------------------- */
#define  CPU_WORD_SIZE_08                          1    /*  8-bit word size (in octets).                                */
#define  CPU_WORD_SIZE_16                          2    /* 16-bit word size (in octets).                                */
#define  CPU_WORD_SIZE_32                          4    /* 32-bit word size (in octets).                                */
#define  CPU_WORD_SIZE_64                          8    /* 64-bit word size (in octets).                                */


                                                        /* ------------------ CPU WORD-ENDIAN ORDER ------------------- */
#define  CPU_ENDIAN_TYPE_NONE                      0u
#define  CPU_ENDIAN_TYPE_BIG                       1u   /* Big-   endian word order (see Note #1a).                     */
#define  CPU_ENDIAN_TYPE_LITTLE                    2u   /* Little-endian word order (see Note #1b).                     */


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

                                                        /* ------------------ CPU STACK GROWTH ORDER ------------------ */
#define  CPU_STK_GROWTH_NONE                       0u
#define  CPU_STK_GROWTH_LO_TO_HI                   1u   /* CPU stk incs towards higher mem addrs (see Note #1a).        */
#define  CPU_STK_GROWTH_HI_TO_LO                   2u   /* CPU stk decs towards lower  mem addrs (see Note #1b).        */


#endif                                                  /* End of CPU def module include.                               */

