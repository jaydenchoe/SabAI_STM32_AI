;/********************* COPYRIGHT(c) 2019 STMicroelectronics ********************
;* File Name          : se_interface_exception.s
;* Author             : MCD Application Team
;* Description        : This file defines function to handle user interrupts
;*                      raised in Firewall
;*******************************************************************************
;*  @attention
;*
;* Copyright (c) 2017 STMicroelectronics. All rights reserved.
;*
;*  This software component is licensed by ST under Ultimate Liberty license
;*  SLA0044, the "License"; You may not use this file except in compliance with
;*  the License. You may obtain a copy of the License at:
;*                              www.st.com/SLA0044
;*
;*******************************************************************************
;

        SECTION .SE_IF_Code_Entry:CODE:NOROOT(2)

        EXPORT SE_UserHandlerWrapper

        IMPORT SeCallGateStatusParam
        IMPORT __ICFEDIT_SE_CallGate_region_ROM_start__

; ******************************************
; Function Name  : SE_UserHandlerWrapper
; Description    : call Appli IT Handler
; input          : R0 : @ Appli IT Handler
;                : R1 : Primask value
; internal       : R3
; output         : R0 : SE_CallGate 1st input param: Service ID
;                : R1 : SE_CallGate 2nd input param: return status var @
;                : R2 : SE_CallGate 3rd input param: Primask
; return         : None
; ******************************************
SE_UserHandlerWrapper
; restore Primask
        MSR PRIMASK, R1
; call User IT Handler
        BLX R0
; disable IT
        CPSID i
; set input param for SE_CallGate
        MOV R0, #0x0001000UL ; SE_EXIT_INTERRUPT
        MOV R1, #0x00000000  ; se_status param is NULL pointer
        MOV R2, #0xFFFFFFFF ; invalid Primask
; re-enter in firewall via SE_CallGate
        LDR R3, =__ICFEDIT_SE_CallGate_region_ROM_start__
        ADD R3, R3, #1
        BLX R3 ; LR shall be updated otherwise __IS_CALLER_SE_IF will fail in Se_CallGate
; we shall not raise this point
        ; B NVIC_SystemReset

        END
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
