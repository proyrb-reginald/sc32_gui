/**
 * @file fs_cfg.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 顶层文件系统配置。
 * @version 0.1
 * @date 2025-08-18
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __FS_CFG_H__
#define __FS_CFG_H__

/****************************** 配置功能 ******************************/

/* 启用测试接口 */
// #define USE_TEST

/* 启用片上文件系统 */
#define USE_FS_ROM

/* 启用外部Flash文件系统 */
#define USE_FS_FLASH

/* 启用SD卡文件系统 */
// #define USE_FS_SD

#define DEV_ROM 0
#define DEV_FLASH 1
#define DEV_SD 2

/* 启用日志 */
#ifdef __FS_C__
#    define INC_LOG <log.h>
#    define LOG_LEV INFO_LOG
#    define LOG_IF_PRTF rt_kprintf
#    define LOG_IF_GET_TICK rt_tick_get
#endif

/* 启用操作系统 */
#ifdef __FS_C__
#    define INC_RTOS <rtthread.h>
#    define RTOS_DELAY_IF rt_thread_delay
#endif

/****************************** 目录配置 ******************************/

/* 根目录（不可更改） */
#define ROOT_D "/"

/* 根目录结构 */
#ifdef ROOT_D

/* 系统目录 */
#    define ROOT_SYS_D ROOT_D "sys/"

/* 系统目录结构 */
#    ifdef ROOT_SYS_D

/* 启用文件 */
#        define BOOT_FILE "boot.dat"
#        define BOOT_FILE_P ROOT_SYS_D BOOT_FILE

#    endif

/* 资源目录 */
#    define ROOT_RES_D ROOT_D "res/"

/* 资源目录结构 */
#    ifdef ROOT_RES_D

/* 资源文件 */
#        define RES_FILE "sbg.jpg"
#        define RES_FILE_P ROOT_RES_D RES_FILE

#    endif
#endif

#endif