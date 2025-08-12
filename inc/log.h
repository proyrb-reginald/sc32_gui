#ifndef LOG_H
#define LOG_H

/**
 * @brief 这是日志模块。
 * @details
 * @file log.h
 * @author proyrb
 * @date 2025/8/8
 * @note
 */

/********** 导入需要的头文件 **********/

#include <rtthread.h>

/********** 配置模块功能 **********/

/* 日志等级 */
#define TRAC_LOG 0
#define INFO_LOG 1
#define NEWS_LOG 2
#define WARN_LOG 3
#define ERRO_LOG 4
#define NONE_LOG 5

/* 日志输出阈值：输出大于等于该级别的日志 */
#define LOG_LEV INFO_LOG

/* 格式化打印函数接口 */
#define INTERFACE_PRINTF rt_kprintf

/* 系统时刻函数接口 */
#define GET_TICK rt_tick_get

/* 打印无附加信息的日志 */
#define PRTF(lev, fmt, ...)                                                              \
    if (lev >= LOG_LEV) {                                                                \
        INTERFACE_PRINTF(fmt, ##__VA_ARGS__);                                            \
    }

/* 打印附带系统信息的日志 */
#define OS_PRTF(lev, fmt, ...)                                                           \
    if (lev >= LOG_LEV) {                                                                \
        INTERFACE_PRINTF("[%u:%s] " fmt, GET_TICK(), rt_thread_self()->name,             \
                         ##__VA_ARGS__);                                                 \
    }

#endif  // LOG_H
