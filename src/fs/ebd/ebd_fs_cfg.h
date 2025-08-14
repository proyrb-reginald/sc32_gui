/**
 * @file ebd_fs_cfg.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 片上文件系统配置。
 * @version 0.1
 * @date 2025-08-14
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __EBD_FS_CFG_H__
#define __EBD_FS_CFG_H__

/****************************** 功能配置 ******************************/

/* 启用测试 */
#define USE_TEST

/* 启用日志 */
#define INC_LOG <log.h>
#define LOG_LEV NEWS_LOG
#define LOG_IF_PRTF rt_kprintf
#define LOG_IF_GET_TICK rt_tick_get

/* 启用操作系统 */
#define INC_RTOS <rtthread.h>
#define RTOS_DELAY_IF rt_thread_delay

/* 配置参数 **/

/* 配置片上Flash基地址 */
#define FLASH_BASE_ADR FLASH_BASE

/* 配置片上Flash块大小（即最小物理擦除扇区） */
#define BLK_SIZE 512

/* 配置片上Flash块的最大擦写次数 */
#define BLK_CYCLE 10000

/* 配置片上文件系统相对基地址的偏移量 */
#define FS_OST_ADR 0x0003B000

/* 配置片上文件系统偏移后的起始地址 */
#define FS_START_ADR (FLASH_BASE_ADR + FS_OST_ADR)

/* 配置片上文件系统尾地址 */
#define FS_END_ADR (FLASH_BASE_ADR + FLASH_SIZE)

/* 配置片上文件系统的块数量 */
#define FS_BLK_CNT ((FLASH_SIZE - FS_OST_ADR) / BLK_SIZE)

/* 配置片上文件系统的缓存区大小（读写各一个缓冲区） */
#define FS_CACHE_SIZE 64

#endif