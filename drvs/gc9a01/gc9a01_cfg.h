/**
 * @file ep15301t_cfg.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 屏幕驱动程序配置。
 * @version 0.1
 * @date 2025-08-26
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __GC9A01_CFG_H__
#define __GC9A01_CFG_H__

/****************************** 功能配置 ******************************/

/* 启用LVGL */
#define USE_LVGL
#ifdef USE_LVGL
#    define LVGL_DISP &port_disp_drv
#    define LVGL_DONE(disp) lv_disp_flush_ready(disp)
#endif

/* 启用DMA */
#define USE_DMA

/* 启用测试 */
// #define USE_TEST

/* 启用日志 */
#define INC_LOG <log.h>
#define LOG_LEV INFO_LOG
#define LOG_IF_PRTF rt_kprintf
#define LOG_IF_GET_TICK rt_tick_get

/* 启用操作系统 */
#define INC_RTOS <rtthread.h>
#define RTOS_DELAY_IF rt_thread_delay

/* 启用生产商初始化指令 */
#define USE_FACT_INIT

/****************************** 外设配置 ******************************/

/* 复位引脚 */
#define RESET_GPIO_GRP GPIOA
#define RESET_GPIO_PIN GPIO_Pin_14

/* 片选引脚 */
#define CHIP_GPIO_GRP GPIOB
#define CHIP_GPIO_PIN GPIO_Pin_2

/* 数据命令选择引脚 */
#define DC_GPIO_GRP GPIOB
#define DC_GPIO_PIN GPIO_Pin_4

/* 选择QSPI */
#define QSPI_DEV QSPI1

/* 选择DMA */
#ifdef USE_DMA
#    define DMA_DEV DMA0
#endif

#endif