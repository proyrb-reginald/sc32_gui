/**
 * @file main.h
 * @author proyrb (proyrb@yeah.net)
 * @brief
 * @version 0.1
 * @date 2025-08-14
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __SC32_GUI_MAIN_H__
#define __SC32_GUI_MAIN_H__

/****************************** 功能配置 ******************************/

/* 启用日志 */
#define INC_LOG <log.h>
#define LOG_LEV NEWS_LOG
#define LOG_IF_PRTF rt_kprintf
#define LOG_IF_GET_TICK rt_tick_get

/* 启用操作系统 */
#define INC_RTOS <rtthread.h>
#define RTOS_DELAY_IF rt_thread_delay

#endif