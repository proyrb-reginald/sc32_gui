/**
 * @file gc9a01_if.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 屏幕驱动程序接口。
 * @version 0.1
 * @date 2025-08-26
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __GC9A01_IF_H__
#define __GC9A01_IF_H__

/****************************** 导入头文件 ******************************/

#include <sc32_conf.h>

/****************************** 类型定义 ******************************/

/* 填充区域 */
typedef struct {
    int16_t start_p_x;
    int16_t start_p_y;
    int16_t end_p_x;
    int16_t end_p_y;
} lcd_area_t;

/****************************** 函数接口 ******************************/

/**
 * @brief 初始化。
 * @return int
 * @retval 0 固定不变。
 * @warning 必须执行一次初始化操作后才可以进行后续操作。
 * @note
 */
extern int lcd_init(void);

/**
 * @brief 向屏幕指定区域填充图像数据。
 * @param area 指定区域
 * @param buf 图像数据缓冲区。
 * @param unit_size 单个像素所占用的字节数。
 * @retval
 * @warning
 * @note
 */
extern void lcd_fill(const lcd_area_t * area, const uint8_t * buf);

/**
 * @brief 测试驱动程序功能。
 * @param thread_args 任务参数，暂时未使用。
 * @retval
 * @warning 此函数不返回！
 * @note 请在`gc9a01_cfg.h`中添加`USE_TEST`宏。
 */
extern void lcd_test(void * thread_args);

/**
 * @brief 处理DMA中断。
 * @retval
 * @warning 此函数不返回！
 * @note 请在`gc9a01_cfg.h`中添加`USE_DMA`宏。
 */
extern void lcd_dma_irq(void);

#endif