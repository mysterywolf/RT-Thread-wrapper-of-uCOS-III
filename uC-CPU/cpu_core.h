/*
 * Copyright (c) 2021, Meco Jianting Man <jiantingman@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-07     Meco Man     the first verion
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

#include  "cpu.h"
#include  <lib_def.h>
#include  "cpu_cfg.h"

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
#include  <lib_mem.h>
#include  <lib_str.h>
#endif


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
*                                           CPU ERROR CODES
*********************************************************************************************************
*/

typedef enum cpu_err {

    CPU_ERR_NONE                            =         0u,
    CPU_ERR_NULL_PTR                        =        10u,

    CPU_ERR_NAME_SIZE                       =      1000u,

    CPU_ERR_TS_FREQ_INVALID                 =      2000u

} CPU_ERR;


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
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

#if    (CPU_CFG_NAME_EN   == DEF_ENABLED)
CPU_CORE_EXT  CPU_CHAR         CPU_Name[CPU_CFG_NAME_SIZE];     /* CPU host name.                                       */
#endif

/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         CPU_SW_EXCEPTION()
*
* Description : Trap unrecoverable software exception.
*
* Argument(s) : err_rtn_val     Error type &/or value of the calling function to return (see Note #2b).
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : (1) CPU_SW_EXCEPTION() deadlocks the current code execution -- whether multi-tasked/
*                   -processed/-threaded or single-threaded -- when the current code execution cannot
*                   gracefully recover or report a fault or exception condition.
*
*                   Example CPU_SW_EXCEPTION() call :
*
*                       void  Fnct (CPU_ERR  *p_err)
*                       {
*                           :
*
*                           if (p_err == (CPU_ERR *)0) {        If 'p_err' NULL, cannot return error ...
*                               CPU_SW_EXCEPTION(;);            ... so trap invalid argument exception.
*                           }
*
*                           :
*                       }
*
*                   See also 'cpu_core.c  CPU_SW_Exception()  Note #1'.
*
*               (2) (a) CPU_SW_EXCEPTION()  MAY be developer-implemented to output &/or handle any error or
*                       exception conditions; but since CPU_SW_EXCEPTION() is intended to trap unrecoverable
*                       software  conditions, it is recommended that developer-implemented versions prevent
*                       execution of any code following calls to CPU_SW_EXCEPTION() by deadlocking the code
*                       (see Note #1).
*
*                           Example CPU_SW_EXCEPTION() :
*
*                               #define  CPU_SW_EXCEPTION(err_rtn_val)      do {                         \
*                                                                               Log(__FILE__, __LINE__); \
*                                                                               CPU_SW_Exception();      \
*                                                                           } while (0)
*
*                   (b) (1) However, if execution of code following calls to CPU_SW_EXCEPTION() is required
*                           (e.g. for automated testing); it is recommended that the last statement in
*                           developer-implemented versions be to return from the current function to prevent
*                           possible software exception(s) in the current function from triggering CPU &/or
*                           hardware exception(s).
*
*                           Example CPU_SW_EXCEPTION() :
*
*                               #define  CPU_SW_EXCEPTION(err_rtn_val)      do {                         \
*                                                                               Log(__FILE__, __LINE__); \
*                                                                               return  err_rtn_val;     \
*                                                                           } while (0)
*
*                           (A) Note that 'err_rtn_val' in the return statement MUST NOT be enclosed in
*                               parentheses.  This allows CPU_SW_EXCEPTION() to return from functions that
*                               return 'void', i.e. NO return type or value (see also Note #2b2A).
*
*                       (2) In order for CPU_SW_EXCEPTION() to return from functions with various return
*                           types/values, each caller function MUST pass an appropriate error return type
*                           & value to CPU_SW_EXCEPTION().
*
*                           (A) Note that CPU_SW_EXCEPTION()  MUST NOT be passed any return type or value
*                               for functions that return 'void', i.e. NO return type or value; but SHOULD
*                               instead be passed a single semicolon.  This prevents possible compiler
*                               warnings that CPU_SW_EXCEPTION() is passed too few arguments.  However,
*                               the compiler may warn that CPU_SW_EXCEPTION() does NOT prevent creating
*                               null statements on lines with NO other code statements.
*
*                           Example CPU_SW_EXCEPTION() calls :
*
*                               void  Fnct (CPU_ERR  *p_err)
*                               {
*                                   :
*
*                                   if (p_err == (CPU_ERR *)0) {
*                                       CPU_SW_EXCEPTION(;);            Exception macro returns NO value
*                                   }                                       (see Note #2b2A)
*
*                                   :
*                               }
*
*                               CPU_BOOLEAN  Fnct (CPU_ERR  *p_err)
*                               {
*                                   :
*
*                                   if (p_err == (CPU_ERR *)0) {
*                                       CPU_SW_EXCEPTION(DEF_FAIL);     Exception macro returns 'DEF_FAIL'
*                                   }
*
*                                   :
*                               }
*
*                               OBJ  *Fnct (CPU_ERR  *p_err)
*                               {
*                                   :
*
*                                   if (p_err == (CPU_ERR *)0) {
*                                       CPU_SW_EXCEPTION((OBJ *)0);     Exception macro returns NULL 'OBJ *'
*                                   }
*
*                                   :
*                               }
*
*********************************************************************************************************
*/

#ifndef  CPU_SW_EXCEPTION                                                       /* See Note #2.                         */
#define  CPU_SW_EXCEPTION(err_rtn_val)              do {                    \
                                                        CPU_SW_Exception(); \
                                                    } while (0)
#endif


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

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void             CPU_Init                 (void);
void             CPU_SW_Exception         (void);



#if (CPU_CFG_NAME_EN == DEF_ENABLED)                                    /* -------------- CPU NAME FNCTS -------------- */
void             CPU_NameClr              (void);

void             CPU_NameGet              (       CPU_CHAR  *p_name,
                                                  CPU_ERR   *p_err);

void             CPU_NameSet              (const  CPU_CHAR  *p_name,
                                                  CPU_ERR   *p_err);
#endif


#endif                                                          /* End of CPU core module include.                      */
