/**
 * @file w25q_cfg.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 外部闪存配置。
 * @version 0.1
 * @date 2025-08-18
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __W25Q_CFG_H__
#define __W25Q_CFG_H__

/****************************** 功能配置 ******************************/

/* 启用型号：W25Q[64/128] */
#define USE_MODEL 128

/* 启用测试 */
#define USE_TEST

/* 启用DMA */
#define USE_DMA

/* 启用日志 */
#define INC_LOG <log.h>
#define LOG_LEV INFO_LOG
#define LOG_IF_PRTF rt_kprintf
#define LOG_IF_GET_TICK rt_tick_get

/* 启用操作系统 */
#define INC_RTOS <rtthread.h>
#define RTOS_DELAY_IF rt_thread_delay

/****************************** 外设配置 ******************************/

/* 片选引脚 */
#define CHIP_GPIO_GRP GPIOB
#define CHIP_GPIO_PIN GPIO_Pin_13

/* 选择QSPI */
#define QSPI_DEV QSPI0

/* 选择DMA */
#ifdef USE_DMA
#    define DMA_TX_DEV DMA1
#    define DMA_RX_DEV DMA2
#endif

// 设备唯一ID
#define W25Q64_Unique_ID 0xe464b4b293303836

#endif