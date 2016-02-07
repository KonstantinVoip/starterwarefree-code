;******************************************************************************
;
; exceptionhandler.asm - Definitions of exception handlers
;
;******************************************************************************
;
; Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
;
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions
;  are met:
;
;    Redistributions of source code must retain the above copyright
;    notice, this list of conditions and the following disclaimer.
;
;    Redistributions in binary form must reproduce the above copyright
;    notice, this list of conditions and the following disclaimer in the
;    documentation and/or other materials provided with the
;    distribution.
;
;    Neither the name of Texas Instruments Incorporated nor the names of
;    its contributors may be used to endorse or promote products derived
;    from this software without specific prior written permission.
;
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
;  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
;  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
;  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;******************************************************************************
    .cdecls C,LIST,"hw_intc.h"
    .cdecls C,LIST,"soc_AM335x.h"

;************************** Global symbols ************************************
        .global IRQHandler
        .global FIQHandler
        .global AbortHandler
        .global SVC_Handler
        .global UndefInstHandler
        .global DataAbortHandler   ;Pre-fetch abort
        .ref CPUAbortHandler
        .ref fnRAMVectors       
        .ref isr_FIQHandler
        .ref isr_UndefInstHandler
        .ref isr_AbortHandler

_fnRAMVectors:
    .word fnRAMVectors
_CPUAbortHandler:
    .word CPUAbortHandler
_isr_FIQHandler:
    .word isr_FIQHandler
_isr_UndefInstHandler:
    .word isr_UndefInstHandler
_isr_AbortHandler:
    .word isr_AbortHandler



ADDR_SIR_IRQ      .word  SOC_AINTC_REGS + INTC_SIR_IRQ
ADDR_SIR_FIQ      .word  SOC_AINTC_REGS + INTC_SIR_FIQ
ADDR_CONTROL      .word  SOC_AINTC_REGS + INTC_CONTROL
ADDR_THRESHOLD    .word  SOC_AINTC_REGS + INTC_THRESHOLD
ADDR_IRQ_PRIORITY .word  SOC_AINTC_REGS + INTC_IRQ_PRIORITY

MASK_ACTIVE_IRQ   .set  INTC_SIR_IRQ_ACTIVEIRQ
MASK_ACTIVE_FIQ   .set  INTC_SIR_FIQ_ACTIVEFIQ
NEWIRQAGR         .set  INTC_CONTROL_NEWIRQAGR
NEWFIQAGR         .set  INTC_CONTROL_NEWFIQAGR

MASK_SVC_NUM      .set   0xFF000000
MODE_SYS          .set   0x1F
MODE_IRQ          .set   0x12
I_BIT             .set   0x80
  
;**************************** Text Section ************************************
        .text

; This source file is assembled for ARM instructions
        .state32
;******************************************************************************
;*                  Function Definition of SWI Handler
;******************************************************************************    
;
; The SVC Handler switches to system mode if the SVC number is 458752. If the
; SVC number is different, no mode switching will be done. No other SVC are 
; handled here
;
SVC_Handler:
        STMFD    r13!, {r0-r1, r14}       ; Save context in SVC stack
        SUB      r13, r13, #0x4           ; Adjust the stack pointer
        LDR      r0, [r14, #-4]           ; R0 points to SWI instruction
        BIC      r0, r0, #MASK_SVC_NUM    ; Get the SWI number
        CMP      r0, #458752
        MRSEQ    r1, spsr                 ; Copy SPSR
        ORREQ    r1, r1, #0x1F            ; Change the mode to System
        MSREQ    spsr_cf, r1              ; Restore SPSR
        ADD      r13, r13, #0x4           ; Adjust the stack pointer
        LDMFD    r13!, {r0-r1, pc}^       ; Restore registers from IRQ stack

;******************************************************************************
;*                  Function Definition of IRQ Handler
;******************************************************************************    
;
; The IRQ handler jumps to the ISR of highest priority pending IRQ. 
; This handler is a prioritized interrupt handler. The handler is a
; re-entrant IRQ handler. So interrupt service routines are processed
; in system mode to avoid lr_irq corruption
;
IRQHandler:
        SUB      r14, r14, #4             ; Apply lr correction
        STMFD    r13!, {r0-r3, r12, r14}  ; Save context in IRQ stack
        MRS      r12, spsr                ; Copy spsr
        VMRS     r1,  fpscr               ; Copy fpscr
        STMFD    r13!, {r1, r12}          ; Save fpscr and spsr
        VSTMDB   r13!, {d0-d7}            ; Save D0-D7 NEON/VFP registers

        LDR      r0, ADDR_THRESHOLD       ; Get the IRQ Threshold
        LDR      r1, [r0, #0]
        STMFD    r13!, {r1}               ; Save the threshold value

        LDR      r2, ADDR_IRQ_PRIORITY   ; Get the active IRQ priority
        LDR      r3, [r2, #0]
        STR      r3, [r0, #0]             ; Set the priority as threshold
        LDR      r1, ADDR_SIR_IRQ         ; Get the Active IRQ
        LDR      r2, [r1]
        AND      r2, r2, #MASK_ACTIVE_IRQ ; Mask the Active IRQ number

        MOV      r0, #NEWIRQAGR           ; To enable new IRQ Generation
        LDR      r1, ADDR_CONTROL

        CMP      r3, #0                   ; Check if non-maskable priority 0
        STRNE    r0, [r1]                 ; if > 0 priority, acknowledge INTC
        DSB                               ; Make sure acknowledgement is completed

        ;
        ; Enable IRQ and switch to system mode. But IRQ shall be enabled
        ; only if priority level is > 0. Note that priority 0 is non maskable.
        ; Interrupt Service Routines will execute in System Mode.
        ;
        MRS      r14, cpsr                ; Read cpsr
        ORR      r14, r14, #MODE_SYS
        BICNE    r14, r14, #I_BIT         ; Enable IRQ if priority > 0
        MSR      cpsr_cxsf, r14

        STMFD    r13!, {r14}              ; Save lr_usr
        LDR      r0, _fnRAMVectors        ; Load the base of the vector table
        ADD      r14, pc, #0              ; Save return address in LR
        LDR      pc, [r0, r2, lsl #2]     ; Jump to the ISR

        LDMFD    r13!, {r14}              ; Restore lr_usr
        ;
        ; Disable IRQ and change back to IRQ mode
        ;
        CPSID    i, #MODE_IRQ

        LDR      r0, ADDR_THRESHOLD      ; Get the IRQ Threshold
        LDR      r1, [r0, #0]
        CMP      r1, #0                   ; If priority 0
        MOVEQ    r2, #NEWIRQAGR           ; Enable new IRQ Generation
        LDREQ    r1, ADDR_CONTROL
        STREQ    r2, [r1]
        LDMFD    r13!, {r1}
        STR      r1, [r0, #0]             ; Restore the threshold value
       VLDMIA   r13!, {d0-d7}            ; Restore D0-D7 Neon/VFP registers
        LDMFD    r13!, {r1, r12}          ; Get fpscr and spsr
        MSR      spsr_cxsf, r12           ; Restore spsr
        VMSR     fpscr, r1                ; Restore fpscr
        LDMFD    r13!, {r0-r3, r12, pc}^  ; Restore the context and return

;******************************************************************************
;*                  Function Definition of FIQ Handler
;******************************************************************************    
;
; FIQ is not supported for this SoC.
FIQHandler:
;        SUBS     pc, r14, #0x4            ; Return to program before FIQ
		 LDR      pc, _isr_FIQHandler
;******************************************************************************
;*             Function Definition of Abort/Undef Handler
;******************************************************************************    
;
; The Abort handler goes to the C handler of abort mode. Note that the undefined
; instruction is not handled separately.
; if nothing is done in the abort mode, the execution enters infinite loop.
;

;A Prefetch Abort occurs when you try to execute code from non-existing memory regions.
;Pre-fetch abort
AbortHandler:
;			SUBS     pc, r14, #0x4            ; Return to program before FIQ
			LDR      pc, _isr_AbortHandler      ; Go to C handler
UndefInstHandler:
;			SUBS     pc, r14, #0x4            ; Return to program before FIQ
			LDR      pc, _isr_UndefInstHandler      ; Go to C handler

;This is the default Data Abort exception handler. Your application is trying to read or write an illegal memory location
; You can calculate the illegal memory location using by subtracting 8 from the value in R14 (link register).
;Subtracting 8 adjusts for the instruction queue giving you the address of the instruction that caused this exception. For example:
;R14 value is 0x0000021e
;0x0000021e - 8 = 0x00000216.  The instruction which caused the exception is at address 0x00000216
DataAbortHandler:
; Disable all the interrupts
;
        MRS     r0, cpsr                  ; Read from CPSR
        ORR     r0, r0, #0xC0             ; Clear the IRQ and FIQ bits    
        MSR     cpsr_c, r0                ; Write to CPSR
        ADD     r14, pc, #0               ; Store the return address
        LDR     pc, _CPUAbortHandler      ; Go to C handler
;
; Go to infinite loop if returned from C handler
;
loop0:
        B       loop0                      

;*****************************************************************************
;
; End of the file
;
    .end
    
    

