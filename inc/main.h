#ifndef __SC32_GUI_MAIN_H__
#define __SC32_GUI_MAIN_H__

/**
 * @brief
 * @file main.h
 * @author proyrb
 * @date 2025/8/13
 * @note
 */

// 启用日志
#define INC_LOG <log.h>
#define LOG_IF_PRTF rt_kprintf
#define LOG_IF_GET_TICK rt_tick_get

// 启用操作系统
#define INC_RTOS <rtthread.h>
#define RTOS_DELAY_IF rt_thread_delay

#endif