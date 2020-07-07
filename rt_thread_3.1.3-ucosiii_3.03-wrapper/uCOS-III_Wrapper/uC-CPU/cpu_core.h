/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-07     Meco Man     the first verion
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
* Filename      : cpu_core.h
* Version       : V1.30.00
* Programmer(s) : SR
*                 ITJ
*********************************************************************************************************
* Note(s)       : (1) Assumes the following versions (or more recent) of software modules are included in 
*                     the project build :
*
*                     (a) uC/LIB V1.35.00
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) This core CPU header file is protected from multiple pre-processor inclusion through use of 
*               the  core CPU module present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  CPU_CORE_MODULE_PRESENT                                /* See Note #1.                                         */
#define  CPU_CORE_MODULE_PRESENT


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   CPU_CORE_MODULE
#define  CPU_CORE_EXT
#else
#define  CPU_CORE_EXT  extern
#endif


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>

/*
*********************************************************************************************************
*                                          CPU CONFIGURATION
*
* Note(s) : (1) The following pre-processor directives correctly configure CPU parameters.  DO NOT MODIFY.
*
*           (2) CPU timestamp timer feature is required for :
*
*               (a) CPU timestamps
*               (b) CPU interrupts disabled time measurement
*
*               See also 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                      & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1'.
*********************************************************************************************************
*/

#ifdef   CPU_CFG_TS_EN
#undef   CPU_CFG_TS_EN
#endif


#if    ((CPU_CFG_TS_32_EN == DEF_ENABLED) || \
        (CPU_CFG_TS_64_EN == DEF_ENABLED))
#define  CPU_CFG_TS_EN                          DEF_ENABLED
#else
#define  CPU_CFG_TS_EN                          DEF_DISABLED
#endif

#if    ((CPU_CFG_TS_EN == DEF_ENABLED) || \
(defined(CPU_CFG_INT_DIS_MEAS_EN)))
#define  CPU_CFG_TS_TMR_EN                      DEF_ENABLED
#else
#define  CPU_CFG_TS_TMR_EN                      DEF_DISABLED
#endif


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      CPU TIMESTAMP DATA TYPES
*
* Note(s) : (1) CPU timestamp timer data type defined to the binary-multiple of 8-bit octets as configured 
*               by 'CPU_CFG_TS_TMR_SIZE' (see 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #2').
*********************************************************************************************************
*/

typedef  CPU_INT32U  CPU_TS32;
typedef  CPU_INT64U  CPU_TS64;

typedef  CPU_TS32    CPU_TS;                                    /* Req'd for backwards-compatibility.                   */


/*
*********************************************************************************************************
*                                           CPU_VAL_UNUSED()
*
* Description : 
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : #### various.
*
* Note(s)     : none.
*********************************************************************************************************
*/


#define  CPU_VAL_UNUSED(val)        ((void)&(val));


#define  CPU_VAL_IGNORED(val)       CPU_VAL_UNUSED(val)


/*
*********************************************************************************************************
*                                          CPU_TYPE_CREATE()
*
* Description : Creates a generic type value.
*
* Argument(s) : char_1      1st ASCII character to create generic type value.
*
*               char_2      2nd ASCII character to create generic type value.
*
*               char_3      3rd ASCII character to create generic type value.
*
*               char_4      4th ASCII character to create generic type value.
*
* Return(s)   : 32-bit generic type value.
*
* Caller(s)   : various.
*
* Note(s)     : (1) (a) Generic type values should be #define'd with large, non-trivial values to trap 
*                       & discard invalid/corrupted objects based on type value.
*
*                       In other words, by assigning large, non-trivial values to valid objects' type 
*                       fields; the likelihood that an object with an unassigned &/or corrupted type 
*                       field will contain a value is highly improbable & therefore the object itself 
*                       will be trapped as invalid.
*
*                   (b) (1) CPU_TYPE_CREATE()  creates a 32-bit type value from four values.
*
*                       (2) Ideally, generic type values SHOULD be created from 'CPU_CHAR' characters to 
*                           represent ASCII string abbreviations of the specific object types.  Memory 
*                           displays of object type values will display the specific object types with 
*                           their chosen ASCII names.
*
*                           Examples :
*
*                               #define  FILE_TYPE  CPU_TYPE_CREATE('F', 'I', 'L', 'E')
*                               #define  BUF_TYPE   CPU_TYPE_CREATE('B', 'U', 'F', ' ')
*********************************************************************************************************
*/

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  CPU_TYPE_CREATE(char_1, char_2, char_3, char_4)        (((CPU_INT32U)((CPU_INT08U)(char_1)) << (3u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_2)) << (2u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_3)) << (1u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_4)) << (0u * DEF_OCTET_NBR_BITS)))

#else

#if    ((CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_64) || \
        (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_32))
#define  CPU_TYPE_CREATE(char_1, char_2, char_3, char_4)        (((CPU_INT32U)((CPU_INT08U)(char_1)) << (0u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_2)) << (1u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_3)) << (2u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_4)) << (3u * DEF_OCTET_NBR_BITS)))


#elif   (CPU_CFG_DATA_SIZE   == CPU_WORD_SIZE_16)
#define  CPU_TYPE_CREATE(char_1, char_2, char_3, char_4)        (((CPU_INT32U)((CPU_INT08U)(char_1)) << (2u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_2)) << (3u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_3)) << (0u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_4)) << (1u * DEF_OCTET_NBR_BITS)))

#else                                                           /* Dflt CPU_WORD_SIZE_08.                               */
#define  CPU_TYPE_CREATE(char_1, char_2, char_3, char_4)        (((CPU_INT32U)((CPU_INT08U)(char_1)) << (3u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_2)) << (2u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_3)) << (1u * DEF_OCTET_NBR_BITS)) | \
                                                                 ((CPU_INT32U)((CPU_INT08U)(char_4)) << (0u * DEF_OCTET_NBR_BITS)))
#endif
#endif



#endif                                                          /* End of CPU core module include.                      */
