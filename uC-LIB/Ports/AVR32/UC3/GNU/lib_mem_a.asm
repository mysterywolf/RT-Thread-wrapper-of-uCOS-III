/*
*********************************************************************************************************
*                                               uC/LIB
*                                       Custom Library Modules
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
*
*                                     STANDARD MEMORY OPERATIONS
*
*                                              AVR32 UC3
*                                            GNU Compiler
*
* Filename : lib_mem_a.asm
* Version  : V1.39.00
*********************************************************************************************************
* Note(s)  : (1) NO compiler-supplied standard library functions are used in library or product software.
*
*                (a) ALL standard library functions are implemented in the custom library modules :
*
*                    (1) \<Custom Library Directory>\lib*.*
*
*                    (2) \<Custom Library Directory>\Ports\<cpu>\<compiler>\lib*_a.*
*
*                          where
*                                  <Custom Library Directory>      directory path for custom library software
*                                  <cpu>                           directory name for specific processor (CPU)
*                                  <compiler>                      directory name for specific compiler
*
*                (b) Product-specific library functions are implemented in individual products.
*
*            (2) Assumes AVR32 CPU mode configured for Big Endian.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           PUBLIC FUNCTIONS
*********************************************************************************************************
*/

        .file   "LIB_MEM"
        .global  Mem_Copy


/*
*********************************************************************************************************
*                                      CODE GENERATION DIRECTIVES
*********************************************************************************************************
*/

        .section .text, "ax"


/*
*********************************************************************************************************
*                                             Mem_Copy()
*
* Description : Copy data octets from one buffer to another buffer.
*
* Argument(s) : pdest       Pointer to destination memory buffer.
*
*               psrc        Pointer to source      memory buffer.
*
*               size        Number of data buffer octets to copy.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Null copies allowed (i.e. 0-octet size).
*
*               (2) Memory buffers NOT checked for overlapping.
*
*               (3) Modulo arithmetic is used to determine whether a memory buffer starts on a 'CPU_ALIGN'
*                   address boundary.
*********************************************************************************************************
*/

/*
void  Mem_Copy (void        *pdest,       ;  ==>  R12
                void        *psrc,        ;  ==>  R11
                CPU_SIZE_T   size)        ;  ==>  R10
*/


Mem_Copy:
Mem_Copy_Arg_Chk:
        CP.W    R12, 0
        RETEQ   R12                                             /* return if pdest == NULL.                             */
        CP.W    R11, 0
        RETEQ   R12                                             /* return if psrc  == NULL.                             */

Mem_Copy_Start:
        MOV     R9, R12

Mem_Copy_Chk_32:                                                /* If less than 32 bytes, copy byte-by-byte.            */
        CP.W    R10, 32
        BRGE    Mem_Copy_Chk_Align

Mem_Copy_Unaligned:                                             /* Copy byte-by-byte.                                   */
        SUB     R10, 16
        BRLT    Mem_Copy_01_1

Mem_Copy_16_1:                                                  /* Copy 16-bytes at a time.                             */
        .rept   16
        LD.UB   R8, R11++
        ST.B    R9++, R8
        .endr
        SUB     R10, 16
        BRGE    Mem_Copy_16_1

Mem_Copy_01_1:                                                  /* Copy remaining bytes.                                */
        NEG     R10
        ADD     PC, PC, R10 << 2                                /* Jump to remaining copy position.                     */
        .rept   15
        LD.UB   R8, R11++
        ST.B    R9++, R8
        .endr

        RETAL   R12

Mem_Copy_Chk_Align:
        PUSHM   R0-R7
                                                                /* Check if both dest & src are 32-bit aligned.         */
        MOV     R5, R9
        ANDL    R5, 3, COH
        MOV     R6, R11
        ANDL    R6, 3, COH
        CP.W    R5, R6
        BREQ    Mem_Copy_Aligned_32
                                                                /* Check if both dest & src are 16-bit aligned.         */
        ANDL    R5, 1
        ANDL    R6, 1
        EOR     R6, R5
        BREQ    Mem_Copy_Aligned_16
                                                                /* dest & src are not aligned.                          */
        POPM    R0-R7
        BRAL    Mem_Copy_Unaligned

Mem_Copy_Aligned_32:
        CP.W    R5, 0
        BREQ    Mem_Copy_Chk_128_1
                                                                /* Align dest & src to next 32-bit boundary.            */
        RSUB    R6, R5, 4
        SUB     R10, R6
        ADD     PC, PC, R5 << 2
        .rept   3
        LD.UB   R8, R11++
        ST.B    R9++, R8
        .endr

Mem_Copy_Chk_128_1:
        SUB     R10, 4*4*8
        BRLT    Mem_Copy_Chk_16_1
Mem_Copy_128_1:                                                 /* Copy 128-bytes at a time.                            */
        .rept   4
        LDM     R11++, R0-R7
        STM     R9,  R0-R7
        SUB     R9,  -4*8
        .endr
        SUB     R10,  4*4*8
        BRGE    Mem_Copy_128_1

Mem_Copy_Chk_16_1:
        SUB     R10, -(4*4*8 - 16)
        BRLT    Mem_Copy_01_2
Mem_Copy_16_2:                                                  /* Copy 16-bytes at a time.                             */
        LDM     R11++, R0-R3
        STM     R9,  R0-R3
        SUB     R9,  -16
        SUB     R10, 16
        BRGE    Mem_Copy_16_2

Mem_Copy_01_2:                                                  /* Copy remaining bytes.                                */
        NEG     R10
        ADD     PC, PC, R10 << 2                                /* Jump to remaining copy position.                     */
        .rept   15
        LD.UB   R8, R11++
        ST.B    R9++, R8
        .endr

        POPM    R0-R7
        RETAL   R12


Mem_Copy_Aligned_16:
        CP.W    R5, 1
        BRNE    Mem_Copy_Chk_64_2
                                                                /* Align dest & src to next 16-bit boundary.            */
        LD.UB   R0, R11++
        ST.B    R9++, R0
        SUB     R10, 1

Mem_Copy_Chk_64_2:
        SUB     R10, 64*2
        BRLT    Mem_Copy_Chk_16_2
Mem_Copy_128_2:                                                 /* Copy 128-bytes at a time.                            */
        .rept   64
        LD.UH   R0, R11++
        ST.H    R9++, R0
        .endr
        SUB     R10,  64*2
        BRGE    Mem_Copy_128_2

Mem_Copy_Chk_16_2:
        SUB     R10, -(64*2 - 16)
        BRLT    Mem_Copy_01_2
Mem_Copy_16_3:                                                  /* Copy 16-bytes at a time.                             */
        .rept   8
        LD.UH   R0, R11++
        ST.H    R9++, R0
        .endr
        SUB     R10, 8*2
        BRGE    Mem_Copy_16_3
        BRAL    Mem_Copy_01_2                                   /* Copy remaining bytes.                                */

        .end

