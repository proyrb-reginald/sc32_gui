/**
 * @file log.h
 * @author proyrb (proyrb@yeah.net)
 * @brief 日志接口。
 * @version 0.1
 * @date 2025-08-14
 * @copyright Copyright (c) 2025
 * @note
 */

#ifndef __LOG_H__
#define __LOG_H__

/****************************** 导入头文件 ******************************/

#include <rtthread.h>

/****************************** 功能配置 ******************************/

/* 日志等级 */
#define TRAC_LOG 0
#define INFO_LOG 1
#define NEWS_LOG 2
#define WARN_LOG 3
#define ERRO_LOG 4
#define NONE_LOG 5

/* 日志输出阈值：输出大于等于该级别的日志 */
#ifndef LOG_LEV
#    error "Please provide a log-level!"
#endif

/* 格式化打印接口 */
#ifndef LOG_IF_PRTF
#    error "Please provide a interface to print!"
#endif

/* 获取系统时刻接口 */
#ifndef LOG_IF_GET_TICK
#    error "Please provide a interface to get system tick!"
#endif

/* 打印无附加信息的日志 */
#define PRTF_LOG(lev, fmt, ...)                                                          \
    if (lev >= LOG_LEV) {                                                                \
        LOG_IF_PRTF(fmt, ##__VA_ARGS__);                                                 \
    }

/* 打印附带系统信息的日志 */
#define PRTF_OS_LOG(lev, fmt, ...)                                                       \
    if (lev >= LOG_LEV) {                                                                \
        LOG_IF_PRTF("[%u:%s:%s:%d] " fmt, LOG_IF_GET_TICK(), rt_thread_self()->name,     \
                    __FUNCTION__, __LINE__, ##__VA_ARGS__);                              \
    }

#endif