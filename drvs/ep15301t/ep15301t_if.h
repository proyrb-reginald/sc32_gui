/**
 * @file ep15301t_if.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 屏幕驱动程序接口。
 * @version 0.1
 * @date 2025-08-14
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __EP15301T_IF_H__
#define __EP15301T_IF_H__

/****************************** 类型定义 ******************************/

/* 主要常用指令 */
typedef enum {
    Set_X_RAG = 0x2A,
    Set_Y_RAG = 0x2B,
    Write_RAM = 0x2C,
} ep15301t_ins_t;

/* 指令结构 */
typedef struct __attribute__((packed)) {
    ep15301t_ins_t ins;
    uint8_t *      data;
    uint32_t       size;
} ep15301t_cmd_t;

/* 填充区域 */
typedef struct {
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} ep15301t_area_t;

/****************************** 函数接口 ******************************/

/**
 * @brief 初始化。
 * @return int
 * @retval 0 固定不变。
 * @warning 必须执行一次初始化操作后才可以进行后续操作。
 * @note
 */
extern int ep15301t_init(void);

/**
 * @brief 发送指令。
 * @param cmd 指令与参数。
 * @retval
 * @warning 即使ins非枚举值也会按照实际数值发送出去；data与size任意一个为0表示不发送参数。
 * @note
 */
extern void ep15301t_ctl(const ep15301t_cmd_t * const cmd);

/**
 * @brief 向屏幕指定区域填充图像数据。
 * @param area 指定区域
 * @param buf 图像数据缓冲区。
 * @param unit_size 单个像素所占用的字节数。
 * @retval
 * @warning
 * @note
 */
extern void ep15301t_fill(const ep15301t_area_t * const area,
                          const void * const            buf,
                          const uint8_t                 unit_size);

/**
 * @brief 测试驱动程序功能。
 * @param thread_args 任务参数，暂时未使用。
 * @retval
 * @warning 此函数不返回！
 * @note 请在`ep15301t_cfg.h`中添加`USE_TEST`宏。
 */
extern void ep15301t_test(void * thread_args);

/**
 * @brief 处理DMA中断。
 * @retval
 * @warning 此函数不返回！
 * @note 请在`ep15301t_cfg.h`中添加`USE_DMA`宏。
 */
extern void ep15301t_dma_irq(void);

#endif