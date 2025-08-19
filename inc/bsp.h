/**
 * @file bsp.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 片上与板载外设初始化接口与中断实现。
 * @version 0.1
 * @date 2025-08-14
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __SC32_GUI_BSP_H__
#define __SC32_GUI_BSP_H__

/****************************** 函数接口 ******************************/

void rcc_init(void);
void uart1_init(void);
void qspi0_init(void);
void qspi1_init(void);
void dma0_init(void);
void dma1_init(void);
void dma2_init(void);

#endif