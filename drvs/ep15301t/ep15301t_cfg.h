#ifndef __EP15301T_CFG_H__
#define __EP15301T_CFG_H__

/**
 * @brief 亦亚徽圆形显示屏配置文件。
 * @file ep15301t_cfg.h
 * @author proyrb
 * @date 2025/8/13
 * @note
 */

/********** 配置功能 **********/

// 启用LVGL
// #define USE_LVGL

// 启用DMA搬运
#define USE_DMA

// 启用测试接口
#define USE_TEST

// 启用日志
#define INC_LOG <log.h>
#define LOG_IF_PRTF rt_kprintf
#define LOG_IF_GET_TICK rt_tick_get

// 启用操作系统
#define INC_RTOS <rtthread.h>
#define RTOS_DELAY_IF rt_thread_delay

// 启用生产厂商的初始化指令
#define USE_FACT_INIT

/********** 配置外设 **********/

// 配置复位引脚
#define RESET_GPIO_GRP GPIOA
#define RESET_GPIO_PIN GPIO_Pin_14

// 配置片选引脚
#define CHIP_GPIO_GRP GPIOB
#define CHIP_GPIO_PIN GPIO_Pin_2

// 配置背光引脚
#define BACK_GPIO_GRP GPIOD
#define BACK_GPIO_PIN GPIO_Pin_10

// 配置QSPI
#define QSPI_DEV QSPI1

// 配置DMA
#ifdef USE_DMA
#    define DMA_DEV DMA0
#endif

/********** 配置LVGL **********/

#ifdef USE_LVGL
#    define LVGL_DISP &port_disp_drv
#    define LVGL_DONE(disp) lv_disp_flush_ready(disp)
#endif

#endif