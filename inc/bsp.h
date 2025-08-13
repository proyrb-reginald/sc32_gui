#ifndef __SC32_GUI_BSP_H__
#define __SC32_GUI_BSP_H__

/**
 * @brief 实现中断并提供初始化接口。
 * @file bsp.h
 * @author proyrb
 * @date 2025/8/12
 * @note
 */

void rcc_init(void);
void uart1_init(void);
void qspi1_init(void);

#endif