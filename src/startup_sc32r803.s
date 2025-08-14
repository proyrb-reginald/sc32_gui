;******************************************************************************
;* File Name          : startup_xx.s
;* Author             : SOC SA Team
;* Description        : SC32F11XXB devices vector table for MDK-ARM toolchain.
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == Reset_Handler
;*                      - Set the vector table entries with the exceptions ISR address
;*                      - Branches to __main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the CortexM0 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;* <<< Use Configuration Wizard in Context Menu >>>
;****************************************************************************** 
; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000200
                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                   ; Top of Stack
                DCD     Reset_Handler                  ; Reset Handler
                DCD     NMI_Handler                    ; NMI Handler
                DCD     HardFault_Handler              ; Hard Fault Handler
                DCD     MemManage_Handler              ; MPU Fault Handler
                DCD     BusFault_Handler               ; Bus Fault Handler
                DCD     UsageFault_Handler             ; Usage Fault Handler
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     SVC_Handler                    ; SVCall Handler
                DCD     DebugMon_Handler               ; Debug Monitor Handler
                DCD     0                              ; Reserved
                DCD     PendSV_Handler                 ; PendSV Handler
                DCD     SysTick_Handler                ; SysTick Handler

                ; External Interrupts
                DCD     INT0_IRQHandler                ; INT0
				DCD     INT1_7_IRQHandler              ; INT1~INT17
				DCD     INT8_11_IRQHandler             ; INT8~INT11
				DCD     INT12_15_IRQHandler            ; INT12~INT15
				DCD		RCC_IRQHandler				   ; RCC：停振检测
				DCD		0                              ; LDP
                DCD     BTM_IRQHandler                 ; BTM
                DCD     UART0_2_4_IRQHandler           ; UART0-UART2(LIN)-UART4
				DCD     UART1_3_5_IRQHandler           ; UART1-UART3-UART5
				DCD     TWIx_QSPIx_0_2_IRQHandler      ; TWI_QSPIx_0_2_IRQn
				DCD     TWIx_QSPIx_1_3_IRQHandler      ; TWI_QSPIx_1_3_IRQn
				DCD     DMA0_IRQHandler                ; DMA0
				DCD     DMA1_IRQHandler                ; DMA1
				DCD     DMA2_IRQHandler                ; DMA2
                DCD     DMA3_IRQHandler                ; DMA3
                DCD     TIMER0_IRQHandler              ; TIMER0
                DCD     TIMER1_IRQHandler              ; TIMER1
				DCD     TIMER2_IRQHandler              ; TIMER2
				DCD     TIMER3_IRQHandler              ; TIMER3
				DCD     TIMER4_5_IRQHandler            ; TIMER4-TIMER5
				DCD     TIMER6_7_IRQHandler            ; TIMER6-TIMER7
				DCD     PWM0_IRQHandler                ; PWM0
				DCD     LEDPWM_IRQHandler              ; LEDPWM
				DCD     0                              ; Reserved 
				DCD     0                              ; Reserved 
				DCD     0                              ; Reserved
                DCD     0                              ; Reserved
				DCD		0                              ; Reserved	
				DCD     CAN_IRQHandler                 ; CAN
                DCD     ADC_IRQHandler                 ; ADC
                DCD     CMP_IRQHandler                 ; CMP
                DCD     TK_IRQHandler                  ; TK               

__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler routine
Reset_Handler    PROC
                 EXPORT  Reset_Handler                  [WEAK]
        IMPORT  __main
        IMPORT  SystemInit  
			
				 LDR 	 R0,=0x40003000
				 LDR 	 R1,[R0]
				 LDR 	 R2,=0x00000001
				 ORRS 	 R1,R1,R2
				 STR 	 R1,[R0]              ;AHB_CFG
				
				 LDR 	 R0,=0x4001083C
				 LDR 	 R1,=0xA05F0100
				 STR 	 R1,[R0]              ;DMA0_REV

				 LDR 	 R0,=0x4001087C
				 LDR 	 R1,=0xA05F0100
				 STR 	 R1,[R0]              ;DMA1_REV

				 LDR 	 R0,=0x400108BC
				 LDR 	 R1,=0xA05F0100
				 STR 	 R1,[R0]              ;DMA2_REV

				 LDR 	 R0,=0x400108FC
				 LDR 	 R1,=0xA05F0100
				 STR 	 R1,[R0]              ;DMA3_REV
				 
				 LDR 	 R0,=0x40003000
				 LDR 	 R1,[R0]
				 LDR 	 R2,=0xFFFFFFFE
				 ANDS 	 R1,R1,R2
				 STR 	 R1,[R0]              ;AHB_CFG
			
				 LDR	 R0,=0x40003010
				 LDR     R1,=0xA05FF010
				 STR     R1,[R0]              ;RCC_REV
				 
				 LDR	 R0,=0x400003DC
				 LDR     R1,=0xA05FF001
				 STR     R1,[R0]              ;IAP_REV
				 
				 LDR	 R0,=0x40021000
				 LDR     R1,=0x00800020
				 STR     R1,[R0]              ;APB1_CFG
				 
				 LDR	 R0,=0x4002105C
				 LDR     R1,=0xA05F0080
				 STR     R1,[R0]              ;SPI1_REV
				 				 
				 LDR	 R0,=0x40021000
				 LDR     R1,=0x00000000
				 STR     R1,[R0]              ;APB1_CFG
				 
				 LDR	 R0,=0x40022000
				 LDR     R1,=0x00800040
				 STR     R1,[R0]              ;APB2_CFG

				 LDR	 R0,=0x4002213C
				 LDR     R1,=0xA05F0F00
				 STR     R1,[R0]              ;ADC_REV
				 
				 LDR	 R0,=0x40022000
				 LDR     R1,=0x00000000
				 STR     R1,[R0]              ;APB2_CFG
				 
                 LDR     R0, =SystemInit
                 BLX     R0
                 LDR     R0, =__main
                 BX      R0
                 ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                     [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler               [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler               [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler                [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler              [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                     [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler                [WEAK]
                B       .
                ENDP					
PendSV_Handler  PROC
                EXPORT  PendSV_Handler                  [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler                 [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  INT0_IRQHandler                 [WEAK]
                EXPORT  INT1_7_IRQHandler               [WEAK]
                EXPORT  INT8_11_IRQHandler              [WEAK]
				EXPORT	INT12_15_IRQHandler			    [WEAK]
                EXPORT  RCC_IRQHandler               	[WEAK]
                EXPORT  BTM_IRQHandler             		[WEAK]
                EXPORT  UART0_2_4_IRQHandler            [WEAK]
                EXPORT  UART1_3_5_IRQHandler            [WEAK]
                EXPORT  TWIx_QSPIx_0_2_IRQHandler      	[WEAK]
                EXPORT  TWIx_QSPIx_1_3_IRQHandler  	  	[WEAK]
                EXPORT  DMA0_IRQHandler 				[WEAK]
                EXPORT  DMA1_IRQHandler                 [WEAK]
                EXPORT  DMA2_IRQHandler 				[WEAK]
                EXPORT  DMA3_IRQHandler             	[WEAK]
                EXPORT  TIMER0_IRQHandler               [WEAK]
                EXPORT  TIMER1_IRQHandler               [WEAK]
                EXPORT  TIMER2_IRQHandler               [WEAK]
                EXPORT  TIMER3_IRQHandler               [WEAK]
				EXPORT  TIMER4_5_IRQHandler             [WEAK]
				EXPORT  TIMER6_7_IRQHandler             [WEAK]
				EXPORT  PWM0_IRQHandler               	[WEAK]
				EXPORT  LEDPWM_IRQHandler               [WEAK]
				EXPORT	CAN_IRQHandler                  [WEAK]
				EXPORT  ADC_IRQHandler                  [WEAK]
				EXPORT  CMP_IRQHandler                  [WEAK]
				EXPORT  TK_IRQHandler                   [WEAK]

INT0_IRQHandler
INT1_7_IRQHandler
INT8_11_IRQHandler
INT12_15_IRQHandler
RCC_IRQHandler
BTM_IRQHandler
UART0_2_4_IRQHandler
UART1_3_5_IRQHandler
TWIx_QSPIx_0_2_IRQHandler
TWIx_QSPIx_1_3_IRQHandler
DMA0_IRQHandler
DMA1_IRQHandler
DMA2_IRQHandler
DMA3_IRQHandler
TIMER0_IRQHandler
TIMER1_IRQHandler
TIMER2_IRQHandler
TIMER3_IRQHandler
TIMER4_5_IRQHandler
TIMER6_7_IRQHandler
PWM0_IRQHandler
LEDPWM_IRQHandler
CAN_IRQHandler
ADC_IRQHandler
CMP_IRQHandler
TK_IRQHandler

                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit

                 ELSE

                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap

__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END

;************************ (C) COPYRIGHT SinOneChip *****END OF FILE*****