#ifndef __EP15301T_IF_H__
#define __EP15301T_IF_H__

/**
 * @brief 亦亚徽圆形显示屏驱动接口。
 * @file ep15301t.h
 * @author proyrb
 * @date 2025/8/13
 * @note
 */

/********** 导入需要的头文件 **********/

#include <stdint.h>

/********** 类型定义 **********/

// 常用指令
typedef enum {
    Set_X_RAG = 0x2A,  // 设置填充区域的x范围
    Set_Y_RAG = 0x2B,  // 设置填充区域的y范围
    Write_RAM = 0x2C,  // 写入图像数据到显示屏
} ep15301t_ins_t;

// 指令参数
typedef struct __attribute__((packed)) {
    ep15301t_ins_t ins;   // 具体指令
    uint8_t *      data;  // 数据指针
    uint32_t       size;  // 数据字节数
} ep15301t_cmd_t;

// 填充区域
typedef struct {
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} ep15301t_area_t;

/********** 函数接口声明 **********/

/**
 * @brief 初始化ep15301t。
 * @param
 * @retval
 * @warning 必须先初始化后才能进行后续操作。
 * @note
 */
extern int ep15301t_init(void);

/**
 * @brief 发送指令与可选参数。
 * @param cmd 指令：可以使用不在ep15301t_cmd_t范围的值，该值会被自动作为8位字节发送。
 * @param arg 可选参数：无参时请使用0填充；非0值都会被实际发送出去。
 * @retval
 * @warning
 * @note
 */
extern void ep15301t_ctl(const ep15301t_cmd_t * const cmd);

/**
 * @brief 向屏幕指定区域填充图像数据。
 * @param area 填充区域：指定数据应该填充到屏幕的哪块区域。
 * @param buf 数据：需要被发送至屏幕的数据缓冲区。
 * @param unit_size 单像素字节数：每个像素用多少字节来表示。
 * @retval
 * @warning
 * @note
 */
extern void ep15301t_fill(const ep15301t_area_t * const area,
                          const void * const            buf,
                          const uint8_t                 unit_size);

/**
 * @brief 测试屏幕功能。
 * @param thread_args 任务参数：暂时没有使用。
 * @retval
 * @warning 需要在ep15301t_cfg.h中添加USE_TEST宏后才可使用。
 * @note
 */
__attribute__((noreturn)) extern void ep15301t_test(void * thread_args);

/**
 * @brief dma中断处理。
 * @param
 * @retval
 * @warning 需要在ep15301t_cfg.h中添加USE_DMA宏才可使用。
 * @note
 */
extern void ep15301t_dma_irq(void);

#endif