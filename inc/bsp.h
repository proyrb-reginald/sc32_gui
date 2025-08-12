#ifndef __SC32_GUI_BSP_H__
#define __SC32_GUI_BSP_H__

/**
 * @brief 提供中断与初始化接口。
 * @file bsp.h
 * @author proyrb
 * @date 2025/8/12
 * @note
 */

__attribute__((interrupt)) void SysTick_Handler(void);
__attribute__((interrupt)) void UART1_3_5_IRQHandler(void);

void rcc_init(void);
void uart1_init(void);

#endif