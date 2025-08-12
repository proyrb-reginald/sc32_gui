#ifndef ST7789V_H
#define ST7789V_H

/**
 * @brief st7789v驱动程序。
 * @details 先调用st7789v_init初始化模块；再调用st7789v_async_fill来填充指定的屏幕区域。
 * @file st7789v.h
 * @author proyrb
 * @date 2025/8/8
 * @note
 */

/********** 导入需要的头文件 **********/

#include <sc32_conf.h>

/********** 配置模块行为 **********/

#ifdef ST7789V_C

/* 配置GPIO */
#    define RESET_GPIO_GRP GPIOC
#    define RESET_GPIO_PIN GPIO_Pin_12

#    define MODE_GPIO_GRP GPIOC
#    define MODE_GPIO_PIN GPIO_Pin_13

#    define CHIP_GPIO_GRP GPIOC
#    define CHIP_GPIO_PIN GPIO_Pin_14

/* 配置SPI */
#    define USE_SPI SPI2

/* 配置DMA */
#    define USE_DMA DMA0

/* 配置LVGL */
#    define LVGL_DISP &port_disp_drv
#    define LVGL_DONE(disp) lv_disp_flush_ready(disp)

/* 启用测试功能 */
#    define ST7789V_TEST 0

#endif

/********** 主要使用的指令 **********/

typedef enum {
    Wake           = 0x11,  // 唤醒
    SetRAMReadMode = 0x36,  // 设置内存读取方式
    SetColorFmt    = 0x3A,  // 设置颜色格式
    OnReverse      = 0x21,  // 开启反色
    OnDisplay      = 0x29,  // 开启显示
    SetColumn      = 0x2A,  // 设置窗口的列范围
    SetRow         = 0x2B,  // 设置窗口的行范围
    SetRGB         = 0xB0,  // 设置RGB字节序
    Write          = 0x2C,  // 写入数据
} st7789v_cmd_t;

/********** 指令的可选参数 **********/

typedef struct {
    const uint8_t * data;  // 参数指针
    uint32_t        size;  // 参数字节数
} st7789v_arg_t;

/********** 绘制区域 **********/

typedef struct {
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} st7789v_area_t;

// typedef st7789v_area_t lv_area_t;

/********** 导出的函数 **********/

/**
 * @brief 初始化st7789v模块。
 * @param
 * @retval pdPASS：初始化成功。
 * @retval pdFAIL：初始化失败，内存分配失败。
 * @warning 必须先初始化模块后才能进行后续操作。
 * @note
 */
extern int st7789v_init(void);

/**
 * @brief dma中断处理。
 * @param
 * @retval
 * @warning 禁止在非中断中调用。
 * @note
 */
extern void st7789v_dma_irq(void);

/**
 * @brief 发送控制指令与附带的可选参数。
 * @param cmd 指令：可以使用不在st7789v_cmd_t范围的值，该值会被自动作为8位命令发送。
 * @param arg
 * 可选参数：不追加参数时请使用NULL填充；非NULL时，无论使用什么cmd值，都会发送该字节流。
 * @retval
 * @warning 线程安全；同步的。
 * @note
 */
extern void st7789v_ctl(const st7789v_cmd_t cmd, const st7789v_arg_t * const arg);

/**
 * @brief 向屏幕指定区域填充字节流。
 * @param s_x 该区域的 x 轴起始坐标，被视为该区域的左边界（会被填充）。
 * @param e_x 该区域的 x 轴终止坐标，被视为该区域的右边界（会被填充）。
 * @param s_y 该区域的 y 轴起始坐标，被视为该区域的上边界（会被填充）。
 * @param e_y 该区域的 y 轴终止坐标，被视为该区域的下边界（会被填充）。
 * @retval
 * @warning 线程安全；异步的。
 * @note
 */
extern void st7789v_async_fill(const st7789v_area_t * const area,
                               const void * const           buf,
                               const uint8_t                pixel_size);

/**
 * @brief 屏幕填充任务。
 * @param thread_args 任务参数：暂时没有使用。
 * @retval
 * @warning
 * @note
 */
extern void st7789v_test(void * thread_args);

#endif
