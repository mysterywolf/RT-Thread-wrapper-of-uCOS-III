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
*                                            AVR32 AP7000
*                                            IAR Compiler
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

        MODULE  LIB_MEM
        PUBLIC  Mem_Copy


/*
*********************************************************************************************************
*                                      CODE GENERATION DIRECTIVES
*********************************************************************************************************
*/

        RSEG CODE32:CODE:NOROOT(2)


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

#define pdest R12
#define psrc  R11
#define size  R10

Mem_Copy:
        CP.W    pdest, 0
        BRNE    Mem_Copy_1                                      /* return if pdest == NULL.                             */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */

Mem_Copy_1:
        CP.W    psrc, 0
        BRNE    Mem_Copy_2                                      /* return if psrc  == NULL.                             */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */

Mem_Copy_2:
        CP.W    size, 0
        BRNE    Mem_Copy_3                                      /* return if size  == 0.                                */
        MOV     PC, LR                                          /* Restore Program Counter (return)                     */

Mem_Copy_3:
        PREF    psrc[0]
        MOV     pdest, R12

Mem_Copy_Chk_32:                                                /* If less than 32 bytes, copy byte-by-byte.            */
        CP.W    size, 32
        BRGE    Mem_Copy_More_31

        SUB     size, 1
        RETLT   R12
Mem_Copy_01_1:
        LD.UB   R8, psrc++
        ST.B    pdest++, R8
        SUB     size, 1
        BRGE    Mem_Copy_01_1
        RETAL   R12

Mem_Copy_More_31:
        PUSHM   R0-R7, LR

Chk_Align_32:                                                   /* Check if both dest & src 32-bit aligned.             */
        MOV     R8, psrc
        ANDL    R8, 31, COH
        BRNE    Mem_Copy_Unaligned_src
        MOV     R8, pdest
        ANDL    R8, 3, COH
        BRNE    Mem_Copy_Unaligned_dest

Mem_Copy_Aligned:
        SUB     size, 32
        BRLT    Mem_Copy_Chk_16
Mem_Copy_32_1:                                                 /* Copy 32 bytes at a time                              */
        LDM     psrc, R0-R7
        SUB     psrc, -32
        STM     pdest, R0-R7
        SUB     pdest, -32
        SUB     size, 32
        BRGE    Mem_Copy_32_1

Mem_Copy_Chk_16:
        SUB     size, -16
        BRLT    Mem_Copy_01_2
Mem_Copy_16_1:                                                  /* Copy 16 more bytes if possible                       */
        LDM     psrc, r0-r3
        SUB     psrc, -16
        SUB     size, 16
        STM     pdest, r0-r3
        SUB     pdest, -16

Mem_Copy_01_2:                                                  /* Copy remaining byte-by-byte.                         */
        NEG     size
        ADD     PC, PC, size << 2                               /* Jump to remaining copies position.                   */
        REPT    15
        LD.UB   R0, psrc++
        ST.B    pdest++, R0
        ENDR

        POPM    R0-R7, PC


Mem_Copy_Unaligned_src:
        RSUB    R8, R8, 32                                      /* Make src cacheline-aligned. R8 = (psrc & 31)         */
        SUB     size, R8
Mem_Copy_01_3:
        LD.UB   R0, psrc++
        ST.B    pdest++, R0
        SUB     R8, 1
        BRNE    Mem_Copy_01_3

Chk_Align_dest:                                                 /* Check if dest 32-bit aligned.                        */
        PREF    psrc[0]
        MOV     R8, 3
        TST     pdest, R8
        BREQ    Mem_Copy_Aligned

Mem_Copy_Unaligned_dest:                                        /* Bad performance, src is aligned but dest is not.     */
        SUB     size, 4
        BRLT    Mem_Copy_01_4
Mem_Copy_04_1:
        LD.W    R0, psrc++
        ST.W    pdest++, R0
        SUB     size, 4
        BRGE    Mem_Copy_04_1

Mem_Copy_01_4:
        NEG     size
        ADD     PC, PC, size << 2
        REPT    3
        LD.UB   R0, psrc++
        ST.B    pdest++, R0
        ENDR

        POPM    R0-R7, PC

        ENDMOD


        END

