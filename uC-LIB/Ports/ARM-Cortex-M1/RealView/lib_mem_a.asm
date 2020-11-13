;********************************************************************************************************
;                                               uC/LIB
;                                       Custom Library Modules
;
;                    Copyright 2004-2020 Silicon Laboratories Inc. www.silabs.com
;
;                                 SPDX-License-Identifier: APACHE-2.0
;
;               This software is subject to an open source license and is distributed by
;                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
;                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
;
;********************************************************************************************************


;********************************************************************************************************
;
;                                     STANDARD MEMORY OPERATIONS
;
;                                           ARM-Cortex-M1
;                                     RealView Development Suite
;                            RealView Microcontroller Development Kit (MDK)
;                                      ARM Developer Suite (ADS)
;                                           Keil uVision
;
; Filename : lib_mem_a.asm
; Version  : V1.39.00
;********************************************************************************************************
; Note(s)  : (1) NO compiler-supplied standard library functions are used in library or product software.
;
;                (a) ALL standard library functions are implemented in the custom library modules :
;
;                    (1) \<Custom Library Directory>\lib*.*
;
;                    (2) \<Custom Library Directory>\Ports\<cpu>\<compiler>\lib*_a.*
;
;                          where
;                                  <Custom Library Directory>      directory path for custom library software
;                                  <cpu>                           directory name for specific processor (CPU)
;                                  <compiler>                      directory name for specific compiler
;
;                (b) Product-specific library functions are implemented in individual products.
;
;            (2) Assumes ARM CPU mode configured for Little Endian.
;********************************************************************************************************


;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

        EXPORT  Mem_Copy


;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8


;********************************************************************************************************
;                                             Mem_Copy()
;
; Description : Copy data octets from one buffer to another buffer.
;
; Argument(s) : pdest       Pointer to destination memory buffer.
;
;               psrc        Pointer to source      memory buffer.
;
;               size        Number of data buffer octets to copy.
;
; Return(s)   : none.
;
; Caller(s)   : Application.
;
; Note(s)     : (1) Null copies allowed (i.e. 0-octet size).
;
;               (2) Memory buffers NOT checked for overlapping.
;
;               (3) Modulo arithmetic is used to determine whether a memory buffer starts on a 'CPU_ALIGN'
;                   address boundary.
;
;               (4) ARM Cortex-M3 processors use a subset of the ARM Thumb-2 instruction set which does
;                   NOT support 16-bit conditional branch instructions but ONLY supports 8-bit conditional
;                   branch instructions.
;
;                   Therefore, branches exceeding 8-bit, signed, relative offsets :
;
;                   (a) CANNOT be implemented with     conditional branches; but ...
;                   (b) MUST   be implemented with non-conditional branches.
;********************************************************************************************************

; void  Mem_Copy (void        *pdest,       ;  ==>  R0
;                 void        *psrc,        ;  ==>  R1
;                 CPU_SIZE_T   size)        ;  ==>  R2

Mem_Copy
        CMP         R0, #0
        BNE         Mem_Copy_1
        BX          LR                      ; return if pdest == NULL

Mem_Copy_1
        CMP         R1, #0
        BNE         Mem_Copy_2
        BX          LR                      ; return if psrc  == NULL

Mem_Copy_2
        CMP         R2, #0
        BNE         Mem_Copy_3
        BX          LR                      ; return if size  == 0

Mem_Copy_3
        STMFD       SP!, {R3-R7}            ; save registers on stack

Chk_Align_32                                ; check if both dest & src 32-bit aligned

        LSLS        R3, R0, #30
        LSRS        R3, R3, #30

        LSLS        R4, R1, #30
        LSRS        R4, R4, #30

        CMP         R3, R4
        BNE         Chk_Align_16            ; not 32-bit aligned, check for 16-bit alignment

        CMP         R3, #0
        BEQ         Copy_32_1

Pre_Copy_1
        CMP         R3, #4                  ; copy 1-2-3 bytes (to align to the next 32-bit boundary)
        BCS         Copy_32_1               ; start real 32-bit copy
        CMP         R2, #1                  ; check if any more data to copy
        BCS         Pre_Copy_1_Cont
        B           Mem_Copy_END            ;           no more data to copy (see Note #4b)

Pre_Copy_1_Cont
        LDRB        R4, [R1]
        STRB        R4, [R0]
        ADDS        R1, #1
        ADDS        R0, #1
        ADDS        R3, #1
        SUBS        R2, #1
        B           Pre_Copy_1


Chk_Align_16                                ; check if both dest & src 16-bit aligned
        LSLS        R3, R0, #31
        LSRS        R3, R3, #31

        LSLS        R4, R1, #31
        LSRS        R4, R4, #31

        CMP         R3, R4
        BEQ         Pre_Copy_2
        B           Copy_08_1               ; not 16-bit aligned, start 8-bit copy (see Note #4b)

Pre_Copy_2
        CMP         R3, #1                  ; copy 1 byte (to align to the next 16-bit boundary)
        BCC         Copy_16_1               ; start real 16-bit copy

        LDRB        R4, [R1]
        STRB        R4, [R0]
        ADDS        R1, #1
        ADDS        R0, #1
        SUBS        R3, #1
        SUBS        R2, #1
        B           Pre_Copy_2


Copy_32_1
        CMP         R2, #(04*05*09)         ; Copy 9 chunks of 5 32-bit words (180 octets per loop)
        BCC         Copy_32_2
        LDM         R1!, {R3-R7}
        STM         R0!, {R3-R7}
        LDM         R1!, {R3-R7}
        STM         R0!, {R3-R7}
        LDM         R1!, {R3-R7}
        STM         R0!, {R3-R7}
        LDM         R1!, {R3-R7}
        STM         R0!, {R3-R7}
        LDM         R1!, {R3-R7}
        STM         R0!, {R3-R7}
        LDM         R1!, {R3-R7}
        STM         R0!, {R3-R7}
        LDM         R1!, {R3-R7}
        STM         R0!, {R3-R7}
        LDM         R1!, {R3-R7}
        STM         R0!, {R3-R7}
        LDM         R1!, {R3-R7}
        STM         R0!, {R3-R7}
        SUBS        R2, #(04*05*09)
        B           Copy_32_1

Copy_32_2
        CMP         R2, #(04*05*01)         ; Copy chunks of 5 32-bit words (20 octets per loop)
        BCC         Copy_32_3
        LDM         R1!, {R3-R7}
        STM         R0!, {R3-R7}
        SUBS        R2, #(04*05*01)
        B           Copy_32_2

Copy_32_3
        CMP         R2, #(04*01*01)         ; Copy remaining 32-bit words
        BCC         Copy_16_1
        LDM         R1!, {R3}
        STM         R0!, {R3}
        SUBS        R2, #(04*01*01)
        B           Copy_32_3

Copy_16_1
        CMP         R2, #(02*01*16)         ; Copy chunks of 16 16-bit words (32 octets per loop)
        BCC         Copy_16_2
        LDRH        R3, [R1,  #0]
        STRH        R3, [R0,  #0]
        LDRH        R3, [R1,  #2]
        STRH        R3, [R0,  #2]
        LDRH        R3, [R1,  #4]
        STRH        R3, [R0,  #4]
        LDRH        R3, [R1,  #6]
        STRH        R3, [R0,  #6]
        LDRH        R3, [R1,  #8]
        STRH        R3, [R0,  #8]
        LDRH        R3, [R1, #10]
        STRH        R3, [R0, #10]
        LDRH        R3, [R1, #12]
        STRH        R3, [R0, #12]
        LDRH        R3, [R1, #14]
        STRH        R3, [R0, #14]
        LDRH        R3, [R1, #16]
        STRH        R3, [R0, #16]
        LDRH        R3, [R1, #18]
        STRH        R3, [R0, #18]
        LDRH        R3, [R1, #20]
        STRH        R3, [R0, #20]
        LDRH        R3, [R1, #22]
        STRH        R3, [R0, #22]
        LDRH        R3, [R1, #24]
        STRH        R3, [R0, #24]
        LDRH        R3, [R1, #26]
        STRH        R3, [R0, #26]
        LDRH        R3, [R1, #28]
        STRH        R3, [R0, #28]
        LDRH        R3, [R1, #30]
        STRH        R3, [R0, #30]
        ADDS        R1, #32
        ADDS        R0, #32
        SUBS        R2, #(02*01*16)
        B           Copy_16_1

Copy_16_2
        CMP         R2, #(02*01*01)         ; Copy remaining 16-bit words
        BCC         Copy_08_1
        LDRH        R3, [R1]
        STRH        R3, [R0]
        ADDS        R1, #2
        ADDS        R0, #2
        SUBS        R2, #(02*01*01)
        B           Copy_16_2

Copy_08_1
        CMP         R2, #(01*01*16)         ; Copy chunks of 16 8-bit words (16 octets per loop)
        BCC         Copy_08_2
        LDRB        R3, [R1,  #0]
        STRB        R3, [R0,  #0]
        LDRB        R3, [R1,  #1]
        STRB        R3, [R0,  #1]
        LDRB        R3, [R1,  #2]
        STRB        R3, [R0,  #2]
        LDRB        R3, [R1,  #3]
        STRB        R3, [R0,  #3]
        LDRB        R3, [R1,  #4]
        STRB        R3, [R0,  #4]
        LDRB        R3, [R1,  #5]
        STRB        R3, [R0,  #5]
        LDRB        R3, [R1,  #6]
        STRB        R3, [R0,  #6]
        LDRB        R3, [R1,  #7]
        STRB        R3, [R0,  #7]
        LDRB        R3, [R1,  #8]
        STRB        R3, [R0,  #8]
        LDRB        R3, [R1,  #9]
        STRB        R3, [R0,  #9]
        LDRB        R3, [R1, #10]
        STRB        R3, [R0, #10]
        LDRB        R3, [R1, #11]
        STRB        R3, [R0, #11]
        LDRB        R3, [R1, #12]
        STRB        R3, [R0, #12]
        LDRB        R3, [R1, #13]
        STRB        R3, [R0, #13]
        LDRB        R3, [R1, #14]
        STRB        R3, [R0, #14]
        LDRB        R3, [R1, #15]
        STRB        R3, [R0, #15]
        ADDS        R1, #16
        ADDS        R0, #16
        SUBS        R2, R2, #(01*01*16)
        B           Copy_08_1

Copy_08_2
        CMP         R2, #(01*01*01)         ; Copy remaining 8-bit words
        BCC         Mem_Copy_END
        LDRB        R3, [R1]
        STRB        R3, [R0]
        ADDS        R1, #1
        ADDS        R0, #1
        SUBS        R2, #(01*01*01)
        B           Copy_08_2


Mem_Copy_END
        LDMFD       SP!, {R3-R7}            ; restore registers from stack
        BX          LR                      ; return


        END

